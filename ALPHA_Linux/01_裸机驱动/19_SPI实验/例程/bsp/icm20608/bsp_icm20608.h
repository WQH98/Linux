#ifndef __BSP_ICM20608_H
#define __BSP_ICM20608_H

#include "imx6u.h"
#include "bsp_gpio.h"
#include "bsp_spi.h"

void icm20608_init(void);

/* 片选的宏 */
#define ICM20608_CSN(n) (n ? gpio_write_pin(GPIO1, 20, 1) : gpio_write_pin(GPIO1, 20, 0))

#endif
