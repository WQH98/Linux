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
#include "linux/i2c.h"
#include "linux/delay.h"

struct ft5426_dev {
    void *private_data;
};
static struct fx5426_dev fx5426;

/* 读取FX5426的N个寄存器值 */
static int ft5426_read_regs(struct ft5426_dev *dev, u8 reg, void *val, int len) {
    
    int ret = 0;
    struct i2c_msg msg[2];
    struct i2c_client* client = (struct i2c_client*)dev->private_data;

    msg[0].addr = client->addr;
    msg[0].flags = 0;
    msg[0].buf = &reg;
    msg[0].len = 1;

    msg[1].addr = client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].buf = val;
    msg[1].len = len;

    ret = i2c_transfer(client->adapter, msg, 2);

    return ret;
}

/* 向FT5426写N个寄存器的数据 */
static int ft5426_write_regs(struct ft5426_dev *dev, u8 reg, u8 *buf, u8 len) {
    
    int ret = 0;
    u8 b[256];
    struct i2c_msg msg;
    struct i2c_client *client = (struct i2c_client *)dev->private_data;

    b[0] = reg;
    memcpy(&b[1], buf, len);

    msg.addr = client->addr;
    msg.flags = 0;
    msg.buf = b;
    msg.len = len + 1;

    ret = i2c_transfer(client->adapter, &msg, 1);
}

/* 向FT5426一个寄存器写数据 */
static void ft5426_write_reg(struct ft5426_dev *dev, u8 reg, u8 data) {
    u8 buf = 0;
    buf = data;
    ft5426_write_regs(dev, reg, &buf, 1);
}

static int ft5426_probe(struct i2c_client *client, const struct i2c_device_id *id) {

    int ret = 0;

    printk("ft5426_probe\r\n");

    return ret;
}

static int ft5426_remove(struct i2c_client *client) {
    
    int ret = 0;


    return ret;
}


/* 传统的匹配表 */
static struct i2c_device_id ft5426_id[] = {
    {"edt-ft5426", 0},
    {}
};

/* 设备树匹配表 */
static struct of_device_id ft5426_of_match[] = {
    {.compatible = "edt,edt-ft5426"},
    {}
};

static struct i2c_driver ft5426_driver = {
    .probe = ft5426_probe,
    .remove = ft5426_remove,
    .driver = {
        .name = "edt_ft5426",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(ft5426_of_match),
    },
    .id_table = ft5426_id,
};

static int __init ft5426_init(void) {

    int ret = 0;

    ret = i2c_add_driver(&ft5426_driver);

    return ret;
}

static void __exit ft5426_exit(void) {

    i2c_del_driver(&ft5426_driver);
}


module_init(ft5426_init);
module_exit(ft5426_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");
