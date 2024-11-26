#include "bsp_icm20608.h"


/* 初始化ICM60208 */
void icm20608_init(void) {
    /* 1、SPI引脚的初始化 */
    IOMUXC_SetPinMux(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 1);      // 复用为ECSPI3_SCLK
    IOMUXC_SetPinMux(IOMUXC_UART2_CTS_B_ECSPI3_MOSI, 1);        // 复用为ECSPI3_MOSI
    IOMUXC_SetPinMux(IOMUXC_UART2_RTS_B_ECSPI3_MISO, 1);        // 复用为ECSPI3_MISO

    IOMUXC_SetPinConfig(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_UART2_CTS_B_ECSPI3_MOSI, 0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_UART2_RTS_B_ECSPI3_MISO, 0x10B0);

    /* 片选引脚初始化 */
    IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0);        // 复用为ECSPI3_MISO
    IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0x10B0);

    gpio_pin_config_t cs_config;
    cs_config.direction = kGpio_DigitalOutput;
    cs_config.outputLogic = 0;
    gpio_init(GPIO1, 20, &cs_config);

    /* 2、SPI控制器的初始化 */
    spi_init(ECSPI3);

    /* 3、ICM20608初始化 */

}


/* ICM20608 SPI 接口读数据 */
unsigned char icm20608_read_reg(unsigned char reg) {
    ICM20608_CSN(0);            // 片选拉低

    

    ICM20608_CSN(1);            // 片选拉高 停止访问
}
