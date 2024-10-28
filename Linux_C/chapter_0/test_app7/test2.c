/*
    dup函数系统调用分配的文件描述符是由系统分配的 
    遵循文件描述符分配原则 并不能自己指定一个文件
    描述符 而dup2函数可以手动指定文件描述符 
    而不需要文件描述符分配原则
    注意：文件描述符并不是只能复制一次 实际上
    可以对同一个文件描述符fd调用dup或dup2
    函数复制多次 得到多个不同的文件描述符
*/

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


int main() {
    int fd1 = 0, fd2 = 0;
    int ret = 0;

    // 打开一个文件
    fd1 = open("test_file", O_RDWR);
    if(-1 == fd1) {
        perror("open error");
        exit(-1);
    }

    // 复制这个文件描述符
    fd2 = dup2(fd1, 100);
    if(-1 == fd2) {
        perror("dup2 error");
        ret = -1;
        goto err1;
    }

    printf("fd1: %d\tfd2: %d\r\n", fd1, fd2);

    ret = 0;
    close(fd2);

err1:
    // 关闭文件
    close(fd1);
    exit(ret);
}

