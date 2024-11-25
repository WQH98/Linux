#include "fsl_iomuxc.h"
#include "MCIMX6Y2.h"

// 使能外设时钟
void clk_enable(void) {
    CCM->CCGR0 = 0xFFFFFFFF;
    CCM->CCGR1 = 0xFFFFFFFF;
    CCM->CCGR2 = 0xFFFFFFFF;
    CCM->CCGR3 = 0xFFFFFFFF;
    CCM->CCGR4 = 0xFFFFFFFF;
    CCM->CCGR5 = 0xFFFFFFFF;
    CCM->CCGR6 = 0xFFFFFFFF;
}

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

// 短延时
void delay_short(volatile unsigned int n) {
    while(n--);
}

// 延时 一次循环大概是1ms 在主频396MHz
// n：延时的毫秒数
void delay_ms(volatile unsigned int n) {
    while(n--) {
        delay_short(0x7FF);
    }
}

// 打开LED灯
void led_on(void) {
    GPIO1->DR &= ~(1 << 3);
}

// 关闭LED灯
void led_off(void) {
    GPIO1->DR |= (1 << 3);
}

int main(void) {
    // 使能外设时钟
    clk_enable();
    // 初始化LED
    led_init();
    while(1) {
        led_on();
        delay_ms(2000);
        led_off();
        delay_ms(500);
    }
    return 0;
}
