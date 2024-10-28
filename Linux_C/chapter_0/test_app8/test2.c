/*
    此文件用来测试pwrite函数
    如果把pread和pwrite函数换成read和write函数
    则最后打印出来的偏移量就是100了 因为读取或写入
    了100个字节 对应的偏移量就会向后移动100字节
*/

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main() {
    unsigned char buffer[100] = {0};
    int fd = 0, ret = 0;

    // 打开文件 没有文件的话创建
    fd = open("./test_file1", O_RDWR|O_CREAT|O_EXCL, 
                               S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR);
    if(-1 == fd) {
        perror("open error");
        exit(-1);
    }

    // 初始化buffer缓存区
    memset(buffer, 0xFF, sizeof(buffer));

    // 向文件中写数据 从偏移量1024处开始写
    ret = pwrite(fd, buffer, sizeof(buffer), 1024);
    if(-1 == ret) {
        perror("pwrite error");
        goto err;
    }

    // 读取当前的位置偏移量
    ret = lseek(fd, 0, SEEK_CUR);
    if(-1 == ret) {
        perror("lseek error");
        goto err;
    }

    printf("current offset: %d\r\n", ret);

    ret = 0;

err:
    // 关闭文件
    close(fd);
    exit(ret);

}

