/* 示例代码4.9.7 关闭标准输出 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    printf("Hello World\r\n");
    printf("Hello World");
    /* 关闭标准输出 */
    fclose(stdout);
    for(;;) {
        sleep(1);
    }
    exit(0);
}
