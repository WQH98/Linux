/* 示例代码4.9.3 普通I/O方式 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static char buf[8192];

int main(void) {
    int fd;
    int count;

    /* 打开文件 */
    fd = open("./test3_file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if(0 > fd) {
        perror("open error");
        exit(-1);
    }

    /* 写文件 */
    count = 10000;
    /* 循环10000次， 每次写入4096个字节数据 */
    while(count--) {
        if(4096 != write(fd, buf, 4096)) {
            perror("write error");
            close(fd);
            exit(-1);
        }
    }

    /* 关闭文件并退出程序 */
    close(fd);
    exit(0);
}

