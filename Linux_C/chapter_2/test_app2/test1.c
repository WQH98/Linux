/* 示例代码4.8.1 格式化输出函数使用练习 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>


int main(void) {
    char buf[50] = {0};
    printf("%d(%s)%d(%s)\r\n", 111, "test", 222, "测试");

    fprintf(stdout, "%d(%s)%d(%s)\r\n", 111, "test", 222, "测试");

    dprintf(STDOUT_FILENO, "%d(%s)%d(%s)\r\n", 111, "test", 222, "测试");

    sprintf(buf, "%d(%s)%d(%s)\r\n", 111, "test", 222, "测试");
    printf("%s", buf);

    memset(buf, 0x00, sizeof(buf));
    snprintf(buf, sizeof(buf), "%d(%s)%d(%s)\r\n", 111, "test", 222, "测试");
    printf("%s", buf);

    exit(0);
}

