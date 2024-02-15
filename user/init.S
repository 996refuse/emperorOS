.globl start
start:
    ldr r0, =fn
    ldr r1, =argv
    swi #4

fn:
.string "0:bin/init"

.align 2
argv:
.word 0
