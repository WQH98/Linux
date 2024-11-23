#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "main.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"
#include "bsp_uart.h"
#include "stdio.h"
#include "bsp_lcd.h"
#include "bsp_lcdapi.h"

/* 背景颜色索引 */
unsigned int backcolor[10] = {
	LCD_BLUE, 		LCD_GREEN, 		LCD_RED, 	LCD_CYAN, 	LCD_YELLOW, 
	LCD_LIGHTBLUE, 	LCD_DARKBLUE, 	LCD_WHITE, 	LCD_BLACK, 	LCD_ORANGE
}; 

int main(void) {    
    unsigned char index = 0;
    static unsigned char state = 0;

    int_init();             // 初始化中断
    imx6u_clk_init();       // 初始化系统时钟
    delay_init();           // 初始化延时
    uart1_init();           // 初始化串口1
    clk_enable();           // 使能外设时钟
    led_init();             // 初始化LED
    beep_init();            // 初始化蜂鸣器
    key_init();             // 初始化按键
    lcd_init();             // 初始化LCD

    tftlcd_dev.forecolor = LCD_RED;
    tftlcd_dev.backcolor = LCD_WHITE;

    while(1) {
        state = !state;
        led_switch(LED0, state);
        lcd_clear(backcolor[index]);
        lcd_show_string(10, 40, 260, 32, 32, (char *)"ALPHA IMX6U");
        lcd_show_string(10, 80, 260, 24, 32, (char *)"RGBLCD TEST");
        lcd_show_string(10, 110, 260, 16, 32, (char *)"ATOM@ALIENTEK");
        lcd_show_string(10, 130, 260, 12, 32, (char *)"2024/11/23");
        index++;
		if(index == 10)
			index = 0;   
        delay_ms(1000);
    }
    return 0;
}
