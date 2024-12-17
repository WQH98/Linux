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



#define TIMER_CNT       1
#define TIMER_NAME      "timer"
#define CLOSE_CMD       _IO(0xEF, 1)           // 关闭命令
#define OPEN_CMD        _IO(0xEF, 2)           // 打开命令
#define SETPERIOD_CMD   _IOW(0xEF, 3, int)     // 设置周期


struct timer_dev {
    dev_t devid;                // 设备号
    int major;                  // 主设备号
    int minor;                  // 次设备号
    struct cdev cdev;           // 字符设备
    struct class *class;        // 类
    struct device *device;      // 设备
    struct timer_list timer;    // 定时器
    struct device_node *nd;     // 设备节点
    int led_gpio;               // 设备编号
    int timer_period;           // 定时器周期
};
struct timer_dev timer;         // 定时器设备


static int timer_open(struct inode *inode, struct file *filp) {

    int ret = 0;

    filp->private_data = &timer;

    return ret;
}


static int timer_close(struct inode *inode, struct file *filp) {

    int ret = 0;


    return ret;
}


static long timer_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {

    int ret = 0;
    int value = 0;
    struct timer_dev *dev = filp->private_data;

    switch(cmd) {
        case CLOSE_CMD:
            printk("CLOSE_CMD\r\n");
            del_timer_sync(&dev->timer);
        break;
        case OPEN_CMD:
            printk("OPEN_CMD\r\n");
            mod_timer(&dev->timer, jiffies + msecs_to_jiffies(dev->timer_period));
        break;
        case SETPERIOD_CMD:
            printk("SETPERIOD_CMD\r\n");
            ret = copy_from_user(&value, (int *)arg, sizeof(int));
            if(ret < 0) {
                printk("fail to copy\r\n");
                return -EFAULT;
            }
            printk("value is %d\r\n", value);
            dev->timer_period = value;
            mod_timer(&dev->timer, jiffies + msecs_to_jiffies(dev->timer_period));
        break;
    }

    return ret;
}


static const struct file_operations timer_fops = {
    .owner = THIS_MODULE,
    .open = timer_open,
    .release = timer_close,
    .unlocked_ioctl = timer_ioctl,
};


/* 定时器处理函数 */
static void timer_func(unsigned long arg) {

    static int sta = 1;
    struct timer_dev *dev = (struct timer_dev*)arg;

    sta = !sta;
    gpio_set_value(dev->led_gpio, sta);

    /* 重新开启定时器 */
    mod_timer(&dev->timer, jiffies + msecs_to_jiffies(dev->timer_period));
}


/* 初始化LED */
static int led_init(struct timer_dev *dev) {

    int ret = 0;

    /* 获取设备节点 */
    dev->nd = of_find_node_by_path("/gpio_led");
    /* 判断设备节点是否获取成功 */
    if(dev->nd == NULL) {
        printk("fail to find node\r\n");
        ret = -EINVAL;
        goto fail_find_node;
    }
    
    /* 获取GPIO编号 */
    dev->led_gpio = of_get_named_gpio(dev->nd, "led-gpios", 0);
    /* 判断GPIO编号是否获取成功 */
    if(dev->led_gpio < 0) {
        printk("fail to get gpio\r\n");
        ret = -EINVAL;
        goto fail_get_gpio;
    }
    /* 打印设备编号 */
    printk("gpio index is %d\r\n", dev->led_gpio);

    /* 申请GPIO */
    ret = gpio_request(dev->led_gpio, "led");
    if(ret < 0) {
        printk("fail to request gpio\r\n");
        goto fail_request_gpio;
    }

    /* 初始化GPIO */
    ret = gpio_direction_output(dev->led_gpio, 1);
    if(ret < 0) {
        printk("fail to init gpio\r\n");
        goto fail_init_gpio;
    }

    return 0;

fail_init_gpio:
    gpio_free(dev->led_gpio);
fail_request_gpio:
fail_get_gpio:
fail_find_node:
    return ret;
}


/* 入口 */
static int __init timer_init(void) {

    int ret = 0;

    /* 注册字符设备驱动 */
    timer.major = 0;
    if(timer.major) {
        /* 已经给定了设备号 */
        timer.devid = MKDEV(timer.major, 0);
        ret = register_chrdev_region(timer.devid, TIMER_CNT, TIMER_NAME);
    }
    else {
        /* 没有给定设备号 */
        ret = alloc_chrdev_region(&timer.devid, 0, TIMER_CNT, TIMER_NAME);
        timer.major = MAJOR(timer.devid);
        timer.minor = MINOR(timer.devid);
    }
    /* 判断字符设备是否注册成功 */
    if(ret < 0) {
        printk("fail to region chrdev\r\n");
        goto fail_region_chrdev;
    }
    /* 打印设备号 */
    printk("timer chrdev major is %d, minor is %d\r\n", timer.major, timer.minor);

    
    /* 初始化字符设备 */
    cdev_init(&timer.cdev, &timer_fops);
    /* 添加字符设备 */
    ret = cdev_add(&timer.cdev, timer.devid, TIMER_CNT);
    /* 判断字符设备是否添加成功 */
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }


    /* 创建类 */
    timer.class = class_create(THIS_MODULE, TIMER_NAME);
    /* 判断类是否创建成功 */
    if(IS_ERR(timer.class)) {
        ret = PTR_ERR(timer.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }


    /* 创建设备 */
    timer.device = device_create(timer.class, NULL, timer.devid, NULL, TIMER_NAME);
    /* 判断设备是否创建成功 */
    if(IS_ERR(timer.device)) {
        ret = PTR_ERR(timer.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }


    /* 初始化LED */
    ret = led_init(&timer);
    if(ret < 0) {
        printk("fail to init led\r\n");
        goto fail_init_led;
    }


    /* 初始化定时器 */
    init_timer(&timer.timer);
    /* 设置定时器周期 */
    timer.timer_period = 500;
    /* 添加定时器中断函数 */
    timer.timer.function = timer_func;
    /* 设置定时器周期 */
    timer.timer.expires = jiffies + msecs_to_jiffies(timer.timer_period);
    /* 设置传递给定时器中断的值 */
    timer.timer.data = (unsigned long)&timer;
    /* 添加定时器到系统 */
    add_timer(&timer.timer);


    return 0;


fail_init_led:
    /* 摧毁设备 */
    device_destroy(timer.class, timer.devid);
fail_create_device:
    /* 摧毁类 */
    class_destroy(timer.class);
fail_create_class:
    /* 删除字符设备 */
    cdev_del(&timer.cdev);
fail_add_cdev:
    /* 释放设备号 */
    unregister_chrdev_region(timer.devid, TIMER_CNT);
fail_region_chrdev:
    return ret;
}

/* 出口 */
static void __exit timer_exit(void) {

    /* 关闭LED */
    gpio_set_value(timer.led_gpio, 1);

    /* 释放设备号 */
    unregister_chrdev_region(timer.devid, TIMER_CNT);

    /* 删除字符设备 */
    cdev_del(&timer.cdev);

    /* 摧毁设备 */
    device_destroy(timer.class, timer.devid);

    /* 摧毁类 */
    class_destroy(timer.class);

    /* 删除定时器 */
    del_timer(&timer.timer);

    /* 释放GPIO */
    gpio_free(timer.led_gpio);
}
 

/* 加载驱动入口与出口 */
module_init(timer_init);
module_exit(timer_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");
