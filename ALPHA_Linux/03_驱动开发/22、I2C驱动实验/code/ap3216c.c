#include "linux/module.h"
#include "linux/kernel.h"
#include "linux/init.h"
#include "linux/fs.h"
#include "linux/uaccess.h"
#include "linux/io.h"
#include "linux/cdev.h"
#include "linux/device.h"
#include "linux/of.h"
#include "linux/of_address.h"
#include "linux/of_gpio.h"
#include "linux/slab.h"
#include "linux/gpio.h"
#include "linux/atomic.h"
#include "linux/jiffies.h"
#include "linux/timer.h"
#include "linux/interrupt.h"
#include "linux/of_irq.h"
#include "linux/input.h"
#include "linux/i2c.h"
#include "ap3216c_reg.h"
#include "linux/delay.h"

#define AP3216C_NUM     1
#define AP3216C_NAME    "ap2316c"

struct ap3216c_dev {
    dev_t devid;
    int major;
    int minor;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    void *private_data;
    unsigned short ir;
    unsigned short ps;
    unsigned short als;
};
static struct ap3216c_dev ap3216cdev;

/* 返回AP3216C的N个寄存器值 */
static int ap3216c_read_regs(struct ap3216c_dev *dev, u8 reg, void *val, int len) {

    int ret = 0;

    struct i2c_msg msg[2];
    struct i2c_client *client = (struct i2c_client *)dev->private_data;

    /* msg[0]表示发送要读取的寄存器首地址 */
    /* 从机地址 也就是ap3216c地址 */
    msg[0].addr = client->addr;
    /* 表示为要发送的数据 */
    msg[0].flags = 0;
    /* 要发送的数据 也就是寄存器地址 */
    msg[0].buf = &reg;
    /* 要发送的寄存器地址长度为1 */
    msg[0].len = 1;

    /* msg[1]读取数据 */
    /* 从机地址 也就是ap3216c地址 */
    msg[1].addr = client->addr;
    /* 表示要读数据 */
    msg[1].flags = I2C_M_RD;
    /* 接收到的从机发送的数据 */
    msg[1].buf = val;
    /* 要读取的寄存器长度 */
    msg[1].len = len;


    ret = i2c_transfer(client->adapter, msg, 2);


    return ret;
}

/* 向AP3216C写N个寄存器的数据 */
static int ap3216c_write_regs(struct ap3216c_dev *dev, u8 reg, u8 *buf, u8 len) {

    int ret = 0;

    u8 b[256];
    struct i2c_msg msg;
    struct i2c_client *client = (struct i2c_client *)dev->private_data;

    /* 构建要发送的数据 也就是寄存器首地址 + 实际的数据 */
    b[0] = reg;
    memcpy(&b[1], buf, len);

    /* 从机地址，也就是AP3216C地址 */
    msg.addr = client->addr;
    /* 表示要发送的数据 */
    msg.flags = 0;
    /* 要发送的数据 也就是寄存器地址 + 实际的数据*/
    msg.buf = b;
    /* 要发送的数据长度：寄存器地址长度 + 实际的数据长度 */
    msg.len = len + 1;

    ret = i2c_transfer(client->adapter, &msg, 1);

    return ret;
}

/* 读取AP3216C一个寄存器 */
static unsigned char ap3216c_read_reg(struct ap3216c_dev *dev, u8 reg) {

    u8 data = 0;

    ap3216c_read_regs(dev, reg, &data, 1);

    return data;
}

/* 向AP3216C写一个寄存器 */
static void ap3216c_write_reg(struct ap3216c_dev *dev, u8 reg, u8 data) {

    u8 buf = 0;
    
    buf = data;
    ap3216c_write_regs(dev, reg, &buf, 1);
}

static void ap3216c_read_data(struct ap3216c_dev *dev)
{
    unsigned char buf[6];
    unsigned char i;

	/* 循环读取所有传感器数据 */
    for(i = 0; i < 6; i++)	
    {
        buf[i] = ap3216c_read_reg(dev, AP3216C_IRDATALOW + i);	
    }
	
    if(buf[0] & 0X80) 	/* IR_OF位为1,则数据无效 */
		dev->ir = 0;					
	else 				/* 读取IR传感器的数据 */
		dev->ir = ((unsigned short)buf[1] << 2) | (buf[0] & 0X03); 			
	
	dev->als = ((unsigned short)buf[3] << 8) | buf[2];	/* 读取ALS传感器的数据 */  
	
    if(buf[4] & 0x40)	/* IR_OF位为1,则数据无效 */
		dev->ps = 0;    													
	else 				/* 读取PS传感器的数据 */
		dev->ps = ((unsigned short)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F); 	
}


