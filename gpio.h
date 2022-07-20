#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

#define MMIO_BASE   0x20000000U
#define GPIO_BASE   (MMIO_BASE + 0x200000)

#define GPIO_GPFSEL     (GPIO_BASE+0x00)
#define GPIO_GPSET      (GPIO_BASE+0x1C)
#define GPIO_GPCLR      (GPIO_BASE+0x28)
#define GPIO_GPLEV      (GPIO_BASE+0x34)
#define GPIO_GPEDS      (GPIO_BASE+0x40)
#define GPIO_GPREN      (GPIO_BASE+0x4C)
#define GPIO_GPFEN      (GPIO_BASE+0x58)
#define GPIO_GPHEN      (GPIO_BASE+0x64)
#define GPIO_GPLEN      (GPIO_BASE+0x70)
#define GPIO_GPAREN     (GPIO_BASE+0x7C)
#define GPIO_GPAFEN     (GPIO_BASE+0x88)
#define GPIO_GPPUD      (GPIO_BASE+0x94)
#define GPIO_GPPUDCLK   (GPIO_BASE+0x98)

void gpio_func_sel(int gpio_pin, int gpio_func);
void gpio_output(int gpio_pin, int value);
void gpio_enable_jtag();

#endif
