#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"

static void sig_handler(int sig) {
    printf("received signal: %d\r\n", sig);
}

int main(int argc, char *argv[]) {
    struct sigaction sig = {0};
    int ret = 0;

    sig.sa_handler = sig_handler;
    sig.sa_flags = 0;

    ret = sigaction(SIGINT, &sig, NULL);
    if(-1 == ret) {
        perror("sigaction error");
        exit(-1);
    }

    for(;;) {
        /* 向自身发送SIGINT信号 */
        ret = raise(SIGINT);
        if(0 != ret) {
            printf("raise error\r\n");
            exit(-1);
        }

        sleep(3);       // 每隔3秒发送一次
    }

    exit(0);
}
