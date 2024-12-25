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
#include "linux/miscdevice.h"

#define MISC_BEEP_NAME      "misc_beep"
#define MISC_BEEP_MINOR     144

#define BEEP_ON             1
#define BEEP_OFF            0

/* platform 匹配表结构体 */
static const struct of_device_id beep_of_match[] = {
    {.compatible = "alientek,beep"},
    {/* sentinel */},
};

struct misc_beep_dev {
    int beep_gpio;              // beep gpio编号
    struct device_node *nd;     // 设备节点
};
struct misc_beep_dev misc_beep;  

static int misc_beep_open(struct inode *inode, struct file *filp) {

    filp->private_data = &misc_beep;

    return 0;
}

static int misc_beep_close(struct inode *inode, struct file *filp) {

    return 0;
}

static ssize_t misc_beep_write(struct file *filp, const char __user *buf, size_t count, loff_t *lofft) {

    int ret = 0;
    unsigned char data_buf[1];
    struct misc_beep_dev *dev = filp->private_data;

    ret = copy_from_user(data_buf, buf, count);
    if(ret < 0) {
        return -EINVAL;
    }

    if(data_buf[0] == BEEP_ON) {
        gpio_set_value(dev->beep_gpio, 0);
    }
    else if(data_buf[0] == BEEP_OFF) {
        gpio_set_value(dev->beep_gpio, 1);
    }

    return 0;
}

/* 字符设备操作集 */
static const struct file_operations misc_beep_fops = {
    .owner = THIS_MODULE,
    .open = misc_beep_open,
    .release = misc_beep_close,
    .write = misc_beep_write,
};

static struct miscdevice beep_misc_dev = {
    .minor = MISC_BEEP_MINOR,
    .name = MISC_BEEP_NAME,
    .fops = &misc_beep_fops,
};

static int misc_beep_probe(struct platform_device *dev) {

    int ret = 0;

    /* 初始化蜂鸣器IO */
    misc_beep.nd = dev->dev.of_node;
    misc_beep.beep_gpio = of_get_named_gpio(misc_beep.nd, "beep-gpios", 0);
    if(misc_beep.beep_gpio < 0) {
        ret = -EINVAL;
        printk("fail to get gpio\r\n");
        goto fail_get_gpio;
    }
    printk("misc_beep gpio is %d\r\n", misc_beep.beep_gpio);

    ret = gpio_request(misc_beep.beep_gpio, "beep-gpio");
    if(ret < 0) {
        printk("fail to request gpio\r\n");
        goto fail_request_gpio;
    }

    /* 配置为输出 默认输出高电平 */
    ret = gpio_direction_output(misc_beep.beep_gpio, 1);
    if(ret < 0) {
        printk("fail to init gpio\r\n");
        goto fail_init_gpio;
    }
    
    /* misc驱动注册 */
    ret = misc_register(&beep_misc_dev);
    if(ret < 0) {
        printk("fail to register misc\r\n");
        goto fail_register_misc;
    }

    return 0;

fail_register_misc:
fail_init_gpio:
    gpio_free(misc_beep.beep_gpio);
fail_request_gpio:
fail_get_gpio:
    return ret;
}

static int misc_beep_remove(struct platform_device *dev) {

    int ret = 0;

    gpio_set_value(misc_beep.beep_gpio, 1);

    gpio_free(misc_beep.beep_gpio);

    misc_deregister(&beep_misc_dev);

    return ret;
}

/* platform结构体 */
static struct platform_driver misc_deep_driver = {
    .driver = {
        .name = "imx6ull-beep",
        /* 设备树匹配表 */
        .of_match_table = beep_of_match,
    },
    .probe = misc_beep_probe,
    .remove = misc_beep_remove,
};

/* 驱动入口函数 */
static int __init misc_beep_init(void) {

    return platform_driver_register(&misc_deep_driver);
}

/* 驱动出口函数 */
static void __exit misc_beep_exit(void) {

    platform_driver_unregister(&misc_deep_driver);
}


/* 加载驱动入口和出口函数 */
module_init(misc_beep_init);
module_exit(misc_beep_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");
