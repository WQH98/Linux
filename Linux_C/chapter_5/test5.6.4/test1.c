/* 示例代码5.6.4  utimes使用示例*/
#include "stdio.h"
#include "stdlib.h"
#include "utime.h"
#include "time.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/time.h"

#define MY_FILE "./test1_file.txt"

int main(void) {
    
    int ret = 0;
    time_t cur_time;
    struct timeval times[2];

    /* 检查文件是否存在 */
    ret = access(MY_FILE, F_OK);
    if(-1 == ret) {
        printf("%s file does not exist\r\n", MY_FILE);
        exit(-1);
    }

    /* 获取当前时间 */
    time(&cur_time);
    for(int i = 0; i < 2; i++) {
        times[i].tv_sec = cur_time;
        times[i].tv_usec = 0;
    }

    /* 修改文件时间戳 */
    ret = utimes(MY_FILE, times);
    if(-1 == ret) {
        perror("utimes error");
        exit(-1);
    }

    exit(0);
}

