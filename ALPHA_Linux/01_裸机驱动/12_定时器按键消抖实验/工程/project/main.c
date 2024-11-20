#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "main.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"
#include "bsp_exti.h"  
#include "bsp_epit.h"
#include "bsp_keyfilter.h"

int main(void) {    
    unsigned char state = OFF;

    int_init();             // 初始化中断
    imx6u_clk_init();       // 初始化系统时钟
    clk_enable();           // 使能外设时钟
    led_init();             // 初始化LED
    beep_init();            // 初始化蜂鸣器
    key_init();             // 初始化按键
    keyfilter_init();       // 初始化按键过滤

    while(1) {
        state = !state;
        led_switch(LED0, state);
        delay_ms(500);
    }
    return 0;
}
