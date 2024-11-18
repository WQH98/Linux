/* waitpid()轮询方式 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "errno.h"

int main(void) {
    int status = 0;
    int ret = 0;
    int i = 0;

    /* 循环创建3个子进程 */
    for(i = 1; i <= 3; i++) {
        ret = fork();
        switch(ret) {
            case -1:
                perror("fork error");
                exit(-1);
            
            /* 子进程 */
            case 0:
                printf("子进程<%d>被创建\r\n", getpid());
                sleep(i);
                exit(i);
            
            /* 父进程 */
            default:
                break;
        }
    }
    
    sleep(1);
    printf("~~~~~~~~~~~\r\n");

    for(;;) {
        ret = waitpid(-1, &status, WNOHANG);
        if(0 > ret) {
            if(ECHILD == errno) {
                exit(0);
            }
            else {
                perror("waipidt error");
                exit(-1);
            }
        }
        else if(0 == ret) {
            continue;
        }
        else {
            printf("回收子进程<%d>，终止状态<%d>\r\n", ret, WEXITSTATUS(status));
        }
    }

    exit(0);
}

