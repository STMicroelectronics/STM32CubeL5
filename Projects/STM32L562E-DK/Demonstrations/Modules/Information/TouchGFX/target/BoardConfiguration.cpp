#include <common/TouchGFXInit.hpp>
#include <touchgfx/hal/BoardConfiguration.hpp>
#include <touchgfx/hal/GPIO.hpp>
#include <platform/driver/lcd/LCD16bpp.hpp>

#if defined(USE_BPP) && USE_BPP!=16
#error "This platfrom support only 16bpp LCD display"
#endif

#include <touchgfx/hal/OSWrappers.hpp>

#include <STM32L5HAL_DPI.hpp>
#include <touchgfx/hal/NoDMA.hpp>

#include <STM32L5Instrumentation.hpp>
#include <STM32L5TouchController.hpp>

/* USER CODE BEGIN user includes */

/* USER CODE END user includes */

extern "C"
{

#include "stm32l5xx.h"
#include "stm32l5xx_hal_dma.h"
#include "stm32l5xx_hal_rcc_ex.h"
#include "stm32l5xx_hal_tim.h"
#include "stm32l562e_discovery_ospi.h"
#include "stm32l562e_discovery.h"
#include "stm32l562e_discovery_lcd.h"
}

#define NB_BUFFERS      1

#ifdef __GNUC__
#ifdef __CODE_RED
#include <cr_section_macros.h>
#define LOCATION_SRAM_PRAGMA
#define LOCATION_SRAM_ATTRIBUTE __NOLOAD(SRAM)
#else
#define LOCATION_SRAM_PRAGMA
#define LOCATION_SRAM_ATTRIBUTE __attribute__ ((section ("FrameBufferSection"))) __attribute__ ((aligned(2)))
#endif
#elif defined __ICCARM__
#define LOCATION_SRAM_PRAGMA _Pragma("location=\"FrameBufferSection\"") _Pragma("data_alignment=2")
#define LOCATION_SRAM_ATTRIBUTE
#elif defined(__ARMCC_VERSION)
#define LOCATION_SRAM_PRAGMA
#define LOCATION_SRAM_ATTRIBUTE __attribute__ ((section ("FrameBufferSection"))) __attribute__ ((aligned(2)))
#endif

LOCATION_SRAM_PRAGMA
uint16_t frameBuf0[240*240*NB_BUFFERS] LOCATION_SRAM_ATTRIBUTE;

extern "C" {
    static bool os_inited = false;
    uint16_t tick = 0;
    volatile bool disableChromArt = false;

    /**
      * @brief  Reads 4 bytes from device.
      * @param  ReadSize: Number of bytes to read (max 4 bytes)
      * @retval Value read on the SPI
      */
    void AppSysTick_Handler(void)
    {
        if (os_inited)
        {
            tick++;
            if(tick == 1) /* Entering Active Area */
            {
                HAL::getInstance()->vSync();
                OSWrappers::signalVSync();
                // Swap frame buffers immediately instead of waiting for the task to be scheduled in.
                // Note: task will also swap when it wakes up, but that operation is guarded and will not have
                // any effect if already swapped.
                HAL::getInstance()->swapFrameBuffers();
                GPIO::set(GPIO::VSYNC_FREQ);
            }

            if(tick > 17) /* simulate 60Hz display frame rate : exiting active area */
            {
                tick = 0;
                GPIO::clear(GPIO::VSYNC_FREQ);
                HAL::getInstance()->frontPorchEntered();
            }
        }
    }

    uint32_t LCD_GetXSize(void)
    {
        return 240;
    }

    uint32_t LCD_GetYSize(void)
    {
        return 240;
    }
}

void GRAPHICS_HW_Init()
{
    BSP_OSPI_NOR_Init_t sOSPI_NOR_Init;

    sOSPI_NOR_Init.InterfaceMode = BSP_OSPI_NOR_OPI_MODE;
    sOSPI_NOR_Init.TransferRate  = BSP_OSPI_NOR_DTR_TRANSFER;
    if (BSP_OSPI_NOR_Init(0, &sOSPI_NOR_Init) == BSP_ERROR_NONE)
    {
        BSP_OSPI_NOR_EnableMemoryMappedMode(0);
    }

    /* Initialize the OctoSPI */
    BSP_LCD_Init(0, LCD_ORIENTATION_PORTRAIT);
    BSP_LCD_DisplayOff(0);

    /* Set active layer */
    BSP_LCD_SetActiveLayer(0, 0);

    GPIO::init();
}

STM32L5Instrumentation mcuInstr;
STM32L5TouchController tc;
NoDMA no_dma;
static LCD16bpp display;
static uint16_t bitDepth = 16;

namespace touchgfx
{
void touchgfx_init()
{
    uint16_t dispWidth = LCD_GetXSize();
    uint16_t dispHeight = LCD_GetYSize();
    HAL& hal = touchgfx_generic_init<STM32L5HAL_DPI>(no_dma, display, tc, dispWidth, dispHeight, 0, 0);

    hal.setFrameBufferStartAddress((uint16_t*)frameBuf0, bitDepth, false, false);

    // This platform can handle simultaneous DMA and TFT accesses to SRAM.
    hal.lockDMAToFrontPorch(true);

    mcuInstr.init();

    //Set MCU instrumentation and Load calculation
    hal.setMCUInstrumentation(&mcuInstr);
    hal.enableMCULoadCalculation(false);
}
}

void GRAPHICS_Init()
{
    touchgfx::touchgfx_init();
}

void GRAPHICS_MainTask(void)
{
    os_inited = true;
    touchgfx::HAL::getInstance()->taskEntry();
}
