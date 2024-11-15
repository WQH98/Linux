#include "stdio.h"
#include "stdlib.h"
#include "time.h"

int main(void) {
    struct tm local_t;
    char tm_str[100];
    time_t sec;

    /* 获取时间 */
    sec = time(NULL);
    if(-1 == sec) {
        perror("time error");
        exit(sec);
    }

    localtime_r(&sec, &local_t);
    asctime_r(&local_t, tm_str);
    printf("本地时间：%s\r\n", tm_str);

    exit(0);
}
