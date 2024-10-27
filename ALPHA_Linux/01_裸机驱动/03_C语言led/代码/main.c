#include "main.h"

// 使能外设时钟
void clk_enable(void) {
    CCM_CCGR0 = 0xFFFFFFFF;
    CCM_CCGR1 = 0xFFFFFFFF;
    CCM_CCGR2 = 0xFFFFFFFF;
    CCM_CCGR3 = 0xFFFFFFFF;
    CCM_CCGR4 = 0xFFFFFFFF;
    CCM_CCGR5 = 0xFFFFFFFF;
    CCM_CCGR6 = 0xFFFFFFFF;
}

// 初始化LED
void led_init(void) {
    // 复用为GPIO1_IO03
    SW_MUX_GPIO1_IO03 = 0x05;
    // 设置GPIO0_IO03电气属性
    SW_PAD_GPIO1_IO03 = 0x10B0;
    // GPIO初始化 设置为输出
    GPIO1_GDIR = 0x08;
    // 默认打开LED灯 输出低电平
    GPIO1_DR = 0x00;
}

// 短延时
void delay_short(unsigned int n) {
    while(n--);
}

// 延时 1次循环大概是1ms
void delay(unsigned int n) {
    while(n--) {
        delay_short(0x7FF);
    }
}

// 打开LED灯
void led_on(void) {
    GPIO1_DR &= ~(1 << 3);
}

// 关闭LED灯
void led_off(void) {
    GPIO1_DR |= 1 << 3;
}

int main(void) {
    // 初始化外设时钟
    clk_enable();
    // 初始化LED
    led_init();
    // 设置LED闪烁
    while(1) {
        led_on();
        delay(500);
        led_off();
        delay(500);
    }
    return 0;
} 
