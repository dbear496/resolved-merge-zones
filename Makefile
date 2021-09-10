# templated from https://www.thegeekstuff.com/2013/07/write-linux-kernel-module/
obj-m += resolved-merge-zones.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
