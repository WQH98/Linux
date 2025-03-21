#include "bsp_ft5426.h"
#include "bsp_gpio.h"
#include "bsp_int.h"
#include "bsp_delay.h"
#include "bsp_i2c.h"
#include "stdio.h"


struct ft5426_dev_struc ft5426_dev;

/* 初始化FT5426 */
void ft5426_init(void) {
    
    unsigned char reg_value[2];

	ft5426_dev.initfalg = FT5426_INIT_NOTFINISHED;
	int i;
	for(i = 0; i < 5; i++)	{	
        /* 避免编译器自动赋值 */
		ft5426_dev.x[i] = 0;
		ft5426_dev.y[i] = 0;
	}
	ft5426_dev.point_num = 0;

    /* 1、IO初始化 */
    IOMUXC_SetPinMux(IOMUXC_UART5_TX_DATA_I2C2_SCL, 1);     // 复用为I2C2_SCL
    IOMUXC_SetPinMux(IOMUXC_UART5_RX_DATA_I2C2_SDA, 1);     // 复用为I2C2_SDA

    IOMUXC_SetPinConfig(IOMUXC_UART5_TX_DATA_I2C2_SCL, 0x70B0);
    IOMUXC_SetPinConfig(IOMUXC_UART5_RX_DATA_I2C2_SDA, 0x70B0);

    /* 初始化INT引脚 使能中断 */
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO09_GPIO1_IO09, 0);      // 复用为GPIO1_IO09
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO09_GPIO1_IO09, 0xF080);
    
    gpio_pin_config_t ctint_config;
    ctint_config.direction = kGpio_DigitalInput;
    ctint_config.interruptMode = kGPIO_IntRisingOrFillingEdge;
    gpio_init(GPIO1, 9, &ctint_config);

    GIC_EnableIRQ(GPIO1_Combined_0_15_IRQn);
    system_register_irqhandler(GPIO1_Combined_0_15_IRQn, gpio1_io09_irqhandler, NULL);
    gpio_enableint(GPIO1, 9);

    /* 初始化RST引脚 */
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER9_GPIO5_IO09, 0);      // 复用为GPIO5_IO09
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER9_GPIO5_IO09, 0x10B0);
    gpio_pin_config_t ctrst_config;
    ctrst_config.direction = kGpio_DigitalOutput;
    ctrst_config.outputLogic = 0;
    ctrst_config.interruptMode = kGPIO_Nointmode;
    gpio_init(GPIO5, 9, &ctrst_config);

    gpio_write_pin(GPIO5, 9, 0);        // 复位FT5426
    delay_ms(50);
    gpio_write_pin(GPIO5, 9, 1);        // 停止复位
    delay_ms(50);

    /* 2、I2C2初始化 */
    i2c_init(I2C2);

    /* 3、FT5426初始化 */
    ft5426_write_byte(FT5426_ADDR, FT5426_DEVICE_MODE, 0); 	/* 进入正常模式 */
	ft5426_write_byte(FT5426_ADDR, FT5426_IDG_MODE, 1); 	/* FT5426中断模式 */

	ft5426_read_len(FT5426_ADDR, FT5426_IDGLIB_VERSION, 2, reg_value);
	printf("Touch Frimware Version: %#X\r\n", (((unsigned short)reg_value[0] << 8) + reg_value[1]));
	
	ft5426_dev.initfalg = FT5426_INIT_FINISHED;	/* 标记FT5426初始化完成 */
	ft5426_dev.intflag = 0;
}

/* 中断处理函数 */
void gpio1_io09_irqhandler(unsigned int gicciar, void *param) {

    if(ft5426_dev.initfalg == FT5426_INIT_FINISHED)
	{
		//ft5426_dev.intflag = 1;
		ft5426_read_tpcoord();
	}

    /* 清除中断标志位 */
    gpio_clearintflags(GPIO1, 9);
}


