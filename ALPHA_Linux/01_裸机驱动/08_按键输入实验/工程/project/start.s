.global _start
.global _bss_start
.global _bss_end

_bss_start:
    /*  这应该相当于C语言里面的变量
        .ward表示在这里放了一个变量
        __bss_start就是这个变量的标签 类似于变量名
     */
    .word __bss_start

_bss_end:
    .word __bss_end

_start:
    @ 设置处理器进入SVC模式
    @ 读取cpsr到r0
    mrs r0, cpsr
    @ 清除cpsr的bit4-0
    bic r0, r0, #0x1f
    @ 使用SVC模式
    orr r0, r0, #0x13
    @ 将r0写入cpsr
    msr cpsr, r0

    @ 清除BSS段
    ldr r0, _bss_start
    ldr r1, _bss_end
    /*  使用ldr伪指令将数据加载到寄存器的时候
        需要在数据前面添加'='前缀
        使用mov伪指令将数据加载到寄存器的时候
        需要在数据前面添加'#'前缀
    */
    mov r2, #0
bss_loop:
    stmia r0!, {r2}
    @ 比较r0和r1里面的值
    cmp r0, r1
    @ 如果r0地址小于r1 继续清除bss段
    ble bss_loop


    @ 设置sp指针
    ldr sp, =0x80200000
    @ 跳转到C语言main函数
    b main


