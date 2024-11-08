/* 示例代码5.3.1 fstat函数使用示例 */
#include "stdio.h"
#include "stdlib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
    struct stat st;
    int ret = 0;
    int fd = 0;

    fd = open("./test1_file.txt", O_RDONLY);
    if(0 > fd) {
        perror("open error");
        exit(-1);
    }

    ret = fstat(fd, &st);
    if(0 > ret) {
        perror("fstst error");
        close(fd);
        exit(-1);
    }

    close(fd);
    exit(0);
}
