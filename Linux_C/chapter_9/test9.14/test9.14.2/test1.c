/* 使用文件锁实现单例模式运行 */
#include "stdio.h"
#include "stdlib.h"
#include "sys/file.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "string.h"

#define LOCK_FILE       "./testAPP.pid"

int main(void) {
    char str[20] = {0};
    int fd = 0;

    /* 打开lock文件，如果文件不存在则创建 */
    fd = open(LOCK_FILE, O_WRONLY | O_CREAT, 0666);
    if(-1 == fd) {
        perror("open error");
        exit(-1);
    }

    /* 以非阻塞方式获取文件锁 */
    if(-1 == flock(fd, LOCK_EX | LOCK_NB)) {
        fputs("不能重复执行该程序\n", stderr);
        close(fd);
        exit(-1);
    }

    puts("程序运行中...");

    ftruncate(fd, 0);   // 将文件长度截断为0
    sprintf(str, "%d\n", getpid());
    write(fd, str, strlen(str));        // 写入pid

    for(;;) {
        sleep(1);
    }

    exit(0);
}
