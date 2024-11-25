#ifndef __BSP_KEYFILTER_H
#define __BSP_KEYFILTER_H

#include "imx6u.h"

void keyfilter_init(void);
void filter_timer_init(unsigned int value);
void gpio1_io16_io31_irqhandler(unsigned int gicciar, void *param);
void filter_timer_irqhandler(unsigned int gicciar, void *param);
void filter_timer_start(void);
void filter_timer_stop(void);
void filter_timer_restart(unsigned int value);


#endif
