/*
    该文件夹用来测试空洞文件
    打开文件hole_file 没有这个文件的话就会新建一个
    从偏移4K处开始写入4K的数据
    程序运行完毕以后 使用ls -l命令可以看到hole_file文件的大小是8K
    使用du -h hole_file命令查看文件时 显示4K
    du命令查看的大小是文件实际占用存储块的大小
*/

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

int main() {
    int fd = 0;
    int ret = 0;
    char buf[1024] = {0};
    int i = 0;
    // 打开文件
    fd = open("hole_file", O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(-1 == fd) {
        perror("open error");
        exit(-1);
    }

    // 将文件读写位置偏移到文件头4096个字节处（4K）
    ret = lseek(fd, 4096, SEEK_SET);
    if(-1 == ret) {
        perror("lseek error");
        goto err;
    }

    // 初始化buffer为0xFF
    memset(buf, 0xFF, sizeof(buf));

    // 循环写入4次 每次写入1K
    for(i = 0; i < 4; i++) {
        ret = write(fd, buf, sizeof(buf));
        if(-1 == ret) {
            perror("write error");
            goto err;
        }
    }

    ret = 0;
// 关闭文件
err:
    close(fd);
    exit(ret);

}

