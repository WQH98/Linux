/* 示例代码5.4.2 chown使用示例2 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

/* 当直接执行此代码时，用户组ID为1000，使用sudo执行代码时，用户组ID为0 */

int main(void) {
    printf("uid: %d\r\n", getuid());

    exit(0);
}
