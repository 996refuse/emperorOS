#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

// memory management
#define PGSIZE       0x00100000
#define PA_START     0x00000000
#define PA_END       0x08000000 // only use 128M memory

#define VA_BASE      0xC0000000
#define PA_IO_START  0x20000000
#define PA_IO_END    0x21000000

// virtual memory
#define PDE          0x00000000 // page dir entry
#define PDX(va)      (((uint32_t)(va) >> PDXSHIFT) & 0xFFF) // page dir index
#define PDXSHIFT     20  // offset of PDX in a linear address

#define PDX_AP(a)    (((a) & 0b11) << 10)
#define PDX_TYPE(t)  (((t) & 0b11) << 0)

// access permissions
#define AP_A_NA      0b00 // No access
#define AP_U_NA      0b01 // Privileged RW, User No access
#define AP_U_RO      0b10 // Privileged RW, User RO
#define AP_U_RW      0b11 // Privileged RW, User RW

// level 1 page table type
#define TYPE_FAULT   0b00 // Translation fault
#define TYPE_COARSE  0b01 // Coarse page table
#define TYPE_SECTION 0b10 // Section (1MB)

#define V2P(a) (((uint32_t) (a)) - VA_BASE)
#define P2V(a) ((void *)(((char *) (a)) + VA_BASE))

void     kfree(void *);
void     kinit(void);
char*    kalloc(void);
void     setupkvm(void);
uint32_t createuvm(char *init, uint32_t sz);
void     loaduvm(uint32_t pgd);
uint32_t copyuvm(uint32_t pgd);

#endif
