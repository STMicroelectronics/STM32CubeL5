#include <STM32L5TouchController.hpp>

/* USER CODE BEGIN BSP user includes */
#include "stm32l562e_discovery_ts.h"
/* USER CODE END BSP user includes */

extern "C"
{
    uint32_t LCD_GetXSize();
    uint32_t LCD_GetYSize();
    uint32_t __ST7789H2_GetOrientation();
}

using namespace touchgfx;

void STM32L5TouchController::init()
{
    /* USER CODE BEGIN L5TouchController_init */

    /* Add code for touch controller Initialization*/
    TS_Init_t TSInit = {LCD_GetXSize(), LCD_GetYSize(), __ST7789H2_GetOrientation(), 2};
    BSP_TS_Init(0, &TSInit);
    /* USER CODE END  L5TouchController_init  */
}

static int32_t lastTime, lastX, lastY;

bool STM32L5TouchController::sampleTouch(int32_t& x, int32_t& y)
{
    /* USER CODE BEGIN  L5TouchController_sampleTouch  */
    unsigned int time;
    TS_State_t  TS_State = { 0 };
    BSP_TS_GetState(0, &TS_State);
    if (TS_State.TouchDetected)
    {
        x = lastX = TS_State.TouchX;
        y = lastY = TS_State.TouchY;
        lastTime = HAL_GetTick();
        return true;
    }

    time = HAL_GetTick();
    if (((lastTime + 120) > time)
        && (lastX > 0) && (lastY > 0)) {
        x = lastX;
        y = lastY;
        return true;
    }

    return false;

    /* USER CODE END L5TouchController_sampleTouch */
}
