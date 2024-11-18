/* 孤儿进程测试 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int main(void) {
    /* 创建子进程 */
    switch(fork()) {
        case -1:
            perror("fork error");
            exit(-1);
        case 0:
            printf("子进程<%d>被创建，父进程<%d>\r\n", getpid(), getppid());
            sleep(3);
            printf("父进程<%d>\r\n", getppid());
            exit(0);
        default:
            break;
    }
    sleep(1);
    printf("父进程结束\r\n");

    exit(0);
}
