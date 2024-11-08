#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>


int main(void) {
    int ret = 0;
    struct stat st;
    ret = stat("./test1_file.txt", &st);
    if(0 < ret) {
        perror("stat error");
        exit(-1);
    }
    
    printf("st_ino: %ld\r\n", st.st_ino);
    printf("st_size: %ld\r\n", st.st_size);

    exit(0);
}


