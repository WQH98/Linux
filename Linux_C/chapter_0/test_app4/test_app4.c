/*
    当你多次打开同一个文件时 返回的文件描述符是不一样的
    一个进程内多次调用open打开同一个文件 就会得到多个不同的文件描述符fd
    同理在关闭文件的时候也需要调用close依次关闭多个文件描述符
    调用open函数时使用的什么权限 则返回的文件描述符就有什么权限
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int fd1 = 0, fd2 = 0, fd3 = 0;
    int ret = 0;

    // 第一次打开文件
    fd1 = open("test_file", O_RDWR);
    if(-1 == fd1) {
        perror("open error");
        exit(-1);
    }

    // 第二次打开文件
    fd2 = open("test_file", O_RDWR);
    if(-1 == fd2) {
        perror("open error");
        ret = -1;
        goto err1;
    }

    // 第三次打开文件
    fd3 = open("test_file", O_RDWR);
    if(-1 == fd3) {
        perror("open error");
        ret = -1;
        goto err2;
    }

    // 打印出三个文件描述符
    printf("%d %d %d\r\n", fd1, fd2, fd3);

    close(fd3);

err2:
    close(fd2);

err1:
    // 关闭文件
    close(fd1);
    exit(ret);

}

