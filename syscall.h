#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_fork    1
#define SYS_printa  2
#define SYS_printb  3
#define SYS_exec    4

extern void (*syscalls[32])(void);
int getsyscallnum(uint32_t *);

#endif