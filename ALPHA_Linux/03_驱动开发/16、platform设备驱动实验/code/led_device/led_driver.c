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

#define LED_OFF         0
#define LED_ON          1
#define PLATFORM_LED_NUM         1
#define PLATFORM_LED_NAME        "platform_led"

/* 地址映射后的虚拟地址指针 */
static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_GPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;

/* LED设备结构体 */
struct newchar_led_dev {
    struct cdev cdev;           // 字符设备
    dev_t devid;                // 设备号
    int major;                  // 主设备号
    int minor;                  // 次设备号
    struct class *class;        // 类
    struct device *device;      // 设备
};
struct newchar_led_dev newchar_led;


/* 打开、关闭LED */
static void led_switch(u8 sta) {
    u32 val = 0;
    if(sta == LED_ON) {
        val = readl(GPIO1_DR);
        /* bit3清零 打开LED */
        val &= ~(1 << 3);
        writel(val, GPIO1_DR);
    }
    else if(sta == LED_OFF) {
        val = readl(GPIO1_DR);
        /* bit3置1 关闭LED */
        val |= (1 << 3);
        writel(val, GPIO1_DR);
    }
}

static int newchr_led_open(struct inode *inode, struct file *filp) {
    
    filp->private_data = &newchar_led;

    return 0;
}

static int newchr_led_close(struct inode *inode, struct file *filp) {
    
    return 0;
}

static ssize_t newchr_led_write(struct file* filp, const char __user *buf, size_t count, loff_t *lofft) {
    
    int ret_value = 0;
    unsigned char data_buf[1];

    ret_value = copy_from_user(data_buf, buf, count);
    if(ret_value < 0) {
        printk("kernel write fail\r\n");
        return -EFAULT;
    }

    /* 判断是开灯还是关灯 */
    led_switch(data_buf[0]);

    return 0;
}

static const struct file_operations newchr_led_fops= {
    .owner = THIS_MODULE,
    .open = newchr_led_open,
    .release = newchr_led_close,
    .write = newchr_led_write,
};

