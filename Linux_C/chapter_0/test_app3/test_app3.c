/*
    O_APPEND标志
    如果调用open函数打开文件时 携带了O_APPEND标志
    每次使用write函数对文件进行写操作时候 都会自动把文件当前的位置偏移量移动到文件末尾
    从文件末尾开始写入数据 也就是意味着每次写入数据都是从文件末尾开始的

    注意：
    1、O_APPEND标志并不会影响到读文件时的偏移量 即使使用了O_APPEND标志位
    读文件的偏移量也是在文件开头开始的
    2、当使用了O_APPEND标志位 通过lseek函数也无法修改写文件时的偏移量
    写文件时偏移量也是在文件末尾开始
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void) {
    char buf[16] = {0};
    int fd = 0;
    int ret = 0;

    // 打开文件
    fd = open("test_file", O_RDWR | O_APPEND);
    if(-1 == fd) {
        perror("open error");
        goto err;
    }

    // 初始化buf中的文件
    memset(buf, 0x55, sizeof(buf));

    // 写入数据 写入4个字节的数据
    ret = write(fd, buf, 4);
    if(-1 == ret) {
        perror("write error");
        goto err;
    }

    // 将buf清零
    memset(buf, 0, sizeof(buf));

    // 将位置移动到末尾前4个字节处
    ret = lseek(fd, -4, SEEK_END);
    if(-1 == ret) {
        perror("lseek error");
        goto err;
    }

    // 读取数据
    ret = read(fd, buf, 4);
    if(-1 == ret) {
        perror("read error");
        goto err;
    }

    printf("0x%x 0x%x 0x%x 0x%x\r\n", buf[0], buf[1], buf[2], buf[3]);
    
    ret = 0;
err:
    // 关闭文件
    close(fd);
    exit(ret);

}
