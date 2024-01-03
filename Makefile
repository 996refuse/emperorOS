TOOLPREFIX = arm-none-eabi

CC = $(TOOLPREFIX)-gcc
AS = $(TOOLPREFIX)-gcc
LD = $(TOOLPREFIX)-ld
OBJCOPY = $(TOOLPREFIX)-objcopy

CFLAGS  = -mcpu=arm1176jzf-s -fpic -ffreestanding -g -O0 -std=gnu99
ASFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding -g

SRCS=$(wildcard *.c fatfs/*.c)
OBJS=$(SRCS:.c=.o)
ASMSRCS=$(wildcard *.S)
ASMOBJS=$(ASMSRCS:.S=.o)

libgcc := $(shell $(CC) -print-file-name=libgcc.a)

kernel: $(OBJS) $(ASMOBJS) kernel.ld
	$(LD) -T kernel.ld --oformat elf32-littlearm -o kernel.elf $(OBJS) $(ASMOBJS) -L$(dir $(libgcc)) -lgcc
	$(OBJCOPY) kernel.elf -O binary kernel.img

clean: 
	rm -f *.o *.elf *.img */*.o */*.elf */*.img */*.bin */*.hex

USERSRCS=$(wildcard user/*.S)
USERTARGETS=$(USERSRCS:.S=.hex)

user/%.hex: user/%.bin
	xxd $^ > $@

user/%.bin: user/%.elf
	$(OBJCOPY) -S -O binary $^ $@

user/%.elf: user/%.o
	$(LD) -N -e start -Ttext 0 $^ -o $@

user/%.o: user/%.S
	$(CC) $(CFLAGS) -nostdinc -I. -c $^ -o $@

user: $(USERTARGETS)
