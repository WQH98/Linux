#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "signal.h"

static void sig_handler(int sig) {
    puts("alerm timeout");
    exit(0);
}

int main(int argc, char *argv[]) {
    struct sigaction sig = {0};
    int ret = 0;
    int second = 0;

    /* 校验传参个数 */
    if(2 > argc) {
        printf("未接收到传参，结束程序\r\n");
        exit(-1);
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
    printf("定时时长：%d秒\r\n", second);
    alarm(second);

    /* 循环 */
    for(;;) {
        sleep(1);
    }

    exit(0);
}

