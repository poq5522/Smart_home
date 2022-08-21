# make ARCH=arm modname=demoA
# make ARCH=x86 modname=demoA
ifeq ($(arch),arm)
	CROSS_COMPILE ?= arm-linux-gnueabihf-
else
	CROSS_COMPILE ?=
endif
CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar
all:
#	make -C ./led_core 
#	$(CC) main.c -L./led_core -lled -o main
#	cp main  ~/nfs/rootfs
#	$(CC) main.c -L./led_core -lled -L./bee_core -lbee -L./m74hc595_core -lm74hc595 -o main

	cp ./led_core/led_up.c ./
	cp ./led_core/led_up.h ./
	cp ./led_core/led.h ./
	cp ./bee_core/bee_up.c ./
	cp ./bee_core/bee_up.h ./
	cp ./bee_core/bee.h ./
	cp ./spi_core/spi_up.c ./
	cp ./spi_core/spi_up.h ./
	cp ./spi_core/spi.h ./

#	$(CC) main.c led_up.c  bee_up.c -o main
	$(CC) *.c -o text -pthread

	cp text  ~/nfs/rootfs
	cp run.sh ~/nfs/rootfs
led:
	make -C ./led_core 
	$(CC) test.c -L./led_core -lled -o test

bee:
	make -C ./bee_core 
	$(CC) test.c -L./bee_core -lbee -o test

spi:
	make -C ./spi_core 
	$(CC) test.c -L./spi_core -lspi -o test

fan_m:
	make -C ./fan

i2c:
	make -C ./si7006

motor_m:
	make -C ./motor

clean:
	make -C $(KERNELDIR)  M=$(PWD)   clean
	rm test

install:
	cp *.ko   ~/nfs/rootfs
	cp test  ~/nfs/rootfs

help:
	echo "make (led or bee) arch=arm or x86 modname=drivers file name"

