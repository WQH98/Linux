/*
    检查或复位状态
    调用fread读取数据时 如果返回值小于参数number所指定的数 表示发生了错误
    或已经达到了文件末尾(文件结束end-of-file) 但fread无法确定具体是哪一种
    情况：在这种情况下 可以通过判断错误标志或end-of-file标志来确定具体情况

    库函数feof用于测试参数stream所指文件的end-of-file标志 
    如果end-of-file标志被设置了 则调用feof函数将返回一个非0值 
    如果end-of-file标志没有被设置 则返回0

    库函数ferror用于测试参数stream所指文件的错误标志
    如果错误标志被设置了 则调用ferror函数将返回一个非零值
    如果错误标志没有被设置 则返回0

    clearerr用于清除end-of-file标志和错误标志 当调用feof或
    ferror校验这些标志以后 通常需要清除这些标志 避免下次校验时
    使用到的是上一次设置的值 此时可以手动调用clearerr函数清除标志

    注意：对于end-of-file标志 除了使用clearerr显示清除之外 
    当调用fseek成功时也会清除文件的enf-of-file标志
*/

#include "stdio.h"
#include "stdlib.h"

int main() {
    FILE *fp = NULL;
    int ret = 0;
    char buf[20] = {0};

    // 打开文件
    fp = fopen("./test1_file", "r");
    if(NULL == fp) {
        perror("fopen error");
        exit(-1);
    }

    printf("open file success\r\n");

    // 将文件读写位置移动到文件末尾
    ret = fseek(fp, 0, SEEK_END);
    if(ret < 0) {
        perror("fseek error");
        fclose(fp);
        exit(-1);
    }

    // 读文件
    ret = fread(buf, 1, 10, fp);
    if(ret < 10) {
        if(feof(fp)) {
            printf("end-of-file flag is ok, file end\r\n");
        }
        clearerr(fp);
    }

    // 关闭文件
    fclose(fp);
    exit(0);
}

