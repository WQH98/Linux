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
#include "linux/slab.h"


#define DTS_LED_CNT     1           // 设备号数量
#define DTS_LED_NAME    "dts_led"   // 设备名
#define LED_ON          1
#define LED_OFF         0


/* 地址映射后的虚拟复制指针 */
static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *IMX6U_SW_MUX_GPIO1_IO03;
static void __iomem *IMX6U_SW_PAD_GPIO1_IO03;
static void __iomem *IMX6U_GPIO1_DR;
static void __iomem *IMX6U_GPIO1_GDIR;


/* dts_led设备结构体 */
struct dts_led_dev {
    dev_t devid;            // 设备号
    struct cdev cdev;       // 字符设备
    struct class *class;    // 类
    struct device *device;  // 设备
    int major;              // 主设备号
    int minor;              // 次设备号
    struct device_node *nd; // 设备节点
};

struct dts_led_dev dts_led;         // LED设备

static int dts_led_open(struct inode *inode, struct file *filp) {
    int ret = 0;

    filp->private_data = &dts_led;

    return ret;
}

static int dts_led_close(struct inode *inode, struct file *filp) {
    int ret = 0;

    struct dts_led_dev *dev = (struct dts_led_dev *)filp->private_data;
    if(dev->major) {

    }

    return ret;
}

/* LED打开关闭函数 */
static void led_switch(u8 sta) {
    u32 val = 0;
    if(sta == LED_ON) {
        val = readl(IMX6U_GPIO1_DR);
        val &= ~(1 << 3);
        writel(val, IMX6U_GPIO1_DR);
    }
    else if(sta == LED_OFF) {
        val = readl(IMX6U_GPIO1_DR);
        val |= (1 << 3);
        writel(val, IMX6U_GPIO1_DR);       
    }
}

static ssize_t dts_led_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos) {
    
    int ret = 0;
    int ret_value = 0;
    u8 data_buf[1];

    ret_value = copy_from_user(data_buf, buf, count);
    if(ret_value < 0) {
        printk("kernel write failed\r\n");
        return -EFAULT;
    }

    /* 判断是开灯还是关灯 */
    led_switch(data_buf[0]);

    return ret;
}

/* dts_led字符设备操作集 */
static const struct file_operations dts_led_fops = {
    .owner = THIS_MODULE,
    .open = dts_led_open,
    .release = dts_led_close,
    .write = dts_led_write,
};

