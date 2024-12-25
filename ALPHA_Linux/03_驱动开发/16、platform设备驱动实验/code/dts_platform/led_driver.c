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
#include "linux/platform_device.h"

#define DTS_PLATFORM_NUM                1
#define DTS_PLATFORM_NAME               "led"
#define LED_ON                          1
#define LED_OFF                         0


struct of_device_id led_of_match[] = {
    {.compatible = "alientek,gpio_led"},
    {/* sentinel */},
};

/* 设备结构体 */
struct dts_platform_dev {
    dev_t devid;                    // 设备号
    int major;                      // 主设备号
    int minor;                      // 次设备号
    struct cdev cdev;               // 字符设备
    struct class *class;            // 类
    struct device *device;          // 设备
    struct device_node *nd;         // 节点
    int led_gpio_index;             // GPIO编号
};
struct dts_platform_dev dts_platform;       // platform设备

static int gpio_led_open(struct inode *inode, struct file *filp) {

    int ret = 0;

    filp->private_data = &dts_platform;

    return ret;
}

static int gpio_led_close(struct inode *inode, struct file *filp) {

    int ret = 0;

    return ret;
}

static ssize_t gpio_led_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos) {

    int ret = 0;
    unsigned char data_buf[1];
    struct dts_platform_dev *dev = filp->private_data;

    ret = copy_from_user(data_buf, buf, count);
    if(ret < 0) {
        return -EINVAL;
    }

    if(data_buf[0] == LED_ON) {
        gpio_set_value(dev->led_gpio_index, 0);
    }
    else if(data_buf[0] == LED_OFF) {
        gpio_set_value(dev->led_gpio_index, 1);
    }

    return ret;
}

static const struct file_operations dts_platform_fops = {
    .owner = THIS_MODULE,
    .open       = gpio_led_open,
    .release    = gpio_led_close,
    .write      = gpio_led_write,
};

static int led_init(struct dts_platform_dev *dev) {

    int ret = 0;

    /* 获取设备节点 */
#if 1
    dev->nd = of_find_node_by_path("/gpio_led");
    /* 判断节点是否获取成功 */
    if(dev->nd == NULL) {
        printk("fail to find node\r\n");
        goto fail_find_node;
    }
#endif


    /* 获取设备编号 */
    dev->led_gpio_index = of_get_named_gpio(dev->nd, "led-gpios", 0);
    /* 判断设备编号是否获取成功 */
    if(dev->led_gpio_index < 0) {
        printk("fail to get gpio\r\n");
        goto fail_get_gpio;
    }

    /* 申请GPIO */
    ret = gpio_request(dev->led_gpio_index, "led");
    if(ret < 0) {
        printk("fail to request gpio\r\n");
        goto fail_request_gpio;
    }

    /* 初始化GPIO */
    ret = gpio_direction_output(dev->led_gpio_index, 1);
    /* 判断GPIO是否初始化成功 */
    if(ret < 0) {
        printk("fail to init gpio\r\n");
        goto fail_init_gpio;
    }

    /* 输出高电平 关闭LED */
    gpio_set_value(dev->led_gpio_index, 1);

    return 0;


fail_init_gpio:
    /* 释放GPIO */
    gpio_free(dev->led_gpio_index);
fail_request_gpio:
fail_get_gpio:
fail_find_node:
    return ret;
}


