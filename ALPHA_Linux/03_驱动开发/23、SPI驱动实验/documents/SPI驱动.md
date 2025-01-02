##### 一、SPI驱动框架

###### 1、裸机下的SPI驱动框架

首先编写SPI控制器驱动，bsp_spi.c和bsp_spi.h。最终提供SPI读写函数spich0_readwrite_byte。

SPI具体芯片驱动：ICM20608

###### 2、Linux下的SPI驱动框架

主机控制器驱动：SOC的SPI外设驱动，此驱动是半导体原厂编写好的，为spi-imx.c。当SPi控制器的设备和驱动匹配以后，spi_imx_probe函数就会执行，完善I.MX6ULL的SPI控制器驱动。

SPI控制器驱动核心就是spi_master结构体的构建。spi_master里面就有如何通过SPI控制器与SPI外设进行通信的函数，此函数是原厂编写的。

```c
spi_master->transfer
spi_master->transfer_one_message // 6ULL主机控制器使用此函数
    
spi_imx_data->spi_bitbang->spi_master
    					 ->setup_transfer
    					 ->txrx_bufs
```

spi_master一般需要使用spi_alloc_master函数申请，释放的话使用函数spi_master_put。

需要初始化spi_master，最终使用函数spi_register_master注册进内核，需要从内核删除spi_master使用函数spi_unregister_master。

```
spi_bitbang->setup_transfer->spi_imx->rx = spi_imx_buf_rx_u8;	最终的SPI接收函数spi_bitbang->setup_transfer->spi_imx->tx = spi_imx_buf_tx_u8;	  最终的SPI发送函数
```

###### 3、SPI设备驱动

SPI设备驱动就是具体的SPI芯片驱动，比如ICM20608。

spi_device：每个spi_device下都有一个spi_master，每个SPI设备，肯定挂载到了一个SPI控制器，比如ICM20608挂载到了6ULL的ECSPI3接口上。

spi_driver：申请、定义一个spi_driver，然后初始化spi_driver中的各个成员变量，当SPI设备和驱动匹配以后，spi_driver下的probe函数就会执行。spi_driver初始化成功以后需要向内核注册，使用函数spi_register_driver，从内核中注销spi_driver使用函数spi_unregister_driver。

##### 二、驱动编写与测试

1、修改设备树，添加IO相关信息。
ECSPI3_SCLK		 ->	UART2_RXD
ECSPI3_MOSI		->	UART2_CTS
ECSPI3_MISO		->	UART2_RTS
ECSPI3_SS0		   ->	UART2_TXD
片选信号不作为硬件片选，而是作为普通的GPIO，我们在程序里自行控制片选引脚。

2、在ECSPI3节点下创建icm20608子节点。

3、需要初始化icm20608芯片，然后从里面读取原始数据。这个过程就要用到如何使用Linux内的SPI驱动API来读写icm20608。用到两个重要的结构体：spi_transfer和spi_message。

spi_transfer用来构建收发数据内容。构建spi_transfer，然后将其打包到spi_message里面。需要使用spi_message_init初始化api_message，然后使用spi_message_add_tail将spi_transfer添加到spi_message里面，最终使用spi_sync和spi_async来发送。
