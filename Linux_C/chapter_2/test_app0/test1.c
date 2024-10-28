/*
    此文件使用标准IO的方式对文件进行读写操作
    并使用fseek进行读写偏移量的移动
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main() {
    char buf[] = "Hello World\r\n";
    FILE *fp = NULL;
    int ret = 0;

    // 打开文件
    fp = fopen("./test1_file", "r+");
    if(NULL == fp) {
        printf("open error");
        exit(-1);
    }

    printf("open file success\r\n");

    // 写入数据
    ret = fwrite(buf, 1, sizeof(buf), fp);
    if(ret < sizeof(buf)) {
        printf("fwrite error");
        fclose(fp);
        exit(-1);
    }

    printf("write file success\r\n");

    // 清除buf缓冲区
    memset(buf, 0, sizeof(buf));

    // 移动文件偏移量
    fseek(fp, 0, SEEK_SET);

    // 读出数据
    ret = fread(buf, 1, sizeof(buf), fp);
    if(ret < sizeof(buf)) {
        printf("fread error\r\n");
        fclose(fp);
        exit(-1);
    }

    // 打印出读到的数据
    printf("buf: %s\r\n", buf);

    // 关闭文件
    fclose(fp);
    exit(-1);
}

