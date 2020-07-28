
#ifndef _UTILITY_H_
#define _UTILITY_H_

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "stm32l5xx.h"


struct dwtTime {
    uint32_t fcpu;
    int s;
    int ms;
    int us;
};


void dwtIpInit(void);
void dwtReset(void);
uint32_t dwtGetCycles(void);
uint32_t GetSystemCoreClock(void);
int dwtCyclesToTime(uint64_t clks, struct dwtTime *t);
float dwtCyclesToFloatMs(uint64_t clks);


#endif /* _UTILITY_H_ */