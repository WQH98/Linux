/* wait函数使用示例 */
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
        ret = wait(&status);
        if(-1 == ret) {
            if(ECHILD == errno) {
                printf("没有需要回收的子进程\r\n");
                exit(0);
            }
            else {
                perror("wait error");
                exit(-1);
            }
        }
        printf("回收子进程<%d>，终止状态<%d>\r\n", ret, WEXITSTATUS(status));
    }
    exit(0);
}
