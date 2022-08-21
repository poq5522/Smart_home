#include "/home/ubuntu/S_H/smart_home_m/smart_home/spi_core/spi_up.h"


int spi_ctl(int fd,int which,int dat)
{
    ioctl(fd,IO_SPI_WHICH_DAT,which|dat<<8);
    return 0;
}