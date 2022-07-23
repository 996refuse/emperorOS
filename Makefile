TOOLPREFIX = arm-none-eabi

CC = $(TOOLPREFIX)-gcc
AS = $(TOOLPREFIX)-gcc
LD = $(TOOLPREFIX)-ld
OBJCOPY = $(TOOLPREFIX)-objcopy

CFLAGS  = -mcpu=arm1176jzf-s -fpic -ffreestanding -g -O0
ASFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding -g

SRCS=$(wildcard *.c */*.c)
OBJS=$(SRCS:.c=.o)

libgcc := $(shell $(CC) -print-file-name=libgcc.a)

kernel: $(OBJS) entry.o kernel.ld
	$(LD) -T kernel.ld --oformat elf32-littlearm -o kernel.elf $(OBJS) entry.o -L$(dir $(libgcc)) -lgcc
	$(OBJCOPY) kernel.elf -O binary kernel.img

clean: 
	rm -f *.o *.elf *.img */*.o */*.elf */*.img 
