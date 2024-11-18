/* 异步方式监视wait回收子进程 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "signal.h"
#include "sys/types.h"
#include "sys/wait.h"

static void wait_child(int sig) {
    /* 替子进程收尸 */
    printf("父进程回收子进程\r\n");
    while(waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
}

int main(void) {
    struct sigaction sig = {0};

    /* 为SIGCHLD信号绑定处理函数 */
    sigemptyset(&sig.sa_mask);
    sig.sa_handler = wait_child;
    sig.sa_flags = 0;
    if(-1 == sigaction(SIGCHLD, &sig, NULL)) {
        perror("sigaction error");
        exit(-1);
    }

    /* 创建子进程 */
    switch(fork()) {
        // 创建失败
        case -1:
            perror("fork error");
            exit(-1);
        
        // 子进程
        case 0:
            printf("子进程<%d>被创建\r\n", getpid());
            sleep(1);
            printf("子进程结束\r\n");
            exit(0);

        // 父进程
        default:
            break;
    }

    sleep(3);
    exit(0);
}
