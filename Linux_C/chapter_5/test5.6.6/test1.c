/* 示例代码5.6.6 utimensat函数使用示例 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/stat.h"

#define MY_FILE "/home/wqh/Project/Linux/Linux_C/chapter_5/test5.6.6/test1_file.txt"

int main(void) {

    int ret = 0;
    struct timespec times_arr[2];

    /* 检查文件是否存在 */
    ret = access(MY_FILE, F_OK);
    if(-1 == ret) {
        printf("%s file does not exist\r\n", MY_FILE);
        exit(-1);
    }

    /* 修改文件时间戳 */
#if 1
    ret = utimensat(-1, MY_FILE, NULL, AT_SYMLINK_NOFOLLOW);        // 同时设置为当前时间
#endif

#if 1
    times_arr[0].tv_nsec = UTIME_OMIT;      // 访问时间保持不变
    times_arr[1].tv_nsec = UTIME_NOW;       // 内容修改时间修改为当前系统时间
    ret = utimensat(-1, MY_FILE, times_arr, AT_SYMLINK_NOFOLLOW);
#endif

#if 1
    times_arr[0].tv_nsec = UTIME_NOW;        // 访问时间保持不变
    times_arr[1].tv_nsec = UTIME_OMIT;       // 内容修改时间修改为当前系统时间
    ret = utimensat(-1, MY_FILE, times_arr, AT_SYMLINK_NOFOLLOW);
#endif

    if(-1 == ret) {
        perror("utimensat error");
        exit(-1);
    }

    exit(0);

}   
