    <span style="font-size:18px;">#include "mycdev.h"  
    #include "myinode.h"  
    #include "myfs.h"  
    #define INODEMAX 255  
    struct inode inodearr[INODEMAX];  
    /*模拟VFS插入节点*/  
    int insert_inode(dev_t dev, struct cdev *cdev)  
    {  
        if(dev >= INODEMAX)  
            return -1;  
        inodearr[dev].i_cdev = cdev;  
        return 0;  
    }  
    /*模拟VFS查找节点*/  
    struct inode *lookup_inode(dev_t dev)  
    {  
        if(dev >= INODEMAX)  
            return 0;  
        return &inodearr[dev];  
    }</span>  
