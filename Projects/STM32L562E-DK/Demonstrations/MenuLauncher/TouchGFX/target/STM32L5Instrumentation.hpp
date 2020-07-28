#ifndef STM32L5INSTRUMENTATION_HPP
#define STM32L5INSTRUMENTATION_HPP

#include <platform/core/MCUInstrumentation.hpp>
#include <stdint.h>
#include <main.h>

namespace touchgfx
{
class STM32L5Instrumentation: public MCUInstrumentation
{
public:
    virtual void init();
    virtual unsigned int getElapsedUS(unsigned int start, unsigned int now, unsigned int clockfrequency);
    virtual unsigned int getCPUCycles();
    virtual void setMCUActive(bool active);
private:
    uint32_t m_sysclkRatio;
};

} // namespace touchgfx

#endif // STM32L5INSTRUMENTATION_HPP
