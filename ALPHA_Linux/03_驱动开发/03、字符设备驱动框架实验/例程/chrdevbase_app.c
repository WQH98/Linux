#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"

/*
    argc：应用程序参数个数
    argv：应用程序参数,字符串形式
    ./chrdevbase_app <filename> <1:2> 1表示读 2表示写
    ./chrdevbase_app /dev/chrdevbase 1    表示从驱动里面读数据
    ./chrdevbase_app /dev/chrdevbase 2    表示向驱动里面写数据
 */
int main(int argc, char *argv[]) {

    int fd = 0, ret = 0;
    char read_buf[100];
    char write_buf[100];
    char *filename = argv[1];
    static char app_data[] = {"app data!"};
    
    if(argc != 3) {
        printf("error usage!\r\n");
        return -1;
    }

    fd = open(filename, O_RDWR);
    if(fd < 0) {
        printf("can't open file %s\r\n", filename);
        return -1;
    }

    /* 读操作 */
    if(atoi(argv[2]) == 1) {
        /* read */
        ret = read(fd, read_buf, 50);
        if(ret < 0) {
            printf("read file %s failed!\r\n", filename);
        }
        else {
            printf("app read data: %s\r\n", read_buf);
        }
    }

    /* 写操作 */
    if(atoi(argv[2]) == 2) {
        memcpy(write_buf, app_data, sizeof(app_data));
        /* write */
        ret = write(fd, write_buf, 50);
        if(ret < 0) {
            printf("write file %s failed!\r\n", filename);
        }
        else {
            printf("app write data: %s\r\n", write_buf);
        }
    }


    /* close */
    ret = close(fd);
    if(ret < 0) {
        printf("close file %s failed!\r\n", filename);
    }

    return 0;
}
