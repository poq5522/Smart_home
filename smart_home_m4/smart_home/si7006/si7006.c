#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/i2c.h>
#include "smart_home.h"

#define CNAME "si7006"

struct i2c_client* gclient;
struct cdev *cdev = NULL;
int major = 0;
int minor = 0;
int count = 1;
struct class *cls = NULL;
struct device *dev = NULL;

int i2c_read_tmp_hum(unsigned char reg)
{
    int ret;
    unsigned char r_buf[] = { reg };
    unsigned short data;
    // 1.封装消息
    struct i2c_msg r_msg[] = {
        [0] = {
            .addr = gclient->addr,
            .flags = 0,
            .len = 1,
            .buf = r_buf,
        },
        [1] = {
            .addr = gclient->addr,
            .flags = 1,
            .len = 2,
            .buf =  (__u8 *)&data,
        },
    };
    // 2发送消息
    ret = i2c_transfer(gclient->adapter, r_msg, ARRAY_SIZE(r_msg));
    if (ret != ARRAY_SIZE(r_msg)) {
        printk("i2c read serial or firmware error\n");
        return -EAGAIN;
    }
    data = data >> 8 | data << 8;
    return data;
}

int my_open(struct inode *inode, struct file *file)
{
    printk("%s:%s__%d__\n", __FILE__, __func__, __LINE__);
    return 0;
}

long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret,data;
    switch (cmd) {
    case GET_SI7006_TMP:
        data = i2c_read_tmp_hum(TMP_ADDR);
        ret = copy_to_user((void *)arg,&data,GET_CMD_SIZE(GET_SI7006_TMP));
        if(ret){
            printk("copy data to user error\n");
            return -EIO;
        }
        break;
    case GET_SI7006_HUM:
        data = i2c_read_tmp_hum(HUM_ADDR);
        ret = copy_to_user((void *)arg,&data,GET_CMD_SIZE(GET_SI7006_HUM));
        if(ret){
            printk("copy data to user error\n");
            return -EIO;
        }
        break;
    }

    return 0;
}

int my_close(struct inode *inode, struct file *file)
{
    printk("%s:%s__%d__\n", __FILE__, __func__, __LINE__);
    return 0;
}

// 定义操作方法结构体的变量，并对成员进行初始化
struct file_operations fops = {
    .open = my_open,
    .unlocked_ioctl = my_ioctl,
    .release = my_close,
};

int si7006_probe(struct i2c_client* client, const struct i2c_device_id* id)
{
    int ret;
    dev_t devno;
    gclient = client;
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

int si7006_remove(struct i2c_client* client)
{
    printk("%s:%s__%d__\n", __FILE__, __func__, __LINE__);
    device_destroy(cls, MKDEV(major,0));
    class_destroy(cls);
    cdev_del(cdev);
    unregister_chrdev_region(MKDEV(major,minor),count);
    kfree(cdev);
    return 0;
}

const struct of_device_id oftable[] = {
    {
        .compatible = "my_i2c",
    },
    { /*end*/ }
};
MODULE_DEVICE_TABLE(of, oftable);
struct i2c_driver si7006 = {
    .probe = si7006_probe,
    .remove = si7006_remove,
    .driver = {
        .name = "heihei",
        .of_match_table = oftable,
    }
};

module_i2c_driver(si7006);
MODULE_LICENSE("GPL");