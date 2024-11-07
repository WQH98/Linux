/* 示例代码4.8.2 scanf()函数使用示例 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    int a = 0;
    float b = 0;
    char str[100] = {0};

    printf("please input one int: \r\n");
    scanf("%d", &a);
    printf("input int is %d\r\n", a);

    printf("please input one float: \r\n");
    scanf("%f", &b);
    printf("input float is %f\r\n", b);

    printf("please input one string: \r\n");
    scanf("%s", str);
    printf("input string is %s\r\n", str);


    exit(0);
}
