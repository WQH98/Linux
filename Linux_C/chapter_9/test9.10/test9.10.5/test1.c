/* 产生僵尸进程 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int main(void) {
    /* 创建子进程 */
    switch(fork()) {
        case -1:
            perror("fork error");
            exit(-1);

        // 子进程
        case 0:
            printf("子进程<%d>被创建\n", getpid());
            sleep(1);
            printf("子进程结束\n");
            exit(0);
        // 父进程 
        default:
            break;
    }
    
    for(;;) {
        sleep(1);
    } 
 
    exit(0);
}

