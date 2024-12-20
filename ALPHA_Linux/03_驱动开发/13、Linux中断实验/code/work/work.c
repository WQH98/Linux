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


#define IMX6U_IRQ_NUM           1
#define IMX6U_IRQ_NAME          "work"
#define KEY_NUM                 1
#define KEY0_VALUE              0x01
#define INVAKEY                 0xFF

struct irq_key_dev {
    int gpio_index;                 // GPIO编号
    int irq_num;                    // 中断号
    unsigned char value;            // 按键值
    char name[10];                  // 名字
    irqreturn_t (*handler) (int, void *);     // 中断处理函数
};

struct imx6u_irq_dev {
    dev_t devid;                    // 设备号
    int major;                      // 主设备号
    int minor;                      // 次设备号
    struct cdev cdev;               // 字符设备
    struct class *class;            // 类
    struct device *device;          // 设备
    struct device_node *nd;         // 设备节点
    struct irq_key_dev irq_key[KEY_NUM];  // 按键设备
    struct timer_list timer;        // 定时器
    atomic_t key_value;             // 按键值
    atomic_t key_release_flag;      // 释放按键标志
    struct work_struct work;        // work
};
struct imx6u_irq_dev imx6u_irq;     // 定义imx6u_irq设备

static int imx6u_irq_open(struct inode *inode, struct file *filp) {

    int ret = 0;

    filp->private_data = &imx6u_irq;

    return ret;
}

