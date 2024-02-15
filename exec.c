#include "exec.h"
#include "fatfs/ff.h"
#include "uart.h"
#include "interrupt.h"
#include "string.h"
#include "memory.h"

void exec() {
    execve();
}

void execve() {
    char * path = (char * )curproc->context.r[0];
    char **argv = (char **)curproc->context.r[1];
    // char **env  = curproc->context.r[2];

    struct elfhdr elf;
    struct proghdr ph;

    FATFS fs;
    FATFS* pfs = &fs;
    FRESULT res;
    res = f_mount(pfs, "", 0);
    if (res) while (1);

    FIL fil;
    res = f_open(&fil, path, FA_READ);
    if (res) while (1);

    UINT count;
    res = f_read(&fil, &elf, sizeof(elf), &count);
    if (res) while (1);
    printf("elf: elfhdr %d bytes loaded\n", count);

    if (elf.magic != ELF_MAGIC) while (1);
    printf("elf: elf.phoff %x\n", elf.phoff);
    printf("elf: elf.phnum %x\n", elf.phnum);

    char* mem = kalloc();

    int i, off;
    for (i = 0,  off = elf.phoff; i < elf.phnum; i++, off += sizeof(ph)) {
        res = f_lseek(&fil, off);
        if (res) while (1);
        res = f_read(&fil, &ph, sizeof(ph), &count);
        if (res) while (1);
        printf("elf: proghdr %d bytes loaded\n", count);

        if (ph.type != ELF_PROG_LOAD) {
            continue;
        }

        if (ph.memsz < ph.filesz) while (1);
        res = f_lseek(&fil, ph.off);
        if (res) while (1);
        res = f_read(&fil, mem+ph.vaddr, ph.filesz, &count);
        if (res) while (1);
        printf("elf: segment %d bytes loaded\n", count);
    }
    uint32_t pgd = createuvm(0, 0, mem);

    // Push argument strings, prepare user stack.
    uint32_t sp = (uint32_t)mem+0x00100000;

    #define MAXARG 128
    uint32_t ustack[MAXARG + 1];

    int argc = 0;
    for (argc = 0; argv[argc]; argc++) {
        if (argc >= MAXARG) while(1);
        sp = (sp - (strlen(argv[argc]) + 1)) & (~3);
        memmove((void *)sp, argv[argc], strlen(argv[argc]) + 1);
        ustack[argc] = sp;
    }
    ustack[argc] = 0;

    sp -= (argc + 1) * 4;
    curproc->context.r[0] = argc;
    curproc->context.r[1] = sp;
    memmove((void *)sp, ustack, (argc + 1) * 4);

    // Save program name for debugging.
    char *last, *s;
    for (last = s = path; *s; s++) {
        if (*s == '/') {
            last = s + 1;
        }
    }
    safestrcpy(curproc->name, last, sizeof(curproc->name));

    uint32_t oldpgd = curproc->pgd;
    curproc->pgd = pgd;
    curproc->context.r[15] = elf.entry;
    curproc->context.r[14] = 0xdeadbeef; // fake return PC, never exit now
    curproc->context.r[13] = sp & 0x000fffff;

    // reset fp if compile with nostartfiles
    // curproc->context.r[11] = curproc->context.r[13];

    freeuvm(oldpgd);

    // exec returns only if an error has occurred
    //curproc->context.r[0] = -1;
}
