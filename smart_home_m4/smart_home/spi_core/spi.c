#include <linux/init.h>
#include <linux/module.h>
#include "head.h"
#include "spi.h"

/*
&spi4{
    pinctrl-names = "default";
    pinctrl-0 = <&spi4_pins_a>;
    cs-gpios = <&gpioe 11 0>;
    status = "okay";

    m74hc595@0{
        compatible = "my_spi";
        reg = <0>;
        spi-max-frequency = <10000000>; //10Mhz
    };
};



*/




#define CNAME "spi"

struct device_node* node;
struct gpio_desc* desc;
struct cdev* cdev;
int res;
int devnum,major,minor;
struct class *cls;
struct device *dev;
struct spi_device *gspi;

unsigned char code[]={
    0x3f, //0
    0x06, //1
    0x5b, //2
    0x4f, //3
    0x66, //4
    0x6d, //5
    0x7d, //6
    0x07, //7
    0x7f, //8
    0x6f, //9
    0x77, //A
    0x7c, //b
    0x39, //c
    0x5e, //d
    0x79, //e
    0x71, //f 

    0x3f | 0x80, //10
    0x06 | 0x80, //11
    0x5b | 0x80, //12
    0x4f | 0x80, //13
    0x66 | 0x80, //14
    0x6d | 0x80, //15
    0x7d | 0x80, //16
    0x07 | 0x80, //17
    0x7f | 0x80, //18
    0x6f | 0x80, //19
    0x77 | 0x80, //1A
    0x7c | 0x80, //1b
    0x39 | 0x80, //1c
    0x5e | 0x80, //1d
    0x79 | 0x80, //1e
    0x71 | 0x80, //1f 

    0x00, //清零 20
};

unsigned char which[]={
    0x1,
    0x2,
    0x4,
    0x8,
};

int spi_open (struct inode *inodep, struct file *filp)
{
    printk("this is spi_open!!!\n");
    return 0;
}
int spi_release (struct inode *inodep, struct file *filp)
{
    printk("this is spi_release!!!\n");
    spi_write(gspi,&which[0],1);
    spi_write(gspi,&code[0x20],1);
    spi_write(gspi,&which[1],1);
    spi_write(gspi,&code[0x20],1);
    spi_write(gspi,&which[2],1);
    spi_write(gspi,&code[0x20],1);
    spi_write(gspi,&which[3],1);
    spi_write(gspi,&code[0x20],1);
    return 0;
}


long spi_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
    int i=0;
 //   printk("this is spi_ioctl!!!\n");
//    while(i++<800)
    {
        switch(cmd)
        {
            case IO_SPI_WHICH:
                spi_write(gspi,&which[arg],1);
                break;
            case IO_SPI_DAT:
                spi_write(gspi,&code[arg],1);
                break;
            case IO_SPI_WHICH_DAT:
                spi_write(gspi,&which[arg&0xff],1);
                spi_write(gspi,&code[arg>>8],1);
             //  unsigned long long int args=(code[arg>>8] | which[arg&0xff]<<8);
             //   spi_write(gspi,&(args),2);
                break;
        }
    }

    return 0;     
}


static const struct file_operations fops = {
	.open		= spi_open,
	.release	= spi_release,
    .unlocked_ioctl=spi_ioctl,
};

int spi_probe(struct spi_device *spi)
{
    printk("this is spi_probe!!!\n");
    //1.获取设备树节点
    node=spi->dev.of_node;
    //2.获取spi_device 
    gspi=spi;
    
    /*********************申请设备号，创建字符设备驱动节点*********************/
    //1.分配对象
    cdev=cdev_alloc();
    if(cdev == NULL)
    {
        printk("cdev_alloc error\n");
        goto ERR5;
    }
    //2.对象初始化
    cdev_init(cdev,&fops);
    //3.申请设备号
    res=alloc_chrdev_region(&devnum, 0, 1,CNAME);
    if(res)
    {
        printk("alloc_chrdev_region error\n");
        goto ERR4;
    }
    major=MAJOR(devnum);
    minor=MINOR(devnum);
    //4.字符设备驱动注册
    res=cdev_add(cdev,devnum,1);
    if(res)
    {
        printk("cdev_add error\n");
        goto ERR3;
    }
    //5.自动创建设备节点
    cls=class_create(THIS_MODULE,"spi");
    if(IS_ERR(cls))
    {
        printk("class_create error\n");
        goto ERR2;
    }
    dev=device_create(cls, NULL,devnum, NULL, CNAME);
    if(IS_ERR(dev))
    {
        printk("device_create error\n");
        goto ERR1;        
    }
    return 0;

    ERR1:
        class_destroy(cls);
    ERR2:
        cdev_del(cdev);
    ERR3:
        unregister_chrdev_region(MKDEV(major,minor),1);
    ERR4:
        kfree(cdev);
    ERR5:
        return -1;
}
int spi_remove(struct spi_device *spi)
{
    printk("this is spi_remove!!!\n");
    gpiod_set_value(desc, 0);
    gpiod_put(desc);


    device_destroy(cls,MKDEV(major,minor));
    class_destroy(cls);
    cdev_del(cdev);
    unregister_chrdev_region(MKDEV(major,minor),1);
    kfree(cdev);
    return 0;
}


struct of_device_id oftable[]={
    {
        .compatible="my_spi",
    },
    {},
};

struct spi_driver  spi_device={
    .probe=spi_probe,
    .remove=spi_remove,
    .driver={
        .name="my_spi",
        .of_match_table=oftable,
    },
};

module_spi_driver(spi_device);

MODULE_LICENSE("GPL");