#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"

#define KEY0VALUE       0xF0
#define INVAKEY         0x00

/* 
    argc：应用程序参数个数
    argv：应用程序参数，字符串格式
    ./key_app <filename>    读取按键值
    ./key_app /dev/key 
 */
int main(int argc, char *argv[]) {
    int value = 0;
    int fd = 0, ret = 0;
    char *filename = argv[1];
    char data_buf[1] = {0};

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

    /* 循环读取按键值 */
    while(1) {
        read(fd, &value, sizeof(value));
        if(value == KEY0VALUE) {
            printf("key0 press, value is %d\r\n", value);
        }
    }

    close(fd);

    return 0;
}

