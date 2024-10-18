
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sched.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>

//#include <immintrin.h>


#include "xlogtimetick.h"

//-------------------------------------------------------------------------------------------
/** @ingroup group_xlog
 *
 *  @param void
 *
 *  @return  Time Stamp
 *
 *  @description
 *  This Function returns the Time Stamp by reading the Time Stamp Counter register within the

 *  hardware device.
 *
**/
//-------------------------------------------------------------------------------------------
#define xlog_SYS_CLOCK_TICK
#if defined(xlog_SYS_CLOCK_TICK)
uint64_t xlogTick(void)
{
    struct timespec ts;
#ifdef __x86_64__
    uint64_t time;
#endif
    clock_gettime(CLOCK_REALTIME, &ts);
#ifdef __x86_64__
#define NSEC_PER_SEC  1000000000L
    time = ts.tv_sec * NSEC_PER_SEC + ts.tv_nsec;
    return time;
#else
    return (uint64_t)lo;
#endif
}
#else
uint64_t xlogTick(void)
{
    uint32_t hi, lo;
#ifdef __x86_64__
    uint64_t time;
#endif

    __asm volatile ("rdtsc" : "=a"(lo), "=d"(hi));

#ifdef __x86_64__
    time = ( (uint64_t)lo)|( ((uint64_t)hi)<<32 );
    time = time - gxlogCtx.nReferenceTime;

    return time;
#else
    return (uint64_t)lo;
#endif
}
#endif
