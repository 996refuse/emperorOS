#include "interrupt.h"
#include "systimer.h"
#include "gpio.h"
#include "uart.h"
#include "memory.h"
#include "string.h"
#include "syscall.h"

struct proc procs[NPROC] = {0};
struct proc *curproc, *initproc = 0;
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
    int callnum;
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
            callnum = curproc->context.r[7];
            if (callnum >= 0 && callnum < NSYSCALLS && syscalls[callnum]) {
                syscalls[callnum]();
            } else {
                while(1); // undefined syscall
                // curproc->context.r[0] = -1;
            }
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
    initproc = p;
    p->parent = 0;
    p->pid = nextpid++;

    // init.S 36
    unsigned char initcode[] = {
        0x14, 0x00, 0x9f, 0xe5, 0x14, 0x10, 0x9f, 0xe5, 0x0b, 0x70, 0xa0, 0xe3, 0x00, 0x00, 0x00, 0xef,
        0x30, 0x3a, 0x69, 0x6e, 0x69, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00
    };
    // unsigned char initcode[] = {
    //     0x14, 0x00, 0x9f, 0xe5,
    //     0x14, 0x10, 0x9f, 0xe5,
    //     0x04, 0x00, 0x00, 0xef,
    //     0x30, 0x3a, 0x62, 0x69,
    //     0x6e, 0x2f, 0x69, 0x6e,
    //     0x69, 0x74, 0x00, 0x00,
    //     0x00, 0x00, 0x00, 0x00,
    //     0x0c, 0x00, 0x00, 0x00,
    //     0x18, 0x00, 0x00, 0x00,
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

    if (p == &procs[NPROC]) {
        curproc->context.r[0] = -1;
        return;
    }

    p->state = RUNNABLE;
    p->pid = nextpid++;
    p->parent = curproc;
    p->context = curproc->context;
    strncpy(p->name, curproc->name, 16);
    p->xstatus = 0;

    p->pgd = copyuvm(curproc->pgd, kalloc());

    // in the child, fork returns 0
    p->context.r[0] = 0;

    // in the parent, fork returns child pid
    curproc->context.r[0] = p->pid;
}

void
proc_exit(void)
{
    if (curproc->pid == 0) while (1);

    curproc->xstatus = curproc->context.r[0];
    struct proc *parent = curproc->parent;
    int pid = curproc->pid;
    int status = curproc->xstatus;

    // reparenting
    struct proc *p;
    for (p = procs; p < &procs[NPROC]; p++)
        if (p->parent == curproc) p->parent = initproc;

    // close open files

    // wake up parent
    curproc->state = ZOMBIE;
    if (parent && parent->state == SLEEPING && parent->context.r[7] == SYS_wait4) {
        int *wstatus = (int*)parent->context.r[1];
        if (wstatus) {
            uint32_t pa = (parent->pgd & 0xfff00000) + ((uint32_t)wstatus & 0x000fffff);
            *(int*)P2V(pa) = status;
        }

        freeuvm(curproc->pgd);
        curproc->state = UNUSED;
        curproc->pid = 0;
        curproc->parent = 0;
        curproc->pgd = 0;
        curproc->xstatus = 0;

        parent->context.r[0] = pid;
        parent->state = RUNNABLE;
    }
    return;
}

void
proc_wait4(void)
{
    struct proc *p;
    int havekids = 0;
    int *wstatus = 0;

    if(curproc->context.r[0] > 0) while(1); // wait specific pid does not support yet

    for (p = procs; p < &procs[NPROC]; p++){
        if(p->parent != curproc)
            continue;

        havekids = 1;
        // reaping
        if (p->state == ZOMBIE) {
            int pid = p->pid;
            int status = p->xstatus;
            freeuvm(p->pgd);
            p->state = UNUSED;
            p->pid = 0;
            p->parent = 0;
            p->pgd = 0;
            p->xstatus = 0;

            wstatus = (int*)curproc->context.r[1];
            if (wstatus)
                *wstatus = status;

            curproc->context.r[0] = pid;
            return;
        }
    }

    if (havekids) {
        curproc->state = SLEEPING;
        return;
    }

    // no kids
    curproc->context.r[0] = -1;
    return;
}
