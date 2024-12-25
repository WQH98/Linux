#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "sys/ioctl.h"


/* 
    argc：应用程序参数个数
    argv：应用程序参数，字符串格式
    ./imx6u_irq_app <filename>    读取按键值
    ./imx6u_irq_app /dev/imx6u_irq
 */
int main(int argc, char *argv[]) {

    int fd = 0, ret = 0;
    char *filename = argv[1];
    unsigned char data = 0;


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
        ret = read(fd, &data, sizeof(data));
        if(ret < 0) {

        }
        else {
            if(data) {
                printf("key value is %#x\r\n", data);
            }
        }
    }

    close(fd);

    return 0;
}

