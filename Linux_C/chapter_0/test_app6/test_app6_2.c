/*
    可以使用O_APPEND来将分别写改为连续写
    当open函数使用O_APPEND标志时 再使用write函数进行写入操作时
    会自动将偏移量移动至文件末尾 也就是每次写入都是从文件末尾开始的
*/

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"


int main() {
    unsigned char buffer1[4] = {0}, buffer2[4] = {0};
    int fd1 = 0, fd2 = 0;
    int ret = 0;

    // 创建新文件并打开
    fd1 = open("./test_file", O_RDWR | O_CREAT | O_EXCL | O_APPEND,
                              S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
    if(-1 == fd1) {
        perror("open error");
        exit(-1);
    }

    // 再次打开test_file文件
    fd2 = open("./test_file", O_RDWR | O_APPEND);
    if(-1 == fd2) {
        perror("open error");
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

    // 写数据
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

    // 将读写位置偏移量移动到文件开头
    ret = lseek(fd1, 0, SEEK_SET);
    if(-1 == ret) {
        perror("lseek error");
        goto err2;
    }

    // 将buffer1缓冲区清零
    memset(buffer1, 0, sizeof(buffer1));

    // 开始读文件
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
    printf("\r\n");

    ret = 0;
err2:
    close(fd2);

err1:
    // 关闭文件
    close(fd1);
    exit(ret);

}

