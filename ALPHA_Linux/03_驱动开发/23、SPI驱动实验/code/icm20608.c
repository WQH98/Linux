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
#include "linux/spi/spi.h"
#include "icm20608_reg.h"


#define ICM20608_NUM        1
#define ICM20608_NAME       "icm20608"

/* 设备结构体 */
struct icm20608_dev {
    dev_t devid;
    int major;
    int minor;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    void *private_data;
    int cs_gpio;
    struct device_node *nd;
    signed int gyro_x_adc;		/* 陀螺仪X轴原始值 	 */
	signed int gyro_y_adc;		/* 陀螺仪Y轴原始值		*/
	signed int gyro_z_adc;		/* 陀螺仪Z轴原始值 		*/
	signed int accel_x_adc;		/* 加速度计X轴原始值 	*/
	signed int accel_y_adc;		/* 加速度计Y轴原始值	*/
	signed int accel_z_adc;		/* 加速度计Z轴原始值 	*/
	signed int temp_adc;		/* 温度原始值 			*/
};
static struct icm20608_dev icm20608dev;

#if 0
/* spi读寄存器 */
static int icm20608_read_regs(struct icm20608_dev *dev, u8 reg, void *buf, int len) {
    
    int ret = 0;
    unsigned char tx_data[len];
    struct spi_message m;
    struct spi_transfer *t;
    struct spi_device *spi = (struct spi_device *)dev->private_data;

    /* 片选拉低 */
    gpio_set_value(dev->cs_gpio, 0);

    /* 构建spi_transfer */
    t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);

    /* 1、发送要读取的寄存器地址 */
    tx_data[0] = reg | 0x80;
    t->tx_buf = tx_data;
    t->len = 1;

    spi_message_init(&m);
    spi_message_add_tail(t, &m);
    ret = spi_sync(spi, &m);

    /* 2、接收返回的寄存器数据 */
    tx_data[0] = 0xFF;              // 无效数据
    t->rx_buf = buf;
    t->len = len;

    spi_message_init(&m);
    spi_message_add_tail(t, &m);
    ret = spi_sync(spi, &m);

    kfree(t);

    /* 片选拉高 */
    gpio_set_value(dev->cs_gpio, 1);

    return ret;
}

/* spi写寄存器 */
static int icm20608_write_regs(struct icm20608_dev *dev, u8 reg, u8 *buf, int len) {
    
    int ret = 0;
    unsigned char tx_data[len];
    struct spi_message m;
    struct spi_transfer *t;
    struct spi_device *spi = (struct spi_device *)dev->private_data;

    /* 片选拉低 */
    gpio_set_value(dev->cs_gpio, 0);

    /* 构建spi_transfer */
    t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);

    /* 1、发送要写入的寄存器地址 */
    tx_data[0] = reg & ~0x80;
    t->tx_buf = tx_data;
    t->len = 1;
    spi_message_init(&m);
    spi_message_add_tail(t, &m);
    ret = spi_sync(spi, &m);

    /* 2、接收返回的寄存器数据 */
    t->tx_buf = buf;
    t->len = len;
    spi_message_init(&m);
    spi_message_add_tail(t, &m);
    ret = spi_sync(spi, &m);

    kfree(t);

    /* 片选拉高 */
    gpio_set_value(dev->cs_gpio, 1);

    return ret;
}
#endif

/* spi读寄存器 */
static int icm20608_read_regs(struct icm20608_dev *dev, u8 reg, void *buf, int len) {
    
    int ret = 0;

    struct spi_device *spi = (struct spi_device *)dev->private_data;
    u8 data = 0;

    /* 片选拉低 */
    // gpio_set_value(dev->cs_gpio, 0);

    data = reg | 0x80;
    /* 发送要读取的寄存器地址 */
    // spi_write(spi, &data, 1);

    /* 读取数据 */
    // spi_read(spi, buf, len);

    spi_write_then_read(spi, &data, 1, buf, len);

    /* 片选拉高 */
    // gpio_set_value(dev->cs_gpio, 1);

    return ret;
}

/* spi写寄存器 */
static int icm20608_write_regs(struct icm20608_dev *dev, u8 reg, u8 *buf, int len) {

    int ret = 0;
    struct spi_device *spi = (struct spi_device *)dev->private_data;
    // u8 data = 0;
    u8 *tx_buf;

    /* 片选拉低 */
    // gpio_set_value(dev->cs_gpio, 0);

    // data = reg & ~0x80;
    /* 发送要写入的寄存器地址 */
    // spi_write(spi, &data, 1);

    /* 读取数据 */
    // spi_write(spi, buf, len);

    tx_buf = kzalloc(len + 1, GFP_KERNEL);
    tx_buf[0] = reg & ~0x80;
    memcpy(&tx_buf[1], buf, len);
    spi_write(spi, tx_buf, len + 1);


    /* 片选拉高 */
    // gpio_set_value(dev->cs_gpio, 1);

    return ret;
}

