#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/moduleparam.h>
#include <linux/kdev_t.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/signal.h>
#include <asm/siginfo.h>
#include <linux/interrupt.h>
#include "efm32gg.h"

#define NAME "gamepad"
#define COUNT 1
#define GPIO_EVEN 17
#define GPIO_ODD 18


static int __init gamepad_init(void);
static void __exit gamepad_exit(void);
static int gamepad_open(struct inode*, struct file*);
static int gamepad_release(struct inode*, struct file*);
static ssize_t gamepad_read(struct file*, char* __user, size_t, loff_t*);
static ssize_t gamepad_write(struct file*, char* __user, size_t, loff_t*);
static irqreturn_t gpio_interrupt_handler(int, void*, struct pt_regs*);
static int gamepad_fasync(int, struct file*, int mode);


static dev_t device_num;
struct cdev gamepad_cdev;
struct fasync_struct* fasync;
struct class *cl;

static struct file_operations gamepad_fops = {
    .owner = THIS_MODULE,
    .open = gamepad_open,
    .release = gamepad_release,
    .read = gamepad_read,
    .write = gamepad_write,
    .fasync = gamepad_fasync,
};

static int gamepad_fasync(int fd, struct file *filp, int mode){
	printk("gamepad_fasync\n");
    return fasync_helper(fd, filp, mode, &fasync);
}

irqreturn_t gpio_interrupt_handler(int irq, void* dev_id, struct pt_regs* regs){
    printk("Handling GPIO interrupt\n");
    if(fasync){
    	kill_fasync(&fasync, SIGIO, POLL_IN);
	}
    iowrite32(ioread32(GPIO_IF), GPIO_IFC);
    return IRQ_HANDLED;
}


static int __init gamepad_init(void){
	int result;
    printk("Attempting to load gamepad driver module\n");

    result = alloc_chrdev_region(&device_num, 0, COUNT, NAME);

    if (result < 0) {
        printk(KERN_ALERT "Failed to allocate device numbers\n");
        return -1;
    }

    if (request_mem_region((resource_size_t)GPIO_PC_MODEL, 1, NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_MODEL memory region\n");
        return -1;
    }
    if (request_mem_region((resource_size_t)GPIO_PC_DOUT, 1, NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_DOUT memory region\n");
        return -1;
    }
    if (request_mem_region((resource_size_t)GPIO_PC_DIN, 1, NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_DIN memory region\n");
        return -1;
    }

    iowrite32(0x33333333, GPIO_PC_MODEL);
    iowrite32(0xFF, GPIO_PC_DOUT);
    iowrite32(0x22222222, GPIO_EXTIPSELL);
    
    //TODO release these
    if (request_mem_region((resource_size_t)DAC0_CTRL, 1, NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting DAC0_CTRL memory region, already in use?\n");
        return -1;
    }
    
    if (request_mem_region((resource_size_t)DAC0_CH0CTRL, 1, NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting DAC0_CH0CTRL memory region, already in use?\n");
        return -1;
    }
    
    if (request_mem_region((resource_size_t)DAC0_CH1CTRL, 1, NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting DAC1_CH0CTRL memory region, already in use?\n");
        return -1;
    }
	
	iowrite32(0x50010, DAC0_CTRL);
	iowrite32(1, DAC0_CH0CTRL);
	iowrite32(1, DAC0_CH1CTRL);
	
	//TODO set in open
    request_irq(GPIO_EVEN, (irq_handler_t)gpio_interrupt_handler, 0, NAME, &gamepad_cdev);
    request_irq(GPIO_ODD, (irq_handler_t)gpio_interrupt_handler, 0, NAME, &gamepad_cdev);
	
	//TODO add code to remove these on exit
    cdev_init(&gamepad_cdev, &gamepad_fops);
    gamepad_cdev.owner = THIS_MODULE;
    cdev_add(&gamepad_cdev, device_num, COUNT);
    cl = class_create(THIS_MODULE, NAME);
	device_create(cl, NULL, device_num, NULL, NAME);

    iowrite32(0xFF, GPIO_EXTIFALL);
    iowrite32(0x00FF, GPIO_IEN);
    iowrite32(0xFF, GPIO_IFC);

    printk("Gamepad driver loaded.\n");
    return 0;
}


static void __exit gamepad_exit(void)
{
    printk("Unloading gamepad driver\n");

    iowrite32(0x0000, GPIO_IEN);
	//TODO place in close
    free_irq(GPIO_EVEN, &gamepad_cdev);
    free_irq(GPIO_ODD, &gamepad_cdev);

    release_mem_region((resource_size_t)GPIO_PC_MODEL, 1);
    release_mem_region((resource_size_t)GPIO_PC_DIN, 1);
    release_mem_region((resource_size_t)GPIO_PC_DOUT, 1);


    cdev_del(&gamepad_cdev);
    
    
    unregister_chrdev_region(device_num, COUNT);
}


static int gamepad_open(struct inode* inode, struct file* filp)
{
    printk("Gamepad driver opened\n");
    return 0;
}

static int gamepad_release(struct inode* inode, struct file* filp)
{
    printk("Gamepad driver closed\n");
    return 0;
}


static ssize_t gamepad_read(struct file* filp, char* __user buff,
        size_t count, loff_t* offp)
{
	int data = ioread32(GPIO_PC_DIN);
    if (copy_to_user(buff, &data, 1)){
    	printk("gapepad_read copy_to_user error");
    }
    return 1;
}

static ssize_t gamepad_write(struct file* filp, char* __user buff,
        size_t count, loff_t* offp)
{
	int data = 0;
    copy_from_user(&data, &buff, 1);
    printk("%d\n", data);
    return 1;
}

module_init(gamepad_init);
module_exit(gamepad_exit);
MODULE_DESCRIPTION("Device driver for the gamepad used in TDT4258");
MODULE_LICENSE("GPL");

