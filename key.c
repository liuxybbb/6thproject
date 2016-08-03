
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");


#define  KEY_MARJOR  290
#define  KEY_MINOR   0


struct  class  *  cls;
dev_t  devno;
struct  platform_device  *mydev;
int  key;
wait_queue_head_t   key_readq;
int  flag  = 0;
struct timer_list  mytimer;


static ssize_t fs4412_key_read (struct file *file, char __user *buf, size_t  size, loff_t *loff)
{

	wait_event_interruptible(key_readq,flag  != 0);
	printk("fs4412_key_read  \n");
	if(copy_to_user(buf,&key,size))
		return  -EINVAL;


	flag  = 0;

	return  size;

}
static int fs4412_key_open (struct inode *inode, struct file *file)
{

	printk("fs4412_key_open  \n");
	return  0;
}
struct  file_operations    fs4412_led_fops  = {

	.owner  = THIS_MODULE,
	.open  = fs4412_key_open,
	.read  =  fs4412_key_read,

};
irqreturn_t  key_handler(int  irqnum,void  *  dev)
{


	mytimer.data  =  irqnum;
	/*设置延时时间*/
	mod_timer(&mytimer,jiffies +48);



	return  IRQ_HANDLED;


}

void timer_function(unsigned  long  data)
{

	if(mydev->resource[0].start ==  mytimer.data)
	{
	
		key  = 1;
	
	}

	if(mydev->resource[1].start ==  mytimer.data)
	{
	
		key  = 2;
	
	}

	printk("timer_function  \n");
	
	flag  = 1;
	wake_up_interruptible(&key_readq);


}
static int fs4412_key_probe(struct platform_device *pdev)
{

#if  1

	int ret  =-ENOMEM;
	mydev  = pdev;
	devno  = MKDEV(KEY_MARJOR,KEY_MINOR);

	if(register_chrdev(KEY_MARJOR,"key",&fs4412_led_fops))
		goto  err1;


	ret  = request_irq(pdev->resource[0].start,key_handler,IRQF_TRIGGER_FALLING|IRQF_DISABLED,"key1",NULL);
    if(0  != ret)
	{
	
		printk("request_irq  resource  0  fail  \n");
		goto  err2;
	}

	ret  = request_irq(pdev->resource[1].start,key_handler,IRQF_TRIGGER_FALLING|IRQF_DISABLED,"key2",NULL);
    if(0  != ret)
	{
	
		printk("request_irq  resource  1  fail  \n");
		goto  err3;
	}

	init_waitqueue_head(&key_readq);
	init_timer(&mytimer);
	mytimer.function  = timer_function;
	add_timer(&mytimer);
	cls  =  class_create(THIS_MODULE,"fskey");
	device_create(cls,NULL,devno,NULL,"key");
#endif
	printk("match  ok\n");
	return  0;
err3:
	free_irq(pdev->resource[0].start,NULL);
err2:
	unregister_chrdev(KEY_MARJOR,"key");

#if  1
err1:
	return  ret;
#endif
}
static int fs4412_key_remove(struct platform_device *pdev)
{



	device_destroy(cls,devno);
	class_destroy(cls);
	del_timer(&mytimer);
	free_irq(pdev->resource[0].start,NULL);
	free_irq(pdev->resource[1].start,NULL);
	unregister_chrdev(KEY_MARJOR,"key");

	printk("fs4412_key_remove  \n");
	return   0;
}

struct  of_device_id  fs4412_key_table[]  =  {

	{
	
		.compatible  = "fs4412,key",
	
	},


};
struct  platform_driver   prv  = {


	.driver  = {
	
		.name  = "fs4412-key",
		.of_match_table  =fs4412_key_table ,
	
	},
	.probe   =  fs4412_key_probe,
	.remove  =  fs4412_key_remove,


}; 

int  fs4412_key_init(void)
{

	platform_driver_register(&prv);
	printk("fs4412_key_init  \n");
	return   0;
}

void  fs4412_key_exit(void)
{

	platform_driver_unregister(&prv);
	printk("fs4412_key_exit  \n");
}

module_init(fs4412_key_init);
module_exit(fs4412_key_exit);



