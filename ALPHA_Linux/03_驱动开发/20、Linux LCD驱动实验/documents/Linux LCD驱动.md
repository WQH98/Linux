###### 一、Framebuffer设备

在裸机下，LCD驱动编写流程如下：
①、初始化IMX6U的eLCDIF控制器，重点是LCD屏幕宽度（width）、高度（height）、hspw、hbp、hfp、vspw、vbp和vfp等信息。
②、初始化LCD像素时钟。
③、设置RGBLCD显存。
④、应用程序直接通过操作显存来操作LCD，实现在LCD上显示字符、图片等信息。

只支持RGB LCD屏幕，framebuffer是一种机制，应用程序操作驱动里面LCD显存的一种机制，因为应用程序需要通过操作显存来在LCD上显示字符、图片等信息。

通过framebuffer机制将底层的LCD抽象为/dev/fbx，x=0、1、2....，应用程序可以通过操作/dev/fbx来操作屏幕。

framebuffer在内核中的表现就是fb_info结构体，屏幕驱动重点就是初始化fb_info里面的各个成员变量。初始化完成fb_info以后，通过register_framebuffer函数向内核注册刚刚初始化以后的fb_info。

卸载驱动的时候调用unregister_framebuffer来卸载前面注册的fb_info。

###### 二、LCD驱动简析

驱动文件为mxsfb.c，为platform驱动框架，驱动和设备匹配以后，mxsfb_probe函数就会执行。

结构体mxsfb_info，给mxsfb_info申请内存，申请fb_info，然后将这两个联系起来。

host.base就是内存映射以后得LCDIF外设基地址。

mxsfb.probe函数会调用mxsfb_init_fbinfo来初始化fb_info。

初始化LCDIF控制器，mxsfb_init_fbinfo_dt函数会从设备树中读取相关属性信息。

###### 三、驱动编写

1、屏幕引脚设置，将屏幕引脚电气属性改为0x49，就是修改LCD引脚驱动能力，因为加了ID电阻的原因，所以要降低驱动能力。

2、背光，一般屏幕背光用PWM控制亮度，一般测试屏幕的时候直接将背光引脚拉高或拉低。