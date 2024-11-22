###### 一、DDR内存简介

1、RAM和ROM

RAM：随机存储器，可以随时进行读写操作，速度很快，掉电以后数据会丢失。
ROM：只读存储器， 现在的ROM应该叫做Flash，是可以进行读写操作的，写起来比较麻烦，需要先进行擦除，然后再发送要写的地址和扇区，最后才是要写入的数据。向Flash写入数据要复杂的多，意味着相比RAM速度就会变慢，但是容量可以做的很大，而且掉电以后数据不会丢失，适合用来存储资料。

2、SRAM

一开始是芯片内部RAM，后面因为应用需求需要外扩RAM，比如STM32F103/F407开发外扩1MB SRAM。
IS62WV51216这是一个16位宽的1MB的SRAM。
SRAM最大的缺点就是成本高。

3、SDRAM

SDRAM是同步动态随机存储器，同步的意思是SDRAM工作需要时钟线，动态的意思是SDRAM中的数据需要不断的刷新来保证数据不会丢失，随机的意思是可以读写任意地址的数据。
SDRAM需要时钟线，常见的频率就是100MHz、133MHz、166MHz、200MHz。

###### 二、DDR3时间参数

1、传输速率
DDR3 1600MT/s、DDR3 1866MT/s、DDR3 2400MT/s、DDR3 3200MT/s。

2、tRCD
tRCD 全称是 RAS-to-CAS Delay，也就是行寻址到列寻址之间的延迟。

3、CL参数
当列地址发出以后就会触发数据传输，但是数据从存储单元到内存芯片 IO 接口上还需要一段时间，这段时间就是非常著名的 CL(CAS Latency)，也就是列地址选通潜伏期。

4、tRC
tRC 是两个 ACTIVE 命令，或者 ACTIVE 命令到 REFRESH 命令之间的周期。

5、tRSA
tRAS 是 ACTIVE 命令到 PRECHARGE 命令之间的最小时间。

###### 三、I.MX6U MMDC控制器

1、多模支持DDR3/DDR3L、LPDDR2 x16位。
2、MMDC最高支持DDR3频率是400MHz、800MT/s。
3、MMDC提供的DDR3连接信号。6ULL给DDR提供了专用的IO。

DDR时钟配置
DDR使用的时钟源为MMDC_CLK_ROOT = PLL2_PFD2 = 396MHz。在前面时钟章节已经设置为396MHz。
CBCMR寄存器的PRE_PERIPH2_CLK_SEL位来选择，也就是bit22:21，设置pre_periph2时钟源，设置为01，也就是PLL2_PFD2作为pre_periph2时钟源。
CBCDR寄存器的PERIPH2_CLK_SEL位，也就是bit26，设置为0，PLL2作为MMDC时钟源，396MHz。
CBCDR寄存器的FABRIC_MMDC_PODF位，bit5~3设置0，也就是1分频。
最终MMDC_CLK_ROOT = 396MHz。

###### 四、DDR3L初始化与测试

1、ddr_stree_tester配置文件
excel配置文件，excel配置好以后realvew.inc会同步的更新。

2、.inc文件
ddr_stree_tester工具需要用到.inc文件。

3、测试
ddr_stree_tester通过USB口讲inc中的配置信息下载到开发板里面。

4、做校准  