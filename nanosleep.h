#ifndef NANOSLEEP_H
#define NANOSLEEP_H 


struct timespec32 {
    int tv_sec;
    int tv_nsec;
};

void nanosleep(void);
static void nanowakeup();

#endif