/* 入口 */
static int __init dts_led_init(void) {
    
    int ret = 0;
    const char *str;
    u32 *reg_data;
    u8 i = 0;
    u32 elemsize = 0;
    u32 val = 0;

    /* 1、注册字符设备 */
    dts_led.major = 0;              // 设备号由内核分配
    /* 定义了设备号 */
    if(dts_led.major) {
        dts_led.devid = MKDEV(dts_led.major, 0);
        ret = register_chrdev_region(dts_led.devid, DTS_LED_CNT, DTS_LED_NAME);
    } else {
        /* 申请设备号 */
        ret = alloc_chrdev_region(&dts_led.devid, 0, DTS_LED_CNT, DTS_LED_NAME);
        dts_led.major = MAJOR(dts_led.devid);
        dts_led.minor = MINOR(dts_led.devid);
    }
    if(ret < 0) {
        goto fail_devid;
    }

    /* 2、添加字符设备 */
    dts_led.cdev.owner = THIS_MODULE;
    cdev_init(&dts_led.cdev, &dts_led_fops);
    ret = cdev_add(&dts_led.cdev, dts_led.devid, DTS_LED_CNT);
    if(ret < 0) {
        goto fail_cdev;
    }

    /* 3、自动创建设备节点 */
    dts_led.class = class_create(THIS_MODULE, DTS_LED_NAME);
    if(IS_ERR(dts_led.class)) {
        ret = PTR_ERR(dts_led.class);
        goto fail_class;
    }

    dts_led.device = device_create(dts_led.class, NULL, dts_led.devid, NULL, DTS_LED_NAME);
    if(IS_ERR(dts_led.device)) {
        ret = PTR_ERR(dts_led.device);
        goto fail_device;
    }

    /* 获取设备树属性内容 */
    dts_led.nd = of_find_node_by_path("/alpha_led");
    /* 失败 */
    if(dts_led.nd == NULL) {
        ret = -EINVAL;
        goto fail_findnd;
    }

    ret = of_property_read_string(dts_led.nd, "status", &str);
    if(ret < 0) {
        goto fail_rs;
    }
    else {
        printk("status is %s\r\n", str);
    }

    ret = of_property_read_string(dts_led.nd, "compatible", &str);
    if(ret < 0) {
        goto fail_rs;
    }
    else {
        printk("compatible is %s\r\n", str);
    }

    elemsize = of_property_count_elems_of_size(dts_led.nd, "reg", sizeof(u32));
    if(ret < 0) {
        ret = -EINVAL;
        goto fail_rs;
    }
    else {
        printk("reg's count is %d\r\n", elemsize);
    }

    reg_data = kmalloc(elemsize * sizeof(u32), GFP_KERNEL);
    if(!reg_data) {
        ret = -EINVAL;
        goto fail_rs;
    }

    ret = of_property_read_u32_array(dts_led.nd, "reg", reg_data, elemsize);
    if(ret < 0) {
        goto fail_read32;
    }
    else {
        printk("reg data: \r\n");
        for(i = 0; i < elemsize; i++) {
            printk("reg[%d] is %#x\r\n", i, *(reg_data + i));
        }
    }

    /* 设置地址映射 */
#if 0    
    IMX6U_CCM_CCGR1 = ioremap(reg_data[0], reg_data[1]);
    IMX6U_SW_MUX_GPIO1_IO03 = ioremap(reg_data[2], reg_data[3]);
    IMX6U_SW_PAD_GPIO1_IO03 = ioremap(reg_data[4], reg_data[5]);
    IMX6U_GPIO1_DR = ioremap(reg_data[6], reg_data[7]);
    IMX6U_GPIO1_GDIR = ioremap(reg_data[8], reg_data[9]);
#else
    IMX6U_CCM_CCGR1 = of_iomap(dts_led.nd, 0);
    IMX6U_SW_MUX_GPIO1_IO03 = of_iomap(dts_led.nd, 1);
    IMX6U_SW_PAD_GPIO1_IO03 = of_iomap(dts_led.nd, 2);
    IMX6U_GPIO1_DR = of_iomap(dts_led.nd, 3);
    IMX6U_GPIO1_GDIR = of_iomap(dts_led.nd, 4);
#endif
    /* 初始化GPIO */
    val = readl(IMX6U_CCM_CCGR1);
    val &= ~(3 << 26);      // 先清除之前设置的bit26、bit27
    val |= (3 << 26);       // bit26、bit27置1
    writel(val, IMX6U_CCM_CCGR1);

    writel(0x05, IMX6U_SW_MUX_GPIO1_IO03);          // 设置复用
    writel(0x10B0, IMX6U_SW_PAD_GPIO1_IO03);        // 设置电气属性

    val = readl(IMX6U_GPIO1_GDIR);
    val |= (1 << 3);        // bit3置1，设置为输出
    writel(val, IMX6U_GPIO1_GDIR);

    val = readl(IMX6U_GPIO1_DR);
    val |= (1 << 3);        // bit3置1，关闭LED
    writel(val, IMX6U_GPIO1_DR);

    kfree(reg_data);

    return 0;

fail_read32:
    kfree(reg_data);
fail_rs:
fail_findnd:
    device_destroy(dts_led.class, dts_led.devid);       // 摧毁设备
fail_device:
    class_destroy(dts_led.class);       // 摧毁类
fail_class:
    cdev_del(&dts_led.cdev);        //  删除字符设备
fail_cdev:
    unregister_chrdev_region(dts_led.devid, DTS_LED_CNT);       // 释放设备号
fail_devid:
    return ret;
}

/* 出口 */
static void __exit dts_led_exit(void) {

    u32 val = 0;

    /* 关闭LED */
    val = readl(IMX6U_GPIO1_DR);
    val |= (1 << 3);
    writel(val, IMX6U_GPIO1_DR);

    /*  释放内存映射 */
    iounmap(IMX6U_CCM_CCGR1);
    iounmap(IMX6U_SW_MUX_GPIO1_IO03);
    iounmap(IMX6U_SW_PAD_GPIO1_IO03);
    iounmap(IMX6U_GPIO1_DR);
    iounmap(IMX6U_GPIO1_GDIR);

    /* 删除字符设备 */
    cdev_del(&dts_led.cdev);

    /* 释放设备号 */
    unregister_chrdev_region(dts_led.devid, DTS_LED_CNT);

    /* 摧毁设备 */
    device_destroy(dts_led.class, dts_led.devid);

    /* 摧毁类 */
    class_destroy(dts_led.class);       
}


/* 加载驱动入口和出口 */
module_init(dts_led_init);
module_exit(dts_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");