static int led_probe(struct platform_device *dev) {

    int ret = 0;

    /* 申请设备号 */
    dts_platform.major = 0;
    /* 如果给定了设备号 */
    if(dts_platform.major) {
        dts_platform.devid = MKDEV(dts_platform.major, 0);
        dts_platform.minor = MINOR(dts_platform.minor);
        ret = register_chrdev_region(dts_platform.devid, DTS_PLATFORM_NUM, DTS_PLATFORM_NAME);
    }
    /* 没有给定设备号 */
    else {
        ret = alloc_chrdev_region(&dts_platform.devid, 0, DTS_PLATFORM_NUM, DTS_PLATFORM_NAME);
        dts_platform.major = MAJOR(dts_platform.devid);
        dts_platform.minor = MINOR(dts_platform.minor);
    }
    /* 判断设备号是否申请成功 */
    if(ret < 0) {
        printk("fail to region chrdev\r\n");
        goto fail_region_chrdev;
    }
    /* 打印设备号 */
    printk("platform devid major is %d, minor is %d\r\n", dts_platform.major, dts_platform.minor);

    /* 初始化字符设备 */
    cdev_init(&dts_platform.cdev, &dts_platform_fops);
    /* 添加字符设备 */
    ret = cdev_add(&dts_platform.cdev, dts_platform.devid, DTS_PLATFORM_NUM);
    /* 判断字符设备是否添加成功 */
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }

    /* 创建类 */
    dts_platform.class = class_create(THIS_MODULE, DTS_PLATFORM_NAME);
    /* 判断类是否创建成功 */
    if(IS_ERR(dts_platform.class)) {
        ret = PTR_ERR(dts_platform.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }

    /* 创建设备 */
    dts_platform.device = device_create(dts_platform.class, NULL, dts_platform.devid, NULL, DTS_PLATFORM_NAME);
    /* 判断设备是否创建成功 */
    if(IS_ERR(dts_platform.device)) {
        ret = PTR_ERR(dts_platform.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }
#if 0   
    /* 初始化LED_GPIO */
    ret = led_init(&dts_platform);
    /* 判断LED_GPIO是否初始化成功 */
    if(ret < 0) {
        printk("fail to init led\r\n");
        goto fail_init_led;
    }
#endif

    /* 获取设备节点 */
    dts_platform.nd = dev->dev.of_node;
#if 0
    dts_platform.nd = of_find_node_by_path("/gpio_led");
    if(dts_platform.nd == NULL) {
        ret = -EINVAL;
        goto fail_find_node;
    }
#endif

    /* 获取LED所对应的GPIO */
    dts_platform.led_gpio_index = of_get_named_gpio(dts_platform.nd, "led-gpios", 0);
    if(dts_platform.led_gpio_index < 0) {
        printk("can't find led gpio\r\n");
        ret = -EINVAL;
        goto fail_find_gpio;
    }
    printk("led gpio index is %d\r\n", dts_platform.led_gpio_index);

    /* 申请IO */
    ret = gpio_request(dts_platform.led_gpio_index, "gpio_led");
    if(ret) {
        printk("fail to request the led gpio\r\n");
        ret = -EINVAL;
        goto fail_gpio_request;
    }

    /* 使用IO */
    ret = gpio_direction_output(dts_platform.led_gpio_index, 1);
    if(ret) {
        ret = -EINVAL;
        goto fail_gpio_set_output;
    }

    /* 输出低电平，点亮LED灯 */
    gpio_set_value(dts_platform.led_gpio_index, 0);

    return 0;



fail_gpio_set_output:
    /* 释放GPIO */
    gpio_free(dts_platform.led_gpio_index);
fail_gpio_request:
fail_find_gpio:
#if 0
fail_find_node:
#endif
    /* 摧毁设备 */
    device_destroy(dts_platform.class, dts_platform.devid);
fail_create_device:
    /* 摧毁类 */
    class_destroy(dts_platform.class);
fail_create_class:
    /* 删除字符设备 */
    cdev_del(&dts_platform.cdev);
fail_add_cdev:
    /* 释放设备号 */
    unregister_chrdev_region(dts_platform.devid, DTS_PLATFORM_NUM);
fail_region_chrdev:
    return ret;
}

static int led_remove(struct platform_device *dev) {

    /* 关灯 */
    gpio_set_value(dts_platform.led_gpio_index, 1);

    /* 释放GPIO */
    gpio_free(dts_platform.led_gpio_index);

    /* 释放设备号 */
    unregister_chrdev_region(dts_platform.devid, DTS_PLATFORM_NUM);

    /* 删除字符设备 */
    cdev_del(&dts_platform.cdev);

    /* 摧毁设备 */
    device_destroy(dts_platform.class, dts_platform.devid);

    /* 摧毁类 */
    class_destroy(dts_platform.class);

    return 0;
}

struct platform_driver led_driver = {
    .driver = {
        /* 无设备树和设备进行匹配 驱动名字 */
        .name = "imx6ull-led",
        /* 设备树匹配表 */  
        .of_match_table = led_of_match,
    },
    .probe = led_probe,
    .remove = led_remove,
};


/* 入口 */
static int __init led_driver_init(void) {

    return platform_driver_register(&led_driver);
}

/* 出口 */
static void __exit led_driver_exit(void) {
    platform_driver_unregister(&led_driver);
}


/* 加载驱动入口和出口 */
module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");
