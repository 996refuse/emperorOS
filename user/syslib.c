#include "syslib.h"
#include "../syscall.h"

int fork() {
    register int r7 asm("r7") = SYS_fork;
    register int r0 asm("r0");

    asm volatile (
        "svc #0"
        : "=r" (r0)
        : "r" (r7)
        : "memory"
    );
    return r0;
}

void exit(int status) {
    register int r7 asm("r7") = SYS_exit;
    register int r0 asm("r0") = status;

    asm volatile (
        "svc #0"
        :
        : "r" (r7), "r" (r0)
        : "memory"
    );
    while(1);
}

int wait(int pid, int *ws){
    register int r7 asm("r7") = SYS_wait4;
    register int r0 asm("r0") = pid;
    register int r1 asm("r1") = (uintptr_t)ws;

    int res = 0;
    asm volatile (
        "svc #0"
        : "+r" (r0)
        : "r" (r7), "r" (r0), "r" (r1)
        : "memory"
    );
    return r0;
}

int nanosleep(const struct timespec32 *req, struct timespec32 *rem) {
    register int r7 asm("r7") = SYS_nanosleep_time32;
    register int r0 asm("r0") = (uintptr_t)req;
    register int r1 asm("r1") = (uintptr_t)rem;

    asm volatile (
        "svc #0"
        : "+r" (r0)
        : "r" (r7), "r" (r1)
        : "memory"
    );
    return r0;
}

void _putchar(char character) {
    register int r7 asm("r7") = SYS_putchar;
    register int r0 asm("r0") = character;

    asm volatile (
        "svc #0"
        :
        : "r" (r7), "r" (r0)
        : "memory"
    );
}
