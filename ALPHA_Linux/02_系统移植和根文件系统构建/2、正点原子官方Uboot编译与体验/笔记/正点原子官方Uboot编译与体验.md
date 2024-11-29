###### 一、何为Uboot

1、uboot是一个裸机程序，比较复杂。

2、uboot就是一个bootloader，作用就是用于启动Linux或其他系统。uboot最主要的工作就是初始化DDR。因为Linux是运行在DDR里面的。 一般Linux镜像（zimage或uimage）和设备树（.dtb）存放在SD卡、EMMC、NAND、SPI FLASH等等外置存储区域。

3、这里就牵扯到一个问题，需要将Linux镜像从外置flash拷贝到DDR中，再去启动。

4、uboot的主要目的就是为系统的启动做准备。

5、uboot不仅仅能启动Linux，也可以启动其他系统，比如vxworks。

6、Linux不仅仅能通过uboot启动。

7、uboot是一个通用的bootloader，支持多种架构。

8、uboot获取可以从uboot官网获取，缺点就是支持少，比如某一款具体芯片驱动等不完善，或者SOC厂商会从uboot官网下载某一个版本的uboot，然后这个版本的uboot上加入相应的SOC以及驱动。这就是SOC厂商定制版的uboot，比如NXP官方的I.MX6ULL EVK开发板。也可以从做开发版的厂商处获取，开发板会参考SOC厂商的板子，开发板必然会和官方的板子不一样，因此开发板厂商又会去修改SOC厂商做好的uboot，以适应自己的板子。

###### 二、正点原子官方uboot编译

1、编译uboot的时候需要先配置。

2、编译完成以后就会生成一个u-boot.bin。必须向u-boot.bin添加头部文件。uboot编译后会通过/tools/mkimage软件添加头部信息，生成u-boot.imx。

3、如果配置过uboot，那么一定要注意shell脚本会清除整个工程，那么配置的文件也会被删除，配置项也会被删除掉。

4、为了方便开发，建议直接在uboot顶层Makefile里面设置好ARCH和CROSS_COMPILE这两个变量的值。