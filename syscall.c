#include "syscall.h"
#include "uart.h"
#include "interrupt.h"
#include "exec.h"

void putchar(void) {
    char c = curproc->context.r[0];
    uart_send(c);
}

void (*syscalls[32])(void) = {
    [SYS_putchar]   putchar,
    [SYS_fork]      proc_fork,
    [SYS_exec]      exec,
    [SYS_exit]      proc_exit,
    [SYS_wait]      proc_wait,
};

int getsyscallnum(uint32_t *pc) {
    return *(pc-1) & 0x00ffffff;
}
