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

#define BEEP_CNT        1
#define BEEP_NAME       "beep"
#define BEEP_ON         1
#define BEEP_OFF        0

static int beep_open(struct inode *inode, struct file *filp);
static int beep_close(struct inode *inode, struct file *filp);
static ssize_t beep_write(struct file *filp, const char *buf, size_t count, loff_t *lofft);


/* beep结构体 */
struct beep_dev {
    dev_t devid;                    // 设备号
    int major;                      // 主设备号
    int minor;                      // 次设备号
    struct cdev cdev;               // 字符设备
    struct class *class;            // 类
    struct device *device;          // 设备
    struct device_node *nd;         // 设备节点
    int index;                      // 设备编号
};
struct beep_dev beep;       // beep设备


/* 操作集 */
const struct file_operations beep_fops = {
    .owner = THIS_MODULE,
    .open = beep_open,
    .release = beep_close,
    .write = beep_write,
};

static int beep_open(struct inode *inode, struct file *filp) {

    int ret = 0;

    filp->private_data = &beep;

    return ret;
}

static int beep_close(struct inode *inode, struct file *filp) {

    int ret = 0;

    return ret;
}

static ssize_t beep_write(struct file *filp, const char *buf, size_t count, loff_t *lofft) {

    int ret = 0;
    u8 data_buf[1] = {0};
    struct beep_dev *dev = (struct beep_dev *)(filp->private_data);

    ret = copy_from_user(data_buf, buf, count);
    if(ret < 0) {
        return -EINVAL;
    }

    if(data_buf[0] == BEEP_ON) {
        gpio_set_value(dev->index, 0);
    }
    else if(data_buf[0] == BEEP_OFF) {
        gpio_set_value(dev->index, 1);
    }

    return ret;
}

static int __init beep_init(void) {

    int ret = 0;

    /* 注册字符设备驱动 */
    beep.major = 0;
    if(beep.major) {
        // 给定主设备号
        beep.devid = MKDEV(beep.major, 0);
        ret = register_chrdev_region(beep.devid, BEEP_CNT, BEEP_NAME);
    }
    else {
        // 没有给定主设备号
        ret = alloc_chrdev_region(&beep.devid, 0, BEEP_CNT, BEEP_NAME);
        beep.major = MAJOR(beep.devid);
        beep.minor = MINOR(beep.devid);
    }
    /* 判断申请设备号是否出错 */
    if(ret < 0) {
        printk("fail to allow devid\r\n");
        goto fail_allow_devid;
    }
    /* 打印设备号 */
    printk("major is %d, minor is %d\r\n", beep.major, beep.minor);

    /* 初始化字符设备 */
    beep.cdev.owner = THIS_MODULE;
    cdev_init(&beep.cdev, &beep_fops);
    ret = cdev_add(&beep.cdev, beep.devid, BEEP_CNT);
    if(ret < 0) {
        /* 添加字符设备失败 */
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }

    /* 创建类 */
    beep.class = class_create(THIS_MODULE, BEEP_NAME);
    /* 判断类创建是否成功 */
    if(IS_ERR(beep.class)) {
        ret = PTR_ERR(beep.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }

    /* 创建设备 */
    beep.device = device_create(beep.class, NULL, beep.devid, NULL, BEEP_NAME);
    /* 判断设备是否创建成功 */
    if(IS_ERR(beep.device)) {
        ret = PTR_ERR(beep.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }

    /* 获取设备节点 */
    beep.nd = of_find_node_by_path("/beep");
    /* 判断设备节点是否获取成功 */
    if(beep.nd == NULL) {
        ret = -EINVAL;
        printk("fail to find node\r\n");
        goto fail_find_node;
    }

    /* 获取设备对应的GPIO编号 */
    beep.index = of_get_named_gpio(beep.nd, "beep-gpios", 0);
    /* 判断获取编号是否成功 */
    if(beep.index < 0) {
        ret = -EINVAL;
        printk("fail to get index\r\n");
        goto fail_get_index;    
    }   
    /* 打印GPIO编号 */
    printk("beep gpio index is %d\r\n", beep.index);

    /* 申请IO */
    ret = gpio_request(beep.index, "beep");
    /* 判断申请IO是否成功 */
    if(ret < 0) {
        printk("fail to request gpio\r\n");
        goto fail_request_gpio;
    }

    /* 使用IO */
    ret = gpio_direction_output(beep.index, 0);
    if(ret < 0) {
        ret = -EINVAL;
        goto fail_output_gpio;
    }

    /* 输出低电平 打开蜂鸣器 */
    gpio_set_value(beep.index, 0);

    return 0;

fail_output_gpio:
    /* 释放IO */
    gpio_free(beep.index);
fail_request_gpio:
fail_get_index:
fail_find_node:
    /* 摧毁设备 */
    device_destroy(beep.class, beep.devid);
fail_create_device:
    /* 摧毁类 */
    class_destroy(beep.class);
fail_create_class:
    /* 删除字符设备 */
    cdev_del(&beep.cdev);
fail_add_cdev:
    /* 释放设备号 */
    unregister_chrdev_region(beep.devid, BEEP_CNT);
fail_allow_devid:

    return ret;
}

static void __exit beep_exit(void) {

    gpio_set_value(beep.index, 1);

    /* 释放设备号 */
    unregister_chrdev_region(beep.devid, BEEP_CNT);

    /* 删除字符设备 */
    cdev_del(&beep.cdev);

    /* 摧毁设备 */
    device_destroy(beep.class, beep.devid);

    /* 摧毁类 */
    class_destroy(beep.class);
    
    /* 释放IO */
    gpio_free(beep.index);
}



/* 加载驱动入口和出口 */
module_init(beep_init);
module_exit(beep_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