static int ap3216c_open(struct inode *inode, struct file *filp) {

    int ret = 0;
    unsigned char value = 0;

    filp->private_data = &ap3216cdev;

    /* 初始化AP3216C */
    ap3216c_write_reg(&ap3216cdev, AP3216C_SYSTEMCONG, 0x04);
    mdelay(50);
    ap3216c_write_reg(&ap3216cdev, AP3216C_SYSTEMCONG, 0x03);
    value = ap3216c_read_reg(&ap3216cdev, AP3216C_SYSTEMCONG);
    printk("AP3216C_SYSTEMCONG is %#x\r\n", value);

    return ret;
}

static int ap3216c_close(struct inode *inode, struct file *filp) {

    int ret = 0;

    printk("ap3216c_close\r\n");

    return ret;    
}


static ssize_t ap3216c_read(struct file *filp, char __user *buf, size_t count, loff_t *lofft) {

    int ret = 0;
    short data[6];
    struct ap3216c_dev *dev = (struct ap3216c_dev *)filp->private_data;

    /* 向应用返回ap3216c的原始数据 */
    ap3216c_read_data(dev);

    data[0] = dev->ir;
    data[1] = dev->als;
    data[2] = dev->ps;

    ret = copy_to_user(buf, data, sizeof(data));

    return ret;
}

static const struct file_operations ap3216c_fops = {
    .owner = THIS_MODULE,
    .open = ap3216c_open,
    .release = ap3216c_close,
    .read = ap3216c_read,
};

static int ap3216c_probe(struct i2c_client *client, const struct i2c_device_id *id) {

    int ret = 0;

    printk("ap3216c_probe\r\n");

    /* 搭建字符设备驱动框架 */
    ap3216cdev.major = 0;
    if(ap3216cdev.major) {
        ap3216cdev.devid = MKDEV(ap3216cdev.major, 0);
        ap3216cdev.minor = MINOR(ap3216cdev.devid);
        ret = register_chrdev_region(ap3216cdev.devid, AP3216C_NUM, AP3216C_NAME);
    }
    else {
        ret = alloc_chrdev_region(&ap3216cdev.devid, 0, AP3216C_NUM, AP3216C_NAME);
        ap3216cdev.major = MAJOR(ap3216cdev.devid);
        ap3216cdev.minor = MINOR(ap3216cdev.devid);
    }
    if(ret < 0) {
        printk("fail to register ap3216c\r\n");
        goto fail_register_ap3216c;
    }
    printk("ap3216c devid major is %d, minor is %d\r\n", ap3216cdev.major, ap3216cdev.minor);

    cdev_init(&ap3216cdev.cdev, &ap3216c_fops);
    ret = cdev_add(&ap3216cdev.cdev, ap3216cdev.devid, AP3216C_NUM);
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }

    ap3216cdev.class = class_create(THIS_MODULE, AP3216C_NAME);
    if(IS_ERR(ap3216cdev.class)) {
        ret = PTR_ERR(ap3216cdev.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }

    ap3216cdev.device = device_create(ap3216cdev.class, NULL, ap3216cdev.devid, NULL, AP3216C_NAME);
    if(IS_ERR(ap3216cdev.device)) {
        ret = PTR_ERR(ap3216cdev.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }

    ap3216cdev.private_data = client;

    return 0;

    device_destroy(ap3216cdev.class, ap3216cdev.devid);
fail_create_device:
    class_destroy(ap3216cdev.class);
fail_create_class:
    cdev_del(&ap3216cdev.cdev);
fail_add_cdev:
    unregister_chrdev_region(ap3216cdev.devid, AP3216C_NUM);
fail_register_ap3216c:
    return ret;
}

static int ap3216c_remove(struct i2c_client *client) {

    unregister_chrdev_region(ap3216cdev.devid, AP3216C_NUM);

    cdev_del(&ap3216cdev.cdev);

    device_destroy(ap3216cdev.class, ap3216cdev.devid);

    class_destroy(ap3216cdev.class);

    return 0;
}

/* 传统的匹配表 */
static struct i2c_device_id ap3216c_id[] = {
    {"alientek,ap3216c", 0},
    {}
};


/* 设备树匹配表 */
static struct of_device_id ap3216c_of_match[] = {
    {.compatible = "alientek,ap3216c"},
    {},
};


static struct i2c_driver ap3216c_driver = {

    .probe = ap3216c_probe,
    .remove = ap3216c_remove,
    .driver = {
        .name = "ap3216c",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(ap3216c_of_match),
    },
    .id_table = ap3216c_id,
};

static int __init ap3216c_init(void) {

    int ret = 0;

    ret = i2c_add_driver(&ap3216c_driver);

    return ret;
}

static void __exit ap3216c_exit(void) {

    i2c_del_driver(&ap3216c_driver);
}


module_init(ap3216c_init);
module_exit(ap3216c_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

