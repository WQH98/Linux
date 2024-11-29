###### 一、uboot源码目录分析

1、因为uboot会使用到一些经过编译才会生成的文件，因此我们在分析uboot的时候需要先编译一下uboot。

2、arch/arm/cpu/u-boot.lds就是整个uboot的链接脚本。

3、board/freescale/mx6ullevk。重点。

4、configs目录是uboot的默认配置文件目录。此目录下都是以defconfig结尾的。这些配置文件对应不同的板子。

5、我们移植uboot的时候重点要关注。board/freescale。

6、当我们执行make xxx_defconfig以后就会生成.config文件，此文件保存了详细的uboot配置信息。

7、顶层README非常重要，介绍uboot。

8、u-boot。这个就是编译出来的带ELF信息的uboot可执行文件。

|  类型  |          名字           |                           描述                            |      备注      |
| :----: | :---------------------: | :-------------------------------------------------------: | :------------: |
| 文件夹 |           api           |                   与硬件无关的API函数。                   |   uboot 自带   |
| 文件夹 |          arch           |                  与架构体系有关的代码。                   |   uboot 自带   |
| 文件夹 |          board          |              不同板子（开发版）的定制代码。               |   uboot 自带   |
| 文件夹 |           cmd           |                      命令相关代码。                       |   uboot 自带   |
| 文件夹 |         commom          |                         通用代码                          |   uboot 自带   |
| 文件夹 |         configs         |                        配置文件。                         |   uboot 自带   |
| 文件夹 |          disk           |                     磁盘分区相关代码                      |   uboot 自带   |
| 文件夹 |           doc           |                          文档。                           |   uboot 自带   |
| 文件夹 |         drivers         |                        驱动代码。                         |   uboot 自带   |
| 文件夹 |           dts           |                         设备树。                          |   uboot 自带   |
| 文件夹 |        examples         |                        示例代码。                         |   uboot 自带   |
| 文件夹 |           fs            |                        文件系统。                         |   uboot 自带   |
| 文件夹 |         include         |                         头文件。                          |   uboot 自带   |
| 文件夹 |           lib           |                         库文件。                          |   uboot 自带   |
| 文件夹 |        Licenses         |                     许可证相关文件。                      |   uboot 自带   |
| 文件夹 |           net           |                      网络相关代码。                       |   uboot 自带   |
| 文件夹 |          post           |                      上电自检程序。                       |   uboot 自带   |
| 文件夹 |         scripts         |                        脚本文件。                         |   uboot 自带   |
| 文件夹 |          test           |                        测试代码。                         |   uboot 自带   |
| 文件夹 |          tools          |                       工具文件夹。                        |   uboot 自带   |
|  文件  |         .config         |                  配置文件，重要的文件。                   | 编译生成的文件 |
|  文件  |       .gitignore        |                    git 工具相关文件。                     |   uboot 自带   |
|  文件  |        .mailmap         |                        邮件列表。                         |   uboot 自带   |
|  文件  | .u-boot.xxx.cmd(一系列) |          这是一系列的文件，用于保存着一些命令。           | 编译生成的文件 |
|  文件  |        config.mk        |               某个 Makefile 会调用此文件。                |   uboot 自带   |
|  文件  |       imxdownload       |              正点原子编写的 SD 卡烧写软件。               |  正点原子提供  |
|  文件  |         Kbuild          |              用于生成一些和汇编有关的文件。               |   uboot 自带   |
|  文件  |         Kconfig         |                  图形配置界面描述文件。                   |   uboot 自带   |
|  文件  |       MAINTAINERS       |                   维护者联系方式文件。                    |   uboot 自带   |
|  文件  |         MAKEALL         |          一个 shell 脚本文件，帮助编译uboot 的。          |   uboot 自带   |
|  文件  |        Makefile         |                  主 Makefile，重要文件！                  |   uboot 自带   |
|  文件  | mx6ull_alientek_emmc.sh |                 上一章编写的编译脚本文件                  | 上一章编写的。 |
|  文件  | mx6ull_alientek_nand.sh |                 上一章编写的编译脚本文件                  | 上一章编写的。 |
|  文件  |         README          |                     相当于帮助文档。                      |   uboot 自带   |
|  文件  |    snapshot.commint     |                          ？？？                           |   uboot 自带   |
|  文件  |       System.map        |                       系统映射文件                        | 编译出来的文件 |
|  文件  |         u-boot          |                 编译出来的 u-boot 文件。                  | 编译出来的文件 |
|  文件  |   u-boot.xxx(一系列)    | 生成的一些 u-boot 相关文件，包括u-boot.bin、u-boot.imx.等 | 编译出来的文件 |



