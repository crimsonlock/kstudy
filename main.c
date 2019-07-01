/*
 *  main.c - kstudy is a kernel module, which registers a virtaul charactor device,
 *  via /dev/kstudy, so user space processes can visit the routines in kernel mod, 
 *  for study x86_64/Linux.
 *
 *
 *  2019 Li Shaopeng
 */

#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/string.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<asm/uaccess.h>
#include "kstudy.h"

static struct cdev this_cdev;
static int this_major;
static struct class *this_class;
static struct device *this_device;
static char msgbuf[KSTUDY_MSG_SIZE] = { 0 };

static int device_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length,
			   loff_t * offset)
{
	unsigned long p0, p1, p2, p3, len;
	unsigned long *kbf = (unsigned long *)buffer;
	memset(msgbuf,0,KSTUDY_MSG_SIZE);

	p0 = *(kbf++);
	p1 = *(kbf++);
	p2 = *(kbf++);
	p3 = *(kbf);

	switch (p0) {
	case KSTUDY_FUNC_READ_MEM_LINEAR:
		kstudy_read_mem_linear(msgbuf, p1, p2, p3);
		break;
	case KSTUDY_FUNC_READ_REGISTER:
		kstudy_read_register(msgbuf);
		break;
	case KSTUDY_FUNC_SUPERBLOCK:
		kstudy_superblock(msgbuf,p1,p2);
		break;
	}

	len = copy_to_user(buffer, msgbuf, KSTUDY_MSG_SIZE);
	return length;

}

static ssize_t device_write(struct file *filp, const char *buff, size_t length,
			    loff_t * offset)
{
	return -EINVAL;
}

static int device_release(struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static int __init dev_init(void)
{
	int result;
	dev_t dev;

	result = alloc_chrdev_region(&dev, 0, 1, KSTUDY_DEV_NAME);
	if (result < 0) {
		printk("%s: alloc_chrdev_region failed", KSTUDY_DEV_NAME);
		return result;
	}
	this_major = MAJOR(dev);

	cdev_init(&this_cdev, &fops);
	result = cdev_add(&this_cdev, dev, 1);
	if (result < 0) {
		printk("%s: cdev_add failed", KSTUDY_DEV_NAME);
		return result;
	}

	this_class = class_create(THIS_MODULE, KSTUDY_DEV_NAME);
	this_device =
	    device_create(this_class, NULL, dev, NULL, KSTUDY_DEV_NAME);

	printk("%s: init ok\n", KSTUDY_DEV_NAME);
	printk("%s: mknod /dev/%s c %d 0\n", KSTUDY_DEV_NAME, KSTUDY_DEV_NAME, this_major);
	return 0;

}

static void __exit dev_exit(void)
{
	device_destroy(this_class, MKDEV(this_major, 0));
	class_destroy(this_class);
	cdev_del(&this_cdev);
	unregister_chrdev_region(MKDEV(this_major, 0), 1);

	printk("%s: exit ok\n", KSTUDY_DEV_NAME);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Li Shaopeng");
MODULE_DESCRIPTION("kstudy");
