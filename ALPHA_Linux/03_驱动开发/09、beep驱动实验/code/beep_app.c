#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"

#define BEEP_ON  1
#define BEEP_OFF 0


/* 
    argc：应用程序参数个数
    argv：应用程序参数，字符串格式
    ./beep_app <filename> <1:0>     1表示打开蜂鸣器 0表示关闭蜂鸣器
    ./beep_app /dev/beep 1     表示打开蜂鸣器
    ./beep_app /dev/beep 0     表示关闭蜂鸣器
 */
int main(int argc, char *argv[]) {
    int fd = 0, ret = 0;
    char *filename = argv[1];
    unsigned char data_buf[1];

    /* 判断传参个数是否正确 */
    if(argc != 3) {
        printf("error usage\r\n");
        return -1;
    }

    /* 打开文件 */
    fd = open(filename, O_RDWR);
    if(fd < 0) {
        printf("fail to open %s\r\n", filename);
        return -1;
    }

    data_buf[0] = atoi(argv[2]);        // 字符转换数字
    ret = write(fd, data_buf, sizeof(data_buf));
    /* 检查写入是否成功 */
    if(ret < 0) {
        printf("fail to write\r\n");
        close(fd);
        return -1;
    }

    close(fd);

    return ret;
}

