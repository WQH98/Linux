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

#define KEY_CNT      1
#define KEY_NAME     "key"
#define KEY0_VALUE   0xF0
#define INVAKEY      0x00

/* key设备结构体 */
struct key_dev {
    dev_t devid;                // 设备号
    int major;                  // 主设备号
    int minor;                  // 次设备号
    struct cdev cdev;           // 字符设备
    struct class *class;        // 类
    struct device *device;      // 设备
    struct device_node *nd;     // 设备节点
    int index;                  // 设备编号
    atomic_t keyvalue;          // 按键值
};
struct key_dev key;         // 定义key设备


/* key io初始化函数 */
static int key_gpio_init(struct key_dev *dev) {

    int ret = 0;

    /* 获取设备节点 */
    dev->nd = of_find_node_by_path("/key");
    /* 判断设备节点是否获取成功 */
    if(dev->nd == NULL) {
        ret = -EINVAL;
        printk("fail to find node\r\n");
        goto fail_find_node;
    }

    /* 获取GPIO对应的编号 */
    dev->index = of_get_named_gpio(dev->nd, "key-gpios", 0);
    if(dev->index < 0) {
        ret = -EINVAL;
        printk("fail to get index\r\n");
        goto fail_get_index;
    }
    /* 打印设备编号 */
    printk("key index is %d\r\n", dev->index);


    /* 申请IO */
    ret = gpio_request(dev->index, KEY_NAME);
    /* 判断IO是否申请成功 */
    if(ret < 0) {
        printk("fail to request gpio\r\n");
        goto fail_request_gpio;
    }


    /* 初始化GPIO */
    ret = gpio_direction_input(dev->index);
    /* 判断GPIO是否初始化成功 */
    if(ret < 0) {
        printk("fail to init gpio\r\n");
        goto fail_init_gpio;
    }

    return 0;

fail_init_gpio:
    /* 释放IO */
    gpio_free(key.index);
fail_request_gpio:
fail_get_index:
fail_find_node:
    return ret;    
}


static int key_open(struct inode *inode, struct file *filp) {

    int ret = 0;

    filp->private_data = &key;

    return ret;
}


static int key_close(struct inode *inode, struct file *filp) {

    int ret = 0;


    return ret;
}


static ssize_t key_read(struct file *filp, char __user *buf, size_t count, loff_t *lofft) {

    int ret = 0;
    struct key_dev *dev = filp->private_data;
    int value = 0;

    /* 按下了 */
    if(gpio_get_value(dev->index) == 0) {
        while(!gpio_get_value(dev->index));
        atomic_set(&dev->keyvalue, KEY0_VALUE);
    }
    /* 没按下 */
    else {
        atomic_set(&dev->keyvalue, INVAKEY);
    }

    value = atomic_read(&dev->keyvalue);

    ret = copy_to_user(buf, &value, sizeof(value));

    return ret;
}


static const struct file_operations key_ops = {
    .owner = THIS_MODULE,
    .open = key_open,
    .release = key_close,
    .read = key_read,
};


/* 入口 */
static int __init key_init(void) {

    int ret = 0;

    /* 初始化atomic */
    atomic_set(&key.keyvalue, INVAKEY);

    /* 注册字符设备驱动 */
    key.major = 0;
    if(key.major) {
        /* 给定了设备号 */
        key.devid = MKDEV(key.major, 0);
        ret = register_chrdev_region(key.devid, KEY_CNT, KEY_NAME);
    }
    else {
        /* 没有给定设备号 */
        ret = alloc_chrdev_region(&key.devid, 0, KEY_CNT, KEY_NAME);
        key.major = MAJOR(key.devid);
        key.minor = MINOR(key.devid);
    }
    /* 注册失败 */
    if(ret < 0) {
        printk("fail to register chrdev\r\n");
        goto fail_register_chrdev;
    }
    /* 打印设备号 */
    printk("major is %d, minor is %d\r\n", key.major, key.minor);


    /* 初始化字符设备 */
    cdev_init(&key.cdev, &key_ops);
    /* 添加字符设备 */
    ret = cdev_add(&key.cdev, key.devid, KEY_CNT);
    /* 判断字符设备是否添加成功 */
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }


    /* 创建类 */
    key.class = class_create(THIS_MODULE, KEY_NAME);
    if(IS_ERR(key.class)) {
        ret = PTR_ERR(key.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }


    /* 创建设备 */
    key.device = device_create(key.class, NULL, key.devid, NULL, KEY_NAME);
    /* 判断设备是否初始化成功 */
    if(IS_ERR(key.device)) {
        ret = PTR_ERR(key.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }

    ret = key_gpio_init(&key);
    if(ret < 0) {
        printk("fail to init gpio\r\n");
        goto fail_init_gpio;
    }
    

    return 0;

fail_init_gpio:
    /* 摧毁设备 */
    device_destroy(key.class, key.devid);
fail_create_device:
    /* 摧毁类 */
    class_destroy(key.class);
fail_create_class:
    /* 删除字符设备 */
    cdev_del(&key.cdev);
fail_add_cdev:
    /* 释放设备号 */
    unregister_chrdev_region(key.devid, KEY_CNT);
fail_register_chrdev:
    return ret;
}


/* 出口 */
static void __exit key_exit(void) {

    /* 释放设备号 */
    unregister_chrdev_region(key.devid, KEY_CNT);

    /* 删除字符设备 */
    cdev_del(&key.cdev);

    /* 摧毁设备 */
    device_destroy(key.class, key.devid);

    /* 摧毁类 */
    class_destroy(key.class);

    /* 释放IO */
    gpio_free(key.index);
}


/* 加载驱动入口与出口 */
module_init(key_init);
module_exit(key_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

