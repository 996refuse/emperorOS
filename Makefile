TOOLPREFIX = arm-none-eabi

CC = $(TOOLPREFIX)-gcc
AS = $(TOOLPREFIX)-gcc
LD = $(TOOLPREFIX)-ld
OBJCOPY = $(TOOLPREFIX)-objcopy

CFLAGS  = -mcpu=arm1176jzf-s -fpic -ffreestanding
ASFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding

kernel: entry.o kernel.ld
	$(LD) -T kernel.ld --oformat elf32-littlearm -o kernel.elf entry.o
	$(OBJCOPY) kernel.elf -O binary kernel.img

clean: 
	rm -f *.o *.elf *.img 
