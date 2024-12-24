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
#include "linux/signal.h"

#define PLATFORM_NUM                1
#define PLATFORM_NAME               "platform"


/* 设备结构体 */
struct platform_dev {
    dev_t devid;                    // 设备号
    int major;                      // 主设备号
    int minor;                      // 次设备号
    struct cdev *cdev;              // 字符设备
    struct class *class;            // 类
    struct device *device;          // 设备
};
struct platform_dev platform;       // platform设备

static const struct file_operations platform_fops = {
    .owner = THIS_MODULE,
};

/* 入口 */
static int __init platform_init(void) {
    
    int ret = 0;

    

    /* 申请设备号 */
    platform.major = 0;
    /* 如果给定了设备号 */
    if(platform.major) {
        platform.devid = MKDEV(platform.major, 0);
        platform.minor = MINOR(platform.minor);
        ret = register_chrdev_region(platform.devid, PLATFORM_NUM, PLATFORM_NAME);
    }
    /* 没有给定设备号 */
    else {
        ret = alloc_chrdev_region(&platform.devid, 0, PLATFORM_NUM, PLATFORM_NAME);
        platform.major = MAJOR(platform.devid);
        platform.minor = MINOR(platform.minor);
    }
    /* 判断设备号是否申请成功 */
    if(ret < 0) {
        printk("fail to region chrdev\r\n");
        goto fail_region_chrdev;
    }
    /* 打印设备号 */
    printk("platform devid major is %d, minor is %d\r\n", platform.major, platform.minor);

    /* 初始化字符设备 */
    cdev_init(platform.cdev, &platform_fops);
    /* 添加字符设备 */
    ret = cdev_add(platform.cdev, platform.devid, PLATFORM_NUM);
    /* 判断字符设备是否添加成功 */
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }

    /* 创建类 */
    platform.class = class_create(THIS_MODULE, PLATFORM_NAME);
    /* 判断类是否创建成功 */
    if(IS_ERR(platform.class)) {
        ret = PTR_ERR(platform.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }

    /* 创建设备 */
    platform.device = device_create(platform.class, NULL, platform.devid, NULL, PLATFORM_NAME);
    /* 判断设备是否创建成功 */
    if(IS_ERR(platform.device)) {
        ret = PTR_ERR(platform.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }

    return 0;

    /* 摧毁设备 */
    device_destroy(platform.class, platform.devid);
fail_create_device:
    /* 摧毁类 */
    class_destroy(platform.class);
fail_create_class:
    /* 删除字符设备 */
    cdev_del(platform.cdev);
fail_add_cdev:
    /* 释放设备号 */
    unregister_chrdev_region(platform.devid, PLATFORM_NUM);
fail_region_chrdev:
    return ret;
}

/* 出口 */
static void __exit platform_exit(void) {

    /* 释放设备号 */
    unregister_chrdev_region(platform.devid, PLATFORM_NUM);

    /* 删除字符设备 */
    cdev_del(platform.cdev);

    /* 摧毁设备 */
    device_destroy(platform.class, platform.devid);

    /* 摧毁类 */
    class_destroy(platform.class);
}

/* 加载驱动入口与出口 */
module_init(platform_init);
module_exit(platform_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

