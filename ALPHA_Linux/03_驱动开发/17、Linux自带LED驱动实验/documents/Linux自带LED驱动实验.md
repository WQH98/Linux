###### 一、内核自带LED驱动使能

1、内核自带的驱动，都是通过图形化界面配置，选择使能或者不使用。
输入：make menuconfig
使能驱动以后再.config里面就会存在：CONFIG_LEDS_GPIO=y
再Linux内核源码里面一般驱动文件夹下Makefile会使用CONFIG_XXX来决定要编译哪个文件。
obj-$(CONFIG_LEDS_GPIO)				+= leds-gpio.o
obj-y += leds-gpio.o	->	leds-gpio.c

###### 二、内核自带LED驱动应用

1、首先将驱动编译进内核里面。

2、根据绑定文档在设备树里面添加对应的设备节点信息。

3、如果无设备树，那么就要使用platform_device_register向总线注册设备。

4、如果有设备树，那么就直接修改设备树，添加指定的节点。