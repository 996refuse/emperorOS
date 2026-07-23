.globl start
start:
    ldr r0, =fn
    ldr r1, =argv
    mov r7, #11
    svc #0

fn:
.string "0:init"

.align 2
argv:
.word 0
