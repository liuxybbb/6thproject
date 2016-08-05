
   <span style="font-size:18px;">#include <string.h>  
    #include "mycdev.h"  
    #include "myinode.h"  
    #include "myfs.h"  
    /*模拟字符设备初始化*/  
    void cdev_init(struct cdev *cdev, const struct file_operations *fops)  
    {  
        memset(cdev, 0, sizeof *cdev);  
        cdev->ops = fops;  
    }  
    /*模拟字符设备注册*/  
    int cdev_add(struct cdev *p, dev_t dev)  
    {  
        p->dev = dev;  
        return insert_inode(dev, p);  
    }</span>  nt cdev_add(struct cdev *p, dev_t dev)  
    {  
        p->dev = dev;  
        return insert_inode(dev, p);  
    }</span> ndef MYTYPES_H  
