#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "sys/ioctl.h"
#include "linux/input.h"


/* 
    argc：应用程序参数个数
    argv：应用程序参数，字符串格式
    ./ap3216c_app <filename>    读取按键值
    ./ap3216c_app /dev/ap3216c
 */

/* input_event结构体 */
static struct input_event input_event_dev;

int main(int argc, char *argv[]) {

    int fd = 0, ret = 0;
    char *filename = argv[1];
    unsigned short data[3] = {0};
    unsigned short ir, ps, als;


    /* 判断传参个数是否正确 */
    if(argc != 2) {
        printf("error usage\r\n");
        return -1;
    }

    /* 打开文件 */
    fd = open(filename, O_RDWR);
    /* 判断文件是否打开 */
    if(fd < 0) {
        printf("fail to open %s\r\n", filename);
        return -1;
    }

    while(1) {
        ret = read(fd, data, sizeof(data));
        if(ret == 0) {
            ir = data[0];
            als = data[1];
            ps = data[2];
            printf("AP3216C ir is %d, als is %d, ps is %d\r\n", ir, als, ps);
        }
        usleep(200000); // 延时200ms
    }
    
    close(fd);

    return 0;
}

