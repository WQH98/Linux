#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"

static void sig_handler(int sig) {
    printf("执行信号处理函数...\r\n");
}

int main(int argc, char *argv[]) {
    struct sigaction sig = {0};
    sigset_t sig_set;
    int ret = 0;

    /* 注册信号处理函数 */
    sig.sa_handler = sig_handler;
    sig.sa_flags = 0;
    ret = sigaction(SIGINT, &sig, NULL);
    if(-1 == ret) {
        exit(-1);
    }

    /* 信号集初始化 */
    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGINT);

    /* 向信号掩码中添加信号 */
    ret = sigprocmask(SIG_BLOCK, &sig_set, NULL);
    if(-1 == ret) {
        exit(-1);
    }

    /* 向自己发送信号 */
    raise(SIGINT);

    /* 休眠2秒 */
    sleep(2);
    printf("休眠结束\r\n");

    /* 从信号掩码中移除添加的信号 */
    ret = sigprocmask(SIG_UNBLOCK, &sig_set, NULL);
    if(-1 == ret) {
        exit(-1);
    }

    exit(0);
}


