/* waitpid()阻塞方式 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "errno.h"

int main(int argc, char *argv[]) {
    int status = 0;
    int ret = 0;
    int i = 0;
    
    /* 循环创建三个子进程 */
    for(i = 1; i <= 3; i++) {
        switch(fork()) {
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

    for(i = 1; i <= 3; i++) {
        ret = waitpid(-1, &status, 0);    // == wait(&status);
        if(-1 == ret) {
            if(ECHILD == errno) {
                printf("没有需要等待回收的子进程\r\n");
                exit(0);
            }
            else {
                perror("fork error");
                exit(-1);
            }
        }
        printf("回收子进程<%d>，终止状态<%d>\r\n", ret, WEXITSTATUS(status));
    }
    exit(0);
}
