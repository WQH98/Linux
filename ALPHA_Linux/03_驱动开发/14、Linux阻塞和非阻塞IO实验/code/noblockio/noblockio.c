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
#include "linux/ide.h"
#include "linux/poll.h"


#define NOBLOCKIO_NUM           1
#define NOBLOCKIO_NAME          "noblockio"
#define KEY_NUM                 1
#define KEY0_VALUE              0x01
#define INVAKEY                 0xFF


/* 按键设备结构体 */
struct key_dev {
    int gpio_index;             // 设备编号
    char name[16];              // 名字
    unsigned char value;        // 按键值
    irqreturn_t (*handler) (int, void *);        // 中断处理函数
    int irq_num;                // 中断号
};


/* 设备结构体 */
struct noblockio_dev {
    dev_t devid;                // 设备号
    int major;                  // 主设备号
    int minor;                  // 次设备号
    struct cdev cdev;           // 字符设备
    struct class *class;        // 类
    struct device *device;      // 设备
    struct device_node *key_nd; // 按键设备节点
    struct key_dev key[KEY_NUM];    // 按键设备
    struct timer_list timer;    // 定时器
    atomic_t key_value;         // 按键值
    atomic_t key_release_flag;  // 释放按键标志
    wait_queue_head_t r_wait;   // 读等待队列头
};
struct noblockio_dev noblockio;     // 阻塞设备

static int noblockio_open(struct inode *inode, struct file *filp) {

    int ret = 0;

    filp->private_data = &noblockio;

    return ret;
}

