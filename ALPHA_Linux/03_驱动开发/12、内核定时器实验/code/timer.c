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


#define TIMER_CNT       1
#define TIMER_NAME      "timer"


struct timer_dev {
    dev_t devid;                // 设备号
    int major;                  // 主设备号
    int minor;                  // 次设备号
};
struct timer_dev timer;         // 定时器设备

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

    

    return 0;



fail_region_chrdev:
    return ret;
}

/* 出口 */
static void __exit timer_exit(void) {

}
 

/* 加载驱动入口与出口 */
module_init(timer_init);
module_exit(timer_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");
