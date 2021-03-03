#include <touchgfx/hal/OSWrappers.hpp>
#include <STM32L5HAL_DPI.hpp>

// Accelerate LCD refresh using regular DMA hardware
// #define TGFX_USE_REGULAR_DMA_FOR_LCD_REFRESH

extern "C"
{
#include "stm32l5xx.h"
#include "stm32l5xx_hal.h"
#include "stm32l562e_discovery_lcd.h"

#ifdef TGFX_USE_REGULAR_DMA_FOR_LCD_REFRESH
#include "stm32l5xx_hal_dma.h"

DMA_HandleTypeDef hdma;
#endif // TGFX_USE_REGULAR_DMA_FOR_LCD_REFRESH

// Retrieve Display Orientation
extern LCD_Drv_t *Lcd_Drv[];
}

#define LCD_REGISTER_ADDR FMC_BANK1_1
#define LCD_DATA_ADDR     (FMC_BANK1_1 | 0x00000002UL)

using namespace touchgfx;

extern uint16_t         frameBuf0 [];
static uint16_t*        currFbBase = 0;
volatile bool firstFrameReadyToDisplay = false;

static touchgfx::Rect refreshRect = {0, 0, 0, 0};

extern "C"
void __ST7789H2_WriteData(uint16_t *pData, uint16_t Length)
{
     uint16_t i = 0;
     while (i < Length)
     {
         /* Write data value */
         *(uint16_t *)LCD_DATA_ADDR = pData[i++];
     }
}

extern "C"
void __ST7789H2_ReadData(uint16_t *pData, uint16_t Length)
{
    uint16_t i = 0;
    while (i < Length)
    {
        /* Read value */
        pData[i++] = *(uint16_t *)LCD_DATA_ADDR;
    }
}

extern "C"
void __ST7789H2_WriteReg(uint16_t Reg, uint16_t *pData, uint16_t Length)
{
    uint16_t i = 0;
    /* Write register address */
    *(uint16_t *)LCD_REGISTER_ADDR = Reg;
    while (i < Length)
    {
        /* Write data value */
        *(uint16_t *)LCD_DATA_ADDR = pData[i++];
    }
}

extern "C"
void __ST7789H2_ReadReg(uint16_t Reg, uint8_t *pData, uint16_t Length)
{
    uint16_t i = 0;
    uint16_t tmp = 0;

    /* Read register address */
    *(uint16_t *)LCD_REGISTER_ADDR = Reg;
    while (i < (2U * Length))
    {
      tmp = *(uint16_t *)LCD_DATA_ADDR;
      pData[i]    = (uint8_t) tmp;
      pData[i + 1U] = (uint8_t)(tmp >> 8U);
      /* Update data pointer */
      i += 2U;
    }
}

extern "C"
uint32_t __ST7789H2_GetOrientation(void)
{
    uint32_t Orientation = LCD_ORIENTATION_LANDSCAPE_ROT180;
    Lcd_Drv[0]->GetOrientation(Lcd_CompObj[0], &Orientation);

    return Orientation; 
}

