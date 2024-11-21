#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "main.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"
#include "bsp_uart.h"
#include "stdio.h"

int main(void) {    
#if 0
    unsigned char a = 0;
#endif

    int a = 0, b = 0;

    int_init();             // 初始化中断
    imx6u_clk_init();       // 初始化系统时钟
    delay_init();           // 初始化延时
    uart1_init();           // 初始化串口1
    clk_enable();           // 使能外设时钟
    led_init();             // 初始化LED
    beep_init();            // 初始化蜂鸣器
    key_init();             // 初始化按键

    while(1) {
# if 0
        puts("请输入1个字符：");
        a = getc();
        putc(a);
        puts("\r\n");
        
        puts("您输入的字符为：");
        putc(a);
        puts("\r\n");
#endif
        printf("请输入两个整数，使用空格隔开：");
        scanf("%d %d", &a, &b);
        printf("\r\n数据 %d + %d = %d", a, b, a + b);

    }
    return 0;
}
