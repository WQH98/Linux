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
/*
	可以通过slot的ABS_MT_TRACKING_ID来新增、替换或删除触摸点。一个非负数的ID表示一个有效的触摸点，-1表示这个ID未使用slot。一个以前不存在的ID表示这是一个新加的触摸点，一个ID如果再也不存在了就表示删除了。
	
*/
ABS_MT_SLOT 0						// 表示要上报第一个触摸点信息
ABS_MT_TRACKING_ID 45				// 通过调用input_mt_report_slot_state
ABS_MT_POSITION_X x[0]				// 第一个点X轴的坐标 使用事件ABS_MT_POSITION_X
ABS_MT_POSITION_Y y[0]				// 第一个点Y轴的坐标 使用事件ABS_MT_POSITION_Y
ABS_MT_SLOT 1						// 表示要上报第二个触摸点信息
ABS_MT_TRACKING_ID 46				// 通过调用input_mt_report_slot_state
ABS_MT_POSITION_X x[1]				// 第二个点X轴的坐标 使用事件ABS_MT_POSITION_X
ABS_MT_POSITION_Y y[1]				// 第二个点Y轴的坐标 使用事件ABS_MT_POSITION_Y
... ...
SYN_REPORT							// 所有点发送完成以后 input_sync
```

上报信息是通过不同的事件来上传的：ABS_MT_XXX

5、中断线程化，硬件中断具有最高优先级，不论什么时候只要硬件中断发生，那么内核都会终止当前正在执行的操作，转而去执行中断处理程序（不考虑关闭中断和中断优先级的情况），如果中断非常频繁的话那么内核将会频繁的执行中断处理程序，导致任务得不到及时的处理。中断线程化以后中断将作为内核线程运行，而且也可以被赋予不同的优先级，任务的优先级可能比中断线程的优先级高，这样做的目的就是保证高优先级的任务能被优先处理，之前把比较耗时的中断放在下半部处理，虽然下半部可能被延迟处理，但是依旧高于线程执行，中断线程化可以让这些比较耗时的下半部与进程进行公平竞争。要注意，并不是所有的中断都可以被线程化，重要的中断就不能这么操作，对于触摸屏而言只要手指放在屏幕上，他可能就会一直产生中断（视具体芯片而定，FT5426是这样的），中断处理程序里面需要通过I2C读取触摸信息并上报给内核，I2C的速度最大只有400KHz，算是低速外设，不断地产生中断、读取触摸信息、上报信息会导致处理器在触摸中断上花费大量时间，但是触摸相对来说不是那么重要的事件，因此可以将触摸中断线程化。如果觉得触摸中断很重要，那么就可以不将其进行线程化处理。总之，要不要将一个中断进行线程化处理是需要自己根据实际情况去衡量的。

###### 二、驱动编写与测试

1、驱动主框架是IIC设备。会用到中断，在中断处理函数里面上报触摸点信息，要用到input子系统。

2、设备树IO修改，IIC节点添加。
INT -> GPIO1_IO09
CT_RST -> SNVS_TAMPER9
I2C_SDA -> UART5_RXD
I2C_SCL -> UART5_TXD

3、在I2C2节点下添加ft5426。

4、主题I2C框架准备好，复位引脚和中断引脚，包括中断的初始化，input子系统框架，初始化FT5426，在中断服务函数里面读取触摸坐标值，上报给系统。