extern "C"
void __ST7789H2_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    if (refreshRect != Rect(Xpos, Ypos, Width, Height))
    {
        // Check ST7789H2_DrawBitmap() implementation
        uint8_t  parameter[8];
        uint32_t Xstart, Xstop, Ystart, Ystop;
        static uint32_t Orientation = __ST7789H2_GetOrientation(); // Fixed Orientation

        /* Compute new Y start and stop values */
        if (Orientation == LCD_ORIENTATION_PORTRAIT)
        {
            Ystart = Ypos;
            Ystop  = Ypos + Height - 1;
            Xstart = Xpos;
            Xstop  = Xpos + Width - 1;
        }
        else if (Orientation == LCD_ORIENTATION_LANDSCAPE)
        {
            Ystart = Ypos;
            Ystop  = Ypos + Height - 1;
            Xstart = Xpos + 0x50U;
            Xstop  = Xpos + Width - 1 + 0x50U;
        }
        else if (Orientation == LCD_ORIENTATION_PORTRAIT_ROT180)
        {
            Ystart = Ypos + 0x50U;
            Ystop  = Ypos + Height - 1 + 0x50U;
            Xstart = Xpos;
            Xstop  = Xpos + Width - 1;
        }
        else if (Orientation == LCD_ORIENTATION_LANDSCAPE_ROT180)
        {
            Ystart = Ypos;
            Ystop  = Ypos + Height - 1;
            Xstart = Xpos;
            Xstop  = Xpos + Width - 1;
        }

        /* CASET: Column Address Set */
        parameter[0] = (uint8_t)(Xstart >> 8);           /* XS[15:8] */
        parameter[1] = 0x00;
        parameter[2] = (uint8_t) Xstart;                 /* XS[7:0] */
        parameter[3] = 0x00;
        parameter[4] = (uint8_t)(Xstop >> 8);            /* XE[15:8] */
        parameter[5] = 0x00;
        parameter[6] = (uint8_t) Xstop;                  /* XE[7:0] */
        parameter[7] = 0x00;
        __ST7789H2_WriteReg(ST7789H2_CASET, (uint16_t *)parameter, 4);

        /* RASET: Row Address Set */
        parameter[0] = (uint8_t)(Ystart >> 8);           /* YS[15:8] */
        parameter[1] = 0x00;
        parameter[2] = (uint8_t) Ystart;                 /* YS[7:0] */
        parameter[3] = 0x00;
        parameter[4] = (uint8_t)(Ystop >> 8);            /* YE[15:8] */
        parameter[5] = 0x00;
        parameter[6] = (uint8_t) Ystop;                  /* YE[7:0] */
        parameter[7] = 0x00;
        __ST7789H2_WriteReg(ST7789H2_RASET, (uint16_t *)parameter, 4);
        
        refreshRect = Rect(Xpos, Ypos, Width, Height);
    }
}

#ifdef TGFX_USE_REGULAR_DMA_FOR_LCD_REFRESH
/**
  * Enable DMA controller clock
  * Configure DMA for memory to memory transfers
  *   hdma
  */
extern "C"
void MX_DMA_Init(uint32_t alignement)
{
    static uint32_t DmaIntialized = 0;
    static uint32_t DmaMemDataAlignment= 0;

    if (DmaMemDataAlignment == 0) /* The very first init is to be performed */
    {
        /* DMA controller clock enable once */
        __HAL_RCC_DMAMUX1_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();
        __HAL_DMA_RESET_HANDLE_STATE(&hdma);
    }

    if (DmaMemDataAlignment != alignement)
    {
        if (DmaIntialized)
        {
            HAL_DMA_DeInit(&hdma);
            __HAL_DMA_RESET_HANDLE_STATE(&hdma);
            DmaIntialized = 0;
        }
        DmaMemDataAlignment = alignement;
    }

    if (!DmaIntialized)
    {
        DmaIntialized = 1;

        /* Configure DMA request hdma on DMA1_Channel1 */
        hdma.Instance = DMA1_Channel1;
        hdma.Init.Request = DMA_REQUEST_MEM2MEM;
        hdma.Init.Direction = /*DMA_MEMORY_TO_MEMORY*/DMA_MEMORY_TO_PERIPH;
        hdma.Init.PeriphInc = DMA_PINC_ENABLE;
        hdma.Init.MemInc = DMA_MINC_ENABLE;
        if (DmaMemDataAlignment == 1)
        {
            hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            hdma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        }
        else
        {
            hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        }
        hdma.Init.Mode = DMA_NORMAL;
        hdma.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma) != HAL_OK)
        {
            while(1);
        }
    }
}
#endif // TGFX_USE_REGULAR_DMA_FOR_LCD_REFRESH

uint16_t* STM32L5HAL_DPI::getTFTFrameBuffer() const
{
    return  currFbBase;
}

