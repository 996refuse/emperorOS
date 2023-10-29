# 玉皇大帝操作系统 emperorOS
忽见天上一火链，好像玉皇要抽烟

--------------------------------------------------------------

## platform

first-generation Raspberry Pi, all Models

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
