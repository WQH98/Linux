/* 示例代码5.6.5 futimens函数使用示例 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "time.h"

#define MY_FILE  "./test1_file.txt"

int main(void) {
    int ret = 0;
    int fd = 0;
    struct timespec tmsp_arr[2];

    /* 检查文件是否存在 */
    ret = access(MY_FILE, F_OK);
    if(-1 == ret) {
        printf("%s file does not exist\r\n", MY_FILE);
        exit(-1);
    }

    /* 打开文件 */
    fd = open(MY_FILE, O_RDONLY);
    if(-1 == fd) {
        perror("open error");
        exit(-1);
    }

    /* 修改文件时间戳 */
#if 1
    ret = futimens(fd, NULL);
#endif

#if 0
    tmsp_arr[0].tv_nsec = UTIME_OMIT;
    tmsp_arr[1].tv_nsec = UTIME_NOW;
    ret = futimens(fd, tmsp_arr);
#endif

#if 0
    tmsp_arr[0].tv_nsec = UTIME_NOW;
    tmsp_arr[1].tv_nsec = UTIME_OMIT;
    ret = futimens(fd, tmsp_arr);
#endif

    if(-1 == ret) {
        perror("futimens error");
        goto err;
    }

err:
    close(fd);
    exit(0);
}
