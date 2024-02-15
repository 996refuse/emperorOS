#include "syscall.h"
#include "uart.h"
#include "interrupt.h"
#include "exec.h"

void printa(void) {
    printf("a\n");
}

void printb(void) {
    printf("b\n");
}

void (*syscalls[32])(void) = {
    [SYS_printa]    printa,
    [SYS_printb]    printb,
    [SYS_fork]      proc_fork,
    [SYS_exec]      exec,
};

int getsyscallnum(uint32_t *pc) {
    return *(pc-1) & 0x00ffffff;
}
