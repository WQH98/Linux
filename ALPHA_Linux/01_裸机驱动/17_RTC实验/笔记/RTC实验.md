###### 一、RTC原理详解

1、6U内部自带了一个RTC外设，确切的说是SRTC。6U和6ULL的RTC内容在SNVS章节。6U的RTC分为LP和HP。LP叫做SRTC，HP叫做RTC。但是HP的RTC掉电以后数据就丢失了，即使使用了纽扣电池也没用，所以必须要使用LP，也就是SRTC。SNVS章节有些是跟加密相关的，需要与NXP签订NDA协议才可以拿到。如果做产品，建议使用外置RTC芯片。
2、RTC类似定时器，外接32.768KHz的晶振，然后就开始计时。RTC使用两个寄存器来保存计数值。
3、RTC使用很简单，打开RTC，然后RTC就开始工作，我们要做的是不断的读取RTC计数寄存器，获取时间值。或者像RTC计数器写入时间值，也就是调整时间。
4、SNVS_HPCOMR的bit31置1，表示所有的软件都可以访问SNVS所有寄存器，bit8也是与安全有关的，可以置1也可以不置1。
5、SNVS_LPCR寄存器，bit0置1，开启SRTC功能。
6、SNVS_LPSRTCMR是高15位RTC计数寄存器，SNVS_LPSRTCLR是低32位RTC计数器寄存器。SNVS_LPSRTCMR与SNVS_LPSRTCLR共同组成了SRTC计数器，每1秒数据加1。6U的RTC模式是从1970年1月1日0时0分0秒开始的。