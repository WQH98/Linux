#include "bsp_adc.h"
#include "bsp_delay.h"


/* 初始化ADC1 */
int adc1ch1_init(void) {

    int ret = 0;

    ADC1->CFG = 0;
    ADC1->CFG |= (3 << 0);      // 时钟源选择ADACK
    ADC1->CFG |= (2 << 2);      // 选择分辨率为12位

    ADC1->GC = 0;
    ADC1->GC |= (1 << 0);       // 使能ADACK

    /* 校准ADC */
    if(adc1_autocalibration() != kStatus_Success) {
        ret = -1;
    }

    return ret;
}


/* 初始化ADC校准 */
status_t adc1_autocalibration(void) {
    status_t ret = kStatus_Success;

    ADC1->GS |= (1 << 2);       // 清除CALF位 写1清零
    ADC1->GC |= (1 << 7);       // 使能校准功能

    // 校准完成之前GC寄存器的CAL位会一直为1，直到校准完成此位自动清零
    while((ADC1->GC & (1 << 7)) != 0) {
        // 如果GS寄存器的CALF位为1的话表示校准失败
        if((ADC1->GS & (1 << 2)) != 0) {
            ret = kStatus_Fail;
            break;
        }
    }

    // 校准成功以后HS寄存器的COCO0位会置1
    if((ADC1->HS  & (1 << 0)) == 0) {
        ret = kStatus_Fail;
    }

    // 如果GS寄存器的CALF位为1的话表示校准失败 
    if((ADC1->GS & (1 << 2)) != 0) {
        ret = kStatus_Fail;
    }

    return ret;
}


/* 获取ADC1的原始值 */
unsigned int getadc_value(void) {

    /* 配置ADC通道1 */
    ADC1->HC[0] = 0;            // 关闭转换结束中断
    ADC1->HC[0] |= (1 << 0);    // 通道1

    while((ADC1->HS & (1 << 0)) == 0);      // 等待转换结束

    return ADC1->R[0];          // 返回ADC值
}


/* 获取ADC平均值 */
unsigned short getadc_average(unsigned char times) {
	unsigned int temp_val = 0;
	unsigned char t;
	for(t = 0; t < times; t++){
		temp_val += getadc_value();
		delay_ms(5);
	}
	return temp_val / times;
}


/* 获取ADC对应的电压值 */
unsigned short getadc_volt(void) {
    unsigned int adcvalue = 0;
    unsigned int ret = 0;
    adcvalue = getadc_average(5);
    ret = (float)adcvalue * (3300.0f / 4096.0f);    	/* 获取计算后的带小数的实际电压值 */
    return  ret;
}
