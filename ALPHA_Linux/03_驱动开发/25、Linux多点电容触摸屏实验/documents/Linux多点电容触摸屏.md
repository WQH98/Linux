###### 一、多点电容触摸屏驱动框架

1、电容触摸屏，需要上报多点触摸信息，通过触摸芯片，比如FT5426，这是一个IC。多点电容触摸屏本质是IIC驱动。

2、触摸IC，比如TF5426，一般都是有INT引脚的，当检测到触摸信息，就会触发中断，那么就要在中断处理函数里面读取触摸点信息。

3、得到触摸点信息以后，LInux系统里面当做一个input设备，LInux系统下有触摸屏上报的流程，涉及到input子系统下触摸信息的上报。

4、触摸协议分为TypeA和TypeB
TypeA：一股脑全部上报所有触摸点信息，系统去甄别这些信息属于哪个触摸点的。上报方式如下：   

```c
ABS_MT_POSITION_X x[0]				// 第一个点X轴的坐标 使用事件ABS_MT_POSITION_X
ABS_MT_POSITION_Y y[0]				// 第一个点Y轴的坐标 使用事件ABS_MT_POSITION_Y
SYN_MT_REPORT		// 点与点之间使用SYN_MT_REPORT隔离 函数是input_mt_sync
ABS_MT_POSITION_X x[1]				// 第二个点X轴的坐标 使用事件ABS_MT_POSITION_X
ABS_MT_POSITION_Y y[1]				// 第二个点Y轴的坐标 使用事件ABS_MT_POSITION_Y
SYN_MT_REPORT		// 点与点之间使用SYN_MT_REPORT隔离 函数是input_mt_sync
... ...
SYN_REPORT			// 所有点发送完成以后使用函数input_sync 上报事件SYN_REPORT
```

TypeB：适用于触摸芯片有硬件追踪能力的。TypeB使用slot来区分触摸点。slot使用ABS_MT_TRACKING_ID来增加、删除、替换一个触摸点信息。

```c
ABS_MT_SLOT 0						// 表示要上报第一个触摸点信息
ABS_MT_TRACKING_ID 45				// 通过调用input_mt_report_slot_state
ABS_MT_POSITION_X x[0]				// 第一个点X轴的坐标 使用事件ABS_MT_POSITION_X
ABS_MT_POSITION_Y y[0]				// 第一个点Y轴的坐标 使用事件ABS_MT_POSITION_Y
ABS_MT_SLOT 1						// 表示要上报第二个触摸点信息
ABS_MT_TRACKING_ID 46				// 通过调用input_mt_report_slot_state
ABS_MT_POSITION_X x[1]
ABS_MT_POSITION_Y y[1]
... ...
SYN_REPORT
```

上报信息是通过不同的事件来上传的：ABS_MT_XXX