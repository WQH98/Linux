#ifndef __BSP_EPIT_H
#define __BSP_EPIT_H

#include "imx6u.h"

void epit_init(unsigned int frac, unsigned int value);
void epit1_irqhandler(unsigned int gicciar, void *pram);

#endif
