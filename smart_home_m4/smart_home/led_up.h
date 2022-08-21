#ifndef __LED_UP_H__
#define __LED_UP_H__

#include "led.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/select.h>

int led_ctl(int fd,unsigned int cmd,void *arg);


#endif