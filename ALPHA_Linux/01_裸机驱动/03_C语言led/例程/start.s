.global _start

_start:
    /* 设置处理器进入SVC模式 */
    mrs r0, cpsr       /* 读取cpsr寄存器的值保存到r0 */
    bic r0, r0, #0x1F  /* 清除r0寄存器的bit4-0 */
    orr r0, r0, #0x13  /* 将r0寄存器的bit4-0 设置为0x13 */
    msr cpsr, r0       /* 将r0寄存器的值写入到cpsr寄存器 使用SVC模式 */

    /* 设置sp指针 */
    ldr sp, =0x80200000

    /* 跳转到C语言环境中的main函数 */
    b main
