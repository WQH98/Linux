/*
    fcntl()函数可以对一个已经打开的文件描述符执行一系列控制操作
    比如复制一个文件描述符 获取/设置文件描述符 获取设置文件状态
    标志等 相当于一个多功能文件描述符管理工具箱
    此文件用来验证复制文件描述符功能
*/

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


int main() {
    int fd1 = 0, fd2 = 0, ret = 0;
    // 打开测试文件
    fd1 = open("./test1_file", O_RDWR);
    if(-1 == fd1) {
        perror("open error");
        exit(-1);
    }

    // 使用fcntl函数复制文件描述符
    fd2 = fcntl(fd1, F_DUPFD, 0);
    if(-1 == fd2) {
        perror("fcntl error");
        ret = -1;
        goto err;
    }

    // 打印出两个文件描述符
    printf("fd1: %d\tfd2: %d\r\n", fd1, fd2);


    ret = 0;
    close(fd2);
err:
    // 关闭文件
    close(fd1);
    exit(ret);
}

