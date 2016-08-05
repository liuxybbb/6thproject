
   <span style="font-size:18px;">#include <stdio.h>  
    #include "mycdev.h"  
    #include "myinode.h"  
    #include "myfs.h"  
    /*模拟驱动代码open函数*/  
    int demo_open(struct inode *inode)  
    {  
        printf("demo_open---\n");  
        return 0;  
    }  
    /*模拟操作方法集对象初始化*/  
    struct file_operations fops = {  
        .open    = demo_open,  
    };  
    /*模拟设备号*/  
    #define DEVNUM 254  
    /*模拟字符设备对象*/  
    struct cdev cdev;  
    /*模拟模块初始化函数*/  
    int init_module(void)  
    {  
        dev_t dev = DEVNUM;  
        cdev_init(&cdev, &fops);  
        cdev_add(&cdev, dev);  
        return 0;  
    }  
    /*************上面是模拟的驱动代码****************/  
    /*模拟系统调用*/  
    int myopen(dev_t dev)  
    {  
        struct inode *inode = lookup_inode(dev);  
        if(0 != inode)  
            if(inode->i_cdev->ops->open)  
                return inode->i_cdev->ops->open(inode);  
        return -1;  
    }  
    int main()  
    {  
        /*模拟模块加载*/  
        init_module();  
        /*模拟应用程序调用*/  
        myopen(DEVNUM);  
    }</span>  nt cdev_add(struct cdev *p, dev_t dev)  
    {  
        p->dev = dev;  
        return insert_inode(dev, p);  
    }</span> ndef MYTYPES_H  
