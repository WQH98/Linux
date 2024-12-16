#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"

#define LED_ON      1
#define LED_OFF     0


/* 
    argc：应用程序参数个数
    argv：应用程序参数，字符串格式
    ./atomic_app1 <filename> <1:0>  1表示打开LED，0表示关闭LED
    ./atomic_app1 /dev/atomic 1     打开LED
    ./atomic_app1 /dev/atomic 0     关闭LED
 */
int main(int argc, char *argv[]) {
    int cnt = 0;
    int fd = 0, ret = 0;
    char *filename = argv[1];
    char data_buf[1] = {0};

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

    /* 接收参数 */
    data_buf[0] = atoi(argv[2]);
    ret = write(fd, data_buf, sizeof(data_buf));
    /* 检查是否写入成功 */
    if(ret < 0) {
        close(fd);
        printf("fail to write data\r\n");
        return -1;
    }

    /* 模拟应用占用驱动25s */
    while(1) {
        sleep(5);
        cnt++;
        printf("app running times is %d\r\n", cnt * 5);
        if(cnt >= 5) {
            break;
        }
    }

    printf("app running finish\r\n");
    close(fd);

    return 0;
}

