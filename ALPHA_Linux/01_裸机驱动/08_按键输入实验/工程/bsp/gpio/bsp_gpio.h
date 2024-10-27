#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "MCIMX6Y2.h"
#include "cc.h"

// 枚举类型和GPIO结构体
typedef enum _gpio_pin_direction {
    kGpio_DigitalInput = 0U,
    kGpio_DigitalOutput = 1U,
}gpio_pin_direction_t;

typedef struct _gpio_pin_config {
    gpio_pin_direction_t direction;
    uint8_t outputLogic;
}gpio_pin_config_t;

void gpio_init(GPIO_Type *base, int pin, gpio_pin_config_t *config);
void gpio_write_pin(GPIO_Type *base, int pin, int value);
int gpio_read_pin(GPIO_Type *base, int pin);


#endif
