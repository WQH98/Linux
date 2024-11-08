#include "stdio.h"
#include "stdlib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

int main(void) {
    int ret = 0;
    struct stat st;
    struct tm file_tm;

    char time[128] = {0};
    
    ret = stat("./test3_file.txt", &st);
    if(0 < ret) {
        perror("stat error");
        exit(-1);
    }
    localtime_r(&st.st_atime, &file_tm);
    strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", &file_tm);
    printf("文件最后被访问的时间: %s\n", time);

    localtime_r(&st.st_mtime, &file_tm);
    strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", &file_tm);
    printf("文件内容最后被修改的时间: %s\n", time);

    localtime_r(&st.st_ctime, &file_tm);
    strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", &file_tm);
    printf("文件状态最后被改变的时间: %s\n", time);

    exit(0);
}

