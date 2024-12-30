##### 一、I2C驱动框架

###### 1、裸机下的I2C驱动框架

首先编写I2C控制器驱动，bsp_i2c.c和bsp_i2c.h为IIC外设驱动，向外提供i2c_master_transfer函数，不管是什么IIC芯片，都可以使用这个函数进行数据读写。

###### 3、I2C总线驱动

I2C适配器在内核里面使用i2c_adapter结构体。IIC控制器、IIC适配器驱动（控制器）核心就是申请i2c_adapter结构体，然后初始化，最后注册。

初始化完成i2c_adapter以后，使用i2c_add_adapter或者i2c_add_number_adapter来向内核注册I2C控制器驱动。

在i2c_adapter结构体里面有个非常重要的成员变量：i2c_algorithm，此变量包含了IIC控制器访问IIC设备的API接口函数，需要I2C适配器编写人员实现。

```c
i2c_adapter
    -> i2c_algorithm
    	-> master_xfer	此函数就是I2C控制器最终进行数据收发的函数。
```

经过查找，I.MX6U的I2C适配器驱动是i2c-imx.c。驱动与设备匹配成功以后，i2c_imx_probe函数就会启动。NXP创建了一个imx_i2c_struct结构体，包含了I.MX6U的I2C的相关属性，这个结构体里面就有i2c_adapter。

设置I2C_adapter下的算法为i2c_imx_algo，通过I.MX6U的I2C控制器驱动I2C或者向I2C设备写入数据的时候最终是通过i2c_imx_xfer函数来完成的。

###### 4、I2C设备驱动

i2c_client：表示I2C设备，不需要我们自己创建i2c_client，我们一般在设备树里面添加具体的I2C芯片，比如fxls8471，系统在解析设备树的时候就会知道有这个I2C设备，然后会创建对应的i2c_client。

I2C设备驱动框架，i2c_driver初始化与注册，需要I2C设备驱动编写人员编写的。I2C驱动程序就是初始化i2c_driver，然后向系统注册，注册使用i2c_register_driver函数、i2c_add_driver宏，如果注销i2c_driver的话，使用i2c_del_driver函数。

###### 5、添加I2C设备信息

在设备树中添加，I2C设备挂到哪个I2C控制器下就在哪个控制器下添加相对应的节点，

##### 二、驱动编写与测试

在I2C1上接了一个AP3216C，UART4_RXD作为I2C1_SDA，UART4_TXD作为I2C1_SCL。

1、修改设备树、IO、添加AP3216C设备节点。

2、编写驱动框架，I2C设备驱动框架，字符设备驱动框架。

3、初始化AP3216C，实现ap3216c_read函数。

重点就是通过I2C控制器来向AP3216C里面发送或者读取数据。使用i2c_transfer这和API函数来完成IIC数据的传输。

```c
/*
	adap：IIC设备对应的适配器，也就是IIC接口，当IIC设备和驱动匹配以后，probe函数会执行，probe函数传递进来的第一个参数就是i2c_client，在i2c_client里面保存了此I2C设备所对应的i2c_adapter。
	msgs:就是构成的I2C传输数据。
*/
int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num);
```

