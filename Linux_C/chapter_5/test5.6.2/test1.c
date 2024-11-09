/* 示例代码5.6.2 utime函数使用示例 */
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "unistd.h"
#include "time.h"
#include "utime.h"

#define MY_FILE "./test1_file.txt"

int main(void) {
    struct utimbuf utm_buf;
    time_t cur_sec;
    int ret;

    /* 检查文件是否存在 */
    ret = access(MY_FILE, F_OK);
    if(-1 == ret) {
        printf("%s file does not exist\r\n", MY_FILE);
        exit(-1);
    }

    /* 获取当前时间 */
    time(&cur_sec);
    utm_buf.actime = cur_sec;
    utm_buf.modtime = cur_sec;

    /* 修改文件时间戳 */
    ret = utime(MY_FILE, &utm_buf);
    if(-1 == ret) {
        perror("utime error");
        exit(-1);
    }

    exit(0);
}

