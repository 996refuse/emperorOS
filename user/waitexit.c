#include "printf.h"
#include "syslib.h"

struct timespec32 ts = {1, 0};
int res;

int _start() {
    res = fork();
    if(res) while(1); // idle

    res = fork();
    if(res) {
        int ws;
        int pid = wait(0, &ws);
        printf("parent end res %d, pid %d\r\n", ws, pid);
        exit(0);
    }
    else {
        for(int i=0; i<5; ++i) {
            printf("%d\r\n", i);
            nanosleep(&ts, 0);
        }
        printf("child end\r\n");
        exit(37);
    }
    return 0;
}
