###### 一、uboot启动log简析

1、以后带有调试性质的开发，uboot都是烧写到SD卡中的，因为方便烧写。

###### 二、uboot命令使用

1、help命令
使用“? 命令名”来查看某一个命令的帮助信息。

2、信息查询
①、bdinfo查看板卡信息。
②、printenv查看当前板子的环境信息。

3、setenv命令
设置环境变量，也可以自定义环境变量，也可以删除环境变量（将环境变量设置为空则为删除）。

4、saveenv命令
保存环境变量。

5、内存操作命令
①、md命令用于显示内存值。
②、nm命令用于修改指定地址的内存值。
③、mm命令也是修改指定地址内存值的。
④、mw命令用于使用一个指定的数据填充一段内存。
⑤、cp命令是数据拷贝命令。
⑥、cmp命令是比较命令，用于比较两端内存的数据是否相等。

6、网络操作命令
网线插入到ENET2上，保证开发板和电脑连接在一个网段内。
①、ping命令可以查看开发板的网络是否可以使用，是否可以和服务器进行通信。注意！只能在uboot中ping其他的机器，其他机器不能ping uboot，因为uboot没有对ping命令做处理，如果用其他的机器ping uboot的话会失败。
②、dhcp 用于从路由器获取 IP 地址，前提得开发板连接到路由器上的，如果开发板是和电脑直连的，那么 dhcp 命令就会失效。
③、nfs(Network File System)网络文件系统，通过 nfs 可以在计算机之间通过网络来分享资源。
④、tftp 命令的作用和 nfs 命令一样，都是用于通过网络下载东西到 DRAM 中，只是 tftp 命令使用的 TFTP 协议，Ubuntu 主机作为 TFTP 服务器。因此需要在 Ubuntu 上搭建 TFTP 服务器。

7、EMMC和SD卡操作命令
①、mmc info 命令用于输出当前选中的 mmc info 设备的信息。
②、mmc rescan 命令用于扫描当前开发板上所有的 MMC 设备，包括 EMMC 和 SD 卡。
③、mmc list 命令用于来查看当前开发板一共有几个 MMC 设备。
④、mmc dev 命令用于切换当前 MMC 设备。
⑤、有时候 SD 卡或者 EMMC 会有多个分区，可以使用命令“mmc part”来查看其分区。
⑥、mmc read 命令用于读取 mmc 设备的数据。
⑦、要将数据写到 MMC 设备里面，可以使用命令“mmc write”。
⑧、如果要擦除 MMC 设备的指定块就是用命令“mmc erase”。

8、FAT格式文件系统操作命令
一般对于I.MX6U来说，SD/EMMC分为三个分区：第一个存放uboot，第二个存放Linux zimage，.deb，此分区的格式是FAT格式，第三个存放系统的根文件系统，此分区的格式是EXT4。
①、fatinfo 命令用于查询指定 MMC 设备分区的文件系统信息。
②、fatls 命令用于查询 FAT 格式设备的目录和文件信息。
③、fstype 用于查看 MMC 设备某个分区的文件系统格式。
④、fatload 命令用于将指定的文件读取到 DRAM 中。
⑤、fatwirte 命令用于将 DRAM 中的数据写入到 MMC 设备中。

9、BOOT操作命令
①、bootz命令：要启动Linux必须将zImage、dtb设备树放到DRAM。
②、bootm命令：bootm 和 bootz 功能类似，但是 bootm 用于启动 uImage 镜像文件。
③、boot命令：boot 命令也是用来启动 Linux 系统的，只是 boot 会读取环境变量 bootcmd 来启动 Linux 系统，bootcmd 是一个很重要的环境变量！