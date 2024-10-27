### 一、官方SDK移植

#### 1、新建cc.h文件

​	SDK包里会用到很多数据类型，所以我们需要在cc.h里面定义一些常用的数据类型。

#### 2、移植文件

​	需要移植的文件fsl_common.h、fsl_iomuxc.h、MCIMX6Y2.h。
​	设备为MCIMX6Y2