#ifndef SYSTIMER_H
#define SYSTIMER_H

#include <stdint.h>

#define MMIO_BASE       0x20000000U
#define SYSTIMER_BASE   (MMIO_BASE + 0x3000)

#define SYSTIMER_CS     (SYSTIMER_BASE+0x0)
#define SYSTIMER_CNT    (SYSTIMER_BASE+0x4)
#define SYSTIMER_CMP    (SYSTIMER_BASE+0xC)

// return the counter value
uint64_t systimer_counter();

// Write a one to clear the corresponding match interrupt
void systimer_clear(int match_channel);

// the matched channel is triggered, negative means isn't triggered.
int systimer_get();

// set compare value for a match
void systimer_set(int match_channel, int value);

void systimer_sleep(int second);

#endif
