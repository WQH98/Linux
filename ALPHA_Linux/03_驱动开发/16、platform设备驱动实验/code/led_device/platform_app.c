#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "sys/ioctl.h"
#include "signal.h"


/* 
    argc：应用程序参数个数
    argv：应用程序参数，字符串格式
    ./platform_app <filename> <0:1>     0表示关灯 1表示开灯
    ./platform_app /dev/platform_led 0  关灯
    ./platform_app /dev/platform_led 1  开灯
 */

#define LED_OFF 0
#define LED_ON  1


int  main(int argc, char *argv[]) {
    
    int ret = 0;
    int fd = 0;
    char *filename = argv[1];
    unsigned char data_buf[1];

    if(argc != 3) {
        printf("err usage\r\n");
        return -1;
    }

    fd = open(filename, O_RDWR);
    if(fd < 0) {
        printf("file %s open filed\r\n", filename);
        return -1;
    }

    /* 将字符转换为数字 */
    data_buf[0] = atoi(argv[2]);
    ret = write(fd, data_buf, sizeof(data_buf));
    if(ret < 0) {
        printf("led control failed\r\n");
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}

