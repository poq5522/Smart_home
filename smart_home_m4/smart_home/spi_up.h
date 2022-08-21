#ifndef __SPI_UP_H__
#define __SPI_UP_H__

#include "spi.h"
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

int spi_ctl(int fd,int which,int dat);


#endif