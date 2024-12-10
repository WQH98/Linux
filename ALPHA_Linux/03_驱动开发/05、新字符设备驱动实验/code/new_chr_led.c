#include "linux/module.h"
#include "linux/kernel.h"
#include "linux/init.h"
#include "linux/fs.h"
#include "linux/uaccess.h"
#include "linux/io.h"
#include "linux/cdev.h"
#include "linux/device.h"

#define NEW_CHR_LED_NAME        "new_chr_led"
#define NEW_CHR_LED_COUNT       1


/* 寄存器物理地址 */
#define CCM_CCGR1_BASE          (0x020C406C)
#define SW_MUX_GPIO1_IO03_BASE  (0x020E0068)
#define SW_PAD_GPIO1_IO03_BASE  (0x020E02F4)
#define GPIO1_DR_BASE           (0x0209C000)
#define GPIO1_GDIR_BASE         (0x0209C004)

/* 地址映射后的虚拟地址指针 */
static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *IMX6U_SW_MUX_GPIO1_IO03;
static void __iomem *IMX6U_SW_PAD_GPIO1_IO03;
static void __iomem *IMX6U_GPIO1_DR;
static void __iomem *IMX6U_GPIO1_GDIR;


#define LED_OFF 0       // 关闭LED
#define LED_ON  1       // 打开LED 

/* 
    LED设备结构体
    结构体最大的作用就是为了抽象一个信息
*/

struct new_chr_led_dev {
    struct cdev cdev;       // 字符设备
    dev_t devid;            // 设备号
    struct class *class;    // 类
    struct device *device;  // 设备
    int major;              // 主设备号
    int minor;              // 次设备号
};

struct new_chr_led_dev new_chr_led;     // led设备

/* LED打开、关闭 */
static void led_switch(u8 sta) {
    u32 val = 0;

    if(sta == LED_ON) {
        val = readl(IMX6U_GPIO1_DR);
        val &= ~(1 << 3);           // bit3清零 打开LED
        writel(val, IMX6U_GPIO1_DR);
    }
    else if(sta == LED_OFF){
        val = readl(IMX6U_GPIO1_DR);
        val |= (1 << 3);           // bit3置1 关闭LED
        writel(val, IMX6U_GPIO1_DR);
    }
}

static int new_chr_led_open(struct inode *inode, struct file *filp) {

    /* 设置私有数据 */
    filp->private_data = &new_chr_led;
    return 0;
}

static int new_chr_led_close(struct inode *inode, struct file *filp) {

    /* 提取私有数据 */
    struct new_chr_led_dev *dev = (struct new_chr_led_dev *)filp->private_data;
    /* 防止编译报警 */
    if(dev->major) {
        
    }
    return 0;
}

static ssize_t new_chr_led_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos) {

    int ret = 0;
    unsigned char data_buf[1];

    ret = copy_from_user(data_buf, buf, count);
    if(ret < 0) {
        printk("kernel write failed\r\n");
        return -EFAULT;
    }

    /* 判断是开灯还是关灯 */
    led_switch(data_buf[0]);

    return 0;
}



static const struct file_operations new_chr_led_fops = {
    .owner = THIS_MODULE,
    .open = new_chr_led_open,
    .write = new_chr_led_write,
    .release = new_chr_led_close,
};