/* ICM20608读取单个寄存器 */
static unsigned char icm20608_read_one_reg(struct icm20608_dev *dev, u8 reg) {
    
    u8 data = 0;
    icm20608_read_regs(dev, reg, &data, 1);

    return data;
}

/* ICM20608写单个寄存器 */
static void icm20608_write_one_reg(struct icm20608_dev *dev, u8 reg, u8 data) {
    
    u8 buf = data;
    icm20608_write_regs(dev, reg, &buf, 1);
}

/* ICM20608初始化 */
static void icm20608_reg_init(struct icm20608_dev *dev) {

    u8 value = 0;

    /* 复位 复位后为0x40 睡眠模式 */
    icm20608_write_one_reg(dev, ICM20_PWR_MGMT_1, 0x80);
    mdelay(50);      // 50ms
    /* 关闭睡眠后 自动选择时钟 */
    icm20608_write_one_reg(dev, ICM20_PWR_MGMT_1, 0x01);
    mdelay(50);      // 50ms

    value = icm20608_read_one_reg(dev, ICM20_WHO_AM_I);
    printk("icm20608 ICM20_WHO_AM_I is %#X\r\n", value);

    value = icm20608_read_one_reg(dev, ICM20_PWR_MGMT_1);
    printk("icm20608 ICM20_PWR_MGMT_1 is %#X\r\n", value);

    icm20608_write_one_reg(dev, ICM20_SMPLRT_DIV, 0x00); 	/* 输出速率是内部采样率					*/
	icm20608_write_one_reg(dev, ICM20_GYRO_CONFIG, 0x18); 	/* 陀螺仪±2000dps量程 				*/
	icm20608_write_one_reg(dev, ICM20_ACCEL_CONFIG, 0x18); 	/* 加速度计±16G量程 					*/
	icm20608_write_one_reg(dev, ICM20_CONFIG, 0x04); 		/* 陀螺仪低通滤波BW=20Hz 				*/
	icm20608_write_one_reg(dev, ICM20_ACCEL_CONFIG2, 0x04); /* 加速度计低通滤波BW=21.2Hz 			*/
	icm20608_write_one_reg(dev, ICM20_PWR_MGMT_2, 0x00); 	/* 打开加速度计和陀螺仪所有轴 				*/
	icm20608_write_one_reg(dev, ICM20_LP_MODE_CFG, 0x00); 	/* 关闭低功耗 						*/
	icm20608_write_one_reg(dev, ICM20_FIFO_EN, 0x00);		/* 关闭FIFO	*/
}

void icm20608_readdata(struct icm20608_dev *dev)
{
	unsigned char data[14] = { 0 };
	icm20608_read_regs(dev, ICM20_ACCEL_XOUT_H, data, 14);

	dev->accel_x_adc = (signed short)((data[0] << 8) | data[1]); 
	dev->accel_y_adc = (signed short)((data[2] << 8) | data[3]); 
	dev->accel_z_adc = (signed short)((data[4] << 8) | data[5]); 
	dev->temp_adc    = (signed short)((data[6] << 8) | data[7]); 
	dev->gyro_x_adc  = (signed short)((data[8] << 8) | data[9]); 
	dev->gyro_y_adc  = (signed short)((data[10] << 8) | data[11]);
	dev->gyro_z_adc  = (signed short)((data[12] << 8) | data[13]);
}

static int icm20608_open(struct inode *inode, struct file *filp) {

    int ret = 0;

    filp->private_data = &icm20608dev;

    return ret;
}

static int icm20608_close(struct inode *inode, struct file *filp) {

    int ret = 0;

    return ret;
}

static ssize_t icm20608_read(struct file *filp, char __user *buf, size_t count, loff_t *lofft) {

    int ret = 0;
    signed int data[7];
    struct icm20608_dev *dev = (struct icm20608_dev *)filp->private_data;

    icm20608_readdata(dev);
    data[0] = dev->gyro_x_adc;
    data[1] = dev->gyro_y_adc;
    data[2] = dev->gyro_z_adc;
    data[3] = dev->accel_x_adc;
    data[4] = dev->accel_y_adc;
    data[5] = dev->accel_z_adc;
    data[6] = dev->temp_adc;
    ret = copy_to_user(buf, data, sizeof(data));

    return ret;
}

static const struct file_operations icm20608_fops = {
    .open = icm20608_open,
    .release = icm20608_close,
    .read = icm20608_read,
    .owner = THIS_MODULE,
};


