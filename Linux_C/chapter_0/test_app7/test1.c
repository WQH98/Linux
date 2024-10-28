/*
    在Linux系统中 open返回得到的文件描述符fd可以进行复制 
    复制成功之后可以得到一个新的文件描述符 使用新的文件描述符
    和旧的文件描述符都可以对文件进行IO操作 复制得到的文件
    描述符和旧的文件描述符拥有相同的权限 
    在Linux系统中 可以使用dup或dup2这两个系统调用对文件
    描述符进行复制
    由于复制得到的文件描述符与源文件描述符都指向同一个文件表
    所以他们的文件读写偏移量是一样的 就可以在不使用O_APPEND
    标志的情况下 通过文件描述符复制来实现连续写
*/

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


int main() {
    unsigned char buffer1[4] = {0}, buffer2[4] = {0};
    int fd1 = 0, fd2 = 0;
    int ret = 0;

    // 创建新的test_file并打开
    fd1 = open("test_file", O_RDWR | O_CREAT | O_EXCL,
                            S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
    if(-1 == fd1) {
        perror("open error");
        exit(-1);
    }

    // 复制一份文件描述符
    fd2 = dup(fd1);
    if(-1 == fd2) {
        perror("dup error");
        ret = -1;
        goto err1;
    }

    // 打印文件描述符的值
    printf("fd1: %d\tfd2: %d\r\n", fd1, fd2);

    // buffer初始化
    buffer1[0] = 0x11;
    buffer1[1] = 0x22;
    buffer1[2] = 0x33;
    buffer1[3] = 0x44;

    buffer2[0] = 0xAA;
    buffer2[1] = 0xBB;
    buffer2[2] = 0xCC;
    buffer2[3] = 0xDD;

    // 循环写入数据
    for(int i = 0; i < 4; i++) {
        ret = write(fd1, buffer1, sizeof(buffer1));
        if(-1 == ret) {
            perror("write error");
            goto err2;
        }
        ret = write(fd2, buffer2, sizeof(buffer2));
        if(-1 == ret) {
            perror("write error");
            goto err2;
        }
    }

    // 将偏移量移动到文件开头
    ret = lseek(fd1, 0, SEEK_SET);
    if(-1 == ret) {
        perror("lseek error");
        goto err2;
    }

    // 清空buffer1缓冲区
    memset(buffer1, 0, sizeof(buffer1));

    // 读取数据
    for(int i = 0; i < 8; i++) {
        ret = read(fd1, buffer1, sizeof(buffer1));
        if(-1 == ret) {
            perror("read error");
            goto err2;
        }

        printf("0x%x 0x%x 0x%x 0x%x ", buffer1[0], buffer1[1],
                                       buffer1[2], buffer1[3]);
        memset(buffer1, 0, sizeof(buffer1));
    }

    ret = 0;
err2:
    close(fd2);

err1:
    // 关闭文件
    close(fd1);
    exit(ret);
}

