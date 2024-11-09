/* 示例代码 5.5.1 access 函数使用示例 */

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#define MY_FILE  "./test1_file.txt"

int main(void) {
    int ret = 0;

    /* 检查文件是否存在 */
    ret = access(MY_FILE, F_OK);
    if(0 > ret) {
        printf("%s file does not exist\r\n", MY_FILE);
        exit(-1);
    }
    else {
        printf("%s file exist\r\n", MY_FILE);
    }

    /* 检查写权限 */
    ret = access(MY_FILE, W_OK);
    if(0 > ret) {
        printf("write permission no\r\n");
    }
    else {
        printf("write permission yes\r\n");
    }

    /* 检查读权限 */
    ret = access(MY_FILE, R_OK);
    if(0 > ret) {
        printf("read permission no\r\n");
    }
    else {
        printf("read permission yes\r\n");
    }

    /* 检查执行权限 */
    ret = access(MY_FILE, X_OK);
    if(0 > ret) {
        printf("execute permission no\r\n");
    }
    else {
        printf("execute permission yes\r\n");
    }

    exit(0);
}
