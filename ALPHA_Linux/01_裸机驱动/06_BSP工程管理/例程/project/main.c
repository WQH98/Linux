#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "main.h"


int main(void) {
    // 使能外设时钟
    clk_enable();
    // 初始化LED
    led_init();
    while(1) {
        led_on();
        delay_ms(500);
        led_off();
        delay_ms(500);
    }
    return 0;
}
