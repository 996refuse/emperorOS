#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

// #define SYS_fork    1

// #define SYS_exec    4
// #define SYS_exit    5
// #define SYS_wait    6

#define NSYSCALLS               256
#define SYS_putchar             0
#define SYS_exit                1
#define SYS_fork                2
#define SYS_read                3
#define SYS_write               4
#define SYS_open                5
#define SYS_close               6
#define SYS_execve              11
#define SYS_wait4               114
#define SYS_nanosleep_time32    162

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

extern void (*syscalls[NSYSCALLS])(void);
// int getsyscallnum(uint32_t *);

#endif