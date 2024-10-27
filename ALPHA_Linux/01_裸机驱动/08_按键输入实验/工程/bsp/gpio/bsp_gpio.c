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
