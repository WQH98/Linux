#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "main.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"

int main(void) {    

    int_init();             // 初始化中断
    imx6u_clk_init();       // 初始化系统时钟
    delay_init();           // 初始化延时
    clk_enable();           // 使能外设时钟
    led_init();             // 初始化LED
    beep_init();            // 初始化蜂鸣器
    key_init();             // 初始化按键
    
    while(1) {
        
        delay_ms(500);
    }
    return 0;
}
