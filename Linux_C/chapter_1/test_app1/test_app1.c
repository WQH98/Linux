#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"

int main() {
    char buf[1024];
    int fd1 = 0, fd2 = 0;
    // 以只读方式打开文件
    fd1 = open("src_fire", O_RDONLY);
    if(fd1 == -1) {
        return -1;
    }
	
    // 使用只写方式创建文件
    // 文件所属者有读写执行权限 其他的只有读权限
    fd2 = open("dest_file", O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
    if(fd2 == -1) {
        return -1;
    }

    // 偏移头部500个字节
    off_t off = lseek(fd1, 500, SEEK_SET);
    if(off == -1) {
        return -1;
    }

    // 读1K个字节出来
    ssize_t size = read(fd1, buf, sizeof(buf));
    if(size == -1) {
        return -1;
    }

    // 写1K个字节进去
    size = write(fd2, buf, size);
    if(size == -1) {
        return -1;
    }

    // 关闭文件
    close(fd1);
    close(fd2);
    
    return 0;
}


