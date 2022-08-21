#include "/home/ubuntu/S_H/smart_home_m/smart_home/bee_core/bee_up.h"


int bee_ctl(int fd,unsigned int cmd,void *arg)
{
    ioctl(fd,cmd,arg);
    return 0;
}