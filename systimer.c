#include "systimer.h"

uint64_t systimer_counter()
{
    return *(volatile uint64_t*)SYSTIMER_CNT;
}

// Write a one to clear the corresponding match interrupt
void systimer_clear(int match_channel)
{
    *(volatile uint32_t*)SYSTIMER_CS = 1 << match_channel;
}

// detect if the match is triggered, zero means isn't triggered.
int systimer_get()
{
    uint32_t match = (*(volatile uint32_t*)SYSTIMER_CS) & 0b1111;
    int res = -1;
    while(match >>= 1) res += 1;
    return res;
}

// set compare value for a match
void systimer_set(int match_channel, int value)
{
    *((volatile uint32_t*)SYSTIMER_CMP + match_channel) = value;
}

void systimer_sleep_one_second()
{
    uint64_t current = systimer_counter();
    while(1) if(systimer_counter() - current > 1000000) break;
}

void systimer_sleep(int second)
{
    for(int i=0 ; i < second; ++i) systimer_sleep_one_second();
}
