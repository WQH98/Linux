### 1、BSP工程管理原理

BSP工程管理的目的就是为了模块化整理代码 将同一个属性的文件存放在同一个目录里面
1、新建所需的文件夹 将同一属性的文件放在相应的文件夹中
2、修改clk、led、delay驱动 创建对应的驱动文件 然后放置到对应的目录中
3、根据编写的新驱动文件 修改main.c文件内容

设置vscode头文件路径：先创建.vscode目录 然后打开c/c++配置器 会在.vscode目录下生成一个叫做c_cpp_properties.json的文件

### 2、Makefile编写

Makefile需要指定头文件路径，需要-I。
我们编译源码的时候需要指定头文件路径，比如bsp/clk/bsp_clk.c 变为 -I bsp/clk/bsp_clk.c
通过一堆的变量，将要编译的原材料准备好了。
