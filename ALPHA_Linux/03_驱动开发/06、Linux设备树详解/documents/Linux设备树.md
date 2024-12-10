###### 一、什么是设备树？

1、uboot启动内核用到zImage、imx6ull-alientek-emmc.dtb，bootz 80800000 - 83000000。
2、设备树：设备和树。
3、在单片机驱动里面比如W25QXX、SPI、速度都是在.c里面写死的。板级信息都写到.c里面，导致Linux内核臃肿，因此，将板子信息做成独立的格式，文件扩展名为.dts。一个平台或者机器对应一个.dts。

###### 二、DTS、DTB和DTC的关系

1、描述设备树的文件叫做DTS，这个DTS文件采用树形结构描述板级设备，也就是开发板上的设备信息，比如CPU数量、内存基地址、IIC接口上接了哪些设备、SPI接口上接了哪些设备等等。
2、.dts相当于.c，就是DTS源码文件。DTC工具相当于gcc编译器，将.dts编译成.dtb。.dtb相当于bin文件，或可执行文件。
3、通过make .dtbs编译所有的dts文件，如果要编译指定的dtbs，make imx6ull-alientek-emmc.dtb。

###### 三、DTS基本语法

1、设备树也有头文件，扩展名为.dtsi。可以将一款SOC的其他设备/平台的共有信息提取出来，作为一个通用的.dtsi文件。
2、DTS文件也是'/'开始。
3、从/节点开始描述设备信息。
4、节点名字，完整的要求是node-name@unit-address。unit-address一般都是外设寄存器的起始地址，有时候是I2C的设备地址，或者其他含义，具体问题具体分析。设备树里面常常遇到如下所示节点名字：intc:interrupt-conyroller@00a01000，:前面是标签，后面才是名字。比如前面的例子，intc是节点标签，interrupt-conyroller@00a01000是节点名字。

###### 四、设备树在系统中的体现

系统激动以后可以在根文件系统里面看到设备树的节点信息。在/proc/device-tree/目录下存放着设备树信息，内核启动的时候会解析设备树，然后在/proc/device-tree/目录下呈现出来。

###### 五、特殊节点

1、aliases节点：单词 aliases 的意思是“别名”，因此 aliases 节点的主要功能就是定义别名,定义别名的目的就是为了方便访问节点。不过我们一般会在节点命名的时候会加上 label，然后通过&label来访问节点，这样也很方便，而且设备树里面大量的使用&label 的形式来访问节点。
2、chosen节点：主要目的就是将uboot里面bootargs环境变量值，传递给Linux内核作为命令行参数，cmd line。内核在启动过程中会输出cmd line值。
3、uboot是如何向kerenl传递bootargs？经过查看发现chosen节点中包含bootargs属性，属性值和uboot的bootargs一致，在uboot的fdt_chosen函数中会查找chosen节点，并且在里面添加bootargs属性，属性值为bootargs变量值。

###### 六、标准属性

1、compatible 属性，compatible 属性也叫做“兼容性”属性,这是非常重要的一个属性！compatible 属性的值是一个字符串列表，compatible 属性用于将设备和驱动绑定起来。字符串列表用于选择设备所要使用的驱动程序。根节点下的compatible属性用于内核查找是否支持此平台或设备。在不使用设备树的时候通过machine id来判断内核是否支持此机器。使用设备树的时候不使用machine id，而是使用根节点/下的compatible属性值。
2、model 属性，model 属性值也是一个字符串，一般 model 属性描述设备模块信息,比如名字什么的。
3、status 属性，status 属性看名字就知道是和设备状态有关的，status 属性值也是字符串，字符串是设备的状态信息。
4、#address-cells 和#size-cells 属性，这两个属性的值都是无符号 32 位整形，#address-cells 和#size-cells 这两个属性可以用在任何拥有子节点的设备中，用于描述子节点的地址信息。 #address-cells 属性值决定了子节点 reg 属性中地址信息所占用的字长(32 位)，#size-cells 属性值决定了子节点 reg 属性中长度信息所占的字长(32 位)。#address-cells 和#size-cells 表明了子节点应该如何编写 reg 属性值，一般 reg 属性都是和地址有关的内容，和地址相关的信息有两种：起始地址和地址长度。
5、reg 属性，reg 属性的值一般是(address,length)对。reg 属性一般用于描述设备地址空间资源信息，一般都是某个外设的寄存器地址范围信息。
6、ranges 属性，ranges 属性值可以为空或者按照(child-bus-address，parent-bus-address，length)格式编写的数字矩阵，ranges 是一个地址映射/转换表，ranges 属性每个项目由子地址、父地址和地址空间长度这三部分组成。
7、name 属性，name 属性值为字符串，name 属性用于记录节点名字，name 属性已经被弃用，不推荐使用name 属性，一些老的设备树文件可能会使用此属性。
8、device_type 属性，device_type 属性值为字符串，IEEE 1275 会用到此属性，用于描述设备的 FCode，但是设备树没有 FCode，所以此属性也被抛弃了。此属性只能用于 cpu 节点或者 memory 节点。

###### 七、Linux内核的OF操作函数

1、驱动如何获取到设备树中节点信息，在驱动中使用OF函数获取设备树属性内容。
2、驱动要想获取到设备树节点内容，首先要找到节点。

```c
/*
	通过节点名字查找指定的节点
	from：开始查找的节点，如果为NULL表示从根节点开始查找整个设备树
	name：要查找的节点名字
	返回值：找到的节点，如果为NULL表示查找失败
*/
struct device_node *of_find_node_by_name(struct device_node *from, const char *name);

/*
	通过 device_type 属性查找指定的节点
	from：开始查找的节点，如果为 NULL 表示从根节点开始查找整个设备树。
	type：要查找的节点对应的 type 字符串，也就是 device_type 属性值。
	返回值：找到的节点，如果为 NULL 表示查找失败。
*/
struct device_node *of_find_node_by_type(struct device_node *from, const char *type);

/*
	根据 device_type 和 compatible 这两个属性查找指定的节点
	from：开始查找的节点，如果为 NULL 表示从根节点开始查找整个设备树。
	type：要查找的节点对应的 type 字符串，也就是 device_type 属性值，可以为 NULL，表示忽略掉 device_type 属性。
	compatible：要查找的节点所对应的 compatible 属性列表。
	返回值：找到的节点，如果为 NULL 表示查找失败
*/
struct device_node *of_find_compatible_node(struct device_node *from, const char *type, const char *compat);

/*
	通过 of_device_id 匹配表来查找指定的节点
	from：开始查找的节点，如果为 NULL 表示从根节点开始查找整个设备树。
	matches：of_device_id 匹配表，也就是在此匹配表里面查找节点。
	match：找到的匹配的 of_device_id。
	返回值：找到的节点，如果为 NULL 表示查找失败
*/
struct device_node *of_find_matching_node_and_match(struct device_node *from, const struct of_device_if *matches, const struct of_device_id **match);

/*
	通过路径来查找指定的节点
	path：带有全路径的节点名，可以使用节点的别名，比如“/backlight”就是 backlight 这个节点的全路径。
	返回值：找到的节点，如果为 NULL 表示查找失败
*/
struct device_node *of_find_node_by_path(const char *path);
```

