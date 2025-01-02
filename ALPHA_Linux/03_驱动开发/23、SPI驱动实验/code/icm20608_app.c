#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "sys/ioctl.h"
#include "linux/input.h"
#include "sys/time.h"


/* 
    argc：应用程序参数个数
    argv：应用程序参数，字符串格式
    ./icm20608_app <filename>
    ./icm20608_app /dev/icm20608
 */

/* input_event结构体 */
static struct input_event input_event_dev;

int main(int argc, char *argv[]) {

    int fd;
    char *filename;
    signed int data_buf[7];
    unsigned char data[14];
    signed int gyro_x_adc, gyro_y_adc, gyro_z_adc;
    signed int accel_x_adc, accel_y_adc, accel_z_adc; 
    signed int temp_adc;

    float gyro_x_act, gyro_y_act, gyro_z_act;
    float accel_x_act, accel_y_act, accel_z_act;
    float temp_act;

    int ret = 0;

    filename = argv[1];

    fd = open(filename, O_RDWR);
    if(fd < 0) {
        printf("can't open file\r\n");
        return -1;
    }

    while(1) {
        ret = read(fd, data_buf, sizeof(data_buf));
        /* 数据读取成功 */
        if(ret == 0) {
            gyro_x_adc = data_buf[0];
            gyro_y_adc = data_buf[1];
            gyro_z_adc = data_buf[2];
            accel_x_adc = data_buf[3];
            accel_y_adc = data_buf[4];
            accel_z_adc = data_buf[5];
            temp_adc = data_buf[6];

            /* 计算实际值 */
            gyro_x_act = (float)(gyro_x_adc) / 16.4;
            gyro_y_act = (float)(gyro_y_adc) / 16.4;
            gyro_z_act = (float)(gyro_z_adc) / 16.4;
            accel_x_act = (float)(accel_x_adc) / 2048.0;
            accel_y_act = (float)(accel_y_adc) / 2048.0;
            accel_z_act = (float)(accel_z_adc) / 2048.0;
            temp_act = ((float)(temp_adc) - 25) / 326.8 + 25;

            printf("\r\n原始值：\r\n");
            printf("gx = %d, gy = %d, gz = %d\r\n", gyro_x_adc, gyro_y_adc, gyro_z_adc);
            printf("ax = %d, ay = %d, az = %d\r\n", accel_x_adc, accel_y_adc, accel_z_adc);
            printf("temp = %d\r\n", temp_adc);
            printf("\r\n实际值：\r\n");
            printf("act gx = %.2f°/S, act gy = %.2f°/S, act gz = %.2f°/S\r\n", gyro_x_act, gyro_y_act, gyro_z_act);
            printf("act ax = %.2fg, act ay = %.2fg, act az = %.2fg\r\n", accel_x_act, accel_y_act, accel_z_act);
            printf("act temp = %.2f℃\r\n", temp_act);
        }
        /* 延时100毫秒 */
        usleep(100000);
    }

    close(fd);

    return 0;
}

