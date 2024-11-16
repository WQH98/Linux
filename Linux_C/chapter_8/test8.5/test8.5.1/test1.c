#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "signal.h"

int main(int argc, char *argv[]) {
    int pid = 0;
    int ret = 0;

    /* 判断传参个数 */
    if(2 > argc) {
        exit(-1);
    }

    /* 将传入的字符串转为整形数字 */
    pid = atoi(argv[1]);
    printf("pid: %d\r\n", pid);

    /* 向pid指定的进程发送信号 */
    ret = kill(pid, SIGINT);
    if(-1 == ret) {
        perror("kill error");
        exit(-1);
    }

    exit(0);
}
