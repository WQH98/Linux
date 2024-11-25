#include "bsp_int.h"

/* 中断嵌套变量 */
static unsigned int irqNeating;

/* 中断处理函数表 */
static sys_irq_handler_t irqTable[NUMBER_OF_INT_VECTORS];

/* 初始化中断处理函数表 */
void sysytem_irqtable_init(void) {
    unsigned int i = 0;
    irqNeating = 0;
    for(i = 0; i < NUMBER_OF_INT_VECTORS; i++) {
        irqTable[i].irqHandler = default_irqhandler;
        irqTable[i].userParam = NULL;
    }
}

/* 注册中断处理函数 */
void system_register_irqhandler(IRQn_Type irq, system_irq_handler_t handler, void *userParam) {
    irqTable[irq].irqHandler = handler;
    irqTable[irq].userParam = userParam;
}

/* 中断初始化函数 */
void int_init(void) {
    GIC_Init();
    sysytem_irqtable_init();

    /* 中断向量偏移设置 */
    __set_VBAR(0x87800000);


}

/* 具体的中断处理函数，IRQ_Handler会调用此函数*/
void system_irqhandler(unsigned int gicciar) {
    unsigned intNum = gicciar & 0x3FF;
    /* 检查中断ID */
    if(intNum >= NUMBER_OF_INT_VECTORS) {
        return;
    }
    irqNeating++;
    /* 根据中断ID号读取中断处理函数执行 */
    irqTable[intNum].irqHandler(intNum, irqTable[intNum].userParam);
    irqNeating--;
}

/* 默认中断处理函数 */
void default_irqhandler(unsigned int gicciar, void *userParam) {
    while(1) {

    }
}
