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


static int __init gamepad_init (void);
static void __exit gamepad_exit (void);
static int gamepad_open (struct inode *, struct file *);
static int gamepad_release (struct inode *, struct file *);
static ssize_t gamepad_read (struct file *, char *__user, size_t, loff_t *);
static ssize_t gamepad_write (struct file *, char *__user, size_t, loff_t *);
static irqreturn_t gpio_interrupt_handler (int, void *, struct pt_regs *);
static int gamepad_fasync (int, struct file *, int mode);
