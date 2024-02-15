void printa() {
    asm ("swi #2\n\t");
}

void printb() {
    asm ("swi #3\n\t");
}

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
int _start() {
    int res = fork();
    while (1) {
        if (res) {
            printa();
        } else {
            printb();
        }
    }
    return 0;
}
