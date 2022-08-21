#include "/home/ubuntu/S_H/smart_home_m/smart_home/led_core/led_up.h"


int led_ctl(int fd,unsigned int cmd,void *arg)
{
    ioctl(fd,cmd,arg);
    return 0;
}