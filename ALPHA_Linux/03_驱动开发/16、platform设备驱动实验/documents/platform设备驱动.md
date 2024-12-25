##### 一、LInux驱动分离与分层

以前编写的驱动实验都很简单，就是对IO的操作。

目的：方便驱动的编写，提高软件的重用以及跨平台性能。

###### 1.1、驱动的分隔与分离

主机驱动、设备驱动

1、单片机驱动开发，IIC设备MPU6050。
2、将驱动分离：主机控制器驱动和设备驱动，主机控制器驱动一般是半导体厂商写的，在Linux驱动框架下编写具体的设备驱动。
3、中间的联系就是核心层。

###### 1.2、驱动的分层

分层就是将一个复杂的工作分成了4层，分而做之，降低难度。每一层只关注自己的事情，系统已经将其中的核心层和事件处理层做好了，所以我们只需要来写硬件相关的驱动层代码就好。

##### 二、总线-驱动-设备（驱动-总线-设备）

根据驱动的分离和分层衍生出了总线（bus）-驱动（driver）-设备（device）驱动框架。

总线代码我们不需要编写，Linux内核提供给我们使用的，我们需要编写驱动和设备，当向总线注册驱动的时候，总线会从现在的所有设备中查找，看看哪个设备和此驱动匹配。同理，当向总线注册设备的时候总线也会在现有的驱动中查看与之匹配的驱动。

驱动：是具体的设备驱动。
设备：设备属性，包括地址范围，如果是IIC的话还有IIC器件地址，速度。

###### 2.1、总线

总线数据类型为：bus_type。向内核注册总线使用bus_register。

1、总线主要工作就是完成总线下的设备和驱动之间的匹配。

###### 2.2、驱动

驱动数据类型为：device_driver，驱动程序向内核注册驱动采用driver_register。

driver_register
	-> bus_add_drive
		-> driver_attach	// 查找bus下所有设备，找预期匹配的。
			-> bus_for_each_dev(div->bus, NULL, div, __driver_attach)
				-> __driver_attach	// 每个设备都调用此函数 查看每个设备是否与驱动匹配
					-> driver_match_device	// 检查是否匹配
						-> driver_probe_device	
							-> really_probe
								-> div->probe(dev)		// 执行driver的probe函数

向总线注册驱动的时候，会检查当前总线下的所有设备，有没有与此驱动匹配的设备，如果有的话就执行驱动里面的probe函数。

驱动和设备匹配以后驱动里面的probe函数就会执行。

###### 2.3、设备

设备数据类型为：device，通过device_register向内核注册设备。

device_register
	-> device_add
		-> bus_add_device
		-> bus_probe_device
			->device_attach
				-> bus_for_each_drv(dev->bus, NULL, dev, __device_attach)
					-> \_\_device_attach
						-> driver_match_device		// 匹配驱动
							-> bus->match
						-> driver_probe_device		// 执行此函数
							-> really_probe
								-> dev->probe(dev)

驱动与设备匹配以后驱动的probe函数就会执行，probe函数就是驱动编写人员去编写的。

##### 三、platform平台驱动模型

根据总线-驱动-设备这样的驱动模型，IIC、SPI、USB这样实实在在的总线是完全匹配的，但是要有一些外设是没法归结为具体的总线：比如定时器、RTC、LCD等。为此Linux内核创造了一个虚拟的总线：platform总线。

1、方便开发，Linux提出了驱动分离与分层。
2、进一步引出了驱动-总线-设备驱动模型，或者框架。
3、对于SOC内部的RTC、timer等等不好归结为具体的总线，为此Linux内核提出了一个虚拟总线：platform总线。platform设备和platform驱动。

