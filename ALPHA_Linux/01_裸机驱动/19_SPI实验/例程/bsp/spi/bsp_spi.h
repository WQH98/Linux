#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "imx6u.h"

void spi_init(ECSPI_Type *base);
unsigned char spicho_read_write_byte(ECSPI_Type *base, unsigned char txdata);


#endif
