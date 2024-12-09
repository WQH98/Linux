#include "linux/module.h"
#include "linux/kernel.h"
#include "linux/init.h"
#include "linux/fs.h"
#include "linux/uaccess.h"

#define CHRDEVBASE_MAJOR    200             // 主设备号
#define CHRDEVBASE_NAME     "chrdevbase"    // 设备名字

static char read_buf[100];                  // 读缓存
static char write_buf[100];                 // 写缓存
static char kernel_data[] = {"kernel data!"};

static int chrdevbase_open(struct inode *inode, struct file *filp) {
    // printk("chrdevbase_open\r\n");
    return 0;
}

static int chrdevbase_release(struct inode *inode, struct file *filp) {
    // printk("chrdevbase_release\r\n");
    return 0;
}

static ssize_t chrdevbase_read(struct file *filp, __user char *buf, size_t count, loff_t *ppos) {
    int ret = 0;
    // printk("chrdevbase_read\r\n");
    /* 应用程序不能直接访问内核数据！必须借助其他的函数 */
    memcpy(read_buf, kernel_data, sizeof(kernel_data));
    ret = copy_to_user(buf, read_buf, count);
    if(ret == 0) {
        printk("read success!\r\n");
    }
    else {
        printk("read failed!\r\n");
    }
    return 0;
}

static ssize_t chrdevbase_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos) {
    int ret = 0;
    // printk("chrdevbase_write\r\n");

    ret = copy_from_user(write_buf, buf, count);
    if(ret == 0) {
        printk("write success!\r\n");
        printk("kernel received data: %s\r\n", write_buf);
    }
    else {
        printk("write failed!\r\n");
    }
    return 0;    
}


/* 字符设备 操作集合 */
static struct file_operations chrdevbase_fops = {
    .owner = THIS_MODULE,
    .open = chrdevbase_open,
    .release = chrdevbase_release,
    .read = chrdevbase_read,
    .write = chrdevbase_write,
};

static int __init chrdevbase_init(void) {
    int ret = 0;
    printk("chrdevbase_init\r\n");
    /* 注册字符设备 */
    ret = register_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops);
    if(ret < 0) {
        printk("chrdevbase init failed!\r\n");
    }
    return 0;
}

static void __exit chrdevbase_exit(void) {
    printk("chrdevbase_exit\r\n");

    /* 注销字符设备 */
    unregister_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME);
}


/* 模块入口与出口 */
module_init(chrdevbase_init);      // 入口
module_exit(chrdevbase_exit);      // 出口


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WangQinghao");

