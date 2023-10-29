.align 5
.globl arm_intr_vector
arm_intr_vector:
	b halt       		    /* Reset */
	b halt    			    /* Undefined instruction */
	b trap_swi_enter  	    /* Software Interrupt exception */
	b halt    			    /* Prefetch Abort */
	b halt    			    /* Data Abort */
	b halt       		    /* UNUSED */
	b trap_irq_enter    	/* IRQ */
	b halt          	    /* FIQ */

halt:
	b halt

trap_irq_enter:
	sub lr, lr, #4

trap_swi_enter:             /* only deal with user mode interrupt?? */
	ldr sp, =0xC0006000     /* set kernel stack */
	stmfd sp!, {lr}         /* save pc */
	stmfd sp!, {r0-r14}^    /* save r0-r14 user mode processor context */
	mrs lr, spsr
	stmfd sp!, {lr} 	    /* save usr cpsr */
	mrs r1, cpsr            /* save cur cpsr on r1 */
	mov r0, sp			    /* save sp */
    cps #0x13               /* change processor state to supervisor mode */
	mov sp, r0			    /* restore sp */
	b trap_enter

.globl trap_return
trap_return:
    add r1, #64

    stmfd r1!, {r0-r14, lr}
    mrs lr, cpsr
    stmfd r1!, {lr}

    mov sp, r0
    /* restore cpsr */
	ldmfd sp!, {lr}
	msr spsr, lr

	/* restore r0-r14 */
	ldmfd sp!, {r0-r14}^

	/* restore pc */
	ldmfd sp!, {lr}
	movs pc,lr

.global schd
schd:
    ldmfd r0!, {lr}
    msr cpsr, lr

    ldmfd r0!, {r0-r14}
    bx lr
