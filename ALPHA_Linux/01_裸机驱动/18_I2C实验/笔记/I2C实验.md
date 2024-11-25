###### 一、IIC协议详解

1、ALPHA开发板上有一个AP3216C，这是一个IIC接口的器件，这是一个环境光传感器。AP3216C连接到了IIC1上。
I2C1_SCL：使用UART4_TXD这个IO，复用为ALT2。
I2C1_SDA：使用UART4_RXD这个IO，复用为ALT2。

2、I2C分为SCL和SDA，这两个必须要接上拉电阻到VCC，比如3.3V，一般是4.7K上拉电阻。

3、I2C总线支持多从机， 