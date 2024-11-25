#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "MCIMX6Y2.h"

void delay_init(void);

void delay_us(unsigned int usdelay);
void delay_ms(unsigned int msdelay);

#if 0
void delay_short(volatile unsigned int n);
void delay_ms(volatile unsigned int n);
#endif

#endif
