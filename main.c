#include "uart.h"
#include "fatfs/ff.h"
#include "gpio.h"
#include "systimer.h"


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
        uart_puts("shit3\n");
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

    // led test
    gpio_func_sel(16, 0b001);
    while(1) {
        gpio_output(16, 1);
        systimer_sleep(1);
        gpio_output(16, 0);
        systimer_sleep(1);
    }
}
