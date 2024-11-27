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
#include "bsp_ap3216c.h"
#include "bsp_icm20608.h"
#include "bsp_ft5426.h"
#include "bsp_backlight.h"
#include "bsp_adc.h"

/*
 * @description	: 使能I.MX6U的硬件NEON和FPU
 * @param 		: 无
 * @return 		: 无
 */
void imx6ul_hardfpu_enable(void)
{
	uint32_t cpacr;
	uint32_t fpexc;

	/* 使能NEON和FPU */
	cpacr = __get_CPACR();
	cpacr = (cpacr & ~(CPACR_ASEDIS_Msk | CPACR_D32DIS_Msk))
		   |  (3UL << CPACR_cp10_Pos) | (3UL << CPACR_cp11_Pos);
	__set_CPACR(cpacr);
	fpexc = __get_FPEXC();
	fpexc |= 0x40000000UL;	
	__set_FPEXC(fpexc);
}


int main(void) {    
	unsigned char i = 0;
	unsigned int adcvalue;
	unsigned char state = OFF;
	signed int integ; 	/* 整数部分 */
	signed int fract;	/* 小数部分 */

    imx6ul_hardfpu_enable();// 使能I.MX6U的硬件浮点
    int_init();             // 初始化中断
    imx6u_clk_init();       // 初始化系统时钟
    delay_init();           // 初始化延时
    clk_enable();           // 使能外设时钟
    uart1_init();           // 初始化串口1
    led_init();             // 初始化LED
    beep_init();            // 初始化蜂鸣器
    key_init();             // 初始化按键
    lcd_init();             // 初始化LCD
    backlight_init();		// 初始化背光PWM
    adc1ch1_init();         // 初始化ADC1通道1

    while(1) {
        adcvalue = getadc_average(5);
        lcd_showxnum(162, 90, adcvalue, 4, 16, 0);	/* ADC原始数据值 */
		printf("ADC orig value = %d\r\n", adcvalue);

		adcvalue = getadc_volt();
		integ = adcvalue / 1000;
		fract = adcvalue % 1000;
		lcd_showxnum(162, 110, integ, 1, 16, 0);	/* 显示电压值的整数部分，3.1111的话，这里就是显示3 */
		lcd_showxnum(178, 110, fract, 3, 16, 0X80);	/* 显示电压值小数部分（前面转换为了整形显示），这里显示的就是111. */
		printf("ADC vola = %d.%dV\r\n", integ, fract);

		delay_ms(50);
		i++;
		if(i == 10)
		{	
			i = 0;
			state = !state;
			led_switch(LED0, state);	
		}
    }
	
	return 0;
}