static ssize_t imx6u_irq_read(struct file *filp, char __user *buf, size_t count, loff_t *lofft) {
    
    int ret = 0;
    unsigned char key_value;
    unsigned char key_release_flag;
    struct imx6u_irq_dev *dev = filp->private_data;

    key_value = atomic_read(&dev->key_value);
    
    key_release_flag = atomic_read(&dev->key_release_flag);

    /* 有效按键 */
    if(key_release_flag) {
        if(key_value & 0x80) {
            key_value &= ~0x80;
            ret = copy_to_user(buf, &key_value, sizeof(key_value));
        }
        /* 不是有效按键 */
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

static const struct file_operations imx6u_irq_ops = {
    .owner = THIS_MODULE,
    .open = imx6u_irq_open,
    .read = imx6u_irq_read,
};


/* 按键中断处理函数 */
static irqreturn_t key0_handler(int irq_num, void *dev_id) {

    struct imx6u_irq_dev *dev = dev_id;

    schedule_work(&dev->work);

    return IRQ_HANDLED;
}


/* 定时器中断处理函数 */
static void timer_func(unsigned long arg) {
    int value = 0;
    struct imx6u_irq_dev *dev = (struct imx6u_irq_dev *)arg;

    /* 获取GPIO值 */
    value = gpio_get_value(dev->irq_key[0].gpio_index);
    /* 按下 */
    if(value == 0) {
        atomic_set(&dev->key_value, dev->irq_key[0].value);
    }
    /* 释放 */
    else if(value == 1) {
        atomic_set(&dev->key_value, 0x80 | (dev->irq_key[0].value));
        atomic_set(&dev->key_release_flag, 1);
    }
}

/* work */
static void key_work(struct work_struct *work) {

#if 0
    imx6u_irq.timer.data = (unsigned long)&imx6u_irq;
    /* 20ms定时 */
    mod_timer(&imx6u_irq.timer, jiffies + msecs_to_jiffies(20));
#endif

    struct imx6u_irq_dev *dev = container_of(work, struct imx6u_irq_dev, work);
    dev->timer.data = (unsigned long)dev;
    /* 定时20ms */
    mod_timer(&dev->timer, jiffies + msecs_to_jiffies(20));
}

/* 按键初始化 */
static int keyio_init(struct imx6u_irq_dev *dev) {
    
    int ret = 0;
    int i = 0;
    /* 按键初始化 */
    /* 获取设备节点 */
    dev->nd = of_find_node_by_path("/key");
    /* 判断设备节点是否获取成功 */
    if(dev->nd == NULL) {
        printk("fail to find node\r\n");
        ret = -EINVAL;
        goto fail_find_node;
    }

    /* 循环获取IO编号 */
    for(i = 0; i < KEY_NUM; i++) {
        /* 获取GPIO编号 */
        dev->irq_key[i].gpio_index = of_get_named_gpio(dev->nd, "key-gpios", i);
        /* 判断GPIO编号是否获取成功 */
        if(dev->irq_key[i].gpio_index < 0) {
            ret = -EINVAL;
            printk("fail to get gpio\r\n");
            goto fail_get_gpio;
        }
        /* 打印编号 */
        printk("key%d gpio index is %d\r\n", i, dev->irq_key[i].gpio_index);
    }


    /* 循环申请GPIO */
    for(i = 0; i < KEY_NUM; i++) {
        /* 清空按键名字 */
        memset(dev->irq_key[i].name, 0, sizeof(dev->irq_key->name));
        /* 设置按键名字 */
        sprintf(dev->irq_key[i].name, "key%d", i);
        /* 申请GPIO */
        ret = gpio_request(dev->irq_key[i].gpio_index, dev->irq_key[i].name);
        /* 判断GPIO是否申请成功 */
        if(ret < 0) {
            printk("fail to request gpio\r\n");
            goto fail_request_gpio;
        }
        /* 初始化GPIO */
        ret = gpio_direction_input(dev->irq_key[i].gpio_index);
        /* 判断GPIO是否初始化成功 */
        if(ret < 0) {
            printk("fail to init gpio\r\n");
            goto fail_init_gpio;
        }
        /* 获取中断号 */
        // dev->irq_key[i].irq_num = gpio_to_irq(dev->irq_key[i].gpio_index);
        dev->irq_key[i].irq_num = irq_of_parse_and_map(dev->nd, i);
        /* 打印中断号 */
        printk("key%d irq num is %d\r\n", i, dev->irq_key[i].irq_num);
    }

    /* 按键中断初始化 */
    dev->irq_key[0].handler = key0_handler;
    dev->irq_key[0].value = KEY0_VALUE;
    for(i = 0; i < KEY_NUM; i++) {
        ret = request_irq(dev->irq_key[i].irq_num, dev->irq_key[i].handler,
                          IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, dev->irq_key[i].name,
                          &imx6u_irq);
        if(ret) {
            printk("fail to request irq%d\r\n", i);
            goto fail_request_irq;
        }
        
    }

    INIT_WORK(&dev->work, key_work);

    /* 初始化定时器 */
    init_timer(&dev->timer);
    dev->timer.function = timer_func;
    

    return 0;

fail_request_irq:
fail_init_gpio:
    /* 释放GPIO */
    for(i = 0; i < KEY_NUM; i++) {
        gpio_free(dev->irq_key[i].gpio_index);
    }
fail_request_gpio:
fail_get_gpio:
fail_find_node:
    return ret;
}


/* 入口 */
static int __init imx6u_irq_init(void) {
    
    int ret = 0;

    /* 申请设备号 */
    imx6u_irq.major = 0;
    /* 给定了设备号 */
    if(imx6u_irq.major) {
        imx6u_irq.devid = MKDEV(imx6u_irq.major, 0);
        ret = register_chrdev_region(imx6u_irq.devid, IMX6U_IRQ_NUM, IMX6U_IRQ_NAME);
    }
    /* 没有给定设备号 */
    else {
        ret = alloc_chrdev_region(&imx6u_irq.devid, 0, IMX6U_IRQ_NUM, IMX6U_IRQ_NAME);
        imx6u_irq.major = MAJOR(imx6u_irq.devid);
        imx6u_irq.minor = MINOR(imx6u_irq.devid);
    }
    /* 判断设备号是否注册成功 */
    if(ret < 0) {
        printk("fail to register chrdev\r\n");
        goto fail_register_chrdev;
    }
    /* 注册成功 打印设备号 */
    printk("major is %d, minor is %d\r\n", imx6u_irq.major, imx6u_irq.minor);


    /* 初始化字符设备 */
    cdev_init(&imx6u_irq.cdev, &imx6u_irq_ops);
    /* 添加字符设备 */
    ret = cdev_add(&imx6u_irq.cdev, imx6u_irq.devid, IMX6U_IRQ_NUM);
    /* 判断字符设备是否添加成功 */
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }

    /* 创建类 */
    imx6u_irq.class = class_create(THIS_MODULE, IMX6U_IRQ_NAME);
    /* 判断类是否创建成功 */
    if(IS_ERR(imx6u_irq.class)) {
        ret = PTR_ERR(imx6u_irq.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }

    
    /* 创建设备 */
    imx6u_irq.device = device_create(imx6u_irq.class, NULL, imx6u_irq.devid, NULL, IMX6U_IRQ_NAME);
    /* 判断设备是否创建成功 */
    if(IS_ERR(imx6u_irq.device)) {
        ret = PTR_ERR(imx6u_irq.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }


    /* 初始化key GPIO */
    ret = keyio_init(&imx6u_irq);
    /* 判断是否初始化成功 */
    if(ret < 0) {
        printk("fail to init key gpio\r\n");
        goto fail_init_key_gpio;
    }


    /* 初始化原子变量 */
    atomic_set(&imx6u_irq.key_value, INVAKEY);
    atomic_set(&imx6u_irq.key_release_flag, 0);

    return 0;

fail_init_key_gpio:
    /* 摧毁设备 */
    device_destroy(imx6u_irq.class, imx6u_irq.devid);
fail_create_device:
    /* 摧毁类 */
    class_destroy(imx6u_irq.class);
fail_create_class:
    /* 删除字符设备 */
    cdev_del(&imx6u_irq.cdev);
fail_add_cdev:
    /* 释放设备号 */
    unregister_chrdev_region(imx6u_irq.devid, IMX6U_IRQ_NUM);
fail_register_chrdev:
    return ret;
}


/* 出口 */
static void __exit imx6u_irq_exit(void) {
    
    int i = 0;

    /* 释放中断 */
    for(i = 0; i < KEY_NUM; i++) {
        free_irq(imx6u_irq.irq_key[i].irq_num, &imx6u_irq);
    }

    /* 释放GPIO */
    for(i = 0; i < KEY_NUM; i++) {
        gpio_free(imx6u_irq.irq_key[i].gpio_index);
    }

    /* 释放设备号 */
    unregister_chrdev_region(imx6u_irq.devid, IMX6U_IRQ_NUM);

    /* 删除字符设备 */
    cdev_del(&imx6u_irq.cdev);

    /* 摧毁设备 */
    device_destroy(imx6u_irq.class, imx6u_irq.devid);

    /* 摧毁类 */
    class_destroy(imx6u_irq.class);

    /* 删除定时器 */
    del_timer_sync(&imx6u_irq.timer);
}


/* 加载驱动入口与出口 */
module_init(imx6u_irq_init);
module_exit(imx6u_irq_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WqngQinghao");
