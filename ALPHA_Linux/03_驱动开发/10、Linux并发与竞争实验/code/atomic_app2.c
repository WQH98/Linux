#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"

/* 
    argc：应用程序参数个数
    argv：应用程序参数，字符串形式
    ./atomic_app2 <filename> <1:0>   1表示打开LED，0表示关闭LED
    ./atomic_app2 /dev/atomic 1      打开LED
    ./atomic_app2 /dev/atomic 0      关闭LED
 */
int main(int argc, char *argv[]) {
    
    int fd = 0, ret = 0;
    char buf_data[1] = {0};
    char *filename = argv[1];


    /* 检查参数个数是否正确 */
    if(argc != 3) {
        printf("error usage\r\n");
        return -1;
    }

    /* 打开文件 */
    ret = open(filename, O_RDWR);
    /* 判断文件是否打开 */
    if(ret < 0) {
        printf("file to open %s\r\n", filename);
        return -1;
    }

    /* 向文件中写值 */
    buf_data[0] = atoi(argv[2]);
    ret = write(fd, buf_data, sizeof(buf_data));
    /* 判断是否写入成功 */
    if(ret < 0) {
        printf("fail to write data\r\n");
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}

