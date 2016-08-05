#include <linux/init.h>
#include <linux/semaphore.h>
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

#define MAX 10
	pthread_t thread[2];
 	pthread_mutex_t mut;
 	int number=0, i;

// define semaphore
struct semaphore my_sem;

//flag for adding data
int flag;

static struct workqueue_struct *queue = NULL;
static struct delayed_work work;

static void workqueue_init(void);

static void work_handler(struct work_struct *data1)
{
	down(&my_sem);
	printk("just a demo for work demo \n");
	printk("B Thread data is %d \n", *data);
	queue_delayed_work(queue, &work, 1*HZ);
	up(&my_sem);
	return;
}

struct timer_list my_timer;

void my_function(unsigned long data1)
{
	down(&my_sem);
	printk("In my_function\n");
	if (flag == 1)
	{
		printk("flag == 1 \n");
		*data += 1;
		flag = 0;
	}
	printk("Thread A data is %d \n", *data);
	mod_timer(&my_timer, jiffies + 1*HZ);
	up(&my_sem);
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

	queue_delayed_work(queue, &work, 1*HZ);
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void *thread1()
 	{
	  /*A线程输入时,B线程停止,,A输入完毕时,B开始工作本线程为B线程*/
	   pthread_mutex_lock(&mut); 
       
           pthread_mutex_unlock(&mut);     
	} 
 
void *thread2() 
 	{
  	/*A线程输入时,B线程停止,A输入完毕时,B开始工作.本线程为A线程*/
	   pthread_mutex_lock(&mut); 

           pthread_mutex_unlock(&mut);        
	} 

void thread_create(void)
 	{
	        int temp;
	        memset(&thread, 0, sizeof(thread));          //comment1
	                        /*创建线程*/
 	        if((temp = pthread_create(&thread[0], NULL, thread1, NULL)) != 0)       //comment2
	                printf("线程B创建失败!\n");
	        else
	                printf("线程B被创建\n");
 	        if((temp = pthread_create(&thread[1], NULL, thread2, NULL)) != 0)  //comment3
 	                printf("线程A创建失败");
	        else
                	printf("线程A被创建\n");
	} 

  void thread_wait(void)
	{
 	        /*等待线程结束*/
 	        if(thread[0] !=0) {                   //comment4
 	                pthread_join(thread[0],NULL);
	                printf("线程1已经结束\n");
 	        }
 	        if(thread[1] !=0) {                //comment5
 	                pthread_join(thread[1],NULL);
 	                printf("线程2已经结束\n");
 	        }

///////////////////////////////////////////////////////////////////////////////////////////////////////////
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
                /*用默认属性初始化互斥锁*/
	        pthread_mutex_init(&mut,NULL);
 	  	thread_create();
 	        thread_wait();

	flag = 1;

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
	//queue_work(queue, &work);

	return;
}

int chrdev_init (void)
{
	//Initialize my_sem
	sema_init(&my_sem, 1);
	workqueue_init();
	int ret;
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
