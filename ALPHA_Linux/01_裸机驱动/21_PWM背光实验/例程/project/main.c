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
    unsigned char keyvalue = 0;
	unsigned char i = 0;
	unsigned char state = OFF;
	unsigned char duty = 0;

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

	tftlcd_dev.forecolor = LCD_RED;
	lcd_show_string(50, 10, 400, 24, 24, (char*)"ZERO-IMX6U BACKLIGHT PWM TEST");  
	lcd_show_string(50, 40, 200, 16, 16, (char*)"ATOM@ALIENTEK");  
	lcd_show_string(50, 60, 200, 16, 16, (char*)"2019/3/27");   
	lcd_show_string(50, 90, 400, 16, 16, (char*)"PWM Duty:   %");  
	tftlcd_dev.forecolor = LCD_BLUE;

	/* 设置默认占空比 10% */
	if(tftlcd_dev.id == ATKVGA)
		duty=100;	//VGA只能在满输出时才能亮屏
	else
		duty = 10;
	lcd_shownum(50 + 72, 90, duty, 3, 16);
	pwm1_setduty(duty);	
	
	while(1)					
	{
		keyvalue = key_get_value();
		if(keyvalue == KEY0_VALUE)
		{
			duty += 10;				/* 占空比加10% */
			if(duty > 100)			/* 如果占空比超过100%，重新从10%开始 */
				duty = 10;
			lcd_shownum(50 + 72, 90, duty, 3, 16);
			pwm1_setduty(duty);		/* 设置占空比 */
		}
		
		delay_ms(10);
		i++;
		if(i == 50)
		{	
			i = 0;
			state = !state;
			led_switch(LED0,state);	
		}
	}
	return 0;
}