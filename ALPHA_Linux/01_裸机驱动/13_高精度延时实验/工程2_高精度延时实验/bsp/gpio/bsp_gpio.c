#include "bsp_gpio.h"

// GPIO初始化
void gpio_init(GPIO_Type *base, int pin, gpio_pin_config_t *config) {
    // 如果是输入的话
    if(config->direction == kGpio_DigitalInput) {
        base->GDIR &= ~(1 << pin);
    }
    // 输出的情况
    else {
        base->GDIR |= (1 << pin);
        // 设置默认输出电平
        gpio_write_pin(base, pin, config->outputLogic);
    }
    gpio_intconfig(base, pin, config->interruptMode);
}

// 控制GPIO高低电平
void gpio_write_pin(GPIO_Type *base, int pin, int value) {
    // 写0
    if(value == 0) {
        base->DR &= ~(1 << pin);
    }
    // 写1
    else {
        base->DR |= (1 << pin);
    }
}

// 读取指定IO电平
int gpio_read_pin(GPIO_Type *base, int pin) {
    return (((base->DR) >> pin) & 0x01);
}


/* 使能指定IO中断 */ 
void gpio_enableint(GPIO_Type *base, unsigned int pin) {
    base->IMR |= (1 << pin);
}

/* 禁止指定IO中断 */
void gpio_disableint(GPIO_Type *base, unsigned int pin) {
    base->IMR &= ~(1 << pin);
}

/* 清除中断标志位 */
void gpio_clearintflags(GPIO_Type *base, unsigned int pin) {
    base->ISR |= (1 << pin);
}

/* GPIO中断初始化函数 */
void gpio_intconfig(GPIO_Type *base, unsigned int pin, gpio_interrupt_mode_t pin_int_mode) {
    volatile unsigned int *icr;
    unsigned int icrShift;

    icrShift = pin;
    base->EDGE_SEL &= ~(1 << pin);
    /* 低16位 */
    if(pin < 16) {
        icr = &(base->ICR1);
    }
    /* 高16位 */
    else {
        icr = &(base->ICR2);
        icrShift -= 16;
    }
    switch (pin_int_mode)
    {
        case kGPIO_IntLowLevel:
            *icr &= ~(3 << (2 * icrShift));
            break;
        
        case kGPIO_IntHighLevel:
            *icr &= ~(3 << (2 * icrShift));
            *icr |= (1 << (2 * icrShift));
            break;
        
        case kGPIO_IntRisingEdge:
            *icr &= ~(3 << (2 * icrShift));
            *icr |= (2 << (2 * icrShift));
            break;

        case kGPIO_IntFallingEdge:
            *icr &= ~(3 << (2 * icrShift));
            *icr |= (3 << (2 * icrShift));
            break;

        case kGPIO_IntRisingOrFillingEdge:
            base->EDGE_SEL |= (1 << pin);
            break;

        default:
            break;
    }
}