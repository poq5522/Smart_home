#ifndef __BEE_UP_H__
#define __BEE_UP_H__

#include "/home/ubuntu/S_H/smart_home/bee_core/bee.h"
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

int bee_ctl(int fd,unsigned int cmd,void *arg);


#endif