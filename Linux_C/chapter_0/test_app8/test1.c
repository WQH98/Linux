/*
    原子操作 是有多步骤操作组成的一个操作
    原子操作要么一步也不执行 一旦执行 就必须要
    执行完全部的步骤 不可能只执行所有步骤中的一个子集
    pread和pwrite函数是系统调用 与read write函数的
    作用一样 用于读取和写入数据 
    区别在于 pread和pwrite可以实现原子操作 调用时候可以
    加一个位置偏移量offset参数
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
    int fd = 0;
    int ret = 0;
    // 打开文件
    fd = open("./test_file", O_RDWR);
    if(-1 == fd) {
        perror("open error");
        exit(-1);
    }
    // 使用pread函数读取数据(从偏移文件头1024字节处开始读取)
    ret = pread(fd, buffer, sizeof(buffer), 1024);
    if(-1 == ret) {
        perror("pread error");
        goto err;
    }

    // 获取当前文件的偏移量
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

/*
    在当前目录下存在一个名为test_file的文件 上述代码中会打开
    test_file文件 并且使用pread函数读取文件的100字节数据
    从文件偏移头1024字节处 读取完成之后再使用lseek函数获取到
    文件当前位置偏移量 并将其打印出来
    加入pread函数会改变文件表中记录的当前位置偏移量 则打印出来
    的数据应该是1024+100=1124 如果不会改变文件表中记录的当前
    位置偏移量 则打印出来的数据应该是0
*/