/* 向FT5426写一个字节的数据 */
unsigned char ft5426_write_byte(unsigned char addr, unsigned char reg, unsigned char data) {
    unsigned char write_data = data;
    unsigned char status = 0;
    struct i2c_transfer master_xfer;

    master_xfer.slaveAddress = addr;
    master_xfer.direction = kI2C_Write;
    master_xfer.subaddress = reg;
    master_xfer.subaddressSize = 1;
    master_xfer.data = &write_data;
    master_xfer.dataSize = 1;

    if(i2c_master_transfer(I2C2, &master_xfer)) {
        status = 1;
    }
    
    return status;
}


/* 从FT5426读取一个字节数据 */
unsigned char ft5426_read_byte(unsigned char addr, unsigned char reg) {
    unsigned char val = 0;
    struct i2c_transfer master_xfer;

    master_xfer.slaveAddress = addr;
    master_xfer.direction = kI2C_Read;
    master_xfer.subaddress = reg;
    master_xfer.subaddressSize = 1;
    master_xfer.data = &val;
    master_xfer.dataSize = 1;
    i2c_master_transfer(I2C2, &master_xfer);

    return val;
}


/* 从FT5426读取多个寄存器数据 */
void ft5426_read_len(unsigned char addr, unsigned char reg, unsigned char len, unsigned char *buf) {
    struct i2c_transfer master_xfer;

    master_xfer.slaveAddress = addr;
    master_xfer.direction = kI2C_Read;
    master_xfer.subaddress = reg;
    master_xfer.subaddressSize = 1;
    master_xfer.data = buf;
    master_xfer.dataSize = len;
    i2c_master_transfer(I2C2, &master_xfer);
}

/*
 * @description	: 读取当前触摸点个数
 * @param 		: 无
 * @return 		: 无
 */
void ft5426_read_tpnum(void)
{
	ft5426_dev.point_num = ft5426_read_byte(FT5426_ADDR, FT5426_TD_STATUS);
}

/*
 * @description	: 读取当前所有触摸点的坐标
 * @param 		: 无
 * @return 		: 无
 */
void ft5426_read_tpcoord(void)
{
	unsigned char i = 0;
	unsigned char type = 0;
	//unsigned char id = 0;
	unsigned char pointbuf[FT5426_XYCOORDREG_NUM];
	
	ft5426_dev.point_num = ft5426_read_byte(FT5426_ADDR, FT5426_TD_STATUS);

	/*
  	 * 从寄存器FT5426_TOUCH1_XH开始，连续读取30个寄存器的值，这30个寄存器
  	 * 保存着5个点的触摸值，每个点占用6个寄存器。
	 */
	ft5426_read_len(FT5426_ADDR, FT5426_TOUCH1_XH, FT5426_XYCOORDREG_NUM, pointbuf);
		
	for(i = 0; i < ft5426_dev.point_num ; i++)
	{
		unsigned char *buf = &pointbuf[i * 6];
		/* 以第一个触摸点为例，寄存器TOUCH1_XH(地址0X03),各位描述如下：
		 * bit7:6  Event flag  0:按下 1:释放 2：接触 3：没有事件
		 * bit5:4  保留
		 * bit3:0  X轴触摸点的11~8位。
		 */
		ft5426_dev.x[i] = ((buf[2] << 8) | buf[3]) & 0x0FFF;
		ft5426_dev.y[i] = ((buf[0] << 8) | buf[1]) & 0x0FFF;
		
		type = buf[0] >> 6;	/* 获取触摸类型 */
		

		/* 以第一个触摸点为例，寄存器TOUCH1_YH(地址0X05),各位描述如下：
		 * bit7:4  Touch ID  触摸ID，表示是哪个触摸点
		 * bit3:0  Y轴触摸点的11~8位。
		 */
		//id = (buf[2] >> 4) & 0x0f;
		
		if(type == FT5426_TOUCH_EVENT_DOWN || type == FT5426_TOUCH_EVENT_ON)    /* 按下 */
		{
		
		} else  {	/* 释放 */	
			
		}
	}	
}
