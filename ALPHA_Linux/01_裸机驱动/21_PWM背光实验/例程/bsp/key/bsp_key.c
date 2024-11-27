#include "bsp_key.h"
#include "bsp_delay.h"
#include "bsp_gpio.h"

// 初始化按键
void key_init(void) {
    // 复用为GPIO1_IO18
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0);
    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0xF080);

    // GPIO初始化
    // 设置为输入(直接操作寄存器)
    // GPIO1->GDIR &= ~(1 << 18);
    // （使用刚写的GPIO库）
    gpio_pin_config_t key_config;
    key_config.direction = kGpio_DigitalInput;
    gpio_init(GPIO1, 18, &key_config);
}

// 读取按键值
// 返回值: 0, 按下  1，未按下
int read_key(void) {
    int ret = 0;
    // ret = (((GPIO1->DR) >> 18) & 0x01);
    ret = gpio_read_pin(GPIO1, 18);
    return ret;
}

int key_get_value(void) {
    int ret = 0;
    // 为1表示按键释放
    static unsigned char release = 1;
    // 如果现在记录的按键值已经被释放 但是获取到按键按下了
    // 被按下了
    if((release == 1) && (read_key() == 0)) {
        // 延时 10ms
        delay_ms(10);
        // 表示按键按下了
        release = 0;
        // 延时后还是按下的状态 表示按键有效
        if(read_key() == 0) {
            ret = KEY0_VALUE;
        }
    }
    // 表示没有按下
    else if(read_key() == 1) {
        // 释放了
        release = 1;
        ret = KEY_NONE;
    }
    return ret;
}
