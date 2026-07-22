#include "printf.h"
#include "syslib.h"

int _start() {
    int res = fork();
    if (res) {
        int ws;
        int pid = wait(0, &ws);
        printf("child end res %d, pid %d\r\n", res, pid);
    }
    else {
        printf("child start\r\n");
        exit(0);
    }
    return 0;
}
