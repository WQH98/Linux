@ 全局标号
.global _start


_start:
    /* 使能所有外设时钟 */
    ldr r0, =0x020c4068  @ CCGR0寄存器
    ldr r1, =0xffffffff  @ 要向CCGR0写入的数据
    str r1, [r0]         @ 将0xffffffff写入到CCGR0中

    ldr r0, =0x020c406c  @ CCGR1寄存器
    str r1, [r0]         @ 将0xffffffff写入到CCGR1中

    ldr r0, =0x020c4070  @ CCGR2寄存器
    str r1, [r0]         @ 将0xffffffff写入到CCGR2中

    ldr r0, =0x020c4074  @ CCGR3寄存器
    str r1, [r0]         @ 将0xffffffff写入到CCGR3中

    ldr r0, =0x020c4078  @ CCGR4寄存器
    str r1, [r0]         @ 将0xffffffff写入到CCGR4中
    
    ldr r0, =0x020c407c  @ CCGR5寄存器
    str r1, [r0]         @ 将0xffffffff写入到CCGR5中
    
    ldr r0, =0x020c4080  @ CCGR6寄存器
    str r1, [r0]         @ 将0xffffffff写入到CCGR6中
    
    /*  配置GPIO1_IO03 PIN的复用功能为GPIO 也就是设置
        IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03 = 5
        IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03的寄存器地址为0x020e0068
    */
    ldr r0, =0x020e0068  @ IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03寄存器
    ldr r1, =0X05        @ 向寄存器写入的数据
    str r1, [r0]         @ 将r1中的数据 写入r0

    /* 
        配置GPIO1_IO03的电气属性 也就是寄存器
        IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03 = 0x10b0
        IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03的地址为0x020e02f4
        bit0:   0 低压摆率
        bit5-3：设置的驱动能力 数值越大 驱动能力越强
                110 R0/6驱动能力
        bit7-6: 10 100MHz速度
        bit11:  0 关闭开漏输出
        bit12:  1 使能pull/kepper
        bit13:  上拉还是保持
                0 保持kepper
        bit15-14： 设置上下拉阻值
                   00 默认100k下拉
        bit16:  0 关闭hys
     */
    ldr r0, =0x020e02f4
    ldr r1, =0x10b0
    str r1, [r0]

    /* 
        设置GPIO
        设置GPIO1_GDIR寄存器 设置GPIO1_IO03为输出
        GPIO1_GDIR寄存器地址为0x0209c004
        设置GPIO1_GDIR寄存器bit3 为 1
        也就是设置GPIO1_IO03为输出
    */
    ldr r0, =0x0209c004
    ldr r1, =0x08
    str r1, [r0]

    /* 
        打开led 也就是设置GPIO1_IO03为0 
        GPIO_DR寄存器地址为0x0209c000
    */
    ldr r0, =0x0209c000
    ldr r1, =0
    str r1, [r0]

loop:
    b loop

