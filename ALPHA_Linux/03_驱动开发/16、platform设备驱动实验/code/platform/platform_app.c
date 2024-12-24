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
    ./async_app <filename>    读取按键值
    ./async_app /dev/async
 */

int fd = 0;

static void sigio_signal_func(int num) {
    
    int err = 0;
    unsigned int key_value = 0;

    err = read(fd, &key_value, sizeof(key_value));
    if(err < 0) {

    }
    else {
        printf("sigio signal! key value is %d\r\n", key_value);
    }
}


int main(int argc, char *argv[]) {

    int ret = 0;
    char *filename = argv[1];
    unsigned char data = 0;
    int flags = 0;


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

    /* 设置信号处理函数 */
    signal(SIGIO, sigio_signal_func);
    /* 设置当前进程接收SIGIO信号 */
    fcntl(fd, F_SETOWN, getpid());

    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | FASYNC);

    while(1) {
        sleep(2);
    }

    close(fd);

    return 0;
}

