#include "bsp_led.h"

// 初始化LED
void led_init(void) {
    // 复用为GPIO1-IO03
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0);
    // 设置GPIO1-IO03电气属性
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0x10B0);
    // GPIO初始化
    // 设置为输出
    GPIO1->GDIR = 0x08;
    // 打开LED灯
    GPIO1->DR = 0x00;
}


// 打开LED灯
void led_on(void) {
    GPIO1->DR &= ~(1 << 3);
}

// 关闭LED灯
void led_off(void) {
    GPIO1->DR |= (1 << 3);
}

// LED灯控制函数
void led_switch(int led, int status) {
    switch(led) {
        case LED0:
            if(status == ON) {
                GPIO1->DR &= ~(1 << 3);
            }
            else if(status == OFF) {
                GPIO1->DR |= (1 << 3);
            }
            break;
    }
}
