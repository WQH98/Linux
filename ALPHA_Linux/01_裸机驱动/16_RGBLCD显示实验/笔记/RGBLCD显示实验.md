###### 一、RGBLCD显示原理简介

1、像素点
像素点就相当于一个“小灯”，不管是液晶屏幕，还是手机、平板，RGBLCD屏幕都是由一个个的彩色小灯构成的。彩色点阵屏每个像素点有三个小灯，红色、绿色和蓝色，也叫做RGB，RGB就是光的三原色。通过调整RGB三种颜色的比例，就可以实现多种颜色的显示。

2、分辨率
想要显示文字、图片、视频等等，就需要很多个像素点，分辨率说的就是像素点的个数，1080P、720P、2K、4K、8K等。1080P = 1920 x 1080，表示一行有1920个像素点，一列有1080个像素点。在显示器尺寸不变的情况下，分辨率越高，显示效果越精细。

3、像素格式
如何将RGB三种颜色进行量化，每种颜色用8bit表示，RGB就需要8 + 8 + 8共24bit。可以描述出2^24 = 16777216 = 1677万种颜色。现在流行10bit，HDR10支持HDR效果的10bit面板，也就是10 + 10 + 10共30bit。
在RGB888的基础上在加上8bit的ALPHA通道，也就是透明通道，ARGB8888 = 32bit。

4、LCD屏幕接口
RGB格式的屏幕，一般叫做RGB接口屏。
屏幕接口有：MIPI、LVDS、MCU、RGB接口。
正点原子的屏幕ID：在RGBLCD屏幕上对R7、G7、B7焊接上拉或下拉电阻实现不同的ID。
正点原子的ALPHA底板RGB屏幕接口使用了3个3157模拟开关，原因是防止LCD屏幕上的ID电阻影响到6ULL的启动。

5、LCD时间参数和LCD时序
水平：HSYNC，水平同步信号也叫做行同步信号。当出现HSYNC信号的时候，表示新的一行开始显示。
1、产生HSYNC信号，表示新的一行数据开始显示，HSYNC信号得维持一段时间，这个时间叫做HSPW。
2、HSYNC信号完成以后需要一段时间延时，这段时间叫做HBP。
3、显示1024个像素点的数据，需要1024个clk。
4、一行像素显示完成以后，到HSYNC下一行信号的产生之间有个延时，叫做HFP。
因此，真正显示一行所需的时间是HSPW + HBP + WIDTH(屏幕水平像素点个数，比如1024) + HFP = 20 + 140 + 1024 + 160 = 1344clk。
垂直：VSYNC，垂直同步信号也叫做帧同步信号。当出现VSYNC信号的时候，表示新的一帧开始显示。
1、VSYNC信号持续一段时间，为VSPW。
2、VSYNC信号完成以后，需要一段时间延时，这段时间叫做VBP。
3、VBP信号结束以后，就是要显示的行数，比如600。
4、所有的行显示完成以后，一段VFP延时。
(VSPW + VBP + height(600) + VFP) * (HSPW + HBP + width(1024) + HFP) = (3 + 20 + 600 + 12) * (20 + 140 + 1024 + 160) = 635 * 1344 = 853440。显示一帧图像需要853440个时钟数，那么显示60帧就是853440 * 60 = 51206400 ≈ 51.2M，所以像素时钟就是51.2M。

6、显存
显存就是显示存储空间，采用ARGB8888 = 32bit = 4B。这4个字节的数据表示一个像素点的信息，必须得存起来。1024 * 600 * 4 = 2.5MB。因此需要流出2.5MB的内存给LCD用，方法很简单，直接定义一个32位的数组，u32 lcdframe[1024 * 600]。

###### 二、6ULL ICDIF控制器接口原理

1、我们使用DOTCLK接口，也就是VSYNC、HSYNC、ENABLE(DE)和DOTCLK(PCLK)。

2、LCDIF_CTRL寄存器，bit0必须置1。bit1设置数据格式24位全部有效，设置为0。bit5设置LCDIF接口工作在主机模式下，必须要置1。bit9~8设置输入像素格式为24bit，设置为3。bit11~10设置数据传输宽度为24bit，设置为3。bit13~12设置数据交换，我们不需要交换，设置为0。bit15~14设置输入数据交换，我们不需要交换，设置为0。bit17置1，LCD工作在DOTCLK模式下。bit19必须置1，因为工作在DOTCLK模式下。bit31是复位功能，必须置0。

3、LCDIF_CTRL1寄存器，bit19~16设置为0x07，相当于24位格式。

4、LCDIF_TRANSFER_COUNT寄存器，bit15~0是LCD一行的像素，设置为1024。bit31~16是LCD一共有多少行，设置为600。

5、LCDIF_VDCTRL0寄存器，bit17~0为VSPW参数，bit20设置VSYNC信号的宽度单位，设置为1。bit21设置为1。bit24设置ENABLE信号极性，为0的时候是低电平有效，为1的时候是高电平有效，设置为1。bit25设置CLK信号极性，设置为0。bit26设置HSYNC信号极性，设置为0，低电平有效。bit27设置VSYNC信号极性，设置为0，低电平有效。bit28设置为1，开始ENABLE信号。bit29设置为0，VSYNC输出。

6、LCDIF_VDCTRL1寄存器为两个VSYNC信号之间的长度，那就是VSPW + VBP + HEIGHT + VFP。

7、LCDIF_VDCTRL2寄存器bit17~0是两个HSYNC信号之间的长度，那就是HSPW + HBP + WIDGET + HFP。bit31~18为HSPW。

8、LCDIF_VDCTRL3寄存器，bit15~0是VBP + VSPW。bit27~16是HBP + HSPW。

9、LCDIF_VDCTRL4寄存器，bit17~0是一行有多少个像素点，1024。

10、LCDIF_CUR_BUF寄存器，LCD当前缓存，显存首地址。

11、LCDIF_NEXT_BUF寄存器，LCD下一帧数据首地址。

12、LCD IO初始化。 

###### 三、LCD像素时钟的设置

1、LCD需要一个CLK信号，这个时钟信号是6ULL的CLK引脚发送给RGBLCD屏幕的。比如7寸1024*600的屏幕需要51.2MHz的CLK。 LCDIF_CLK_ROOT就是6ULL的像素时钟。设置PLL5也就是Video PLL，为LCD的时钟源。

2、PLL5_CLK = Fref * DIV_SELECT，DIV_SELECT就是CCM_ANALOG_PLL_VIDEO的bit6~0，也就是DIV_SELECT位。可选范围27~54。设置CCM_ANALOG_PLL_VIDEO寄存器的bit20~19为2，表示1分频。

3、设置CCM_ANALOG_MISC2寄存器的bit31~30为0，表示1分频。

4、不使用小数分频器，因此CCM_ANALOG_PLL_VODEO_NUM寄存器设置为0，再设置CCM_ANALOG_PLL_VIDEO_DENOM寄存器也等于0。

5、CCM_CSCDR2寄存器的bit17~15，设置LCDIF_PRE_CLK_SEL，选择LCDIF_CLK_ROOT的时钟源，设置为2，表示使用LCDIF时钟源为PLL5。bit14~12为LCDIF_PRED位，设置前级分频，可以设置0~7分别对应1~8分频。bit11~9为LCDIF_CLK_SEL，选择LCD CLK的最终时钟源，设置为0，表示LCDIF的最终时钟源来源于pre-muxed LCDIF clock。

6、CCM_CBCMR寄存器的bit25~23为LCDIF_PODF，设置第二集分频，可设置为0~7分别对应1~8分频。