#include "syscall.h"
#include "uart.h"
#include "interrupt.h"

int printa(void) {
    printf("a\n");
}

int printb(void) {
    printf("b\n");
}

int (*syscalls[32])(void) = {
    [SYS_printa]    printa,
    [SYS_printb]    printb,
    [SYS_fork]      proc_fork,
};

int getsyscallnum(uint32_t *pc) {
    return *(pc-1) & 0x00ffffff;
}
