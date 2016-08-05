#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/param.h>
#include <linux/jiffies.h>
#include "chr_dev.h"
#include <linux/workqueue.h>
#include <linux/delay.h>  


MODULE_LICENSE("GPL");
#define chr_MARJOR 501 
#define chr_MINOR  0
struct class *cls;
dev_t devno;
char buf_kernel[128];
char buf_add[128]="ok";
struct timer_list mytimer;//  this is mytimer
static struct task_struct *tsk;
static struct task_struct *tsk2;
int  flagg = 0;
int flag=0;
struct workqueue_struct *test_wq = NULL;//定义workqueue  
struct delayed_work test_dwq; //定义workqueue要做的delayed工作
void delay_func(struct work_struct *work);//delay函数
void handle_C(int x);

void handle_C(int x)
{
	if(1==x)
	{
		if(!test_wq)
		{
		return ;
		}
	printk("stop B thread\n");
    cancel_delayed_work(&test_dwq);
	flush_workqueue(test_wq);
	}

}
void delay_func(struct work_struct *work)  
{  
	
	printk("B thread print:%s\n",buf_kernel);
	queue_delayed_work(test_wq, &test_dwq, 1000);
} 
static void timer_function(unsigned long data)
{
	if (flag==1)
	{
	
	  printk("A thread add  \n");
	  strcat(buf_kernel,buf_add);
	  flag=0;
	  printk("start B thread \n");
	  queue_delayed_work(test_wq, &test_dwq, 0);

	}

	mod_timer(&mytimer,jiffies  + 1*HZ);
	printk("A thread print:%s\n",buf_kernel);
	return ;

}
static int thread_function(void *unused)
{
		printk("start A thread\n");
		init_timer(&mytimer);//init mytimer
		mytimer.expires=jiffies + 1*HZ;
		mytimer.function = timer_function;//timer function
		add_timer(&mytimer);//add to kernel timer
		return 0;
}

static int bthread(void * unused)
{
	printk("start B thread \n");
	queue_delayed_work(test_wq, &test_dwq, 0); 
     return 0;
}
static int open_chr(struct inode *inode, struct file *file)
{

	printk("open\n");
	return 0;
}
static  ssize_t read_chr(struct file *file, char __user *buf, size_t  size, loff_t *loff)
{
	int ret;
	if(size > 128)
		size =128;
	if(size <0)
		return -ENOMEM;
	printk("buf_kernel:%s\n",buf_kernel);
	ret = copy_to_user(buf,buf_kernel,size);
	if(ret !=0)
		return -EINVAL;
	printk("read\n");
	return 0;
}
static ssize_t write_chr(struct file *file, const char __user *buf, size_t  size, loff_t *loff)
{
	int ret;
	if(size > 128)
		size =128;
	if(size <0)
		return -ENOMEM;
	ret = copy_from_user(buf_kernel,buf,size );
	if(ret !=0)
		return -EINVAL;
	
	printk("buf_kernel:%s\n",buf_kernel);

	printk("write\n");
	return 0;
}
static int release_chr(struct inode *inode,struct file *file)
{
	printk("close\n");
	return 0;
}
static long ioctl_chr(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
	case A://creat A pthread
		tsk=kthread_run(thread_function,NULL,"mythread");
		printk("A ioctl\n");
		break;
	case B:
		tsk2=kthread_run(bthread,NULL,"b_thread");
		printk("B ioctl\n");
		break;
	case C:

		flagg= 1;
		flag=1;
		printk("C ioctl\n");
		handle_C(flagg);
		break;
	case D:
		printk("D\n");
		break;
	case E:
		printk("E\n");
		break;
	default:
		printk("cmd default\n");
		break;
	}
	return 0;
}

struct file_operations fs4412_chr_fops={
	.owner = THIS_MODULE,
	.open = open_chr,
	.read = read_chr,
	.write = write_chr,
	.release =release_chr,
	.unlocked_ioctl =ioctl_chr,
};
int chr_dev_init(void)
{
	devno = MKDEV(chr_MARJOR,chr_MINOR);
	register_chrdev(chr_MARJOR,"chr",&fs4412_chr_fops);
	cls = class_create(THIS_MODULE,"fs4412chr");
	device_create(cls,NULL,devno,NULL,"chr");
	printk("chr_dev_init ok\n");
	test_wq = create_singlethread_workqueue("test_wq"); //create quenue
		if (!test_wq) {  
		printk(KERN_ERR "No memory for workqueue\n");  
		return 1;     
	}  
	printk(KERN_INFO "Create Workqueue successful!\n");  
	INIT_DELAYED_WORK(&test_dwq, delay_func);  
	return 0;
}
void chr_dev_exit(void)
{

	if(!test_wq)
	{
		return ;
	}
	
    cancel_delayed_work(&test_dwq);
	flush_workqueue(test_wq);
	del_timer(&mytimer); 
	destroy_workqueue(test_wq);
	device_destroy(cls,devno);
	class_destroy(cls);
	unregister_chrdev(chr_MARJOR,"chr");
	
	printk("chr_dev_exit ok\n");
}
module_init(chr_dev_init);
module_exit(chr_dev_exit);
