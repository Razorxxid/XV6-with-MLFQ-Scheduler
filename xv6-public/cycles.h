#ifndef _CYCLES_H_
#define _CYCLES_H_

typedef __UINT32_TYPE__ uint32_t;
#define UINT32_MAX (0xffffffffu)

typedef struct {
    uint32_t low;
    uint32_t high;
} cycles_t;

cycles_t cycles_now(void) {
    cycles_t ret;
    asm volatile("rdtsc":"=a"(ret.low),"=d"(ret.high));
    return ret;
}

cycles_t cycles_sub(cycles_t a, cycles_t b) {
    cycles_t sub;
    sub.low = a.low - b.low;
    sub.high = a.high - b.high;
    if (a.low < b.low) {
        --sub.high;
    }
    return sub;
}

double cycles_to_double(cycles_t clk) {
    return (double) clk.low + (double) clk.high * (double) UINT32_MAX;
}

#endif /* _CYCLES_H_ */

