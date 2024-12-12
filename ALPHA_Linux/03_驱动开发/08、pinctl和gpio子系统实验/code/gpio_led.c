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

#define GPIO_LED_CNT        1
#define GPIO_LED_NAME       "gpio_led"
#define LED_OFF             0
#define LED_ON              1

/* gpio_led设备结构体 */
struct gpio_led_dev {
    dev_t devid;            // 设备号
    int major;              // 主设备号
    int minor;              // 次设备号
    struct cdev cdev;       // 字符设备
    struct class *class;    // 类
    struct device *device;  // 设备
    struct device_node *nd; // 节点
    int gpio_led_index;     // 编号
};

struct gpio_led_dev gpio_led;       // LED设备

static int gpio_led_open(struct inode *inode, struct file *filp) {

    int ret = 0;

    filp->private_data = &gpio_led;

    return ret;
}

static int gpio_led_close(struct inode *inode, struct file *filp) {

    int ret = 0;

    return ret;
}

static ssize_t gpio_led_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos) {

    int ret = 0;
    unsigned char data_buf[1];
    struct gpio_led_dev *dev = filp->private_data;

    ret = copy_from_user(data_buf, buf, count);
    if(ret < 0) {
        return -EINVAL;
    }

    if(data_buf[0] == LED_ON) {
        gpio_set_value(dev->gpio_led_index, 0);
    }
    else if(data_buf[0] == LED_OFF) {
        gpio_set_value(dev->gpio_led_index, 1);
    }

    return ret;
}

/* 操作集 */
static const struct file_operations gpio_led_fops = {
    .owner      = THIS_MODULE,
    .open       = gpio_led_open,
    .release    = gpio_led_close,
    .write      = gpio_led_write,
};

/* 驱动入口函数 */
static int __init gpio_led_init(void) {

    int ret = 0;

    /* 注册字符设备驱动 */
    gpio_led.major = 0;
    if(gpio_led.major) {
        // 给定主设备号
        gpio_led.devid = MKDEV(gpio_led.major, 0);
        ret = register_chrdev_region(gpio_led.devid, GPIO_LED_CNT, GPIO_LED_NAME);
    }
    else {
        // 没给定设备号
        ret = alloc_chrdev_region(&gpio_led.devid, 0, GPIO_LED_CNT, GPIO_LED_NAME);
        gpio_led.major = MAJOR(gpio_led.devid);
        gpio_led.minor = MINOR(gpio_led.devid);
    }
    if(ret < 0) {
        printk("fail to allow devid\r\n");
        goto fail_allow_devid;
    }
    printk("major is %d, minor is %d\r\n", gpio_led.major, gpio_led.minor);

    /* 初始化cdev */
    gpio_led.cdev.owner = THIS_MODULE;
    cdev_init(&gpio_led.cdev, &gpio_led_fops);
    ret = cdev_add(&gpio_led.cdev, gpio_led.devid, GPIO_LED_CNT);
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }

    /* 创建类 */
    gpio_led.class = class_create(THIS_MODULE, GPIO_LED_NAME);
    if(IS_ERR(gpio_led.class)) {
        ret = PTR_ERR(gpio_led.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }

    /* 创建设备 */
    gpio_led.device = device_create(gpio_led.class, NULL, gpio_led.devid, NULL, GPIO_LED_NAME);
    if(IS_ERR(gpio_led.device)) {
        ret = PTR_ERR(gpio_led.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }

    /* 获取设备节点 */
    gpio_led.nd = of_find_node_by_path("/gpio_led");
    if(gpio_led.nd == NULL) {
        ret = -EINVAL;
        goto fail_find_node;
    }

    /* 获取LED所对应的GPIO */
    gpio_led.gpio_led_index = of_get_named_gpio(gpio_led.nd, "led-gpios", 0);
    if(gpio_led.gpio_led_index < 0) {
        printk("can't find led gpio\r\n");
        ret = -EINVAL;
        goto fail_find_gpio;
    }
    printk("led gpio index is %d\r\n", gpio_led.gpio_led_index);

    /* 申请IO */
    ret = gpio_request(gpio_led.gpio_led_index, "gpio_led");
    if(ret) {
        printk("fail to request the led gpio\r\n");
        ret = -EINVAL;
        goto fail_gpio_request;
    }

    /* 使用IO */
    ret = gpio_direction_output(gpio_led.gpio_led_index, 1);
    if(ret) {
        ret = -EINVAL;
        goto fail_gpio_set_output;
    }

    /* 输出低电平，点亮LED灯 */
    gpio_set_value(gpio_led.gpio_led_index, 0);


    return 0;


fail_gpio_set_output:
    gpio_free(gpio_led.gpio_led_index);     // 释放IO
fail_gpio_request:
fail_find_gpio:
fail_find_node:
    device_destroy(gpio_led.class, gpio_led.devid);     // 摧毁设备
fail_create_device:
    class_destroy(gpio_led.class);      // 摧毁类
fail_create_class:
    cdev_del(&gpio_led.cdev);       // 删除字符设备
fail_add_cdev:
    unregister_chrdev_region(gpio_led.devid, GPIO_LED_CNT);     // 释放设备号
fail_allow_devid:
    
    
    return ret;
}

/* 驱动出口函数 */
static void __exit gpio_led_exit(void) {

    /* 关灯 */
    gpio_set_value(gpio_led.gpio_led_index, 1);

    /* 删除字符设备 */
    cdev_del(&gpio_led.cdev);

    /* 释放设备号 */
    unregister_chrdev_region(gpio_led.devid, GPIO_LED_CNT);

    /* 摧毁设备 */
    device_destroy(gpio_led.class, gpio_led.devid);

    /* 摧毁类 */
    class_destroy(gpio_led.class);

    /* 释放IO */
    gpio_free(gpio_led.gpio_led_index);
}

/* 驱动加载卸载函数 */
module_init(gpio_led_init);
module_exit(gpio_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

