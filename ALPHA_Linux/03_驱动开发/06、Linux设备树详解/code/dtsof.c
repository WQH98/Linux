#include "linux/module.h"
#include "linux/kernel.h"
#include "linux/init.h"
#include "linux/fs.h"
#include "linux/uaccess.h"
#include "linux/io.h"
#include "linux/cdev.h"
#include "linux/device.h"
#include "linux/of.h"
#include "linux/slab.h"

/* 模块入口 */
static int __init dtsof_init(void) {

    int ret = 0;
    struct device_node *bl_nd = NULL;       // 节点
    struct property *comp_pro = NULL;       // 属性
    const char *str_name;
    u32 def_value = 0;
    u32 *brival;
    u32 elemsize = 0;
    u8 i = 0;

    /* 1、找到backlight节点，路径是：/backlight */
    bl_nd = of_find_node_by_path("/backlight");
    /* 失败 */
    if(bl_nd == NULL) {
        ret = -EINVAL;
        goto fail_findnd;
    }

    /* 2、获取属性 */
    /* 获取字符串属性1 */
    comp_pro = of_find_property(bl_nd, "compatible", NULL);
    /* 失败 */
    if(comp_pro == NULL) {
        ret = -EINVAL;
        goto fail_findpro;
    }
    /* 成功 */
    else {
        printk("compatible is %s\r\n", (char*)comp_pro->value);
    }

    /* 获取字符串属性2 */
    ret = of_property_read_string(bl_nd, "status", &str_name);
    /* 失败 */
    if(ret < 0) {
        goto fail_rs;
    }
    /* 成功 */
    else {
        printk("status is %s\r\n", str_name);
    }

    /* 获取数字属性 */
    ret = of_property_read_u32(bl_nd, "default-brightness-level", &def_value);
    /* 失败 */
    if(ret < 0) {
        goto fail_read32;
    }
    /* 成功 */
    else {
        printk("default-brightness-level is %d\r\n", def_value);
    }

    /* 获取数组类型的属性 */
    elemsize = of_property_count_elems_of_size(bl_nd, "brightness-levels", sizeof(u32));
    /* 失败了 */
    if(elemsize < 0) {
        ret = -EINVAL;
        goto fail_readele;
    }
    /* 成功 */
    else {
        printk("brightness-levels is %d\r\n", elemsize);
    }

    /* 获取数组里面的值 */
    brival = kmalloc(elemsize * sizeof(u32), GFP_KERNEL);            // 分配内存
    /* 失败 */
    if(!brival) {
        ret = -EINVAL;
        goto fail_mem;
    }

    ret = of_property_read_u32_array(bl_nd, "brightness-levels", brival, elemsize);
    /* 失败 */
    if(ret < 0) {
        goto fail_read_u32_array;
    }
    /* 成功 */
    else {
        for(i = 0; i < elemsize; i++) {
            printk("brightness-levels[%d] is %d\r\n", i, *(brival + i));
        }
    }
    kfree(brival);          // 释放内存

    return 0;

fail_read_u32_array:
    kfree(brival);          // 释放内存
fail_mem:
fail_readele:
fail_read32:
fail_rs:
fail_findpro:
fail_findnd:
    return ret;
}
/* 模块出口 */
static void __exit dtsof_exit(void) {

    
}


/* 注册模块入口和出口 */
module_init(dtsof_init);
module_exit(dtsof_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");
