<span style="font-size:18px;">#ifndef MYINODE_H  
    #define MYINODE_H  
    #include "mytypes.h"  
    #include "mycdev.h"  
    struct inode {  
        dev_t            i_rdev;  
        struct cdev        *i_cdev;  
    };  
    int insert_inode(dev_t, struct cdev *);  
    struct inode *lookup_inode(dev_t);  
    #endif</span>  
