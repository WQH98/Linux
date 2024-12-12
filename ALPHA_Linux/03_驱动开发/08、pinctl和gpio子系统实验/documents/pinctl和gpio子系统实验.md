###### 一、6ULL的GPIO使用

1、设置PIN的复用和电气属性。
2、配置GPIO的复用和电气属性。

###### 二、pinctrl子系统

1、借助pinctrl来设置一个PIN的复用和电气属性。
2、pinctrl子系统主要工作内容如下：
	①、获取设备树中pin信息。
	②、根据获取到的pin信息来设置pin的复用功能。
	③、根据获取到的pin信息来设置pin的电气属性，比如上拉/下拉、速度、驱动能力等。
3、打开imx6ull.dtsi和imx6ull-alientek-emmc.dts

```c
/*
	IOMUXC控制器
	内存起始地址0x020E0000
	根据设备的类型，创建对应的子节点，然后设备所用PIN都放到此节点。
*/
iomuxc: iomuxc@020e0000 {
    compatible = "fsl,imx6ul-iomuxc";
    reg = <0x020e0000 0x4000>;
    pinctrl-names = "default";
	pinctrl-0 = <&pinctrl_hog_1>;
	imx6ul-evk {
		pinctrl_hog_1: hoggrp-1 {
			fsl,pins = <
				MX6UL_PAD_UART1_RTS_B__GPIO1_IO19	0x17059 /* SD1 CD */
				MX6UL_PAD_GPIO1_IO05__USDHC1_VSELECT	0x17059 /* SD1 VSELECT */
				MX6UL_PAD_GPIO1_IO09__GPIO1_IO09        0x17059 /* SD1 RESET */
			>;
		};
        ...	...
    };
};

/*
	gpr控制器
	内存起始地址0x020E4000
*/
gpr: iomuxc-gpr@020e4000 {
    compatible = "fsl,imx6ul-iomuxc-gpr", "fsl,imx6q-iomuxc-gpr", "syscon";
    reg = <0x020e4000 0x4000>;
};

/*
	IOMUXC SNVS控制器
	内存起始地址0x02290000
*/
iomuxc_snvs: iomuxc-snvs@02290000 {
    compatible = "fsl,imx6ull-iomuxc-snvs";
    reg = <0x02290000 0x10000>;
};

/*
	如何添加一个pin的信息
	我们在imx6ul-pinfunc.h里面找到：
	#define MX6UL_PAD_UART1_RTS_B__GPIO1_IO19                         0x0090 0x031C 0x0000 0x5 0x0
	宏定义作用为将引脚MX6UL_PAD_UART1_RTS_B复用为GPIO1_IO19，后面16进制数字代表的含义如下：
	<mux_reg conf_reg input_reg mux_mode input_val>
	IOMUXC父节点首地址0x020E0000，因此UART1_RTS_B这个PIN的mux寄存器地址就是0x020E0000 + 0x0090 = 0x020E0090。
	conf_reg地址：0x020E0000 + 0x031C= 0x020E031C。这个寄存器就是UART1_RTS_B的电气属性配置寄存器。
	input_reg = 0表示UART1_RTS_B这个PIN没有输入功能。
	mux_mode = 5表示表示复用为GPIO1_IO19，将其写入0x020E0090。
	input_value是向input_reg寄存器里面写的值。
	input_value是向input_reg寄存器里面写的值。
	0x17059：为PIN的电气属性配置寄存器值。
*/
pinctrl_hog_1: hoggrp-1 {
    fsl,pins = <
        MX6UL_PAD_UART1_RTS_B__GPIO1_IO19	0x17059 /* SD1 CD */
        >;
};
```

4、pinctrl驱动
①、pinctrl和gpio子系统就是驱动分离与分层思想下的产物，驱动分离与分层其实就是按照面向对象编程的设计思想而设计的设备驱动框架。
②、如何找到IMX6ULL对应的pinctrl子系统驱动。设备树里面的节点是如何根绝驱动匹配的呢？通过compatible属性，此属性是字符串列表，驱动里面有一个描述驱动兼容性的东西，当设备树节点的compatible属性与驱动里面的设备兼容性匹配，也就是一模一样的时候就表示设备与驱动匹配了。所以我们只需要全局搜索compatible属性的值，看看在哪个.c文件里面存在，那么此.c文件就是驱动文件。找到pinctrl-imx6ul.c文件，那么此文件就是6UL/6ULL的pinctrl驱动文件。
当驱动和设备匹配以后执行probe函数，也就是imx6ul_pinctrl_probe函数。
imx_pinconf_set函数设置PIN的电气属性。
imx_pmx_set函数设置PIN的复用。

###### 三、gpio子系统

1、使用gpio子系统来使用gpio。
2、gpio在设备树中的表示方法：/device-tree/bindings/gpio。
3、驱动中对gpio的操作函数。首先获取到GPIO所处的设备节点，比如of_find_node_by_path。然后获取GPIO编，of_get_named_gpio函数，返回值就是GPIO编号，使用gpio_request函数请求此编号的GPIO，使用gpio_free函数。设置GPIO，输入或输出，gpio_direction_output或gpio_direction_intput。如果是输入，那么通过gpio_get_value函数读取GPIO值，如果是输出，那么通过gpio_set_value函数设置GPIO值。

```c
/*
	定义了一个cd-gpios属性。
	此处使用GPIO1-IO19。
	如何从设备树中获取属性信息，通过of函数。
*/
&usdhc1 {
	pinctrl-names = "default", "state_100mhz", "state_200mhz";
	pinctrl-0 = <&pinctrl_usdhc1>;
	pinctrl-1 = <&pinctrl_usdhc1_100mhz>;
	pinctrl-2 = <&pinctrl_usdhc1_200mhz>;
	cd-gpios = <&gpio1 19 GPIO_ACTIVE_LOW>;
	keep-power-in-suspend;
	enable-sdio-wakeup;
	vmmc-supply = <&reg_sd1_vmmc>;
	status = "okay";
};
```

4、gpio驱动
①、gpiolib：分为两部分，给原厂编写GPIO底层驱动的，给驱动开发人员使用GPIO操作函数的。使用gpiochip_add向系统添加gpio_chip，这些都是半导体原厂做的，这部分就是最底层的GPIO驱动。
②、gpio驱动，在drivers/gpio目录下，gpio-xxx文件。
③、申请IO失败的时候，大部分原因是因为这个IO被其他外设占用了。需要检查设备树，查找有哪些使用同一IO的设备。需要检查GPIO复用设置（也就是pinctrl设置）和gpio使用。

###### 四、总结

1、添加pinctrl信息。
2、检查当前设备树中要使用的IO有没有被其他设备使用，如果有的话要处理。
3、添加设备节点，在设备树中创建一个属性，此属性描述所使用的gpio。
4、编写驱动，获取对应的gpio编号，并申请IO，成功以后即可使用此IO。