.section ".text"

.globl _start
_start:


ldr r1, =0x20200004 // GPFSEL1
ldr r0, [r1]
ldr r2, =0xffe3ffff
and r0, r2
ldr r2, =0b001      // output
orr r0, r2, lsl #18
str r0, [r1]


ldr r1, =0x20200028 // GPCLR0
ldr r2, =0x2020001C // GPSET0
mov r0, #1
lsl r0, #16

loop:
// led on
    str r0, [r1]

// sleep
    ldr r3, =0x00400000
1:
    subs r3, #1
    beq 2f
    b 1b
2:

// led off
    str r0, [r2]

// sleep
    ldr r3, =0x00400000
3:
    subs r3, #1
    beq 4f
    b 3b
4:
    b loop
