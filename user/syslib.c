#include "syslib.h"

int fork() {
    int res = 0;
    asm volatile (
        "swi #1\n\t"
        "mov %[reg], r0"
        : [reg] "=r" (res)
        :
        : "r0"
    );
    return res;
}

void exit() {
    asm ("swi #5\n\t");
}

int wait() {
    int res = 0;
    asm volatile (
        "swi #6\n\t"
        "mov %[reg], r0"
        : [reg] "=r" (res)
        :
        : "r0"
        // :
        // :
        // : "r0"
    );
    return res;
}

void _putchar(char character) {asm ("swi #2\n\t");}
