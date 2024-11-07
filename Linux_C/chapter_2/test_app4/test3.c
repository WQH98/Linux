/* 示例代码4.9.6 使用fflush()刷新stdio缓冲区 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    printf("Hello World\r\n");
    printf("Hello World");
    /* 刷新标准输出stdio缓冲区 */
    fflush(stdout);
    for(;;) {
        sleep(1);
    }
    exit(0);
}
