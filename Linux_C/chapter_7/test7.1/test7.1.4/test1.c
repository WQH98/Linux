#include "stdio.h"
#include "stdlib.h"
#include "sys/sysinfo.h"

int main(void) {
    struct sysinfo sys_info;
    int ret = 0;

    /* 获取信息 */
    ret = sysinfo(&sys_info);
    if(-1 == ret) {
        perror("sysinfo error");
        exit(-1);
    }

    /* 打印信息 */
    printf("uptime: %ld\r\n", sys_info.uptime);         // 自系统启动之后所经过的时间（以秒为单位）
    printf("totalram: %lu\r\n", sys_info.totalram);     // 还未被使用的内存大小
    printf("freeram: %lu\r\n", sys_info.freeram);       // 还未被使用的内存大小
    printf("procs: %u\r\n", sys_info.procs);            // 系统当前进程数量
    
    exit(0);
}
