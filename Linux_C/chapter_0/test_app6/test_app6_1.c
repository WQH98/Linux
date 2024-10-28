/*
    重复打开同一个文件 比如调用两次open函数打开同一个文件
    分别得到两个文件描述符fd1和fd2 使用这两个文件描述符
    对文件进行写操作 由此可见 两个文件是分别写的 fd1描述符
    的偏移量跟fd2描述符的偏移量不同步 所以在写的时候 fd2
    写入的内容总会覆盖掉fd1写入的内容
*/

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main() {
    unsigned char buffer1[4] = {0};
    unsigned char buffer2[4] = {0};
    int fd1 = 0, fd2 = 0;
    int ret = 0;

    // 创建文件test_file并打开
    fd1 = open("test_file", O_RDWR| O_CREAT| O_EXCL,
                            S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
    if(fd1 == -1) {
        perror("open1 error");
        exit(-1);
    }

    // 再次打开test_file文件
    fd2 = open("test_file", O_RDWR);
    if(fd2 == -1) {
        perror("open2 error");
        ret = -1;
        goto err1;
    }

    // buffer数据初始化
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

    // 将读写位置偏移量移动到文件头
    ret = lseek(fd1, 0, SEEK_SET);
    if(-1 == ret) {
        perror("lseek error");
        goto err2;
    }

    // 清除buffer1缓冲区中的内容
    memset(buffer1, 0, sizeof(buffer1));

    // 读数据
    for(int i = 0; i < 8; i++) {
        ret = read(fd1, buffer1, sizeof(buffer1));
        if(-1 == ret) {
            perror("rad error");
            goto err2;
        }
        printf("%x %x %x %x ", buffer1[0], buffer1[1],
                               buffer1[2], buffer1[3]);
    }
    printf("\r\n");

    ret = 0;
err2:
    close(fd2);

err1:
    // 关闭文件
    close(fd1);
    exit(ret);

}

