#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "sys/ioctl.h"
#include "sys/select.h"
#include "sys/time.h"
#include "poll.h"



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
//    fd_set readfds;
//    struct timeval timeout;
    struct pollfd fds;



    /* 判断传参个数是否正确 */
    if(argc != 2) {
        printf("error usage\r\n");
        return -1;
    }

    /* 打开文件 以非阻塞方式打开 */
    fd = open(filename, O_RDWR | O_NONBLOCK);
    /* 判断文件是否打开 */
    if(fd < 0) {
        printf("fail to open %s\r\n", filename);
        return -1;
    }
#if 0
    while(1) {

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        timeout.tv_sec = 1;             // 1S
        timeout.tv_usec = 0;          

        ret = select(fd + 1, &readfds, NULL, NULL, &timeout);
        switch(ret) {
            /* 超时 */
            case 0:
                printf("select timeout\r\n");
            break;
            /* 错误 */
            case -1:

            break;
            /* 可以读取数据 */
            default:
                if(FD_ISSET(fd, &readfds)) {
                    ret = read(fd, &data, sizeof(data));
                    if(ret < 0) {

                    }
                    else {
                        if(data) {
                            printf("key value is %#x\r\n", data);
                        }
                    }
                }
            break;
        }
    }
#endif

    while(1) {

        fds.fd = fd;
        fds.events = POLLIN;

        /* 超时时间500ms */
        ret = poll(&fds, 1, 500);
        /* 超时 */
        if(ret == 0) {

        }
        /* 错误 */
        else if(ret < 0) {

        }
        /* 正常 可以读取 */
        else {
            if(fds.revents | POLLIN) {
                ret = read(fd, &data, sizeof(data));
                if(ret < 0) {

                }
                else {
                    if(data) {
                        printf("key value is %#x\r\n", data);
                    }
                }
            }
        }
    }

    close(fd);

    return 0;
}

