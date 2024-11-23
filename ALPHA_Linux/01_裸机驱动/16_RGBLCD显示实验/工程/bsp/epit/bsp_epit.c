#include "bsp_epit.h"
#include "bsp_int.h"
#include "bsp_led.h"

/* 初始化EPIT */
void epit_init(unsigned int frac, unsigned int value) {
    if(frac > 4095) {
        frac = 4095;
    }

    /* 配置EPIT1_CR寄存器 */
    EPIT1->CR = 0;

    EPIT1->CR |= (1 << 1);          // 设置计数器的初始值为装载寄存器的值
    EPIT1->CR |= (1 << 2);          // 使能比较中断
    EPIT1->CR |= (1 << 3);          // 设置定时器工作在set-and-forget模式下
    EPIT1->CR |= (frac << 4);       // 设置分频值
    EPIT1->CR |= (1 << 24);         // 设置EPIT的时钟源为ipg_clk = 66Mhz

    EPIT1->LR = value;              // 设置加载寄存器，相当于倒计数值

    EPIT1->CMPR = 0;                // 设置比较寄存器为0

    /* 初始化中断 */
    GIC_EnableIRQ(EPIT1_IRQn);      // 使能中断
    system_register_irqhandler(EPIT1_IRQn, epit1_irqhandler, NULL);// 注册中断

    /* 打开EPIT1定时器 */
    EPIT1->CR |= (1 << 0);          // 设置EPIT使能
}

/* EPIT1中断服务函数 */
void epit1_irqhandler(unsigned int gicciar, void *pram) {
    static unsigned char state = 0;

    state = !state;
    /* 中断发生了 */
    if((EPIT1->SR & 0x01) == 1) {
        led_switch(LED0, state);        // 翻转LED灯
    }  

    /* 清除中断标志位 */
    EPIT1->SR |= (1 << 0);
}
