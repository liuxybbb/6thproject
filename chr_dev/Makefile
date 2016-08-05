KERNELDIR :=/lib/modules/$(shell uname -r)/build
#KERNELDIR :=/home/linux/linux-3.14-fs4412

test:
	make -C $(KERNELDIR) M=$(shell pwd) modules

clean:
	rm -rf  *.ko  *.o  *mod*  *order  Module.symvers

obj-m=chr_dev.o


