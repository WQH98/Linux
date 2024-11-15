#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"

int main(void) {
    char hostname[20];
    int ret = 0;

    memset(hostname, 0x00, sizeof(hostname));
    ret = gethostname(hostname, sizeof(hostname));
    if(-1 == ret) {
        perror("gethostname error");
        exit(ret);
    }

    puts(hostname);
    exit(0);
} 
