#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "main.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"
#include "bsp_exti.h"


int main(void) {
    unsigned char led_state = OFF;
    // 初始化中断
    int_init();
    // 初始化系统时钟
    imx6u_clk_init();
    // 使能外设时钟
    clk_enable();
    // 初始化LED
    led_init();
    // 初始化蜂鸣器
    beep_init();
    // 初始化按键
    key_init();
    // 初始化外部中断
    exti_init();
    while(1) {
        led_state = !led_state;
        led_switch(LED0, led_state);
        delay_ms(500);
    }
    return 0;
}
