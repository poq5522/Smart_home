# make ARCH=arm modname=demoA
# make ARCH=x86 modname=demoA
ifeq ($(arch),arm)
	KERNELDIR := /home/ubuntu/linux-5.10.61
	CROSS_COMPILE ?= arm-linux-gnueabihf-
else
	KERNELDIR := /lib/modules/$(shell uname -r)/build
	CROSS_COMPILE ?=
endif

modname ?=
PWD := $(shell pwd)

CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar

all:
	make -C $(KERNELDIR)  M=$(PWD)   modules
	cp *.ko   ~/nfs/rootfs
	$(CC)  -c led_up.c  -o led_up.o
	
	$(AR) -crs libled.a led_up.o
clean:
	make -C $(KERNELDIR)  M=$(PWD)   clean
	rm test

install:
	cp *.ko   ~/nfs/rootfs
	cp test  ~/nfs/rootfs

help:
	echo "make arch=arm or x86 modname=drivers file name"

obj-m := $(modname).o