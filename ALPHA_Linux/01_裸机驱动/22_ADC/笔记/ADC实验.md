###### 一、ADC寄存器简介

1、ADC_HC寄存器：bit4~0、此5位选择一个输入通道。当通道选择位为0时，关闭ADC转换器。bit7使能转换完成中断，1使能，0不使能。

2、ADC_HS寄存器：bit0，转换完成标志，只读，当比较功能禁用时（ADC_GC[ACFE] = 0）并且硬件平均功能被禁用（ADC_GC[AVGE]=0）时，每次转换都会设置该位，当比较功能被启用（ADC_GC[ACFE]=1）时，只有当比较结果为真时，才会在转换完成时设置标志。

3、ADC_R寄存器：bit11~0，ADC转换结果。

4、ADC_CFG寄存器：bit0，时钟源选择，00为ipg_clk，01为ipg_clk/2，10为保持，11为异步时钟。bit3~2，为转换模式选择，设置ADC分辨率，00为8位，01为10位，10为12位，11为保留。bit6~5为时钟分频选择，00为输入时钟，01为输入时钟/2，10为输入时钟/4，11为输入时钟/8。bit16为数据覆盖使能，是否将下一个转换的结果覆盖到数据结果寄存器中现有的未读取的数据上。

5、ADC_GC寄存器：bit0，异步时钟输出使能。bit1，DMA使能。bit2，使能比较功能的范围。bit3，比较的Greater Than功能。bit4，比较功能使能。bit5，硬件平均功能。bit6，持续转换功能。bit7，校准功能，校准过程中，该位保持置位，校准序列完成后，该位被清除。

6、ADC_GS寄存器：bit0表示正在转换中，转换完成为0。bit1是校准失败标志，0为校准完成，1为校准失败，写1清除。

7、ADC_CV寄存器：bit11~0，包含比较值，用于在比较功能启用时与转换结果进行比较。bit27~16，包含比较值2，用于在比较功能和比较范围功能启用时与转换结果进行比较。

8、ADC_OFS寄存器：bit11~0，设置偏移值。bit12，标志位，为0时原始数值加偏移值，为1时原始数值减偏移值。

9、ADC_CAL寄存器：bit3~0，校准结果值，此值在校准结束时自动加载和更新。

