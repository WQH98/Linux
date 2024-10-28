#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"

int main() {
    int fd1 = 0;
    // 尝试打开文件 文件存在的话就打开 不存在就先新建再打开
    fd1 = open("new_file", O_CREAT | O_WRONLY, S_IRWXU | S_IRGRP | S_IROTH);
    if(fd1 == -1) {
        printf("open failed\r\n");
        return -1;
    }
    char buf[1024] = {0};
    memset(buf, 0x00, sizeof(buf));
    ssize_t size = write(fd1, buf, sizeof(buf));
    if(size == -1) {
        printf("write buf failed\r\n");
        return -1;
    }
    memset(buf, 0xFF, sizeof(buf));
    size = write(fd1, buf, sizeof(buf));
    if(size == -1) {
        printf("write buf failed\r\n");
        return -1;
    }
    printf("all ok\r\n");
    close(fd1);
    return 0;
}
