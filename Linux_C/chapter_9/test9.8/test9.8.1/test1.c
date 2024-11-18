/* fork竞争条件测试代码 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int main(int argc, char *argv[]) {
    switch(fork()) {
        case -1:
            perror("fork error");
            exit(-1);
        // 子进程
        case 0:
            printf("子进程打印信息\r\n");
            exit(0);
        // 父进程
        default:
            printf("父进程打印信息\r\n");
            exit(0);
    }
    exit(0);
}

