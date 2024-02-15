#include "interrupt.h"
#include "systimer.h"
#include "gpio.h"
#include "uart.h"
#include "memory.h"
#include "string.h"
#include "syscall.h"

struct proc procs[NPROC] = {0};
struct proc *curproc = 0;
struct context context_schd = {0};

uint32_t ticks = 0;
uint32_t ticks_interval = 0x00100000;
ARM_INTR_REG* arm_intr_reg = (ARM_INTR_REG *)ARM_INTR_REG_BASE;

void
set_vector_base_addr(uint32_t* addr) {
    asm (
        "mcr p15, 0, %[v], c12, c0, 0\n\t"
        :
        : [v]"r" (addr)
    );
}

void
enable_irq(int bank, int bit) {
    switch (bank)
    {
    case 0:
        arm_intr_reg->cpu_enable    |= 1 << bit;
        break;
    case 1:
        arm_intr_reg->gpu_enable[0] |= 1 << bit;
        break;
    case 2:
        arm_intr_reg->gpu_enable[1] |= 1 << bit;
        break;
    default:
        break;
    }
}

void
trap_enter(struct context *tf, uint32_t cpsr)
{
    curproc->context = *tf;
    int callnum, callres;
	switch(cpsr & 0b11111){
        case IRQ_MODE:
            if ((arm_intr_reg->cpu_pending & 0x100) && (arm_intr_reg->gpu_pending[0] & (1 << TIMER1)))
            {
                systimer_clear(TIMER1);
                uint32_t _c = systimer_counter();
                systimer_set(TIMER1, _c + ticks_interval);
                if (ticks++ % 2 == 0)
                    gpio_output(16, 0);
                else
                    gpio_output(16, 1);
            }
            break;
        case SVC_MODE:
            callnum = getsyscallnum((uint32_t *)curproc->context.r[15]);
            syscalls[callnum]();
            break;
        default:
            while (1);
	}
    schd(&context_schd);
}

int nextpid = 0;

void
proc_init(void) {
    struct proc *p;
    for (p = procs; p < &procs[NPROC]; p++)
        if (p->state == UNUSED)
            break;
    strncpy(p->name, "initcode", 8);
    p->parent = 0;
    p->pid = nextpid++;

    // init.S 36
    unsigned char initcode[] = {
        0x14, 0x00, 0x9f, 0xe5, 
        0x14, 0x10, 0x9f, 0xe5, 
        0x04, 0x00, 0x00, 0xef, 
        0x30, 0x3a, 0x62, 0x69, 
        0x6e, 0x2f, 0x69, 0x6e, 
        0x69, 0x74, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x0c, 0x00, 0x00, 0x00, 
        0x18, 0x00, 0x00, 0x00,
    };

    // printab.S 28
    // unsigned char printab[] = {
    //     0x01, 0x00, 0x00, 0xef,
    //     0x00, 0x00, 0x50, 0xe3,
    //     0x01, 0x00, 0x00, 0x0a,
    //     0x02, 0x00, 0x00, 0xef,
    //     0xfd, 0xff, 0xff, 0xea,
    //     0x03, 0x00, 0x00, 0xef,
    //     0xfd, 0xff, 0xff, 0xea,
    // };

    // inifiniteloop.S 4
    //unsigned char inifiniteloop[] = {0xfe, 0xff, 0xff, 0xea};
    p->pgd = createuvm(initcode, 36, kalloc());
    p->state = RUNNABLE;
    p->context.cpsr  = 0x00000050;
    p->context.r[13] = 0x00100000;
    p->context.r[14] = 0x00000000;
    p->context.r[15] = 0x00000000;
}

void
proc_schd(void) {
	struct proc *p;
    while (1)
    {
        for (p = procs; p < &procs[NPROC]; p++)
        {
            if(p->state != RUNNABLE)
				continue;
            curproc = p;
            loaduvm(p->pgd);
            trap_return(&(p->context), &context_schd);
        }
    }
}

void
proc_fork(void)
{
    struct proc *p;
    // Allocate process.
    for(p = procs; p < &procs[NPROC]; p++) {
        if(p->state == UNUSED) {
            break;
        }
    }

    p->state = RUNNABLE;
    p->pid = nextpid++;
    p->parent = curproc;
    p->context = curproc->context;
    strncpy(p->name, curproc->name, 16);

    p->pgd = copyuvm(curproc->pgd, kalloc());

    // in the child, fork returns 0
    p->context.r[0] = 0;

    // in the parent, fork returns child pid
    curproc->context.r[0] = p->pid;
}
