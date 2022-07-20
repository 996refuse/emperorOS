#include "systimer.h"

uint64_t systimer_counter()
{
    return *(volatile uint64_t*)SYSTIMER_CNT;
}

// enable a match 0-3
void systimer_set(int match_channel)
{
    *(volatile uint32_t*)SYSTIMER_CS = 1 << match_channel;
}

// detect if the match is triggered, zero means isn't triggered.
int systimer_get(int match_channel)
{
    return (*(volatile uint32_t*)SYSTIMER_CS) & (1 << match_channel);
}

// set compare value for a match
void systimer_cmp(int match_channel, int value)
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
