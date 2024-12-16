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

#define LED_CNT     1
#define LED_NAME    "atomic"
#define LED_ON      1
#define LED_OFF     0

static int led_open(struct inode *inode, struct file *filp);
static int led_close(struct inode *inode, struct file *filp);
static ssize_t led_write(struct file *filp, const char __user *buf, size_t count, loff_t * lofft);

/* 字符设备结构体 */
static const struct file_operations led_ops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_close,
    .write = led_write,
};

/* LED设备结构体 */
struct led_dev {
    dev_t devid;                // 设备号
    int major;                  // 主设备号
    int minor;                  // 次设备号
    struct cdev cdev;           // 字符设备
    struct class *class;        // 类
    struct device *device;      // 设备
    struct device_node *nd;     // 设备节点
    int index;                  // 设备编号
    atomic_t lock;              // 原子操作
};
struct led_dev led;             // 定义LED设备

static int led_open(struct inode *inode, struct file *filp) {
    
    int ret = 0;

    filp->private_data = &led;

    /* 判断lock */
    /* 不能使用驱动 */
    if(!atomic_dec_and_test(&led.lock)) {
        atomic_inc(&led.lock);
        return -EBUSY;
    }

#if 0
    /* 判断原子变量lock */
    if(atomic_read(&led.lock) <= 0) {
        return -EBUSY;
    }
    else {
        atomic_dec(&led.lock);
    }
#endif
    return ret;
}


static int led_close(struct inode *inode, struct file *filp) {

    int ret = 0;

    struct led_dev *dev = filp->private_data;

    /* 加一释放驱动 */
    atomic_inc(&dev->lock);

    return ret;
}

static ssize_t led_write(struct file *filp, const char __user *buf, size_t count, loff_t * lofft) {

    int ret = 0;
    u8 data_buf[1];

    ret = copy_from_user(data_buf, buf, count);
    if(ret < 0) {
        return -EINVAL;
    }

    if(data_buf[0] == LED_ON) {
        gpio_set_value(led.index, 0);
    }
    else if(data_buf[0] == LED_OFF) {
        gpio_set_value(led.index, 1);
    }

    return ret;
}

/* 入口 */
static int __init atomic_init(void) {

    int ret = 0;

    /* 初始化原子变量 */
    atomic_set(&led.lock, 1);

    /* 注册字符设备驱动 */
    led.major = 0;
    if(led.major) {
        /* 给定了主设备号 */
        led.devid = MKDEV(led.major, 0);
        ret = register_chrdev_region(led.devid, LED_CNT, LED_NAME);
    }
    else {
        /* 没有给定主设备号 */
        ret = alloc_chrdev_region(&led.devid, 0, LED_CNT, LED_NAME);
        led.major = MAJOR(led.devid);
        led.minor = MINOR(led.devid);
    }
    /* 判断字符设备驱动是否注册成功 */
    if(ret < 0) {
        printk("fail to register chrdev\r\n");
        goto faile_register_chrdev;
    }
    /* 打印设备号 */
    printk("led major is %d, minor is %d\r\n", led.major, led.minor);


    /* 初始化字符设备 */
    cdev_init(&led.cdev, &led_ops);
    /* 添加字符设备 */
    ret = cdev_add(&led.cdev, led.devid, LED_CNT);
    /* 判断字符设备是否添加成功 */
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }


    /* 初始化类 */
    led.class = class_create(THIS_MODULE, "atomic");
    /* 判断类是否初始化成功 */
    if(IS_ERR(led.class)) {
        ret = PTR_ERR(led.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }


    /* 初始化设备 */
    led.device = device_create(led.class, NULL, led.devid, NULL, LED_NAME);
    /* 判断设备是否初始化成功 */
    if(IS_ERR(led.device)) {
        ret = PTR_ERR(led.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }


    /* 获取设备节点 */
    led.nd = of_find_node_by_path("/gpio_led");
    /* 判断设备节点是否获取成功 */
    if(led.nd == NULL) {
        ret = -EINVAL;
        printk("fail to find node\r\n");
        goto fail_find_node;
    }


    /* 获取GPIO对应的编号 */
    led.index = of_get_named_gpio(led.nd, "led-gpios", 0);
    if(led.index < 0) {
        ret = -EINVAL;
        printk("fail to get index\r\n");
        goto fail_get_index;
    }
    /* 打印GPIO对应的编号 */
    printk("led gpio index is %d\r\n", led.index);


    /* 申请IO */
    ret = gpio_request(led.index, "led");
    if(ret < 0) {
        printk("fail to request gpio\r\n");
        goto fail_requests_gpio;
    }


    /* 初始化IO */
    ret = gpio_direction_output(led.index, 0);
    if(ret < 0) {
        printk("fail to init gpio\r\n");
        goto fail_init_gpio;
    }

    /* 设置IO为低电平 点亮LED */
    gpio_set_value(led.index, 0);

    return 0;

fail_init_gpio:
    /* 释放IO */
    gpio_free(led.index);
fail_requests_gpio:
fail_get_index:
fail_find_node:
    /* 摧毁设备 */
    device_destroy(led.class, led.devid);
fail_create_device:
    /* 销毁类 */
    class_destroy(led.class);
fail_create_class:
    /* 删除字符设备 */
    cdev_del(&led.cdev);
fail_add_cdev:
    /* 释放设备号 */
    unregister_chrdev_region(led.devid, LED_CNT);
faile_register_chrdev:
    return ret;
}

static void __exit atomic_exit(void) {

    /* 设置IO为高电平 关闭LED */
    gpio_set_value(led.index, 1);

    /* 释放设备号 */
    unregister_chrdev_region(led.devid, LED_CNT);
    /* 删除字符设备 */
    cdev_del(&led.cdev);
    /* 摧毁设备 */
    device_destroy(led.class, led.devid);
    /* 销毁类 */
    class_destroy(led.class);
    /* 释放IO */
    gpio_free(led.index);
}

/* 出口 */

/* 加载驱动入口与出口 */
module_init(atomic_init);
module_exit(atomic_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");
