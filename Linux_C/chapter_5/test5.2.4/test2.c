#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(void) {
    int ret = 0;
    struct stat st;
    unsigned int mode = 0;

    ret = stat("./test2_file.txt", &st);
    if(0 < ret) {
        perror("stat error");
        exit(-1);
    }

    mode = st.st_mode;

    if((mode & S_IROTH) && (mode & S_IWOTH)) {
        printf("文件对于其他用户有可读写权限\r\n");
    }
    else {
        if(mode & S_IROTH) {
            printf("文件对于其他用户有可读权限\r\n");
        }
        else if(mode & S_IWOTH) {
            printf("文件对于其他用户有可写权限\r\n");
        }
        else {
            printf("文件对于其他用户不具有可读写权限\r\n");
        }
    }

    exit(0);
}
