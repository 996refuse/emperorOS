/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#define MMIO_BASE 0x20000000U

#define GPIO_BASE (MMIO_BASE + 0x200000)
#define GPFSEL1         ((volatile unsigned int*)(GPIO_BASE+0x04))
#define GPPUD           ((volatile unsigned int*)(GPIO_BASE+0x94))
#define GPPUDCLK0       ((volatile unsigned int*)(GPIO_BASE+0x98))

/* Auxilary mini UART registers */
#define AUX_MINI_UART_BASE (MMIO_BASE + 0x215000)
#define AUX_ENABLE      ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x04))
#define AUX_MU_IO       ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x40))
#define AUX_MU_IER      ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x44))
#define AUX_MU_IIR      ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x48))
#define AUX_MU_LCR      ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x4C))
#define AUX_MU_MCR      ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x50))
#define AUX_MU_LSR      ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x54))
#define AUX_MU_MSR      ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x58))
#define AUX_MU_SCRATCH  ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x5C))
#define AUX_MU_CNTL     ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x60))
#define AUX_MU_STAT     ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x64))
#define AUX_MU_BAUD     ((volatile unsigned int*)(AUX_MINI_UART_BASE+0x68))

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    *AUX_ENABLE |=1;       // enable UART1, AUX mini uart
    *AUX_MU_CNTL = 0;
    *AUX_MU_LCR = 3;       // 8 bits
    *AUX_MU_MCR = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_IIR = 0xc6;    // disable interrupts
    *AUX_MU_BAUD = 270;    // 115200 baud
    /* map UART1 to GPIO pins */
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15)); // gpio14, gpio15
    r|=(2<<12)|(2<<15);    // alt5
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;        // flush GPIO setup
    *AUX_MU_CNTL = 3;      // enable Tx, Rx
}

int uart_transmitteridle () {
    return *AUX_MU_LSR&0x20;
}

/**
 * Send a character
 */
void uart_send(unsigned int c) {
    /* wait until we can send */
    do{asm volatile("nop");}while(!uart_transmitteridle());
    /* write the character to the buffer */
    *AUX_MU_IO=c;
}

int uart_dataready() {
    return *AUX_MU_LSR&0x01;
}

/**
 * Receive a character
 */
char uart_getc() {
    char r;
    /* wait until something is in the buffer */
    do{asm volatile("nop");}while(!uart_dataready());
    /* read it and return */
    r=(char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    //return r=='\r'?'\n':r;
    return r;
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while(*s) {
        /* convert newline to carrige return + newline */
        if(*s=='\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

/**
 * Display a binary value in hexadecimal
 */
void uart_hex(unsigned int d) {
    unsigned int n;
    int c;
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        uart_send(n);
    }
    uart_send('\r');
    uart_send('\n');
}
