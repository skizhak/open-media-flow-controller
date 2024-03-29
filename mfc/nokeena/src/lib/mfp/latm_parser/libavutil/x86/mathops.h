#ifndef AVCODEC_X86_MATHOPS_H
#define AVCODEC_X86_MATHOPS_H

//#include "config.h"
#include "libavutil/common.h"

#define  ARCH_X86_32 1

#if ARCH_X86_32

#define MULL MULL
static av_always_inline av_const int MULL(int a, int b, unsigned shift)
{
    int rt, dummy;
    __asm__ (
        "imull %3               \n\t"
        "shrdl %4, %%edx, %%eax \n\t"
        :"=a"(rt), "=d"(dummy)
        :"a"(a), "rm"(b), "ci"((uint8_t)shift)
    );
    return rt;
}

#define MULH MULH
static av_always_inline av_const int MULH(int a, int b)
{
    int rt, dummy;
    __asm__ (
        "imull %3"
        :"=d"(rt), "=a"(dummy)
        :"a"(a), "rm"(b)
    );
    return rt;
}

#define MUL64 MUL64
static av_always_inline av_const int64_t MUL64(int a, int b)
{
    int64_t rt;
    __asm__ (
        "imull %2"
        :"=A"(rt)
        :"a"(a), "rm"(b)
    );
    return rt;
}

#endif /* ARCH_X86_32 */

#define HAVE_CMOV 0

#if HAVE_CMOV
/* median of 3 */
#define mid_pred mid_pred
static inline av_const int mid_pred(int a, int b, int c)
{
    int i=b;
    __asm__ volatile(
        "cmp    %2, %1 \n\t"
        "cmovg  %1, %0 \n\t"
        "cmovg  %2, %1 \n\t"
        "cmp    %3, %1 \n\t"
        "cmovl  %3, %1 \n\t"
        "cmp    %1, %0 \n\t"
        "cmovg  %1, %0 \n\t"
        :"+&r"(i), "+&r"(a)
        :"r"(b), "r"(c)
    );
    return i;
}
#endif

#if HAVE_CMOV
#define COPY3_IF_LT(x, y, a, b, c, d)\
__asm__ volatile(\
    "cmpl  %0, %3       \n\t"\
    "cmovl %3, %0       \n\t"\
    "cmovl %4, %1       \n\t"\
    "cmovl %5, %2       \n\t"\
    : "+&r" (x), "+&r" (a), "+r" (c)\
    : "r" (y), "r" (b), "r" (d)\
);
#endif

// avoid +32 for shift optimization (gcc should do that ...)
#define NEG_SSR32 NEG_SSR32
static inline  int32_t NEG_SSR32( int32_t a, int8_t s){
    __asm__ ("sarl %1, %0\n\t"
         : "+r" (a)
         : "ic" ((uint8_t)(-s))
    );
    return a;
}

#define NEG_USR32 NEG_USR32
static inline uint32_t NEG_USR32(uint32_t a, int8_t s){
    __asm__ ("shrl %1, %0\n\t"
         : "+r" (a)
         : "ic" ((uint8_t)(-s))
    );
    return a;
}

#endif /* AVCODEC_X86_MATHOPS_H */
