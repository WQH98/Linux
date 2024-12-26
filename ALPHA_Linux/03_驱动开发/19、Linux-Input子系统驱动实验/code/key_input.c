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
#include "linux/input.h"


#define KEY_INPUT_NUM       1
#define KEY_INPUT_NAME      "key_input"
#define KEY_NUM             1
#define KEY0_VALUE          0x01
#define INVAKEY             0xFF

/* KEY结构体 */
struct irq_key_desc {
    int gpio;               // io编号
    int irq_num;            // 中断号
    unsigned char value;    // 键值
    char name[10];          // 名字
    irqreturn_t (*handler)(int, void *);            // 中断处理函数
};

/* key_input设备结构体 */
struct key_input_dev {
    struct device_node *nd;
    struct irq_key_desc key[KEY_NUM];
    struct timer_list timer;

    struct input_dev *input_dev;
};
struct key_input_dev key_input;

static const struct file_operations key_input_fops = {
    .owner = THIS_MODULE,
};

static irqreturn_t key0_handler(int irq_num, void *data) {

    struct key_input_dev *dev = data;

    dev->timer.data = (unsigned long)data;

    mod_timer(&dev->timer, jiffies + msecs_to_jiffies(20));

    return IRQ_HANDLED;
}

static void timer_func(unsigned long data) {

    int value = 0;
    struct key_input_dev *dev = (struct key_input_dev *)data;

    value = gpio_get_value(dev->key[0].gpio);

    /* 上报按键值 */
    if(value == 0) {
        input_event(dev->input_dev, EV_KEY, KEY_0, 1);
        input_sync(dev->input_dev);
    }
    else if(value == 1) {
        input_event(dev->input_dev, EV_KEY, KEY_0, 0);
        input_sync(dev->input_dev);
    }
}

static int key_gpio_init(struct key_input_dev *dev) {

    int ret = 0;
    int i = 0, j = 0;

    dev->nd = of_find_node_by_path("/key");
    if(dev->nd == NULL) {
        printk("fail to find node\r\n");
        goto fail_find_node;
    }

    for(i = 0; i < KEY_NUM; i++) {
        dev->key[i].gpio = of_get_named_gpio(dev->nd, "key-gpios", i);
        if(dev->key[i].gpio < 0) {
            printk("fail to get gpio\r\n");
            goto fail_get_gpio;
        }

        memset(dev->key[i].name, 0, sizeof(dev->key[i].name));
        sprintf(dev->key[i].name, "key%d", i);
        ret = gpio_request(dev->key[i].gpio, dev->key[i].name);
        if(ret < 0) {
            printk("fail to request gpio\r\n");
            j = i;
            goto fail_request_gpio;
        }

        ret = gpio_direction_input(dev->key[i].gpio);
        if(ret < 0) {
            printk("fail to init gpio\r\n");
            goto fail_init_gpio;
        }

        dev->key[i].irq_num = irq_of_parse_and_map(dev->nd, i);
        printk("irq num is %d\r\n", dev->key[i].irq_num);
    }

    dev->key[0].handler = key0_handler;
    dev->key[0].value = KEY_0;
    for(i = 0; i < KEY_NUM; i++) {
        ret = request_irq(dev->key[i].irq_num, dev->key[i].handler, 
                          IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, dev->key[i].name, dev);
        if(ret < 0) {
            printk("fail to request irq\r\n");
            goto fail_request_irq;
        }
    }

    init_timer(&dev->timer);
    dev->timer.function = timer_func;



    return 0;


fail_request_irq:
fail_init_gpio:
    for(i = 0; i < j; i++) {
        gpio_free(key_input.key[i].gpio);
    }
fail_request_gpio:
fail_get_gpio:
fail_find_node:
    return ret;
}

static int __init key_input_init(void) {

    int ret = 0;

    ret = key_gpio_init(&key_input);
    if(ret < 0) {
        printk("fail to init gpio\r\n");
        goto fail_init_gpio;
    }

    /* 注册input_dev */
    key_input.input_dev = input_allocate_device();
    if(key_input.input_dev == NULL) {
        ret = -EINVAL;
        printk("fail to allocate device\r\n");
        goto fail_allocate_device;
    }
    key_input.input_dev->name = KEY_INPUT_NAME;
    /* 按键事件 */
    __set_bit(EV_KEY, key_input.input_dev->evbit);
    /* 重复事件 */
    __set_bit(EV_REP, key_input.input_dev->evbit);
    /* 对应的按键值 */
    __set_bit(KEY_0, key_input.input_dev->keybit);


    ret = input_register_device(key_input.input_dev);
    if(ret) {
        printk("fail to register device\r\n");
        goto fail_register_device;
    }

    return 0;

fail_register_device:
    input_free_device(key_input.input_dev);
fail_allocate_device:
fail_init_gpio:
    return ret;
}

static void __exit key_input_exit(void) {

    int i = 0;

    /* 释放中断 */
    for(i = 0; i < KEY_NUM; i++) {
        free_irq(key_input.key[i].irq_num, &key_input);
    }

    for(i = 0; i < KEY_NUM; i++) {
        gpio_free(key_input.key[i].gpio);
    }

    del_timer_sync(&key_input.timer);

    /* 注销input_dev */
    input_unregister_device(key_input.input_dev);
    input_free_device(key_input.input_dev);
}


module_init(key_input_init);
module_exit(key_input_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

