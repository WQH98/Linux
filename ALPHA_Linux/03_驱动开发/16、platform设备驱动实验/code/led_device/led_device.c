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


#define CCM_CCGR1_BASE          (0x020C406C)
#define SW_MUX_GPIO1_IO03_BASE  (0x020E0068)
#define SW_PAD_GPIO1_IO03_BASE  (0x020E02F4)
#define GPIO1_DR_BASE           (0x0209C000)
#define GPIO1_GPIR_BASE         (0x0209C004)
#define REGISTER_LENGTH         4

static void led_device_release(struct device *dev) {

    printk("led device release\r\n");
}

static struct resource led_resources[] = {
    [0] = {
        .start = CCM_CCGR1_BASE,
        .end = CCM_CCGR1_BASE + REGISTER_LENGTH - 1,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = SW_MUX_GPIO1_IO03_BASE,
        .end = SW_MUX_GPIO1_IO03_BASE + REGISTER_LENGTH - 1,
        .flags = IORESOURCE_MEM,
    },
    [2] = {
        .start = SW_PAD_GPIO1_IO03_BASE,
        .end = SW_PAD_GPIO1_IO03_BASE + REGISTER_LENGTH - 1,
        .flags = IORESOURCE_MEM,
    },
    [3] = {
        .start = GPIO1_DR_BASE,
        .end = GPIO1_DR_BASE + REGISTER_LENGTH - 1,
        .flags = IORESOURCE_MEM,
    },
    [4] = {
        .start = GPIO1_GPIR_BASE,
        .end = GPIO1_GPIR_BASE + REGISTER_LENGTH - 1,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device led_device = {
    .name = "imx6ull-led",
    .id = -1,
    .dev = {
        .release = led_device_release,
    },
    .num_resources = ARRAY_SIZE(led_resources),
    .resource = led_resources,
};


/* 入口 */
static int __init led_device_init(void) {

    /* 注册platform设备 */
    return platform_device_register(&led_device);
}

/* 出口 */
static void __exit led_device_exit(void) {

    /* 卸载platform设备 */
    platform_device_unregister(&led_device);
}

/* 模块加载入口与出口 */
module_init(led_device_init);
module_exit(led_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

