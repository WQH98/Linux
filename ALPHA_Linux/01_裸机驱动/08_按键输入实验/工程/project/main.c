#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "main.h"
#include "bsp_beep.h"
#include "bsp_key.h"


int main(void) {
    int i = 0;
    unsigned char led_state = OFF;
    unsigned char beep_state = OFF;
    int key_value = 0;
    // 使能外设时钟
    clk_enable();
    // 初始化LED
    led_init();
    // 初始化蜂鸣器
    beep_init();
    // 初始化按键
    key_init();
    while(1) {
        key_value = key_get_value();
        // 如果key_value为正 代表有效的按键值
        if(key_value) {
            switch(key_value) {
                case KEY0_VALUE:
                    beep_state = !beep_state;
                    beep_switch(beep_state);
                break;
            }
        }
        i++;
        if(i == 50) {
            i = 0;
            led_state = !led_state;
            led_switch(LED0, led_state);
        }
        delay_ms(10);
    }
    return 0;
}
