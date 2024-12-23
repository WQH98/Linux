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

#define ASYNC_NUM           1
#define ASYNC_NAME          "async"
#define KEY_NUM             1
#define KEY_VALUE           0x01
#define INVAKEY             0xFF

struct key_dev {
    int gpio_index;                 // GPIO编号
    char name[16];                  // 名字
    int irq_num;                    // 中断编号
    irqreturn_t (*irq_handler)(int, void *);          // 中断处理函数
};


struct async_dev {
    dev_t devid;                    // 设备号
    int major;                      // 主设备号
    int minor;                      // 次设备号
    struct cdev cdev;               // 字符设备
    struct class *class;            // 类
    struct device *device;          // 设备
    struct device_node *key_nd;     // 按键设备节点
    struct key_dev key[KEY_NUM];    // 按键设备
    struct timer_list timer;        // 定时器
    atomic_t key_value;             // 按键值
    atomic_t key_release_flag;      // 按键状态
    struct fasync_struct *fasync;   // 异步变量
};
struct async_dev async;             // 定义async设备


static ssize_t async_read(struct file *filp, char __user *buf, size_t count, loff_t *lofft) {

    int ret = 0;
    unsigned char key_value;
    unsigned char key_release_flag;
    struct async_dev *dev = filp->private_data;

    key_value = atomic_read(&dev->key_value);
    key_release_flag = atomic_read(&dev->key_release_flag);

    
    if(key_release_flag) {
        /* 有效按键 */
        if(key_value | 0x80) {
            key_value &= ~0x80;
            ret = copy_to_user(buf, &key_value, sizeof(key_value));
        }
        /* 无效按键 */
        else {
            goto data_err;
        }
        atomic_set(&dev->key_release_flag, 0);
    }
    else {
        goto data_err;
    }

    return ret;

data_err:
    return -EINVAL;
}

static int async_open(struct inode *inode, struct file *filp) {

    int ret = 0;

    filp->private_data = &async;

    return ret;
}

static int async_fasync(int fd, struct file *filp, int on) {
    
    struct async_dev *dev = filp->private_data;

    return fasync_helper(fd, filp, on, &dev->fasync);
}

static int async_close(struct inode *inode, struct file *filp) {

    int ret = 0;

    async_fasync(-1, filp, 0);

    return ret;
}

static const struct file_operations async_fops = {
    .owner = THIS_MODULE,
    .read = async_read,
    .open = async_open,
    .fasync = async_fasync,
    .release = async_close,
};

/* 中断处理函数 */
static irqreturn_t key0_handler(int irq_num, void *data) {
    
    struct async_dev *dev = data;

    dev->timer.data = (unsigned long)data;
    
    /* 20ms定时 */
    mod_timer(&dev->timer, jiffies + msecs_to_jiffies(20));
    
    return IRQ_HANDLED;
}

/* 定时器处理函数 */
static void timer_func(unsigned long data) {

    int value = 0;
    struct async_dev *dev = (struct async_dev *)data;


    /* 获取GPIO值 */
    value = gpio_get_value(dev->key[0].gpio_index);

    /* 如果是按下的 */
    if(value == 0) {
        atomic_set(&dev->key_value, KEY_VALUE);
    }
    /* 如果没有按下 */
    else if(value == 1) {
        atomic_set(&dev->key_value, 0x80 | KEY_VALUE);
        atomic_set(&dev->key_release_flag, 1);
    }

    /* 有效的按键过程 */
    if(atomic_read(&dev->key_release_flag)) {
        if(dev->fasync) {
            kill_fasync(&dev->fasync, SIGIO, POLL_IN);
        }
    }
}

