#include "stm32l5xx_hal.h"
#include "stm32l5xx_hal_tim.h"

#include <touchgfx/hal/HAL.hpp>
#include <STM32L5Instrumentation.hpp>

/* USER CODE BEGIN user includes */

/* USER CODE END user includes */

namespace touchgfx
{
void STM32L5Instrumentation::init()
{
    // See: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0337e/CEGHJDCF.html
    //
    //      [24]  Read/write  TRCENA  This bit must be set to 1 to enable use of the trace and debug blocks:
    //                                    Data Watchpoint and Trace (DWT)
    //                                    Instrumentation Trace Macrocell (ITM)
    //                                    Embedded Trace Macrocell (ETM)
    //                                    Trace Port Interface Unit (TPIU).
    //                                    This enables control of power usage unless tracing is required. The application can enable this, for ITM use, or use by a debugger.

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

//Board specific clockfrequency
unsigned int STM32L5Instrumentation::getElapsedUS(unsigned int start, unsigned int now, unsigned int clockfrequency)
{
    return ((now - start) + (clockfrequency / 2)) / clockfrequency;
}

unsigned int STM32L5Instrumentation::getCPUCycles()
{
    return DWT->CYCCNT;
}

void STM32L5Instrumentation::setMCUActive(bool active)
{
    if (active) //idle task sched out
    {
        cc_consumed += getCPUCycles() - cc_in;
    }
    else //idle task sched in
    {
        cc_in = getCPUCycles();
    }
}

}
