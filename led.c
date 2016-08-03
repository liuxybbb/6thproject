#include <linux/init.h>
#include <linux/workqueue.h>
#include<linux/timer.h> // for timer_list API
#include<linux/param.h> // for HZ 
#include<linux/jiffies.h> // for jiffies
#include <linux/slab.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kthread.h>
#include "head.h"
MODULE_LICENSE("GPL");
#define MAJOR1 292
#define MINOR1 0

typedef unsigned int u32;
struct class *cls;
struct inode *i;
int * data;

static struct workqueue_struct *queue = NULL;
static struct delayed_work work;

static void work_handler(struct work_struct *data1)
{
	printk("just a demo for work demo \n");
	printk("B Thread data is %d \n", *data);
	queue_delayed_work(queue, &work, 1*HZ);
	return;
}

struct timer_list my_timer;

void my_function(unsigned long data1)
{
	printk("In my_function\n");
	printk("Thread A data is %d \n", *data);
	mod_timer(&my_timer, jiffies + 1*HZ);
	return;
}

static int athread(void * unused)
{
	init_timer(&my_timer);

	my_timer.expires = jiffies + 1*HZ; //延时delay_sec秒
	my_timer.data = 0; //给定时器处理函数传入的参数
	my_timer.function = my_function; //定时器超时时调用的函数

	/*最后,激活定时器*/
	add_timer(&my_timer);
	/*如果需要改变已经激活的定时器的超时时间,可以调用mod_timer()函数,mod_timer()函数也可以操作已经初始化,但没有激活的定时器.如果调用时定时器未被激活,则函数返回0,否则返回1*/
	printk("In a thread \n");

	return 0;
}


static int bthread(void * unused)
{
	printk("In b thread \n");

	return 0;
}

ssize_t chrdev_write (struct file *file, const char __user *buf, size_t size, loff_t *loff)
{
	return 0;
}

static int chrdev_open (struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t chrdev_read (struct file *file, char __user *buf, size_t size, loff_t *loff)
{
	return 0;
}

long chrdev_ioctl (struct file *file , unsigned int cmd, unsigned long arg)
{
	switch(cmd) {
	case A:
	// A thread 
	kthread_run(athread, NULL, "a_thread");
		break;
	case B:
	//B thread
	kthread_run(bthread, NULL, "b_thread");
		break;
	case C:


		break;
	default:
		break;
	}

	return 0;
}


dev_t device_no;

struct cdev cdev;
struct file_operations chrdev_fops = {
	.owner = THIS_MODULE,
	.open = chrdev_open,
	.read = chrdev_read,
	.write = chrdev_write,
	.unlocked_ioctl = chrdev_ioctl,
};

// workqueue_init
//
static void workqueue_init(void)
{
	queue = create_singlethread_workqueue("workqueue demo");
	if (!queue)
		return ;
	printk("init work queue demo \n");
	INIT_DELAYED_WORK(&work, work_handler);
	queue_delayed_work(queue, &work, 1*HZ);
	//queue_work(queue, &work);

	return;
}

int chrdev_init (void)
{
	int ret;
	workqueue_init();
	// apply for the device number
	device_no =	MKDEV(MAJOR1, MINOR1);
	ret = register_chrdev_region(device_no, 1, "chrdev_led");
	if (0 != ret) {
		alloc_chrdev_region(&device_no, 0, 1, "chrdev_led");
	}
	//initialize the cdev structure
	cdev_init(&cdev, &chrdev_fops);

	// cdev is added into the system
	cdev_add(&cdev, device_no, 1);
	
	cls = class_create(THIS_MODULE, "16021");
	device_create(cls, NULL, device_no+0, NULL, "led0");

	//kmalloc
	data = kmalloc(sizeof(int), GFP_KERNEL);		
	*data = 0;


	printk("Initial value of data is %d \n", *data);
	printk("chardev_init \n");


	return ret;
}
	
void chrdev_exit(void)
{
	int ret;
	ret = cancel_delayed_work(&work);
	flush_workqueue(queue);

	//destroy_workqueue(queue);
	del_timer(&my_timer);
	destroy_workqueue(queue);
	device_destroy(cls, device_no+0);

	class_destroy(cls);
	cdev_del(&cdev); // probably free  cdev structure

	unregister_chrdev_region(device_no, 1); // give back the device number 

	printk("chrdev_exit \n");
}

module_init(chrdev_init);
module_exit(chrdev_exit);
