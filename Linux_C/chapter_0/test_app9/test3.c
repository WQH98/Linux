/*
    使用系统调用truncate()或ftruncate()可将普通文件截断
    为指定长度文件
    两个函数的区别是ftruncate是通过文件描述符fd来指定目标文件
    而 truncate是通过使用文件路径path来指定目标文件
*/

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main() {
    int fd = 0;
    int ret = 0;
    
    // 打开test3_file1文件
    fd = open("./test3_file1", O_RDWR);
    if(-1 == fd) {
        perror("open error");
        exit(-1);
    }

    // 使用ftruncate截断文件
    ret = ftruncate(fd, 0);
    if(-1 == ret) {
        perror("ftruncate error");
        ret = -1;
        goto err;
    }

    // 使用truncate截断文件
    ret = truncate("./test3_file2", 1024);
    if(-1 == ret) {
        perror("truncate error");
        goto err;
    }

    ret = 0;
err:
    close(fd);
    exit(ret);
}