static ssize_t noblockio_read(struct file *flip, char __user *buf, size_t count, loff_t *lofft) {

    int ret = 0;
    struct noblockio_dev *dev = (struct noblockio_dev *)flip->private_data;
    unsigned char key_value = 0;
    unsigned char key_release_flag = 0;

    /* 以非阻塞的方式访问 */
    if(flip->f_flags & O_NONBLOCK) {
        /* 如果现在按键没按下 */
        if(atomic_read(&dev->key_release_flag) == 0) {
            /* 直接返回错误 */
            return -EINVAL;
        }
    }
    /* 以阻塞的方式访问 */
    else {
        /* 等待事件 等待按键有效 */
        wait_event_interruptible(dev->r_wait, atomic_read(&dev->key_release_flag));
    }

#if 0
    /* 等待事件 等待按键有效 */
    wait_event_interruptible(dev->r_wait, atomic_read(&dev->key_release_flag));
#endif
#if 0
    /* 定义一个等待队列项 */
    DECLARE_WAITQUEUE(wait, current);
    /* 按键没按下 */
    if(atomic_read(&dev->key_release_flag) == 0) {
        /* 将队列添加到等待队列头 */
        add_wait_queue(&dev->r_wait, &wait);
        /*  将当前进程设置为可被打断的状态*/
        __set_current_state(TASK_INTERRUPTIBLE);
        /* 切换 */
        schedule();

        /* 唤醒以后从这里运行 */
        /* signal_pending函数用于判断当前进程是否有信号处理 
           返回值不为0的话表示有信号需要处理 */
        if(signal_pending(current)) {
            ret = -ERESTARTSYS;
            goto data_err;
        }
        /* 将当前任务或线程设置为运行状态 */
        __set_current_state(TASK_RUNNING);
        /* 将对应的队列项从等待队列头删除 */
        remove_wait_queue(&dev->r_wait, &wait);
    }
#endif

    key_value = atomic_read(&dev->key_value);
    key_release_flag = atomic_read(&dev->key_release_flag);

    /* 按键按下了 */
    if(key_release_flag) {
        /* 有效按键 */
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
    /* 按键没有按下 */
    else {
        goto data_err;
    }

    return 0;

data_err:
    return ret;
}

static unsigned int noblockio_poll(struct file *filp, struct poll_table_struct *wait) {

    int mask = 0;
    struct noblockio_dev *dev = filp->private_data;

    poll_wait(filp, &dev->r_wait, wait);

    /* 判断是否可读 */
    /* 按键按下可读 */
    if(atomic_read(&dev->key_release_flag)) {
        /* 返回POLLIN */
        mask = POLLIN | POLLRDNORM;
    }
    

    return mask;
}

static const struct file_operations noblockio_fops = {
    .owner = THIS_MODULE,
    .open = noblockio_open,
    .read = noblockio_read,
    .poll = noblockio_poll,
};

/* 按键中断函数 */
static irqreturn_t key0_handler(int irq, void *data) {
    
    struct noblockio_dev *dev = data;
    dev->timer.data = (volatile unsigned long)data;

    /* 20ms定时 */
    mod_timer(&dev->timer, jiffies + msecs_to_jiffies(20));

    return IRQ_HANDLED;
}

/* 定时器中断函数 */
static void timer_func(unsigned long arg) {

    int value = 0;
    struct noblockio_dev *dev = (struct noblockio_dev *)arg;

    /* 获取GPIO值 */
    value = gpio_get_value(dev->key[0].gpio_index);
    /* 按下了 */
    if(value == 0) {
        atomic_set(&dev->key_value, dev->key[0].value);
    }
    /* 没有按下 */
    else if(value == 1) {
        atomic_set(&dev->key_value, 0x80 | (dev->key[0].value));
        atomic_set(&dev->key_release_flag, 1);
    }

    /* 唤醒进程 */
    if(atomic_read(&dev->key_release_flag)) {
        wake_up(&dev->r_wait);
    }
}

static int keyio_init(struct noblockio_dev *dev) {

    int ret = 0;
    int i = 0;
    int j = 0;

    /* 获取设备节点 */
    dev->key_nd = of_find_node_by_path("/key");
    /* 判断设备节点是否获取成功 */
    if(dev->key_nd == NULL) {
        ret = -EINVAL;
        printk("fail to find node\r\n");
        goto fail_find_node;
    }


    /* 循环获取GPIO编号 */
    for(i = 0; i < KEY_NUM; i++) {
        /* 获取GPIO编号 */
        dev->key[i].gpio_index = of_get_named_gpio(dev->key_nd, "key-gpios", i);
        /* 判断GPIO编号是否获取成功 */
        if(dev->key[i].gpio_index < 0) {
            ret = -EINVAL;
            printk("fail to get gpio\r\n");
            goto fail_get_gpio;
        }
        /* 打印GPIO编号 */
        printk("key%d gpio index is %d\r\n", i, dev->key[i].gpio_index);
    }


    /* 循环申请GPIO */
    for(i = 0; i < KEY_NUM; i++) {
        /* 清除缓存 */
        memset(dev->key[i].name, 0, sizeof(dev->key[i].name));
        /* 设置key name */
        sprintf(dev->key[i].name, "key%d", i);
        /* 申请GPIO */
        ret = gpio_request(dev->key[i].gpio_index, dev->key[i].name);
        /* 判断GPIO是否申请成功 */
        if(ret < 0) {
            ret = -EINVAL;
            printk("fail to request gpio\r\n");
            j = i;
            goto fail_request_gpio;            
        }
    }

    /* 循环初始化GPIO */
    for(i = 0; i < KEY_NUM; i++) {
        /* 初始化GPIO为输入 */
        ret = gpio_direction_input(dev->key[i].gpio_index);
        /* 判断初始化是否成功 */
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

    /* 按键中断初始化 */
    dev->key[0].handler = key0_handler;
    dev->key[0].value = KEY0_VALUE;
    for(i = 0; i < KEY_NUM; i++) {
        ret = request_irq(dev->key[i].irq_num, dev->key[i].handler, 
                          IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, dev->key[i].name, dev);
        if(ret < 0) {
            printk("fail to request irq%d\r\n", i);
            j = i;
            goto fail_request_irq;
        }
    }

    /* 初始化定时器 */
    dev->timer.function = timer_func;
    init_timer(&dev->timer);

    return 0;


    
fail_request_irq:
    /* 释放中断 */
    for(i = 0; i < j; i++) {
        free_irq(dev->key[i].irq_num, dev);
    }
fail_init_gpio:
fail_request_gpio:
    /* 循环释放IO */
    for(i = 0; i < j; i++) {
        gpio_free(dev->key[i].gpio_index);
    }
fail_get_gpio:
fail_find_node:
    return ret;
}


/* 入口 */
static int __init noblockio_init(void) {

    int ret = 0;

    /* 注册设备号 */
    noblockio.major = 0;
    /* 给定了设备号 */
    if(noblockio.major) {
        noblockio.devid = MKDEV(noblockio.major, 0);
        ret = register_chrdev_region(noblockio.devid, NOBLOCKIO_NUM, NOBLOCKIO_NAME);
    }
    /* 没有给定设备号 */
    else {
        ret = alloc_chrdev_region(&noblockio.devid, 0, NOBLOCKIO_NUM, NOBLOCKIO_NAME);
    }
    /* 查看设备号是否注册成功 */
    if(ret < 0) {
        printk("fail to register devid\r\n");
        goto fail_register_devid;
    }
    /* 查看设备号 */
    noblockio.major = MAJOR(noblockio.devid);
    noblockio.minor = MINOR(noblockio.devid);
    printk("noblockio devid major is %d, minor is %d\r\n", noblockio.major, noblockio.minor);


    /* 初始化字符设备 */
    cdev_init(&noblockio.cdev, &noblockio_fops);
    /* 添加字符设备 */
    ret = cdev_add(&noblockio.cdev, noblockio.devid, NOBLOCKIO_NUM);
    /* 判断字符设备是否添加成功 */
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }


    /* 创建类 */
    noblockio.class = class_create(THIS_MODULE, NOBLOCKIO_NAME);
    /* 判断类是否创建成功 */
    if(IS_ERR(noblockio.class)) {
        ret = PTR_ERR(noblockio.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }


    /* 创建设备 */
    noblockio.device = device_create(noblockio.class, NULL, noblockio.devid, NULL, NOBLOCKIO_NAME);
    /* 判断设备是否创建成功 */
    if(IS_ERR(noblockio.device)) {
        ret = PTR_ERR(noblockio.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }

    
    /* 初始化按键GPIO */
    ret = keyio_init(&noblockio);
    /* 判断GPIO是否初始化成功 */
    if(ret < 0) {
        printk("fail to init key\r\n");
        goto fail_init_key;
    }


    /* 初始化原子变量 */
    atomic_set(&noblockio.key_value, KEY0_VALUE);
    atomic_set(&noblockio.key_release_flag, 0);


    /* 等待队列头 */
    init_waitqueue_head(&noblockio.r_wait);

    return 0;


fail_init_key:
    /* 摧毁设备 */
    device_destroy(noblockio.class, noblockio.devid);
fail_create_device:
    /* 摧毁类 */
    class_destroy(noblockio.class);
fail_create_class:
    /* 删除字符设备 */
    cdev_del(&noblockio.cdev);
fail_add_cdev:
    /* 释放设备号 */
    unregister_chrdev_region(noblockio.devid, NOBLOCKIO_NUM);
fail_register_devid:
    return ret;
}

/* 出口 */
static void __exit noblockio_exit(void) {

    int i = 0;

    /* 循环释放中断 */
    for(i = 0; i < KEY_NUM; i++) {
        free_irq(noblockio.key[i].irq_num, &noblockio);
    }

    /* 循环释放IO */
    for(i = 0; i < KEY_NUM; i++) {
        gpio_free(noblockio.key[i].gpio_index);
    }

    /* 释放设备号 */
    unregister_chrdev_region(noblockio.devid, NOBLOCKIO_NUM);

    /* 删除字符设备 */
    cdev_del(&noblockio.cdev);

    /* 摧毁设备 */
    device_destroy(noblockio.class, noblockio.devid);

    /* 摧毁类 */
    class_destroy(noblockio.class);

}

/* 加载模块入口与出口 */
module_init(noblockio_init);
module_exit(noblockio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