/* key初始化 */
static int key_init(struct async_dev *dev) {

    int ret = 0;
    int i = 0, j = 0;

    /* 获取KEY GPIO设备节点 */
    dev->key_nd = of_find_node_by_path("/key");
    /* 判断节点是否获取成功 */
    if(dev->key_nd == NULL) {
        printk("fail to find noder\r\n");
        goto fail_find_node;
    }   

    /* 循环获取GPIO编号 */
    for(i = 0; i < KEY_NUM; i++) {
        /* 获取GPIO编号 */
        dev->key[i].gpio_index = of_get_named_gpio(dev->key_nd, "key-gpios", i);
        /* 判断是否获取成功编号 */
        if(dev->key[i].gpio_index < 0) {
            printk("fail to get gpio\r\n");
            ret = -EINVAL;
            goto fail_get_gpio;
        }
        /* 打印GPIO编号 */
        printk("key%d gpio index is %d\r\n", i, dev->key[i].gpio_index);
    }

    /* 循环申请GPIO */
    for(i = 0; i < KEY_NUM; i++) {
        /* 清空缓冲区 */
        memset(dev->key[i].name, 0, sizeof(dev->key[i].name));
        /* 设置按键名字 */
        sprintf(dev->key[i].name, "key%d", i);
        /* 申请GPIO */
        ret = gpio_request(dev->key[i].gpio_index, dev->key[i].name);
        /* 判断GPIO是否申请成功 */
        if(ret < 0) {
            printk("fail to request key%d\r\n", i);
            j = i;
            goto fail_request_gpio;
        }
    }

    /* 循环设置gpio属性 */
    for(i = 0; i < KEY_NUM; i++) {
        /* 设置gpio属性 */
        ret = gpio_direction_input(dev->key[i].gpio_index);
        /* 判断属性设置是否成功 */
        if(ret < 0) {
            printk("fail to init gpio\r\n");
            goto fail_init_gpio;
        }
    }

    /* 循环获取中断号 */
    for(i = 0; i < KEY_NUM; i++) {
        /* 获取中断号 */
        dev->key[i].irq_num = irq_of_parse_and_map(dev->key_nd, i);
        /* 打印中断号 */
        printk("key%d irq num is %d\r\n", i, dev->key[i].irq_num);
    }

    /* 中断初始化 */
    dev->key[0].irq_handler = key0_handler;
    for(i = 0; i < KEY_NUM; i++) {
        ret = request_irq(dev->key[i].irq_num, dev->key[i].irq_handler, 
                          IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, dev->key[i].name, dev);
        if(ret < 0) {
            printk("fail to request irq\r\n");
            goto fail_request_irq;
        }
    }

    /* 初始化定时器 */
    init_timer(&dev->timer);
    dev->timer.function = timer_func;

    return 0;

fail_request_irq:
fail_init_gpio:
    /* 循环释放GPIO */
    for(j = 0; j < i; j++) {
        gpio_free(dev->key[j].gpio_index);
    }
fail_request_gpio:
fail_get_gpio:
fail_find_node:
    return ret;
}


/* 入口 */
static int __init async_init(void) {

    int ret = 0;

    /* 注册设备号 */
    async.major = 0;
    /* 如果给定了设备号 */
    if(async.major) {
        async.devid = MKDEV(async.major, 0);
        async.minor = MINOR(async.devid);
        ret = register_chrdev_region(async.devid, ASYNC_NUM, ASYNC_NAME);
    }
    /* 没有给定设备号 */
    else {
        ret = alloc_chrdev_region(&async.devid, 0, ASYNC_NUM, ASYNC_NAME);
        async.major = MAJOR(async.devid);
        async.minor = MINOR(async.devid);
    }
    /* 判断设备号是否申请成功 */
    if(ret < 0) {
        printk("fail to region chrdev\r\n");
        goto fail_region_chrdev;
    }
    /* 打印设备号 */
    printk("async devid major is %d, minor is %d\r\n", async.major, async.minor);

    /* 初始化字符设备 */
    cdev_init(&async.cdev, &async_fops);
    /* 添加字符设备 */
    ret = cdev_add(&async.cdev, async.devid, ASYNC_NUM);
    /* 判断字符设备是否添加成功 */
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }

    /* 创建类 */
    async.class = class_create(THIS_MODULE, ASYNC_NAME);
    /* 判断类是否创建成功 */
    if(IS_ERR(async.class)) {
        ret = PTR_ERR(async.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }

    /* 创建设备 */
    async.device = device_create(async.class, NULL, async.devid, NULL, ASYNC_NAME);
    /* 判断设备是否创建成功 */
    if(IS_ERR(async.device)) {
        ret = PTR_ERR(async.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }

    /* 按键初始化 */
    ret = key_init(&async);
    /* 判断按键初始化是否成功 */
    if(ret < 0) {
        printk("fail to init key\r\n");
        goto fail_init_key;
    }

    /* 初始化原子变量 */
    atomic_set(&async.key_value, INVAKEY);
    atomic_set(&async.key_release_flag, 0);

    return 0;


fail_init_key:
    /* 摧毁设备 */
    device_destroy(async.class, async.devid);
fail_create_device:
    /* 摧毁类 */
    class_destroy(async.class);
fail_create_class:
    /* 删除字符设备 */
    cdev_del(&async.cdev);
fail_add_cdev:
    /* 释放设备号 */
    unregister_chrdev_region(async.devid, ASYNC_NUM);
fail_region_chrdev:
    return ret;
}

/* 出口 */
static void __exit async_exit(void) {

    int i = 0;

    /* 删除定时器 */
    del_timer_sync(&async.timer);

    /* 循环释放中断 */
    for(i = 0; i < KEY_NUM; i++) {
        free_irq(async.key[i].irq_num, &async);
    }

    /* 循环释放GPIO */
    for(i = 0; i < KEY_NUM; i++) {
        gpio_free(async.key[i].gpio_index);
    }

    /* 释放设备号 */
    unregister_chrdev_region(async.devid, ASYNC_NUM);

    /* 删除字符设备 */
    cdev_del(&async.cdev);

    /* 摧毁设备 */
    device_destroy(async.class, async.devid);

    /* 摧毁类 */
    class_destroy(async.class);

}  


/* 加载驱动入口与出口 */
module_init(async_init);
module_exit(async_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

