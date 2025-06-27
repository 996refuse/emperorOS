#include "printf.h"
#include "syslib.h"

int _start() {
    int res = fork();
    while (1) {
        if (res) {
            printf("a");
        } else {
            printf("b");
        }
    }
    return 0;
}