static int led_probe(struct platform_device *dev) {

    int i = 0;
    int ret = 0;
    unsigned int val = 0;
    /* 初始化LED 字符设备驱动 */
    struct resource *led_source[5];
    for(i = 0; i < 5; i++) {
        led_source[i] = platform_get_resource(dev, IORESOURCE_MEM, i);
        if(led_source[i] == NULL) {
            return -EINVAL;
        }
    }

    // 内存长度
    // led_source[0]->end - led_source[0]->start + 1;
    /* 内存映射 */
    /* 初始化LED灯 地址映射 */
    IMX6U_CCM_CCGR1 = ioremap(led_source[0]->start, resource_size(led_source[0]));
    SW_MUX_GPIO1_IO03 = ioremap(led_source[1]->start, resource_size(led_source[1]));
    SW_PAD_GPIO1_IO03 = ioremap(led_source[2]->start, resource_size(led_source[2]));
    GPIO1_DR = ioremap(led_source[3]->start, resource_size(led_source[3]));
    GPIO1_GDIR = ioremap(led_source[4]->start, resource_size(led_source[4]));

    /* 初始化 */
    val = readl(IMX6U_CCM_CCGR1);
    /* 先清除以前的配置bit26,27 */
    val &= ~(3 << 26);
    /* bit26,27置1 */
    val |= 3 << 26;
    writel(val, IMX6U_CCM_CCGR1);

    /* 设置复用 */
    writel(0x05, SW_MUX_GPIO1_IO03);
    /* 设置电气属性 */
    writel(0x10B0, SW_PAD_GPIO1_IO03);

    val = readl(GPIO1_GDIR);
    /* bit置1，设置为输出 */
    val |= 1 << 3;
    writel(val, GPIO1_GDIR);

    val = readl(GPIO1_DR);
    val |= (1 << 3);
    writel(val, GPIO1_DR);

    /* 注册字符设备 */
    newchar_led.major = 0;
    /* 给定主设备号 */
    if(newchar_led.major) {
        newchar_led.devid = MKDEV(newchar_led.major, 0);
        ret = register_chrdev_region(newchar_led.devid, PLATFORM_LED_NUM, PLATFORM_LED_NAME);
    }
    /* 没有给定主设备号 */
    else {
        ret = alloc_chrdev_region(&newchar_led.devid, 0, PLATFORM_LED_NUM, PLATFORM_LED_NAME);
        newchar_led.major = MAJOR(newchar_led.devid);
        newchar_led.minor = MINOR(newchar_led.devid);
    }
    /* 判断是否申请成功设备号 */
    if(ret < 0) {
        printk("fail to region chrdev\r\n");
        goto fail_region_chrdev;
    }
    /* 打印设备号 */
    printk("newchar led devid major is %d, minor is %d\r\n", newchar_led.major, newchar_led.minor);

    /* 初始化字符设备 */
    cdev_init(&newchar_led.cdev, &newchr_led_fops);
    /* 添加字符设备 */
    ret = cdev_add(&newchar_led.cdev, newchar_led.devid, PLATFORM_LED_NUM);
    /* 判断字符设备是否添加成功 */
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }

    /* 创建类 */
    newchar_led.class = class_create(THIS_MODULE, PLATFORM_LED_NAME);
    /* 判断类是否创建成功 */
    if(IS_ERR(newchar_led.class)) {
        ret = PTR_ERR(newchar_led.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }

    /* 创建设备 */
    newchar_led.device = device_create(newchar_led.class, NULL, newchar_led.devid, NULL, PLATFORM_LED_NAME);
    /* 判断设备是否创建成功 */
    if(IS_ERR(newchar_led.device)) {
        ret = PTR_ERR(newchar_led.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }

    return 0;


    /* 摧毁设备 */
    device_destroy(newchar_led.class, newchar_led.devid);
fail_create_device:
    /* 摧毁类 */
    class_destroy(newchar_led.class);
fail_create_class:
    /* 删除字符设备 */
    cdev_del(&newchar_led.cdev);
fail_add_cdev:
    /* 释放设备号 */
    unregister_chrdev_region(newchar_led.devid, PLATFORM_LED_NUM);
fail_region_chrdev:
    return ret;
}


static int led_remove(struct platform_device *dev) {

    unsigned int val = 0;
    printk("newchr _led_remove\r\n");

    val = readl(GPIO1_DR);
    /* bit3置1 关闭LED灯 */
    val |= (1 << 3);
    writel(val, GPIO1_DR);

    /* 取消地址映射 */
    iounmap(IMX6U_CCM_CCGR1);
    iounmap(SW_MUX_GPIO1_IO03);
    iounmap(SW_PAD_GPIO1_IO03);
    iounmap(GPIO1_DR);
    iounmap(GPIO1_GDIR);

    /* 释放设备号 */
    unregister_chrdev_region(newchar_led.devid, PLATFORM_LED_NUM);

    /* 删除字符设备 */
    cdev_del(&newchar_led.cdev);

    /* 摧毁设备 */
    device_destroy(newchar_led.class, newchar_led.devid);

    /* 摧毁类 */
    class_destroy(newchar_led.class);

    return 0;
}

/* platform驱动结构体 */
static struct platform_driver led_driver = {
    .driver = {
        /* 驱动名字 用于和设备匹配 */
        .name = "imx6ull-led",
    },
    .probe = led_probe,
    .remove = led_remove,
};

/* 入口 */
static int __init led_driver_init(void) {

    /* 注册platform驱动 */
    return platform_driver_register(&led_driver);
}

/* 出口 */
static void __exit led_driver_exit(void) {

    /* 卸载platform驱动 */
    platform_driver_unregister(&led_driver);
}

/* 模块加载入口与出口 */
module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

