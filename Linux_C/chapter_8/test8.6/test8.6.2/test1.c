#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"

static void sig_handler(int sig) {
    puts("alerm timeout");
}

int main(int argc, char *argv[]) {
    struct sigaction sig = {0};
    int second = 0;
    int ret = 0;

    /* 校验传参个数 */
    if(2 > argc) {
        printf("未接收到函数传参，结束程序\r\n");
        exit(0);
    }

    /* 为SIGALRM信号绑定处理函数 */
    sig.sa_handler = sig_handler;
    sig.sa_flags = 0;
    ret = sigaction(SIGALRM, &sig, NULL);
    if(-1 == ret) {
        perror("sigaction error");
        exit(-1);
    }

    /* 启动alerm定时器 */
    second = atoi(argv[1]);
    printf("定时时长: %d秒\r\n", second);
    alarm(second);

    /* 进入休眠状态 */
    pause();
    puts("休眠结束");

    exit(0);
}