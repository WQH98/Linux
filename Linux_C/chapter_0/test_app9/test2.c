/*
    此文件用来验证获取/设置文件状态标志
    首先获取文件状态标志 然后添加一个状态
    最后再读一遍文件状态标志 查看是否添加成功
*/

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


int main() {
    int fd = 0, ret = 0, flag = 0;

    // 打开文件
    fd = open("./test2_file", O_RDWR);
    if(-1 == fd) {
        perror("open error");
        exit(-1);
    }

    // 获取文件状态标志
    flag = fcntl(fd, F_GETFL);
    if(-1 == flag) {
        perror("fcntl error");
        ret = -1;
        goto err;
    }

    printf("flag: 0x%x\r\n", flag);

    // 设置文件状态标志 添加O_APPEND标志
    ret = fcntl(fd, F_SETFL, flag|O_APPEND);
    if(-1 == ret) {
        perror("fcntl error");
        goto err;
    }

    // 获取文件状态标志
    flag = fcntl(fd, F_GETFL);
    if(-1 == flag) {
        perror("fcntl error");
        ret = -1;
        goto err;
    }

    printf("flag: 0x%x\r\n", flag);

    ret = 0;
err:
    // 关闭文件
    close(fd);
    exit(ret);
}

