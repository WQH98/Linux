/* 示例代码4.9.5 将标准输出配置为无缓冲模式 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    /* 将标准输出设置为无缓冲模式 */
    if(setvbuf(stdout, NULL, _IONBF, 0)) {
        perror("setvbuf error");
        exit(-1);
    }
    printf("Hello World\r\n");
    printf("Hello World");
    for(;;) {
        sleep(1);
    }
    exit(0);
}