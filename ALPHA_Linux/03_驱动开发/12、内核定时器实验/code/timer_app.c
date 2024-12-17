#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "sys/ioctl.h"

#define CLOSE_CMD       _IO(0xEF, 1)           // 关闭命令
#define OPEN_CMD        _IO(0xEF, 2)           // 打开命令
#define SETPERIOD_CMD   _IOW(0xEF, 3, int)     // 设置周期


/* 
    argc：应用程序参数个数
    argv：应用程序参数，字符串格式
    ./timer_app <filename>    读取按键值
    ./timer_app /dev/timer
 */
int main(int argc, char *argv[]) {
    int value = 0;
    int fd = 0, ret = 0;
    char *filename = argv[1];
    char data_buf[1] = {0};
    unsigned int cmd;
    unsigned int arg;
    unsigned char str[100];

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
        printf("please input cmd: ");
        ret = scanf("%d", &cmd);
        if(ret != 1) {
            gets(str);          // 防止卡死
        }
        if(cmd == 1) {          // 关闭
            ioctl(fd, CLOSE_CMD, &arg);
        }
        else if(cmd == 2) {     // 打开
            ioctl(fd, OPEN_CMD, &arg);
        }
        else if(cmd == 3) {     // 设置周期
            printf("please input period: ");
            ret = scanf("%d", &arg);
            if(ret != 1) {
                gets(str);          // 防止卡死
            }
            printf("args is %d\r\n", arg);
            ret = ioctl(fd, SETPERIOD_CMD, &arg);
            if(ret < 0) {
                printf("fail to ioctl\r\n");
            }
        }
    }

    close(fd);

    return 0;
}

