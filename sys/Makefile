KERNELDLR :=/lib/modules/$(shell  uname -r)/build

test:
	$(MAKE) -C  $(KERNELDLR) M=$(shell pwd)

clean:
	$(MAKE) -C  $(KERNELDLR) M=$(shell pwd) clean

obj-m=sys.o
