#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "signal.h"


static void sig_handler(int sig, siginfo_t *info, void *context) {
    sigval_t sig_val = info->si_value;
    printf("接收到实时信号：%d\r\n", sig);
    printf("伴随数据为: %d\r\n", sig_val.sival_int);
    printf("PID: %d\r\n", info->si_pid);
    printf("UID: %d\r\n", info->si_uid);
}

int main(int argc, char *argv[]) {
    struct sigaction sig = {0};
    int num = 0;
    int ret = 0;

    /* 判断传参个数 */
    if(2 > argc) {
        exit(-1);
    }

    /* 获取用户传递的参数 */
    num = atoi(argv[1]);

    /* 为实时信号绑定处理数据 */
    sig.sa_sigaction = sig_handler;
    sig.sa_flags = SA_SIGINFO;
    ret = sigaction(num, &sig, NULL);
    if(-1 == ret) {
        perror("sigaction error");
        exit(-1);
    }

    /* 死循环 */
    for(;;) {
        sleep(1);
    }

    exit(0);
}
