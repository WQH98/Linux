###### 一、input子系统简介

按键、鼠标、键盘、触摸屏等都属于输入（input）设备，Linux内核为此专门做了一个叫做input子系统的框架来处理输入事件，输入设备本质上还是字符设备，只是在此基础上套上了input框架，用户只需要负责上报输入事件，比如按键值、坐标等信息，input核心层负责处理这些事件。

input子系统也是字符设备，input核心层会帮我们注册input字符设备驱动。既然内核已经帮我们写好了input驱动，那我们要干啥呢？需要我们去完善具体的输入设备，完善输入设备的时候就要按照input子系统驱动框架的要求来。

1、input_dev

申请、初始化并注册input_dev，使用input_allocate_device申请，evbit表示事件，比如按键对应的事件就是EV_KEY，如果要连按，那么还要加EV_REP。

设置按键对应的键值，也就是keybit。

初始化完成input_dev以后，需要使用input_register_device向内核注册。

2、事件上报

按键按下以后上报事件，比如对于按键而言就是在按键中断服务函数或者消抖定时器里面获取按键按下情况，并且上报，可以使用input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)函数。

对于按键而言也可以使用input_report_key(struct input_dev *dev, unsigned int code, int value)。

使用上面两个函数上报完成输入事件以后，还需要使用input_sync做同步。

