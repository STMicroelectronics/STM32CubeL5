#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#ifndef SIMULATOR
#include "main.h"

const uint32_t SubDemoAddress[MODULES_COUNT] = {
    DEMO_1_BASE_ADDRESS,
    DEMO_2_BASE_ADDRESS,
    DEMO_3_BASE_ADDRESS,
    DEMO_4_BASE_ADDRESS,
    DEMO_5_BASE_ADDRESS
};
#endif

volatile unsigned int SoftwareReset = 0;

Model::Model() : modelListener(0)
{
}

void Model::tick()
{
}

void Model::moduleSelected(int moduleIndex)
{
#ifndef SIMULATOR // quick & durty! should be moved to Model
    if (moduleIndex < MODULES_COUNT)
    {
        /* Store the address of the Sub Demo binary */
        __HAL_RCC_PWR_CLK_ENABLE();
        HAL_PWR_EnableBkUpAccess();
        __HAL_RCC_RTC_ENABLE();
        __HAL_RCC_RTCAPB_CLK_ENABLE();
        WRITE_REG(BKP_REG_SUBDEMO_ADDRESS, (uint32_t) SubDemoAddress[moduleIndex]);

        /* Do Software Reset */
        HAL_NVIC_SystemReset();
    }
#endif
}

