#include "gpio.h"

// gpio_pin 0-53
// 000 = GPIO Pin is an input
// 001 = GPIO Pin is an output
// 100 = GPIO Pin takes alternate function 0
// 101 = GPIO Pin takes alternate function 1
// 110 = GPIO Pin takes alternate function 2
// 111 = GPIO Pin takes alternate function 3
// 011 = GPIO Pin takes alternate function 4
// 010 = GPIO Pin takes alternate function 5
void gpio_func_sel(int gpio_pin, int gpio_func)
{
    uint32_t reg_index  = gpio_pin/10;
    uint32_t reg_offset = (gpio_pin%10) * 3;

    uint32_t value = *((volatile uint32_t*)GPIO_GPFSEL + reg_index);
    value = (value & ~(0b111 << reg_offset)) | gpio_func << reg_offset;

    *((volatile uint32_t*)GPIO_GPFSEL + reg_index) = value;
}

// set gpio_pin if the value is 1, clear if the value is 0
void gpio_output(int gpio_pin, int value)
{
    uint32_t addr = value ? GPIO_GPSET : GPIO_GPCLR;        
    *(volatile uint64_t*)addr = 1 << gpio_pin;
}

void gpio_enable_jtag()
{
    gpio_func_sel(22, 0b011);
	gpio_func_sel(23, 0b011);
	gpio_func_sel(24, 0b011);
	gpio_func_sel(25, 0b011);
    gpio_func_sel(4,  0b010); // alt5
	gpio_func_sel(27, 0b011);

    *(volatile uint32_t*)GPIO_GPPUD = 0;
    for (volatile uint32_t i = 0; i < 150; ++i);
    *(volatile uint64_t*)GPIO_GPPUDCLK = 
        (1<<22) |
        (1<<23) |
        (1<<24) |
        (1<<25) |
        (1<<4 ) |
        (1<<27) ;
    for (volatile uint32_t i = 0; i < 150; ++i);
    *(volatile uint64_t*)GPIO_GPPUDCLK = 0;
}
