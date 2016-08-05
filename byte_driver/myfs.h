    #ifndef MYFS_H  
    #define MYFS_H  
    #include "mytypes.h"  
    #include "myinode.h"  
    struct file_operations {  
        int (*open) (struct inode *);  
    };  
    #endif 
