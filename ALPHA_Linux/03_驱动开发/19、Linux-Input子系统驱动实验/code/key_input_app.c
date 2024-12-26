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
    ./key_input_app <filename>    读取按键值
    ./key_input_app /dev/input/event1
 */

/* input_event结构体 */
static struct input_event input_event_dev;

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
        ret = read(fd, &input_event_dev, sizeof(input_event_dev));
        if(ret > 0) {
            switch(input_event_dev.type) {
                case EV_KEY:
                    printf("EV_KEY事件\r\n");
                    if(input_event_dev.code < BTN_MISC) {
                        printf("key %d %s\r\n", input_event_dev.code, input_event_dev.value?"press":"release");
                    }
                    else {
                        printf("button %d %s\r\n", input_event_dev.code, input_event_dev.value?"press":"release");
                    }
                break;

                case EV_SYN:
                    printf("EV_SYN事件\r\n");
                break;

                case EV_REL:
                    printf("EV_REL事件\r\n");
                break;

                case EV_ABS:
                    printf("EV_ABS事件\r\n");
                break;
            }
        }
        else {
            printf("fail to read\r\n");
            close(fd);
            return -1;
        }
    }

    close(fd);

    return 0;
}

