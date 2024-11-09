/* 示例代码5.5.2 chmod函数使用示例 */

#include "stdio.h"
#include "stdlib.h"
#include "sys/stat.h"

#define MY_FILE "./test1_file.txt" 

int main(void) {
    int ret = 0;
    ret = chmod(MY_FILE, 0777);
    if(-1 == ret) {
        perror("chmod error");
        exit(-1);
    }

    exit(0);
}
