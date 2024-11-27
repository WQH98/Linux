#include "bsp_beep.h"
#include "cc.h"
#include "bsp_gpio.h"

void beep_init(void) {
    gpio_pin_config_t beep_config;
    // 复用为GPIO5_IO01
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0);
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0x10B0);

    // GPIO初始化
    // 设置为输出
    // GPIO5->GDIR |= (1 << 1);
    // 默认关闭
    // GPIO5->DR |= (1 << 1);
    
    beep_config.direction = kGpio_DigitalOutput;
    beep_config.outputLogic = 1;
    gpio_init(GPIO5, 1, &beep_config);
}

// 蜂鸣器控制函数
void beep_switch(int status) {
    if(status == ON) {
        // 输出低电平
        // GPIO5->DR &= ~(1 << 1);
        gpio_write_pin(GPIO5, 1, 0);
    }
    else if(status == OFF){
        // 输出高电平
        // GPIO5->DR |= (1 << 1);
        gpio_write_pin(GPIO5, 1, 1);
    }
}