void STM32L5HAL_DPI::setTFTFrameBuffer(uint16_t* adr)
{
    if (currFbBase != adr)
    {
        currFbBase = adr;
        if (USE_DOUBLE_BUFFERING)
        {
            uint16_t tmp;

            /* Set Cursor */
            __ST7789H2_SetDisplayWindow(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

            /* Prepare to write */
            __ST7789H2_WriteReg(ST7789H2_WRITE_RAM, (uint16_t*)NULL, 0);

            /* Read dummy data */
            __ST7789H2_ReadData(&tmp, 1);

            for (uint32_t height = 0; height < DISPLAY_HEIGHT ; height++)
            {
                __ST7789H2_WriteData((adr + height  * DISPLAY_WIDTH), DISPLAY_WIDTH);
            }
        }
    }
}

void STM32L5HAL_DPI::configureInterrupts()
{
}

void STM32L5HAL_DPI::enableLCDControllerInterrupt()
{
}

void STM32L5HAL_DPI::disableInterrupts()
{
}

void STM32L5HAL_DPI::enableInterrupts()
{
}

bool STM32L5HAL_DPI::beginFrame()
{
    return HAL::beginFrame();
}

void STM32L5HAL_DPI::endFrame()
{
    HAL::endFrame();
    if (frameBufferUpdatedThisFrame)
    {
        if (!firstFrameReadyToDisplay)
        {
            firstFrameReadyToDisplay = true;

            /* Display ON command */
            __ST7789H2_WriteReg(ST7789H2_DISPLAY_ON, (uint16_t*)NULL, 0);

            /* Sleep Out command */
            __ST7789H2_WriteReg(ST7789H2_SLEEP_OUT, (uint16_t*)NULL, 0);
        }
    }
}

// Full refresh
void STM32L5HAL_DPI::flushFrameBuffer()
{
    Rect rect = Rect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    HAL::flushFrameBuffer();
    this->copyFrameBufferBlockToLCD(rect);
}

void  STM32L5HAL_DPI::flushFrameBuffer(const Rect& rect)
{
    // Partial framebuffer might be supported for this platform only when Double
    // Buffering is enabled
    HAL::flushFrameBuffer();
    this->copyFrameBufferBlockToLCD(rect);
}

void STM32L5HAL_DPI::copyFrameBufferBlockToLCD(const Rect rect)
{
    uint16_t tmp;

    // Use default implementation (CPU copy!).
    // This can be accelerated using regular DMA hardware
    uint16_t *ptr = getClientFrameBuffer() + rect.x + rect.y * DISPLAY_WIDTH;

    /* Set Cursor */
    __ST7789H2_SetDisplayWindow(rect.x, rect.y, rect.width, rect.height);

    /* Prepare to write */
    __ST7789H2_WriteReg(ST7789H2_WRITE_RAM, (uint16_t*)NULL, 0);

    /* Read dummy data */
    __ST7789H2_ReadData(&tmp, 1);

    for (uint32_t h = 0; h < rect.height ; h++)
    {
#ifndef TGFX_USE_REGULAR_DMA_FOR_LCD_REFRESH
        __ST7789H2_WriteData((ptr + h * DISPLAY_WIDTH), rect.width);
#else // TGFX_USE_REGULAR_DMA_FOR_LCD_REFRESH
        MX_DMA_Init(2);
        HAL_DMA_Start(&hdma, (uint32_t)((ptr + h * DISPLAY_WIDTH)), (uint32_t)LCD_DATA_ADDR, 2 * rect.width);
        HAL_DMA_PollForTransfer(&hdma, HAL_DMA_FULL_TRANSFER, 100);
#endif // !TGFX_USE_REGULAR_DMA_FOR_LCD_REFRESH
    }
}

bool STM32L5HAL_DPI::blockCopy(void* RESTRICT          dest,
                               const void* RESTRICT    src,
                               uint32_t            numBytes)
{
    bool retval = true;

    // Use default implementation (CPU copy!).
    // This can be accelerated using regular DMA hardware
#ifndef TGFX_USE_REGULAR_DMA_FOR_LCD_REFRESH
    retval    = HAL::blockCopy(dest, src, numBytes);
#else // TGFX_USE_REGULAR_DMA_FOR_LCD_REFRESH
    MX_DMA_Init(1);
    HAL_DMA_Start(&hdma, (uint32_t)src, (uint32_t)dest, numBytes);
    HAL_DMA_PollForTransfer(&hdma, HAL_DMA_FULL_TRANSFER, 100);
#endif // !TGFX_USE_REGULAR_DMA_FOR_LCD_REFRESH

    return retval;
}
