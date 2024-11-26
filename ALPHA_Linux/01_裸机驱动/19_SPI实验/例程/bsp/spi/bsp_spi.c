#include "bsp_spi.h"

/* SPI初始化 */
void spi_init(ECSPI_Type *base) {
    /* 清零 */
    base->CONREG = 0;

    base->CONREG |= (1 << 0);               // 使能SPI
    base->CONREG |= (1 << 3);               // 当向TXFIFO写入数据以后马上开启SPI突发访问
    base->CONREG |= (1 << 4);               // 设置通道0的主从模式
    base->CONREG |= (7 << 20);              // 表示8bit突发

    base->CONFIGREG = 0;

    base->PERIODREG = 0x2000;

    /* SPI时钟 ICM20608的SPI最高8MHz 将SPI_CLK设置为6MHz*/
    base->CONREG &= ~(0x0F << 12);          // bit15~12清零
    base->CONREG &= ~(0x0F << 8);           // bit11~8清零
    base->CONREG |= (9 << 12);              // 设置前级分频为10分频 60MHz / 10 = 6MHz
}

/* SPI发送/接收函数 */
unsigned char spicho_read_write_byte(ECSPI_Type *base, unsigned char txdata) {
    uint32_t spi_tx_data = txdata;
    uint32_t spi_rx_data = 0;

    /* 选择通道0 */
    base->CONREG &= ~(3 << 18);             // bit18~20清零

    /* 数据发送 */
    while((base->STATREG & (1 << 0)) == 0); // 等待发送完成
    base->TXDATA = spi_tx_data;

    /* 数据接收 */
    while((base->STATREG & (1 << 3)) == 0); // 等待接收FIFO有数据
    spi_rx_data = base->RXDATA;

    return spi_rx_data;
}

