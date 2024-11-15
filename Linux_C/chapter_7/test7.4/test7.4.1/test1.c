#include "stdio.h"
#include "stdlib.h"
#include "time.h"

int main(void) {
    int random_number_arr[8];
    int count;

    /* 设置随机数种子 */
    srand(time(NULL));

    /* 生成伪随机数 */
    for(count = 0; count < 8; count++) {
        random_number_arr[count] = rand() % 100;
    }

    /* 打印伪随机数组 */
    printf("[");
    for (count = 0; count < 8; count++) {
        printf("%d", random_number_arr[count]);
        if (count != 8 - 1) {
            printf(", ");
        }
    }
    printf("]\n");
    exit(0);
}
