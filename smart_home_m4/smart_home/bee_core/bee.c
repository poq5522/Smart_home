#include <linux/init.h>
#include <linux/module.h>
#include "head.h"
#include "bee.h"

/*
	smart_home_platform{
		compatible = "myplatform";
		interrupt-parent = <&gpiof>;
		interrupts =<9 0>;
		reg=<0x12345678 0x1E>;
		bee1=<&gpioe 10 0>;
		keys=<&gpiof 9 0>,<&gpiof 7 0>,<&gpiof 8 0>;
	
	};

*/




#define CNAME "bee"

struct device_node* node;
struct gpio_desc* desc;
struct cdev* cdev;
int res;
int devnum,major,minor;
struct class *cls;
struct device *dev;
struct timer_list mytimer;
int flag=0;

void bee_blink(struct timer_list *timer)
{
    //处理函数
    
    if(flag==1)
    {
        gpiod_set_value(desc,!gpiod_get_value(desc));
        mod_timer(&mytimer,jiffies+1);
    }
    else
    {
        gpiod_set_value(desc,0);
    }

}

int bee_open (struct inode *inodep, struct file *filp)
{
    printk("this is bee_open!!!\n");
    return 0;
}
int bee_release (struct inode *inodep, struct file *filp)
{
    printk("this is bee_release!!!\n");
    flag=0;
    return 0;
}
ssize_t bee_read (struct file *filp, char __user *ubuf, size_t size, loff_t *offset)
{
    printk("this is bee_read!!!\n");
    return 0;   
}
ssize_t bee_write (struct file *filp, const char __user *ubuf, size_t size, loff_t *offset)
{
    printk("this is bee_write!!!\n");
    return 0;   
}
long bee_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
    printk("this is bee_ioctl!!!\n");
    switch(cmd)
    {
        case IO_BEE_ON:

            flag=1;
            mod_timer(&mytimer,jiffies+1);
            break;
        case IO_BEE_OFF:

            flag=0;
            break;
    }
    return 0;     
}


static const struct file_operations fops = {
	.owner		= THIS_MODULE,
	.open		= bee_open,
	.release	= bee_release,
	.read		= bee_read,
	.write		= bee_write,
    .unlocked_ioctl=bee_ioctl,
};

int bee_probe(struct platform_device *pdev)
{
    printk("this is bee_probe!!!\n");

    /**************定时器******************/
    mytimer.expires = jiffies+1;  //100HZ
    timer_setup(&mytimer, &bee_blink, 0);
    add_timer(&mytimer);
    //1.获取设备树节点
    node=pdev->dev.of_node;
    //2.获取gpio_desc*
    desc=gpiod_get_from_of_node(node,"bee", 0,GPIOD_OUT_LOW,CNAME);
    
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
    cls=class_create(THIS_MODULE,"BEE");
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
int bee_remove(struct platform_device *pdev)
{
    printk("this is bee_remove!!!\n");
    gpiod_set_value(desc, 0);
    gpiod_put(desc);


    device_destroy(cls,MKDEV(major,minor));
    class_destroy(cls);
    cdev_del(cdev);
    unregister_chrdev_region(MKDEV(major,minor),1);
    kfree(cdev);
    del_timer(&mytimer);
    return 0;
}


struct of_device_id oftable[]={
    {
        .compatible="my_bee",
    },
    {},
};

struct platform_driver bee_device={
    .probe=bee_probe,
    .remove=bee_remove,
    .driver={
        .name="my_bee",
        .of_match_table=oftable,
    },
};

module_platform_driver(bee_device);

MODULE_LICENSE("GPL");