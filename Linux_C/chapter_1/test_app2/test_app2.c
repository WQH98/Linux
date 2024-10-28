#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"

int main() {
    int fd1 = 0;
    // 尝试打开test_file文件 如果不存在则创建 存在就返回错误
    fd1 = open("test_file", O_EXCL | O_CREAT);
    if(fd1 == -1) {
        printf("the test_file is exist\r\n");
        return -1;
    }
    printf("the test_file is not exist, now establish\r\n");
    return 0;
}

