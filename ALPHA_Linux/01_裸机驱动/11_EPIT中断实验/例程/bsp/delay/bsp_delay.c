#include "bsp_delay.h"

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

