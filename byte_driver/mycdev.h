    #ifndef MYCDEV_H  
    #define MYCDEV_H  
    #include "myfs.h"  
    #include "mytypes.h"  
    struct cdev{  
        const struct file_operations *ops;  
        dev_t dev;  
    };  
    void cdev_init(struct cdev *, const struct file_operations *);  
    int cdev_add(struct cdev *, dev_t);  
    #endif  
