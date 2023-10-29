#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

#define SVC_MODE 0b10011
#define IRQ_MODE 0b10010

struct context {
	uint32_t cpsr;
	uint32_t r[16];
};

extern uint32_t arm_intr_vector;

void trap_enter(struct context *tf, uint32_t cpsr);
void set_vector_base_addr(uint32_t *);

enum procstate { UNUSED, RUNNABLE };

struct proc {
	enum procstate state;       // Process state
	int pid;                    // Process ID
	struct proc *parent;
	struct context context;
	char name[16];              // Process name (debugging)
	uint32_t pgd;		        // page descriptor
};

#define NPROC 64                // maximum support 64 processes

void proc_init(void);
void proc_schd(void);

void trap_return(struct context*, struct context* p_context_schd);
void schd(struct context* p_context_schd);

typedef struct ARM_INTR_REG {
    volatile uint32_t cpu_pending;
    volatile uint32_t gpu_pending[2];
    volatile uint32_t fiq_ctl;
    volatile uint32_t gpu_enable[2];
    volatile uint32_t cpu_enable;
    volatile uint32_t gpu_disable[2];
    volatile uint32_t cpu_disable;
} ARM_INTR_REG;

#define ARM_INTR_REG_BASE 0x2000b200

extern uint32_t ticks;
extern uint32_t ticks_interval;

// Bank 0
#define ARM_TIMER 0
#define ARM_MAILBOX 1
#define ARM_DOORBELL_0 2
#define ARM_DOORBELL_1 3
#define VPU0_HALTED 4
#define VPU1_HALTED 5
#define ILLEGAL_TYPE0 6
#define ILLEGAL_TYPE1 7

// Bank 1
//#define TIMER0 0
#define TIMER1 1
#define TIMER2 2
//#define TIMER3 3
// first and third timer comparators are used by the GPU https://forums.raspberrypi.com/viewtopic.php?t=234418
#define CODEC0 4
#define CODEC1 5
#define CODEC2 6
#define VC_JPEG 7
#define ISP 8
#define VC_USB 9
#define VC_3D 10
#define TRANSPOSER 11
#define MULTICORESYNC0 12
#define MULTICORESYNC1 13
#define MULTICORESYNC2 14
#define MULTICORESYNC3 15
#define DMA0 16
#define DMA1 17
#define VC_DMA2 18
#define VC_DMA3 19
#define DMA4 20
#define DMA5 21
#define DMA6 22
#define DMA7 23
#define DMA8 24
#define DMA9 25
#define DMA10 26
#define DMA11 27
#define DMAALL 28
#define AUX 29
#define ARM 30
#define VPUDMA 31

// Bank 2
#define HOSTPORT 0
#define VIDEOSCALER 1
#define CCP2TX 2
#define SDC 3
#define DSI0 4
#define AVE 5
#define CAM0 6
#define CAM1 7
#define HDMI0 8
#define HDMI1 9
#define PIXELVALVE1 10
#define I2CSPISLV 11
#define DSI1 12
#define PWA0 13
#define PWA1 14
#define CPR 15
#define SMI 16
#define GPIO0 17
#define GPIO1 18
#define GPIO2 19
#define GPIO3 20
#define VC_I2C 21
#define VC_SPI 22
#define VC_I2SPCM 23
#define VC_SDIO 24
#define VC_UART 25
#define SLIMBUS 26
#define VEC 27
#define CPG 28
#define RNG 29
#define VC_ARASANSDIO 30
#define AVSPMON 31

void enable_irq(int bank, int bit);

#endif
