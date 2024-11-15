#include "stdio.h"
#include "stdlib.h"
#include "time.h"

void main(void) {
    char tm_str[100] = {0};
    time_t t;

    /* 获取时间 */
    t = time(NULL);
    if(-1 == t) {
        perror("time error");
        exit(t);
    }

    /* 将时间转换为字符串形式 */
    ctime_r(&t, tm_str);

    /* 打印输出 */
    printf("当前时间: %s\r\n", tm_str);

    exit(0);
}
