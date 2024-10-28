/*
    当调用open函数的时候 会将文件数据（文件内容）从磁盘等块设备
    读取到内存中 将文件数据在内存中进行维护 内存中的这份文件我们把
    它称为动态文件
    如果同一份文件被多次打开 那么该文件所对应的动态文件在内存中只存在一份
    下面的例程中 两次打开同一份文件 第一次从头写4个字节 第二次从头读4个字节
    发现第二次读出来的 就是第一次写进去的

    在一个进程内多次open打开同一个文件 不同文件描述符所对应的读写位置是相互独立的
*/

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main() {
    char buffer[4] = {0};
    int fd1 = 0, fd2 = 0;
    int ret = 0;
    // 创建新文件test_file并打开
    fd1 = open("test_file",
               O_RDWR | O_CREAT | O_EXCL,
               S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(-1 == fd1) {
        perror("open error");
        exit(-1);
    }

    // 再次打开test_file文件
    fd2 = open("test_file", O_RDWR);
    if(-1 == fd2) {
        perror("open error");
        ret = -1;
        goto err;
    }

    // 通过fd1文件描述符写入4个字节数据
    buffer[0] = 0x11;
    buffer[1] = 0x22;
    buffer[2] = 0x33;
    buffer[3] = 0x44;
    ret = write(fd1, buffer, sizeof(buffer));
    if(-1 == ret) {
        perror("write error");
        goto err2;
    }

    // 将读写位置偏移量移动到文件头
    ret = lseek(fd2, 0, SEEK_SET);
    if(-1 == ret) {
        perror("lseek error");
        goto err2;
    }

    // 读取数据
    memset(buffer, 0x00, sizeof(buffer));
    ret = read(fd2, buffer, 4);
    if(-1 == ret) {
        perror("read error");
        goto err2;
    }
    
    printf("0x%x 0x%x 0x%x 0x%x\r\n", buffer[0], buffer[1], 
                                      buffer[2], buffer[3]);

    ret = 0;
err2:
    close(fd2);

err:
    // 关闭文件
    close(fd1);
    exit(ret);
}

