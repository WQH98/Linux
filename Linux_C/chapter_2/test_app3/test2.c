/* 示例代码4.9.2 直接I/O示例程序 */
/* 使用宏定义O_DIRECT需要在程序中定义宏_GUN_SOURCE，不然提示O_DIRECT找不到 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* 定义一个用于存放数据的buf，起始地址以4096字节进行对齐 */
static char buf[8192] __attribute((aligned(4096)));

int main(void) {
    int fd;
    int count;

    /* 打开文件 */
    fd = open("./test2_file.txt", O_WRONLY | O_CREAT | O_TRUNC | O_DIRECT, 0664);
    if(0 > fd) {
        perror("open error");
        exit(-1);
    }

    /* 写文件 */
    count = 10000;
    while(count--) {
        if(4096 != write(fd, buf, 4096)) {
            perror("write error");
            exit(-1);
        }
    }

    /* 关闭文件并退出程序 */
    close(fd);
    exit(0);
}

