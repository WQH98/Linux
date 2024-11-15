#include "stdio.h"
#include "stdlib.h"
#include "sys/utsname.h"

int main(void) {
    struct utsname os_info;
    int ret = 0;

    /* 获取信息 */
    ret = uname(&os_info);
    if(-1 == ret) {
        perror("uname error");
        exit(-1);
    }

    /* 打印信息 */ 
    printf("操作系统名称：%s\r\n", os_info.sysname);
    printf("主机名：%s\r\n", os_info.nodename);
    printf("内核版本：%s\r\n", os_info.release);
    printf("发行版本：%s\r\n", os_info.version);
    printf("硬件架构：%s\r\n", os_info.machine);

    exit(0);
}

