#ifndef STM32L5TOUCHCONTROLLER_HPP
#define STM32L5TOUCHCONTROLLER_HPP

#include <platform/driver/touch/TouchController.hpp>
#include <main.h>

namespace touchgfx
{
class STM32L5TouchController: public TouchController
{
    virtual void init();
    virtual bool sampleTouch(int32_t& x, int32_t& y);
};

}

#endif // STM32L5TOUCHCONTROLLER_HPP
