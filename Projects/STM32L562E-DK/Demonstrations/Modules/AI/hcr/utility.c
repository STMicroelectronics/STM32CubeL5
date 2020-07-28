#include "utility.h"

void dwtIpInit(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

#ifdef STM32F7
    DWT->LAR = 0xC5ACCE55;
#endif

    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk | DWT_CTRL_CPIEVTENA_Msk;
}

void dwtReset(void)
{
    DWT->CYCCNT = 0; /* Clear DWT cycle counter */
}

uint32_t dwtGetCycles(void)
{
    return DWT->CYCCNT;
}

uint32_t GetSystemCoreClock(void)
{
#if !defined(STM32H7)
    return HAL_RCC_GetHCLKFreq();
#else
    return HAL_RCC_GetSysClockFreq();
#endif
}

int dwtCyclesToTime(uint64_t clks, struct dwtTime *t)
{
    if (!t)
        return -1;
    uint32_t fcpu = GetSystemCoreClock();
    uint64_t s  = clks / fcpu;
    uint64_t ms = (clks * 1000) / fcpu;
    uint64_t us = (clks * 1000 * 1000) / fcpu;
    ms -= (s * 1000);
    us -= (ms * 1000 + s * 1000000);
    t->fcpu = fcpu;
    t->s = s;
    t->ms = ms;
    t->us = us;
    return 0;
}


float dwtCyclesToFloatMs(uint64_t clks)
{
    float res;
    float fcpu = (float)GetSystemCoreClock();
    res = ((float)clks * 1000.0) / fcpu;
    return res;
}