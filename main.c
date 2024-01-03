#include "uart.h"
#include "fatfs/ff.h"
#include "gpio.h"
#include "systimer.h"
#include "power.h"
#include "memory.h"
#include "interrupt.h"

void
main()
{
    uart_init();
    printf("================================\n");
    printf("uart test\n");
    uart_puts("hello world\n");
    uart_hex(0xdeadbeef);

    printf("================================\n");
    printf("enable jtag\n");
    gpio_enable_jtag();

    printf("================================\n");
    printf("fs test\n");
    FATFS fs;
    FATFS* pfs = &fs;
    FRESULT res;
    res = f_mount(pfs, "0", 0);
    if (res != FR_OK)
    {
        uart_puts("f_mount failed\n");
        return;
    }
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

    printf("================================\n");
    printf("bootloader\n");
    gpio_func_sel(16, 0b001);
    gpio_output(16, 0);
    for(int i=0; i<3; ++i)
    {
        systimer_sleep(1);

        if(uart_dataready()) {
            uint32_t size = 0;
            char c;
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

    printf("================================\n");
    printf("memory init\n");
    kinit();
    unsigned int pc;
    asm (
        "mov %[result], pc\n\t"
        : [result]"=r" (pc)
    );
    uart_puts("pc should be on high address:\n\r");
    uart_hex(pc);

    printf("================================\n");
    printf("interrupt init\n");
    set_vector_base_addr(&arm_intr_vector);
    enable_irq(1, TIMER1);
    uint32_t _c = systimer_counter();
    systimer_set(TIMER1, _c + ticks_interval);

    printf("================================\n");
    printf("process scheduler init\n");
    proc_init();
    proc_schd();
}
