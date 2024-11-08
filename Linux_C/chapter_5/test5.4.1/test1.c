/* 示例代码5.4.1 chown函数使用示例 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int main(void) {
    if(-1 == chown("./test1_file.txt", 1000, 1000)) {
        perror("chown error");
        exit(-1);
    }

    exit(0);
}

