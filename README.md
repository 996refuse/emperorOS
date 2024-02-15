# 玉皇大帝操作系统 emperorOS

great power, great emperor.
像玉皇大帝一样掌握操作系统

![logo](https://raw.githubusercontent.com/996refuse/emperorOS/master/logo.png)

## intro

emperorOS is developed for first-generation Raspberry Pi, all Models based on bcm2835.


> 玉皇大帝操作系统是完全由我国自主研发自主可控的适用于搭载bcm2835的第一代树莓派的操作系统，由于系统功能过于精简，导致代码量很少，即使您只拥有全日制小学学历也可以很容易地像玉皇大帝一样掌握操作系统
> 
> 您可以在下面的链接找到我相关的教程
> 
> * https://blog.74ls74.org/tags/#RPibringup blog.74ls74.org [RPi bring up]
> * https://www.zhihu.com/column/c_1558636235975065600 知乎专栏 [RPi bring up]
> 
> 《30天自制操作系统》太特么慢了！《七周七语言》也得49天！毕竟现在的人都太忙了，看书超过五分钟就看不下去了
> 
> 完成这个hobby os耗费了我大量时间，我希望后来者可以~再接再厉~引以为鉴，1小时就能自制操作系统

## build

```
git clone https://github.com/996refuse/emperorOS.git
cd emperorOS

export PATH=path/to/gcc-arm-none-eabi/:$PATH
make clean
make

cp kernel.img path/to/sdcard
```

## features

1. led-blink
2. gpio
3. uart
4. sd
5. fat32
6. systimer
7. power
8. bootloader
9. memory
10. interrupt
11. syscall

## donate

## license

Anti 996 License Version 1.0
