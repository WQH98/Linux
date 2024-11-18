/* 利用信号来调整进程间动作 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "signal.h"
#include "sys/types.h"

static void sig_handler(int sig) {
    printf("接收到信号\r\n");
}

int main(int argc, char *argv[]) {
    struct sigaction sig = {0};
    sigset_t wait_mask;
    int ret = 0;

    /* 初始化信号集 */
    sigemptyset(&wait_mask);

    /* 设置信号处理方式 */
    sig.sa_handler = sig_handler;
    sig.sa_flags = 0;
    ret = sigaction(SIGUSR1, &sig, NULL);
    if(-1 == ret) {
        perror("sigaction error");
        exit(-1);
    }

    switch(fork()) {
        case -1:
            perror("fork error");
            exit(-1);
        /* 子进程 */
        case 0:
            printf("子进程开始执行\r\n");
            printf("子进程打印信息\r\n");
            printf("~~~~~~~~~~~~\r\n");
            sleep(2);
            kill(getppid(), SIGUSR1);       // 发送信号给父进程，唤醒父进程
            exit(0);
        /* 父进程 */
        default:
            ret = sigsuspend(&wait_mask);   // 挂起、阻塞
            if(-1 != ret) {
                exit(-1);
            }
            printf("父进程开始执行\r\n");
            printf("父进程打印信息\r\n");
            exit(0);
    }
}
