KERNELDIR:=/lib/modules/$(shell uname -r)/build
#KERNELDIR:=/home/linux/Desktop/three/source/linux-3.14/

test:
	make -C $(KERNELDIR) M=$(shell pwd) modules

clean:
	rm -rf *.ko *.o *mod* *order Module.symvers

obj-m=led.o
