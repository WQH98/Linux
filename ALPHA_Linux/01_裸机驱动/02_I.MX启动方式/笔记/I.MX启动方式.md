### 一、硬件启动方式选择

#### 1、启动方式的选择

​	LED灯实验，是从SD卡读取bin文件并启动的，说明6ULL支持从SD卡启动，6ULL 支持多种启动方式。

1、启动方式选择：
	通过BOOT_MODE0和BOOT_MODE1这两个IO来控制（这两个IO内部默认下拉）。选择从USB启动还是内部BOOT启动，如果要烧写系统到开发板中可以选择从USB下载，下载到SD卡、EMMC、NAND等外置存储中。烧写完成设置从内部BOOT启动，然后从相应的外置存储中启动。

#### 2、启动设备的选择

​	前提是，设置MODE1和MODE0是从内部BOOT启动的，也就是MODE1=1、MODE0=0。

​	支持NOR-FLASH、oneNAND、NAND FLASH、QSPI FLASH、SD/EMMC、EEPROM。我们最常用的就是NAND FLASH、SD、EMMC甚至QSPI FLASH。

​	通过BOOT_CFG选择启动设备，有BOOT_CFG1、2、4组成，每个8位。BOOT_CFG是由LCD_DATA0~23来设置的。在ALPHA开发板上，大部分默认都47K下拉电阻接地。BOOT_CFG4的8根线全部接地。BOOT_CFG2的8根线全部接地，除了BOOT_CFG2[3]，此位用来选择SD卡启动接口（6ULL支持两个SD启动，BOOT_CFG2[3]为0是从SD卡1启动，BOOT_CFG2[3]为1是从SD卡2启动）。BOOT_CFG1的0、1、2都是定死的，3、4、5、6、7是可以设置的。

### 二、启动头文件

#### 1、Boot Room 做的事情

​	设置内核时钟为396MHz，使能MMU和Cache，使能L1 Cache、L2 Cache和MMU，目的就是为了加速启动。
​	从BOOT_CFG设置的外置储存中，读取image，然后做相应的处理。

#### 2、IVT和Boot Data数据

​	Bin文件前面要添加头部信息。可以得到，我们烧写到SD卡中的load.imx文件在SD卡中的起始地址是0x400，也就是1024。
​	头部大小为3KB，加上偏移的1KB，一共是4KB。因此在SD卡中bin文件起始地址为4096。
​	IVT大小为32B / 4 = 8条

### 三、DCD数据

​	Derice configuration Data(DCD) ，DCD数据就是配置6ULL内部寄存器的。		
​	首先，将CCGR0~CCGR6全部写为0xFFFFFFFF，表示打开所有外设时钟。然后就是DDR初始化参数。设置DDR控制器，也就是初始化DDR。

### 四、其他的数据

​	检查数据命令、NOP命令、解锁命令。这些也都属于DCD。