/* 入口 */
static int __init new_chr_led_init(void) {

    int ret = 0;
    int val = 0;

    printk("new chr led init\r\n");
    /* 1、初始化LED */
    IMX6U_CCM_CCGR1 = ioremap(CCM_CCGR1_BASE, 4);
    IMX6U_SW_MUX_GPIO1_IO03 = ioremap(SW_MUX_GPIO1_IO03_BASE, 4);
    IMX6U_SW_PAD_GPIO1_IO03 = ioremap(SW_PAD_GPIO1_IO03_BASE, 4);
    IMX6U_GPIO1_DR = ioremap(GPIO1_DR_BASE, 4);
    IMX6U_GPIO1_GDIR = ioremap(GPIO1_GDIR_BASE, 4);

    val = readl(IMX6U_CCM_CCGR1);
    val &= ~(3 << 26);          // 先清除以前的配置
    val |= (3 << 26);           // bit26，27置1
    writel(val, IMX6U_CCM_CCGR1);

    writel(0x05, IMX6U_SW_MUX_GPIO1_IO03);          // 设置复用
    writel(0x10B0, IMX6U_SW_PAD_GPIO1_IO03);        // 设置电气属性

    val = readl(IMX6U_GPIO1_GDIR);
    val |= (1 << 3);            // bit3置1，设置为输出
    writel(val, IMX6U_GPIO1_GDIR);

    val = readl(IMX6U_GPIO1_DR);
    val |= (1 << 3);            // bit3置1，关闭LED
    writel(val, IMX6U_GPIO1_DR);

    /* 2、注册字符设备 */
    new_chr_led.major = 0;      // 设置为0，表示由系统申请设备号

    if(new_chr_led.major) {
        // 给定主设备号
        new_chr_led.devid = MKDEV(new_chr_led.major, 0);
        ret = register_chrdev_region(new_chr_led.devid, NEW_CHR_LED_COUNT, NEW_CHR_LED_NAME);
    }
    else {
        // 没有给定主设备号
        ret = alloc_chrdev_region(&new_chr_led.devid, 0, NEW_CHR_LED_COUNT, NEW_CHR_LED_NAME);
        new_chr_led.major = MAJOR(new_chr_led.devid);
        new_chr_led.minor = MINOR(new_chr_led.devid);
    }

    if(ret < 0) {
        printk("new_chr_led chrdev region err\r\n");
        goto fail_devid;
    }

    printk("new_chr_led major = %d, minor = %d\r\n", new_chr_led.major, new_chr_led.minor);

    /* 3、注册字符设备 */
    new_chr_led.cdev.owner = THIS_MODULE;
    cdev_init(&new_chr_led.cdev, &new_chr_led_fops);
    ret = cdev_add(&new_chr_led.cdev, new_chr_led.devid, NEW_CHR_LED_COUNT);
    if(ret < 0) {
        goto fail_cdev;
    }

    /* 4、自动创建设备节点 */
    new_chr_led.class = class_create(THIS_MODULE, NEW_CHR_LED_NAME);
    if(IS_ERR(new_chr_led.class)) {
        ret = PTR_ERR(new_chr_led.class);
        goto fail_class;
    }

    /* 5、创建设备 */
    new_chr_led.device = device_create(new_chr_led.class, NULL, new_chr_led.devid, NULL, NEW_CHR_LED_NAME);
    if(IS_ERR(new_chr_led.device)) {
        ret = PTR_ERR(new_chr_led.device);
        goto fail_device;
    }

    return 0;

fail_device:
    /* 摧毁类 */
    class_destroy(new_chr_led.class);

fail_class:
    /* 删除字符设备 */
    cdev_del(&new_chr_led.cdev);
    return ret;

fail_cdev:
    /* 注销设备号 */
    unregister_chrdev_region(new_chr_led.devid, NEW_CHR_LED_COUNT);

fail_devid:
    return ret;

}

/* 出口 */
static void __exit new_chr_led_exit(void) {

    unsigned int val = 0;

    printk("new chr led exit\r\n");

    val = readl(IMX6U_GPIO1_DR);
    val |= (1 << 3);
    writel(val, IMX6U_GPIO1_DR);

    /* 1、取消地址映射 */
    iounmap(IMX6U_CCM_CCGR1);
    iounmap(IMX6U_SW_MUX_GPIO1_IO03);
    iounmap(IMX6U_SW_PAD_GPIO1_IO03);
    iounmap(IMX6U_GPIO1_GDIR);
    iounmap(IMX6U_GPIO1_DR);

    /* 2、删除字符设备 */
    cdev_del(&new_chr_led.cdev);

    /* 3、注销设备号 */
    unregister_chrdev_region(new_chr_led.devid, NEW_CHR_LED_COUNT);

    /* 4、摧毁设备 */
    device_destroy(new_chr_led.class, new_chr_led.devid);

    /* 5、摧毁类 */
    class_destroy(new_chr_led.class);
}

/* 注册和卸载模块 */
module_init(new_chr_led_init);
module_exit(new_chr_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");
