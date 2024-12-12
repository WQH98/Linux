#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"

#define LED_OFF 0
#define LED_ON  1


/*
    argc：应用程序参数个数
    argv：应用程序参数,字符串形式
    ./gpio_led_app <filename> <0:1> 0表示关灯 1表示开灯
    ./gpio_led_app /dev/gpio_led 0    表示关灯
    ./gpio_led_app /dev/gpio_led 1    表示开灯
 */
int main(int argc, char *argv[]) {

    int fd = 0;
    char *filename = argv[1];
    unsigned char data_buf[1];
    int ret = 0;

    if(argc != 3) {
        printf("error usage\r\n");
        return -1;
    }

    fd = open(filename, O_RDWR);
    if(fd < 0) {
        printf("file %s open filed\r\n", filename);
        return -1;
    }

    data_buf[0] = atoi(argv[2]);    // 将字符转换为数字
    ret = write(fd, data_buf, sizeof(data_buf));
    if(ret < 0) {
        printf("LED control failed\r\n");
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}
