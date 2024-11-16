#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"

static void sig_handler(int sig) {
    printf("received sig: %d\r\n", sig);
}

int main(int argc, char *argv[]) {
    struct sigaction sig = {0};
    int ret = 0;

    sig.sa_handler = sig_handler;
    sig.sa_flags = 0;

    ret = sigaction(SIGABRT, &sig, NULL);
    if(-1 == ret) {
        perror("sigaction error");
        exit(-1);
    }

    sleep(2);
    abort();
    for(;;) {
        sleep(1);
    }

    exit(0);
}
