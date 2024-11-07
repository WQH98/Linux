/* 示例代码4.9.4 printf()输出测试 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* 标准输出默认采用的是行缓冲模式 只有输出换行符时才会将这一行数据输入到内核缓冲区
   也就是写入到标准输出文件（终端设备）
*/
int main(void) {
    printf("Hello World\r\n");
    printf("Hello World");
    for(;;) {
        sleep(1);
    }
    exit(0);
}
