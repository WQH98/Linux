/*
    ftell可用于获取当前读写位置的偏移量
    成功则返回当前读写位置偏移量
    失败返回-1
    可以通过fseek和ftell来计算出文件大小
*/

#include "stdio.h"
#include "stdlib.h"

int main() {
    FILE *fp = NULL;
    int ret = 0;

    // 打开文件
    fp = fopen("./test2_file", "r");
    if(NULL == fp) {
        perror("fopen error");
        exit(-1);
    }

    printf("open file success\r\n");

    // 将读写位置移动到文件末尾
    ret = fseek(fp, 0, SEEK_END);
    if(-1 == ret) {
        perror("fseek error");
        fclose(fp);
        exit(ret);
    }

    // 获取当前位置的偏移量
    ret = ftell(fp);
    if(-1 == ret) {
        perror("ftell error");
        fclose(fp);
        exit(ret);
    }

    printf("the size of file is %d\r\n", ret);

    // 关闭文件
    fclose(fp);
    exit(0);
}

