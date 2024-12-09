###### 一、正点原子官方Linux内核编译

uboot编译步骤：
	1、distclean清理工程
	2、make xxx_defconfig 使用默认配置工具配置工程
	3、make 编译
	4、make menuconfig 打开配置界面，进行配置。

Linux内核的默认配置文件保存在arch/arm/configs