#include "linux/module.h"
#include "linux/kernel.h"
#include "linux/init.h"

static int __init chrdevbase_init(void) {
    printk("chrdevbase_init\r\n");
    return 0;
}

static void __exit chrdevbase_exit(void) {
    printk("chrdevbase_exit\r\n");
}


/* 模块入口与出口 */
module_init(chrdevbase_init);      // 入口
module_exit(chrdevbase_exit);      // 出口


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

