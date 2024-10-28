#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"

int main() {
    int fd1 = 0;
    // 只读方式打开文件
    fd1 = open("test_file", O_RDONLY);
    __off_t off;
    off = lseek(fd1, 0, SEEK_END) - lseek(fd1, 0, SEEK_SET);
    printf("off = %d\r\n", (int)off);
    close(fd1);
    return 0;
}
