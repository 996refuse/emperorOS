#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_fork    1
#define SYS_putchar 2

#define SYS_exec    4
#define SYS_exit    5
#define SYS_wait    6

// write
// open
// close
// read

// dprintf

// dup
// pipe

// fgets stdin
// chdir
// execvp

// strchr
// strlen
// memset
// malloc

extern void (*syscalls[32])(void);
int getsyscallnum(uint32_t *);

#endif