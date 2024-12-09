#include "linux/module.h"
#include "linux/kernel.h"
#include "linux/init.h"
#include "linux/fs.h"
#include "linux/uaccess.h"
#include "linux/io.h"

#define LED_MAJOR       200
#define LED_NAME        "led"

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


/* LED灯打开或关闭 */
void led_switch(u8 sta) {
    u32 val = 0;
    if(sta == LED_ON) {
        val = readl(IMX6U_GPIO1_DR);
        val &= ~(1 << 3);       // bit3清零 打开LED
        writel(val, IMX6U_GPIO1_DR);
    }
    else if(sta == LED_OFF) {
        val = readl(IMX6U_GPIO1_DR);
        val |= 1 << 3;          // bit3置1 关闭LED
        writel(val, IMX6U_GPIO1_DR);
    }
}

static int led_open(struct inode *inode, struct file *filp) {
    
    return 0;
}

static int led_close(struct inode *inode, struct file *filp) {

#if 0
    unsigned int val = 0;

    /* 关闭LED */
    val = readl(IMX6U_GPIO1_DR);
    val |= 1 << 3;              // bit3置1，关闭LED
    writel(val, IMX6U_GPIO1_DR);
#endif

    return 0;
}

static ssize_t led_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos) {

    int ret_value = 0;
    unsigned char data_buf[1];

    ret_value = copy_from_user(data_buf, buf, count);
    if(ret_value < 0) {
        printk("kerenl write failed\r\n");
        return -EFAULT;
    }

    /* 判断开灯还是关灯 */
    led_switch(data_buf[0]);

    return 0;
}

/* 字符设备操作集 */
static const struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .write = led_write,
    .open = led_open,
    .release = led_close,
};

/* 入口 */
static int __init led_init(void) {

    int ret = 0;
    unsigned int val = 0;
    /* 1、初始化LED灯 地址映射*/
    IMX6U_CCM_CCGR1 = ioremap(CCM_CCGR1_BASE, 4);
    IMX6U_SW_MUX_GPIO1_IO03 = ioremap(SW_MUX_GPIO1_IO03_BASE, 4);
    IMX6U_SW_PAD_GPIO1_IO03 = ioremap(SW_PAD_GPIO1_IO03_BASE, 4);
    IMX6U_GPIO1_DR = ioremap(GPIO1_DR_BASE, 4);
    IMX6U_GPIO1_GDIR = ioremap(GPIO1_GDIR_BASE, 4);

    /* 2、初始化 */
    val = readl(IMX6U_CCM_CCGR1);
    val &= ~(3 << 26);          // 先清除以前的配置bit26，27
    val |= 3 << 26;             // bit26，27置1
    writel(val, IMX6U_CCM_CCGR1);

    writel(0x05, IMX6U_SW_MUX_GPIO1_IO03);          // 设置复用
    writel(0x10B0, IMX6U_SW_PAD_GPIO1_IO03);        // 设置电气属性

    val = readl(IMX6U_GPIO1_GDIR);
    val |= 1 << 3;              // bit3置1，设置为输出
    writel(val, IMX6U_GPIO1_GDIR);

    val = readl(IMX6U_GPIO1_DR);
    val &= ~(1 << 3);              // bit3清零，打开LED
    writel(val, IMX6U_GPIO1_DR);

    /* 2、注册字符设备 */
    ret = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
    if(ret < 0) {
        printk("register chardev failed\r\n");

        return -EIO;
    }

    printk("led_init\r\n");
    return 0;
}

/* 出口 */
static void __exit led_exit(void) {
    
    unsigned int val = 0;

    /* 关闭LED */
    val = readl(IMX6U_GPIO1_DR);
    val |= 1 << 3;              // bit3置1，关闭LED
    writel(val, IMX6U_GPIO1_DR);

    /* 取消地址映射 */
    iounmap(IMX6U_CCM_CCGR1);
    iounmap(IMX6U_SW_MUX_GPIO1_IO03);
    iounmap(IMX6U_SW_PAD_GPIO1_IO03);
    iounmap(IMX6U_GPIO1_DR);
    iounmap(IMX6U_GPIO1_GDIR);

    /* 注销字符设备 */
    unregister_chrdev(LED_MAJOR, LED_NAME);

    printk("led_exit\r\n");
}

/* 注册驱动加载和卸载 */
module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");
