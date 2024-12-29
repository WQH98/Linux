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