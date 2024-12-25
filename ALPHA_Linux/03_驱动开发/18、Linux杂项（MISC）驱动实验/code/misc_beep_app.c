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
    argc: 应用程序参数个数
    argv: 应用程序参数 字符串格式
    ./misc_beep_app <filename> <0:1>     0表示关闭蜂鸣器 1表示打开蜂鸣器
    ./misc_beep_app /dev/miac_beep 1     表示开蜂鸣器
    ./misc_beep_app /dev/miac_beep 0     表示关蜂鸣器
 */

int main(int argc, char *argv[]) {

    int ret = 0;
    char *filename = argv[1];
    int fd = 0;
    unsigned char data_buf[1];

    /* 判断传参个数是否正确 */
    if(argc != 3) {
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

    data_buf[0] = atoi(argv[2]);
    ret = write(fd, data_buf, sizeof(data_buf));
    if(ret < 0) {
        printf("BEEP control failed\r\n");
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}

