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

#define KEY_INPUT_NUM       1
#define KEY_INPUT_NAME      "key_input"
#define KEY_NUM             1
#define KEY0_VALUE          0x01
#define INVAKEY             0xFF

/* KEY结构体 */
struct irq_key_desc {
    int gpio;               // io编号
    int irq_num:            // 中断号
    unsigned car value;     // 键值
    char name[10];          // 名字
    irqreturn_t (*handler)(int, void *);            // 中断处理函数
};

/* key_input设备结构体 */
struct key_input_dev {
    dev_t devid;
    int major;
    int minor;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct device_node *nd;
    struct irq_key_desc key[KEY_NUM];
    struct timer_list timer;

    atomic_t key_value;
    atomic_t key_release_flag;
};
struct key_input_dev key_input;


static int key_input_open(struct inode *inode, struct file *filp) {
    
    filp->private_data = &key_input;

    return 0;
}



