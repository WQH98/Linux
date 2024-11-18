/* 子进程继承父进程文件描述符实现文件共享 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"

int main(int argc, char *argv[]) {
    pid_t pid;
    int fd = 0;
    int i = 0;
    int ret = 0;

    fd = open("./test1_file.txt", O_RDWR | O_TRUNC);
    if(0 > fd) {
        perror("open error");
        exit(-1);
    }

    pid = fork();
    switch(pid) {
        case -1:
            perror("fork error");
            close(fd);
            exit(-1);
        
        /* 子进程 */
        case 0:
            // 写入4次
            for(i = 0; i < 4; i++) {
                write(fd, "1122", 4);
            }
            close(fd);
            _exit(0);

        /* 父进程 */
        default:
            // 写入4次
            for(i = 0; i < 4; i++) {
                write(fd, "AABB", 4);
            }
            close(fd);
            exit(0);
    }

    exit(0);
}

