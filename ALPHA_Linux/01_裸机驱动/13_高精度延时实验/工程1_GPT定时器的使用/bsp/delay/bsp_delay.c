#include "bsp_delay.h"
#include "bsp_int.h"
#include "bsp_led.h"

/* 延时初始化函数 */
void delay_init(void) {

    GPT1->CR = 0;
    GPT1->CR |= (1 << 15);                  // 软复位
    while((GPT1->CR >> 15) & 0x01);         // 等待复位结束

    GPT1->CR |= (1 << 1);                   // 设置计数值默认为0
    GPT1->CR |= (1 << 6);                   // 设置时钟源为ipg_clk = 66MHz

    GPT1->PR = 65;                          // 设置为66分频，频率等于66MHz / 66 = 1MHz

    /* 配置输出比较通道1 */
    GPT1->OCR[0] = 500000;                  // 设置中断周期为500ms

    /* 打开GPT1输出比较通道1中断 */    
    GPT1->IR = 1 << 0;

    /* 设置GIC */
    GIC_EnableIRQ(GPT1_IRQn);
    system_register_irqhandler(GPT1_IRQn, gpt1_irqhandler, NULL);

    GPT1->CR |= (1 << 0);                   // 使能GPT1
}


/* GPT1中断服务函数 */
void gpt1_irqhandler(unsigned int gicciar, void *param) {
    static unsigned char state = 0;

    if((GPT1->SR & 0x01) == 1) {
        state = !state;
        led_switch(LED0, state);
    }

    /* 清除中断标志位 */
    GPT1->SR |= (1 << 0);
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

