#include "stdio.h"
#include "stdlib.h"
#include "signal.h"


int main(int argc, char *argv[]) {
    union sigval sig_val;
    int pid = 0;
    int sig = 0;
    int ret = 0;

    /* 判断传参个数 */
    if(3 > argc) {
        exit(-1);
    }

    /* 获取用户传递的参数 */
    pid = atoi(argv[1]);
    sig = atoi(argv[2]);
    printf("pid: %d\r\nsignal: %d\r\n", pid, sig);

    /* 发送信号 */
    sig_val.sival_int = 10;     // 伴随数据
    ret = sigqueue(pid, sig, sig_val);
    if(-1 == ret) {
        perror("sigqueue error");
        exit(-1);
    }

    puts("信号发送成功");
    exit(0);
}
