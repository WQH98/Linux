###### 一、rtc_device结构体

1、RTC也是一个标准字符设备驱动。Linux内核将RTC设备抽象为rtc_device结构体，因此RTC设备驱动就是申请并初始化rtc_device，最后将rtc_device注册到Linux内核里面，这样Linux内核就有一个RTC设备。

2、rtc_device是RTC设备在内核中的具体实现，找到RTC相关节点。

```c
snvs_rtc: snvs-rtc-lp {
	compatible = "fsl,sec-v4.0-mon-rtc-lp";
	regmap = <&snvs>;
	offset = <0x34>;
	interrupts = <GIC_SPI 19 IRQ_TYPE_LEVEL_HIGH>, <GIC_SPI 20 IRQ_TYPE_LEVEL_HIGH>;
				};
```

根据compatible找到驱动文件，drivers/rtc/rtc-snvs.c。驱动文件里面就是初始化rtc_device，并注册。

3、rtc_device结构体里面重点是rtc_class_ops操作集。此结构体内容如下：

```c
static const struct rtc_class_ops snvs_rtc_ops = {
	.read_time = snvs_rtc_read_time,
	.set_time = snvs_rtc_set_time,
	.read_alarm = snvs_rtc_read_alarm,
	.set_alarm = snvs_rtc_set_alarm,
	.alarm_irq_enable = snvs_rtc_alarm_irq_enable,
};
```



###### 二、IMX6U RTC驱动简析

1、驱动和设备匹配以后，snvs_rtc_probe会执行。NXP为6ULL的RTC外设创建了一个结构体snvs_rtc_data。此结构体里面包含了rtc_device结构体成员。

2、首先从设备树里面获取SNVS RTC外设寄存器，初始化RTC，申请中断处理闹钟，中断处理函数是snvs_rtc_irq_handler，最后通过devm_rtc_device_register函数向内核注册ert_device，重点是注册的时候设置了snvs_rtc_ops。

3、当应用通过ioctl函数来驱动RTC时间的时候，RTC核心层的rtc_dev_ioctl函数会执行，通过cmd来决定具体操作，比如RTC_ALM_READ就是读取闹钟，此时rtc_read_alarm就会执行，rtc_read_alarm会找到具体的rtc_device，运行其下的ops结构体里面的read_alarm函数。

4、RTC_ALM_SET就是设置闹钟

```c
rtc_set_alarm
	-> rtc_timer_enqueue
		-> __rtc_set_alarm
			-> rtc->ops->set_alarm
```

