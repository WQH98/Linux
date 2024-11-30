###### 一、链接脚本u-boot.lds详解

1、从u-boot.lds可知，uboot的地址是__start。
\_\_image_copy_start			->	0x87800000
.vetcors			    		->	0x87800000 存放中断向量表
arch/arm/cpu/armv7/start.o	->	start.c
\_\_image\_copy\_end			 ->	0x8785dc6c
\_\_rel\_dyn\_start				->	0x8785dc6c	rel段
\_\_rel\_dyn\_end				 ->	0x878668a4
\_\_end						->	0x878668a4
\_\_image\_binary\_end 		     ->	0x878668a4
\_\_bss\_start				      ->	0x8785dc6d		bss段
\_\_bss\_end				       ->	0x878a8d74

###### 二、uboot启动流程

1、reset函数
bicne = bic + ne
①、reset函数目的是将处理器设置为SVC模式，并且关闭FIQ和IRQ。
②、设置中断向量。
③、初始化CP15。
④、