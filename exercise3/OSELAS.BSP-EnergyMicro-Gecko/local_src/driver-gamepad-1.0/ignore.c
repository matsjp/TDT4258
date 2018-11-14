/*
 * This is a demo Linux kernel module.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include "efm32gg.h"
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>



/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

#define DRIVER_NAME "gamepad"
#define COUNT 1
#define GPIO_EVEN 17
#define GPIO_ODD 18


static int __init template_init(void);
static void __exit template_cleanup(void);
static int gamepad_open(struct inode*, struct file*);
static int gamepad_release(struct inode*, struct file*);
static ssize_t gamepad_read(struct file*, char* __user, size_t, loff_t*);
static ssize_t gamepad_write(struct file*, char* __user, size_t, loff_t*);

static dev_t device_num;
static cdev gamepad_cdev;


/*irqreturn_t gpio_interrupt_handler(int irq, void* dev_id, struct pt_regs* regs)
{
    printk(KERN_ALERT "Handling GPIO interrupt\n");
    iowrite32(ioread32(GPIO_IF), GPIO_IFC);
    
    return IRQ_HANDLED;
}*/


static ssize_t gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp){
	printk("gamepad_read\n");
	copy_to_user(buff, ioread32(GPIO_PC_DIN), 1);
	return 1;
}

static ssize_t gamepad_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp){
	printk("gamepad_write\n");
	return 1;
}

static int gamepad_open(struct inode *inode, struct file *filp){
	printk("gamepad_open\n");
	return 0;	
}

static int gamepad_release(struct inode *, struct file *){
	printk("gamepad_release\n");
	return 0;
}

struct file_operations gamepad_fops = {
	.owner = THIS_MODULE,
	.open = gamepad_open,
	.release = gameped_release,
	.read = gamepad_read,
	.write = gamepad_write,
};

static int __init template_init(void)
{
	printk("Hello World, here is your module speaking\n");
	
	if(alloc_chrdev_region(&device_num, 0, COUNT, DRIVER_NAME) < 0){
		printk("Error during alloc_chrdev_region\n");
		return -1;
	}
	
	if (request_mem_region(GPIO_PC_MODEL, 1, DRIVER_NAME) == NULL){
		printk("Error during GPIO_PC_MODEL mem request\n");
		return -1;
	}
	
	if (request_mem_region(GPIO_PC_DIN, 1, DRIVER_NAME) == NULL){
		printk("Error during GPIO_PC_DIN mem request\n");
		return -1;
	}
	
	if (request_mem_region(GPIO_PC_DOUT, 1, DRIVER_NAME) == NULL){
		printk("Error during GPIO_PC_OUT mem request\n");
		return -1;
	}
	
	iowrite32(GPIO_PC_MODEL, 0x33333333);
	iowrite32(0xFF, GPIO_PC_DOUT);
	iowrite32(0x22222222, GPIO_EXTIPSELL);
	
	/*request_irq(GPIO_EVEN, (irq_handler_t)gpio_interrupt_handler, 0, DRIVER_NAME, &gamepad_cdev);
	request_irq(GPIO_ODD, (irq_handler_t)gpio_interrupt_handler, 0, DRIVER_NAME, &gamepad_cdev);*/
	
	cdev_init(&gamepad_cdev, &gamepad_fops);
	/*gamepad_cdev.owner = THIS_MODULE;
	cdev_add(&gamepad_cdev, device_num, COUNT);*/
	printk("Driver loaded\n");
	return 0;
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit template_cleanup(void)
{
	 printk("Short life for a small module...\n");
	 //free_chrdev_region(device_num, COUNT);
	 //cdev_del(&gamepad_cdev);
}

module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

