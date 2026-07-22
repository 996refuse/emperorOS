#include "syscall.h"
#include "uart.h"
#include "interrupt.h"
#include "exec.h"

void putchar(void) {
    char c = curproc->context.r[0];
    uart_send(c);
}

void (*syscalls[NSYSCALLS])(void) = {
    [SYS_exit]      proc_exit,      // void _exit(int status);
    [SYS_fork]      proc_fork,      // pid_t fork(void);
    [SYS_read]      0,
    [SYS_write]     0,
    [SYS_open]      0,
    [SYS_close]     0,
    [SYS_execve]    execve,         // int execve(const char *pathname, char *const argv[], char *const envp[]);
    [SYS_wait4]     proc_wait4,     // pid_t wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage);
    [SYS_putchar]   putchar,
};

// int getsyscallnum(uint32_t *pc) {
//     return *(pc-1) & 0x00ffffff;
// }
