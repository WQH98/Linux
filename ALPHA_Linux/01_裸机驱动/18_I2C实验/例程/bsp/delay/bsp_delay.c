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

    /* 1M的频率，计1个数就是1us，那么0xFFFFFFFFus = 4294967295us ≈ 71.5min*/
    GPT1->OCR[0] = 0xFFFFFFFF;

    GPT1->CR |= (1 << 0);                   // 使能GPT1定时器
}

/* 微妙延时 */
void delay_us(unsigned int usdelay) {
    unsigned long old_cnt, new_cnt;
    unsigned long cnt_value = 0;
    old_cnt = GPT1->CNT;
    while(1) {
        new_cnt = GPT1->CNT;
        if(new_cnt != old_cnt) {
            if(new_cnt > old_cnt) {
                cnt_value += new_cnt - old_cnt;
            }
            else {
                cnt_value += 0xFFFFFFFF - old_cnt + new_cnt;
            }
            old_cnt = new_cnt;
            if(cnt_value >= usdelay) {
                break;
            }
        }
    }
}


/* 毫秒延时 */
void delay_ms(unsigned int msdelay) {
    unsigned int i = 0;
    for(i = 0; i < msdelay; i++) {
        delay_us(1000);
    }
}

#if 0
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
#endif
