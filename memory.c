#include "memory.h"
#include "uart.h"
#include "string.h"

struct node {
	struct node *next;
};

struct node* kmem;

void kfree(void *v)
{
	if( (uint32_t)v % PGSIZE ||
		(uint32_t)v <  (uint32_t)P2V(PA_START+PGSIZE) || // page 0 is used by kernel, skip it.
		(uint32_t)v >= (uint32_t)P2V(PA_END) ) {
		uart_puts("@@@ panic @@@ kfree, invalid virtual address\r\n");
		while(1);
	}

    struct node *n = (struct node*)v;
    n->next = kmem;
    kmem = n;
}

void kinit(void)
{
	kmem = 0;
	for(char* p = (char *)(PA_START+PGSIZE); p + PGSIZE <= (char *)PA_END; p += PGSIZE) {
        kfree(P2V(p));
    }
}

char* kalloc(void)
{
    struct node* n = kmem;
    if(n) {
        kmem = kmem->next;
    }
    else {
        uart_puts("@@@ panic @@@ kalloc, out of memory\r\n");
		while(1);
    }
    return (char*)n;
}

void setupkvm(void)
{
    memset(PDE, 0, 0x00004000);

    uint32_t pa, va;
    // map all physical address, 0x0000_0000-0x0800_0000 => 0xC000_0000-0xC800_0000, 128M
	for(pa = PA_START, va = VA_BASE ; pa < PA_END ; pa += PGSIZE, va += PGSIZE)
		((uint32_t*)PDE)[PDX(va)] = pa|PDX_AP(AP_U_NA)|PDX_TYPE(TYPE_SECTION);

    // map kernel.img in place, 0x0000_0000-0x0010_0000 => 0x0000_0000-0x0010_0000, 1M
	((uint32_t*)PDE)[PDX(0)] = 0|PDX_AP(AP_U_NA)|PDX_TYPE(TYPE_SECTION);

    // map IO in place in place, 0x2000_0000-0x2100_0000 => 0x2000_0000-0x2100_0000, 16M
    for(pa = PA_IO_START, va = PA_IO_START ; pa < PA_IO_END ; pa += PGSIZE, va += PGSIZE)
		((uint32_t*)PDE)[PDX(va)] = pa|PDX_AP(AP_U_NA)|PDX_TYPE(TYPE_SECTION);
}

uint32_t createuvm(char *init, uint32_t sz, char* mem)
{
	if(sz > PGSIZE) {
        uart_puts("@@@ panic @@@ inituvm, out of a page\r\n");
        while(1);
    }

	//memset(mem, 0, PGSIZE);
	memmove(mem, init, sz);

    // return the page descriptor
    return V2P(mem)|PDX_AP(AP_U_RW)|PDX_TYPE(TYPE_SECTION);
}

void loaduvm(uint32_t pgd)
{
    ((uint32_t*)P2V(PDE))[0] = pgd;

    // invalid tlb
    asm ("MCR p15, 0, %[r], c8, c7, 0": :[r]"r" (0):);

    // invalid entire instruction cache
    // asm ("MCR p15, 0, %[r], c7, c5, 0": :[r]"r" (0):);
    // invalid entire data cache
    // asm ("MCR p15, 0, %[r], c7, c6, 0": :[r]"r" (0):);
}

uint32_t copyuvm(uint32_t pgd, char* mem)
{
    char* va = P2V(pgd & 0xfff00000);
    memmove(mem, va, PGSIZE);
    return V2P(mem)|PDX_AP(AP_U_RW)|PDX_TYPE(TYPE_SECTION);
}

void freeuvm(uint32_t pgd)
{
    kfree(P2V(pgd & 0xfff00000));
}
