#include "bsp_uart.h"
#include "bsp_gpio.h"

/* 初始化UART1，波特率为115200*/
void uart1_init(void) {
    /* 初始化UART1的IO */
    uart1_gpio_init();

    /* 初始化UART1 */
    uart_disable(UART1);            // 关闭UART1
    uart_soft_reset(UART1);         // 复位UART1

    /* 配置UART1 */
    UART1->UCR1 = 0;
    UART1->UCR2 = 0;
    UART1->UCR3 = 0;

    /* 配置UART1的数据位、奇偶校验、停止位等等 */
    UART1->UCR2 |= (1 << 1);        // 使能接收
    UART1->UCR2 |= (1 << 2);        // 使能发送
    UART1->UCR2 |= (1 << 5);        // 8位数据位
    UART1->UCR2 |= (1 << 14);       // 忽略RTS引脚
    UART1->UCR3 |= (1 << 2);        // 引脚必须为1

    /* 设置波特率为115200 */
    // UART1->UFCR &= ~(7 << 7);       // 对RFDIV进行清零
    // UART1->UFCR |= (5 << 7);        // 1分频 uart_clk = 80MHz
    // UART1->UBIR = 71;               // 
    // UART1->UBMR = 3124;             // 
    uart_setbaudrate(UART1, 115200, 80000000);


    /* 使能串口 */
    uart_enable(UART1);
}


/*
 * @description 		: 波特率计算公式，
 *    			  	  	  可以用此函数计算出指定串口对应的UFCR，
 * 				          UBIR和UBMR这三个寄存器的值
 * @param - base		: 要计算的串口。
 * @param - baudrate	: 要使用的波特率。
 * @param - srcclock_hz	:串口时钟源频率，单位Hz
 * @return		        : 无
 */
void uart_setbaudrate(UART_Type *base, unsigned int baudrate, unsigned int srcclock_hz) {
    uint32_t numerator = 0u;		//分子
    uint32_t denominator = 0U;		//分母
    uint32_t divisor = 0U;
    uint32_t refFreqDiv = 0U;
    uint32_t divider = 1U;
    uint64_t baudDiff = 0U;
    uint64_t tempNumerator = 0U;
    uint32_t tempDenominator = 0u;

    /* get the approximately maximum divisor */
    numerator = srcclock_hz;
    denominator = baudrate << 4;
    divisor = 1;

    while (denominator != 0)
    {
        divisor = denominator;
        denominator = numerator % denominator;
        numerator = divisor;
    }

    numerator = srcclock_hz / divisor;
    denominator = (baudrate << 4) / divisor;

    /* numerator ranges from 1 ~ 7 * 64k */
    /* denominator ranges from 1 ~ 64k */
    if ((numerator > (UART_UBIR_INC_MASK * 7)) || (denominator > UART_UBIR_INC_MASK))
    {
        uint32_t m = (numerator - 1) / (UART_UBIR_INC_MASK * 7) + 1;
        uint32_t n = (denominator - 1) / UART_UBIR_INC_MASK + 1;
        uint32_t max = m > n ? m : n;
        numerator /= max;
        denominator /= max;
        if (0 == numerator)
        {
            numerator = 1;
        }
        if (0 == denominator)
        {
            denominator = 1;
        }
    }
    divider = (numerator - 1) / UART_UBIR_INC_MASK + 1;

    switch (divider)
    {
        case 1:
            refFreqDiv = 0x05;
            break;
        case 2:
            refFreqDiv = 0x04;
            break;
        case 3:
            refFreqDiv = 0x03;
            break;
        case 4:
            refFreqDiv = 0x02;
            break;
        case 5:
            refFreqDiv = 0x01;
            break;
        case 6:
            refFreqDiv = 0x00;
            break;
        case 7:
            refFreqDiv = 0x06;
            break;
        default:
            refFreqDiv = 0x05;
            break;
    }
    /* Compare the difference between baudRate_Bps and calculated baud rate.
     * Baud Rate = Ref Freq / (16 * (UBMR + 1)/(UBIR+1)).
     * baudDiff = (srcClock_Hz/divider)/( 16 * ((numerator / divider)/ denominator).
     */
    tempNumerator = srcclock_hz;
    tempDenominator = (numerator << 4);
    divisor = 1;
    /* get the approximately maximum divisor */
    while (tempDenominator != 0)
    {
        divisor = tempDenominator;
        tempDenominator = tempNumerator % tempDenominator;
        tempNumerator = divisor;
    }
    tempNumerator = srcclock_hz / divisor;
    tempDenominator = (numerator << 4) / divisor;
    baudDiff = (tempNumerator * denominator) / tempDenominator;
    baudDiff = (baudDiff >= baudrate) ? (baudDiff - baudrate) : (baudrate - baudDiff);

    if (baudDiff < (baudrate / 100) * 3)
    {
        base->UFCR &= ~UART_UFCR_RFDIV_MASK;
        base->UFCR |= UART_UFCR_RFDIV(refFreqDiv);
        base->UBIR = UART_UBIR_INC(denominator - 1); //要先写UBIR寄存器，然后在写UBMR寄存器，3592页 
        base->UBMR = UART_UBMR_MOD(numerator / divider - 1);
    }
}


/* 关闭UART */
void uart_disable(UART_Type *base) {
    base->UCR1 &= ~(1 << 0);
}


/* 打开UART */
void uart_enable(UART_Type *base) {
    base->UCR1 |= (1 << 0);
}


/* 复位UART */
void uart_soft_reset(UART_Type *base) {
    base->UCR2 &= ~(1 << 0);
    while((base->UCR2 & 0x01) == 0);
}


/* 
 * UART1的IO初始化
 * UART1_TXD使用的IO为UART1_TX_DATA
 * UART1_RXD使用的IO为UART1_RX_DATA
 */
void uart1_gpio_init(void) {
    IOMUXC_SetPinMux(IOMUXC_UART1_TX_DATA_UART1_TX, 0);         // 复用为UART1_TX
    IOMUXC_SetPinMux(IOMUXC_UART1_RX_DATA_UART1_RX, 0);         // 复用为UART1_RX

    /* 设置电气属性 */
    IOMUXC_SetPinConfig(IOMUXC_UART1_TX_DATA_UART1_TX, 0x10B0); 
    IOMUXC_SetPinConfig(IOMUXC_UART1_RX_DATA_UART1_RX, 0x10B0);
}


/* 通过UART1发送一个字符 */
void putc(unsigned char c) {
    while(((UART1->USR2 >> 3) & 0x01) == 0);          // 判断上一帧数据是否发送完
    UART1->UTXD = c;
}

/* 通过UART1接收一个字符 */
unsigned char getc(void) {
    while(((UART1->USR2) & 0x01) == 0);               // 判断是否有数据接收
    return UART1->URXD;
}

/* 通过UART1发送一串字符 */
void puts(char *str) {
    char *p = str;
    while(*p) {
        putc(*p++);
    }
}

/* 定义raise函数 防止编译报错 */
void raise(int sig_nr) {
    
}
