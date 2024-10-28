/*
    此文件夹用来检测open函数的标志O_TRUNC
    O_TRUNC的作用是调用这个标志时 打开的文件大小会变成0
    也就是把原来的文件内容全部丢弃
*/

#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"


int main() {
    int fd = 0;
    // 打开文件
    fd = open("test_file", O_WRONLY | O_TRUNC);
    if(-1 == fd) {
        perror("open error");
        exit(-1);
    }

    // 关闭文件
    close(fd);
    exit(0);
}

