#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/timer.h>
#include "smart_home.h"
#include "stm32mp1xx_rcc.h"

#define CNAME "motor"

volatile tim16_17_t *tim16_addr;
volatile gpio_t *gpiof_addr;

volatile rcc_t *rcc_addr;

struct cdev *cdev = NULL;

int major = 0;
int minor = 0;
int count = 1;
struct class *cls = NULL;
struct device *dev = NULL;

int my_open(struct inode *inode, struct file *file)
{
    printk("%s:%s__%d__\n", __FILE__, __func__, __LINE__);
    return 0;
}

long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
    case SET_MOTOR_ON:
		tim16_addr->BDTR |=(0x1<<15);
        break;
    case SET_MOTOR_OFF:
		tim16_addr->BDTR &=(~(0x1<<15));
        break;
    }
    return 0;
}

int my_close(struct inode *inode, struct file *file)
{
    printk("%s:%s__%d__\n", __FILE__, __func__, __LINE__);
    tim16_addr->BDTR &=(~(0x1<<15));
    return 0; 
}

// 定义操作方法结构体的变量，并对成员进行初始化
struct file_operations fops = {
    .open = my_open,
    .unlocked_ioctl = my_ioctl,
    .release = my_close,
};

static int __init motor_init(void)
{
    int ret;
    dev_t devno;
    printk("%s:%s__%d__\n", __FILE__, __func__, __LINE__);
    
    // 1. 分配对象
    cdev = cdev_alloc();
    if (cdev == NULL) {
        printk("alloc memory failed\n");
        ret = -ENOMEM;
        goto ERR1;
    }
    // 2. 初始化对象
    cdev_init(cdev, &fops);

    // 3. 申请设备号
    if (major > 0) {
        // 主设备号：哪一类 次设备号:类中的哪一个
        ret = register_chrdev_region(MKDEV(major, minor),count, CNAME);
        if (ret != 0) {
            printk("static:alloc device number failed!\n");
            goto ERR2;
        }
    } else if ( major == 0) {
        ret = alloc_chrdev_region(&devno, minor, count, CNAME);
        if (ret != 0) {
            printk("dynamic:alloc device number failed!\n");
            goto ERR2;
        }
        // 根据devno设备号，获取主设备号和次设备号
        major = MAJOR(devno);
        minor = MINOR(devno);
    }
    // 4. 注册对象
    ret = cdev_add(cdev, MKDEV(major, minor), count);
    if (ret) {
        printk("add cdev failed\n");
        goto ERR3;
    }
    // 5. 向上层提交目录的信息
    cls = class_create(THIS_MODULE, CNAME);
    if (IS_ERR(cls)) {
        printk("create class failed\n");
        ret = PTR_ERR(cls);
        goto ERR4;
    }
    // 6. 向上层提交设备的信息
    dev = device_create(cls, NULL, MKDEV(major,0),NULL,CNAME);
    if (IS_ERR(dev)) {
        printk("create device failed\n");
        ret = PTR_ERR(dev);
        goto ERR5;
    }
	// 完成物理地址到虚拟地址的映射
	gpiof_addr = ioremap(GPIOF_A, sizeof(gpio_t));
	if (gpiof_addr == NULL) {
        printk("phy addr to virl addr failed\n");
        return -ENOMEM;
    }

	tim16_addr = ioremap(TIM16_A, sizeof(tim16_17_t));
	if (tim16_addr == NULL) {
        printk("phy addr to virl addr failed\n");
        return -ENOMEM;
    }

	rcc_addr = ioremap(RCC_A, 4);
	if (rcc_addr == NULL) {
        printk("phy addr to virl addr failed\n");
        return -ENOMEM;
    }
	
	rcc_addr->MP_AHB4ENSETR |= (0x1<<5);
	rcc_addr->MP_APB2ENSETR |= (0x1<<3);

	//1.通过MODER寄存器，设置PB6为复用模式 MODER[13:12] = 10
	gpiof_addr->MODER &=(~(0x3<<12));
	gpiof_addr->MODER |=((0x2<<12));
	//2.设置AFRL寄存器，设置PB6引脚复用功能为TIM4_CH1 AFRL[27:24] = 0010
	gpiof_addr->AFRL &=(~(0xf<<24));
	gpiof_addr->AFRL |=(0x1<<24);
	
	/*********TIM4章节**********/
	//1.设置分频器，将时钟源进行分频 PSC[15:0] = 209 -1  *
	tim16_addr->PSC &=(~(0xffff));
	tim16_addr->PSC |=(0xd0);
	//2.设置自动重载计数器的值 PWM方波的频率1000HZ ARR[15:0] = 1000 *
	tim16_addr->ARR &=(~(0xffff));
	tim16_addr->ARR |=((0x3e8));
	//3.设置捕获/比较寄存器中的值，PWM方波占空比 CCR1[15:0] = 700  *
	tim16_addr->CCR1 &=(~(0xffff));
	tim16_addr->CCR1 |=((0x2bc));
	tim16_addr->CCR1 |=((0x1f4));
	//4.设置TIM4_CH1通道为PWM1模式 CCMR1[16] [6:4] = 0110 *
	tim16_addr->CCMR1 &=(~(0x1<<16));
	tim16_addr->CCMR1 &=(~(0x7<<4));
	tim16_addr->CCMR1 |=((0x6<<4));
	//5.设置TIM4_CH1通道预加载使能 CCMR1[3] = 1 *
	tim16_addr->CCMR1 |=(0x1<<3);
	//6.设置TIM4_CH1通道捕获/比较寄存器输出模式 CCMR1[1:0] = 00 *
	tim16_addr->CCMR1 &=(~(0x3));
	//7.设置TIM4_CH1通道配置为输出极性模式 CCER[3] = 0 *
	tim16_addr->CCER &=(~(0x1<<3));
	//8.设置TIM4_CH1通道起始状态为低/高电平 CCER[1] = 0 *
	tim16_addr->CCER &=(~(0x1<<1));
	//9.设置TIM4_CH1通道比较/捕获寄存器输出使能 CCER[0] = 1 *
	tim16_addr->CCER |=((0x1));
	//10.设置TIM4_CH1通道自动重载预加载使能 CR1[7] = 1 *
	tim16_addr->CR1 |=(0x1<<7);
	//11.设置TIM4_CH1通道边沿对齐方式 CR1[6:5] = 00 *
	tim16_addr->CR1 &=(~(0x3<<5));
	//12.设置TIM4_CH1通道计数方式递减方式 CR1[4] = 1 *	
	tim16_addr->CR1 |=((0x1<<4));
	//13.设置TIM4_CH1通道计数器使能 CR1[0] = 1 *
	tim16_addr->CR1 |=((0x1));

    return 0;  // 不可以省略，goto的标签写到下边
    ERR5:
        device_destroy(cls, MKDEV(major,0));
        class_destroy(cls);
    ERR4:
        cdev_del(cdev);
    ERR3:
        unregister_chrdev_region(MKDEV(major,minor),count);
    ERR2:
        kfree(cdev);
    ERR1:
        return ret;
    return 0;
}

static void __exit motor_exit(void)
{
    printk("%s:%s__%d__\n", __FILE__, __func__, __LINE__);

	iounmap(gpiof_addr);
	iounmap(tim16_addr);
	iounmap(rcc_addr);

    device_destroy(cls, MKDEV(major,minor));
    class_destroy(cls);
    cdev_del(cdev);
    unregister_chrdev_region(MKDEV(major,minor),count);
    kfree(cdev);
}

module_init(motor_init);
module_exit(motor_exit);
MODULE_LICENSE("GPL");