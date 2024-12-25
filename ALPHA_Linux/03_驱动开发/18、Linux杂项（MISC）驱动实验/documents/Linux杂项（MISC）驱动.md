###### 一、内核自带MISC驱动简介

1、MISC设备的主设备号为10，MISC设备会自动创建cdev，不需要我们手动创建。

2、MISC驱动编写的核心就是初始化miscdevice结构体变量，然后使用misc_register向内核注册，卸载驱动的时候使用misc_deregister来卸载miscdevice。

3、MISC驱动是基于platform。

4、如果设置miscdevice里面minor为255的话，表示由内核自动分配次设备号。