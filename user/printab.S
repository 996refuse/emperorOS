.globl start
start:
    swi #1
    cmp r0, #0
    beq 2f
1:
    swi #2
    b 1b
2:
    swi #3
    b 2b
