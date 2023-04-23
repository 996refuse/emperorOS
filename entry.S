// AArch32 mode
 
// To keep this in the first portion of the binary.
.section ".text.boot"
 
// Make _start global.
.globl _start
// Entry point for the kernel.
// r15 -> should begin execution at 0x8000.
// r0 -> 0x00000000
// r1 -> 0x00000C42 - machine id
// r2 -> 0x00000100 - start of ATAGS
// preserve these registers as argument for kernel_main
_start:
	// Setup the stack.
	mov sp, #0x8000
 
	// enable mmu, virtual memory
	bl setupkvm
	mov r0, #0
	mov r1, #1
	mcr p15, 0, r1, c3, c0
	// all memory are set as Domain 0, accesses require being checked
	// Domain Access Control, b01 = Client. Accesses are checked against the access permission bits in the TLB entry
	// first-level descriptor domain bits[8:5] should set to 0. (default)
	mcr p15, 0, r0, c2, c0
	// set TTBR0 = 0x0000
	mcr p15, 0, r1, c1, c0, 0
	// Control, MMU enabled

	add sp, #0xC0000000

	// Clear out bss.
	ldr r4, =__bss_start
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b       2f
1:
	stmia r4!, {r5-r8}
2:
	cmp r4, r9
	blo 1b
 
	// Call kernel_main
	ldr r3, =main
	blx r3

	// halt
halt:
	wfe
	b halt