static int icm20608_probe(struct spi_device *spi) {

    int ret = 0;

    printk("icm20608_probe\r\n");

    icm20608dev.major = 0;
    if(icm20608dev.major) {
        icm20608dev.devid = MKDEV(icm20608dev.major, 0);
        icm20608dev.minor = MINOR(icm20608dev.devid);
        ret = register_chrdev_region(icm20608dev.devid, ICM20608_NUM, ICM20608_NAME);
    }
    else {
        ret = alloc_chrdev_region(&icm20608dev.devid, 0, ICM20608_NUM, ICM20608_NAME);
        icm20608dev.major = MAJOR(icm20608dev.devid);
        icm20608dev.minor = MINOR(icm20608dev.devid);
    }
    if(ret < 0) {
        printk("fail to register devid\r\n");
        goto fail_register_devid;
    }
    printk("icm20608 devid major is %d, minor is %d\r\n", icm20608dev.major, icm20608dev.minor);

    cdev_init(&icm20608dev.cdev, &icm20608_fops);
    ret = cdev_add(&icm20608dev.cdev, icm20608dev.devid, ICM20608_NUM);
    if(ret < 0) {
        printk("fail to add cdev\r\n");
        goto fail_add_cdev;
    }

    icm20608dev.class = class_create(THIS_MODULE, ICM20608_NAME);
    if(IS_ERR(icm20608dev.class)) {
        ret = PTR_ERR(icm20608dev.class);
        printk("fail to create class\r\n");
        goto fail_create_class;
    }

    icm20608dev.device = device_create(icm20608dev.class, NULL, icm20608dev.devid, NULL, ICM20608_NAME);
    if(IS_ERR(icm20608dev.device)) {
        ret = PTR_ERR(icm20608dev.device);
        printk("fail to create device\r\n");
        goto fail_create_device;
    }

#if 0
    /* 获取片选引脚 */
    icm20608dev.nd = of_get_parent(spi->dev.of_node);
    icm20608dev.cs_gpio = of_get_named_gpio(icm20608dev.nd, "cs-gpio", 0);
    if(icm20608dev.cs_gpio < 0) {
        printk("fail to get gpio\r\n");
        goto fail_get_gpio;
    }
    printk("icm20608dev cs_gpio is %d\r\n", icm20608dev.cs_gpio);

    ret = gpio_request(icm20608dev.cs_gpio, "cs");
    if(ret < 0) {
        printk("fail to request gpio\r\n");
        goto fail_request_gpio;
    }

    /* 默认输出高电平 */
    ret = gpio_direction_output(icm20608dev.cs_gpio, 1);
    if(ret < 0) {
        printk("fail to set gpio\r\n");
        goto fail_set_gpio;
    }
#endif

    /* 初始化spi_device */
    spi->mode = SPI_MODE_0;
    spi_setup(spi);

    /* 设置ICM20608的私有数据 */
    icm20608dev.private_data = spi;

    icm20608_reg_init(&icm20608dev);

    return 0;

// fail_set_gpio:
//     gpio_free(icm20608dev.cs_gpio);
// fail_request_gpio:
// fail_get_gpio:
    device_destroy(icm20608dev.class, icm20608dev.devid);
fail_create_device:
    class_destroy(icm20608dev.class);
fail_create_class:
    cdev_del(&icm20608dev.cdev);
fail_add_cdev:
    unregister_chrdev_region(icm20608dev.devid, ICM20608_NUM);
fail_register_devid:
    return ret;
}

static int icm20608_remove(struct spi_device *spi) {

    int ret = 0;

    // gpio_set_value(icm20608dev.cs_gpio, 1);

    // gpio_free(icm20608dev.cs_gpio);

    unregister_chrdev_region(icm20608dev.devid, ICM20608_NUM);

    cdev_del(&icm20608dev.cdev);

    device_destroy(icm20608dev.class, icm20608dev.devid);

    class_destroy(icm20608dev.class);

    return ret;
}

/* 设备树匹配 */
static const struct of_device_id icm20608_of_match[] = {
    {.compatible = "alientek,icm20608"},
    {},
};

/* 传统匹配 */
static const struct spi_device_id icm20608_id[] = {
    {"alientek,icm20608", 0},
    {},
};

/* spi_driver */
static struct spi_driver icm20608_driver = {
    .probe = icm20608_probe,
    .remove = icm20608_remove,
    .driver = {
        .name = "icm20608",
        .owner = THIS_MODULE,
        .of_match_table = icm20608_of_match,
    },
    .id_table = icm20608_id,
};

static int __init icm20608_init(void) {

    int ret = 0;

    ret = spi_register_driver(&icm20608_driver);

    return ret;
}


static void __exit icm20608_exit(void) {

    spi_unregister_driver(&icm20608_driver);

}


module_init(icm20608_init);
module_exit(icm20608_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

