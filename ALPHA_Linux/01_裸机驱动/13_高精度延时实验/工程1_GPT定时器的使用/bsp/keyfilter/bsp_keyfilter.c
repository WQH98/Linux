#include "bsp_keyfilter.h"
#include "bsp_gpio.h"
#include "bsp_int.h"
#include "bsp_beep.h"


/* keyfilter初始化 */
void keyfilter_init(void) {
    
    gpio_pin_config_t key_config;
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0);     /* 复用为GPIO1_IO18 */
    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0xF080);

    /* GPIO初始化 */
    key_config.direction = kGpio_DigitalInput;
    key_config.interruptMode = kGPIO_IntFallingEdge;
    gpio_init(GPIO1, 18, &key_config);

    GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn);
    
    system_register_irqhandler(GPIO1_Combined_16_31_IRQn, gpio1_io16_io31_irqhandler, NULL);


    gpio_enableint(GPIO1, 18);

    /* 初始化定时器 */
    filter_timer_init(660000);          // 定时时间10ms
}


/* 初始化EPIT1定时器 */
void filter_timer_init(unsigned int value) {
    /* 配置EPIT1_CR寄存器 */
    EPIT1->CR = 0;

    EPIT1->CR |= (1 << 1);          // 设置计数器的初始值为装载寄存器的值
    EPIT1->CR |= (1 << 2);          // 使能比较中断
    EPIT1->CR |= (1 << 3);          // 设置定时器工作在set-and-forget模式下
    EPIT1->CR &= ~(0 << 4);         // 设置分频值
    EPIT1->CR |= (1 << 24);         // 设置EPIT的时钟源为ipg_clk = 66Mhz

    EPIT1->LR = value;              // 设置加载寄存器，相当于倒计数值

    EPIT1->CMPR = 0;                // 设置比较寄存器为0

    /* 初始化中断 */
    GIC_EnableIRQ(EPIT1_IRQn);      // 使能中断
    system_register_irqhandler(EPIT1_IRQn, filter_timer_irqhandler, NULL); // 注册中断
}


/* 按键中断处理函数 */
void gpio1_io16_io31_irqhandler(unsigned int gicciar, void *param) {
    /* 开启定时器 */
    filter_timer_restart(660000);
    /* 清除中断标志位 */
    gpio_clearintflags(GPIO1, 18);
}


/* 打开EPIT1定时器 */
void filter_timer_start(void) {
    EPIT1->CR |= (1 << 0);
}


/* 关闭EPIT1定时器 */
void filter_timer_stop(void) {
    EPIT1->CR &= ~(1 << 0);
}


/* 重启EPIT1定时器 */
void filter_timer_restart(unsigned int value) {
    filter_timer_stop();
    EPIT1->LR = value;
    filter_timer_start();
}


/* EPIT1定时器中断处理函数 */
void filter_timer_irqhandler(unsigned int gicciar, void *param) {
    static unsigned char state  = OFF;
    if((EPIT1->SR & 0x01) == 1) {
        /* 关闭定时器 */
        filter_timer_stop();
        if(gpio_read_pin(GPIO1, 18) == 0) {
            /* 蜂鸣器状态反转 */
            state = !state;
            beep_switch(state);
        }
    }
    /* 清除中断标志位 */
    EPIT1->SR |= (1 << 0);
}

