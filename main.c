#include "uart.h"
#include "fatfs/ff.h"
#include "gpio.h"
#include "systimer.h"
#include "power.h"
#include "memory.h"

void
main()
{
    // uart test
    uart_init();
    uart_puts("hello world\n");
    uart_hex(0xdeadbeef);

    // enable jtag
    gpio_enable_jtag();

    // fs test
    FATFS fs;
    FATFS* pfs = &fs;
    FRESULT res;

    res = f_mount(pfs, "0", 0);
    if (res != FR_OK)
    {
        uart_puts("f_mount failed\n");
        return;
    }

    uint32_t nclst = 0;
    res = f_getfree("0", &nclst, &pfs);
    if (res != FR_OK)
    {
        uart_puts("f_getfree failed\n");
        return;
    }

    uart_puts("Number of Free clusters on the volume:\n");
    uart_hex(nclst);

    uart_puts("List All FILEs in root directory:\n");
    DIR d;
    DIR* dp = &d;
    res = f_opendir(dp, "0:/");
    if (res != FR_OK)
    {
        uart_puts("f_opendir failed\n");
        return;
    }

    FILINFO fi;
    FILINFO* fno = &fi;
    while (1)
    {
        res = f_readdir(dp, fno);
        if (res != FR_OK)
        {
            uart_puts("f_readdir failed\n");
            return;
        }

        if(fno->fname[0] == 0)
            break;
        
        uart_puts(fno->fname);
        uart_puts("\n\r");
    }

    // bootloader
    gpio_func_sel(16, 0b001);
    gpio_output(16, 0);
    for(int i=0; i<3; ++i)
    {
        char c;
        systimer_sleep(1);

        if(uart_dataready()) {
            uint32_t size = 0;
            for(int i=0; i<4; ++i)
            {
                c = uart_getc();
                uart_send(c);
                size = size << 8;
                size = size + c;
            }

            char* data = (char *)0x80000;
            char* bp = data;
            for(int s=0; s<size; ++s) {
                *bp = uart_getc();
                uart_send(*bp);
                bp += 1;
            }
            uart_send('#');

            FIL fdst;
            FRESULT res = f_open(&fdst, "0:/KERNEL.IMG", FA_CREATE_ALWAYS | FA_WRITE);
            if (res != FR_OK)
            {
                uart_puts("f_open failed\n");
                return;
            }

            uint32_t sizewrite = 0;
            res = f_write(&fdst, (void *)data, size, (unsigned int*)&sizewrite);
            f_close(&fdst);

            gpio_output(16, 1);
            reset();
        }
    }
    gpio_output(16, 1);
    uart_puts("no data from uart!\n\r");

    // mmu test, cp should be high address
    unsigned int pc_value;
    __asm (
        "mov %[result], pc\n\t"
        : [result]"=r" (pc_value)
        :
        :
    );
    uart_puts("pc_value is :\n\r");
    uart_hex(pc_value);

    // kinit();
    // setupkvm();

    // led test
    gpio_func_sel(16, 0b001);
    while(1) {
        gpio_output(16, 0);
        systimer_sleep(1);
        gpio_output(16, 1);
        systimer_sleep(1);
    }
}
