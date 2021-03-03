/**
  ******************************************************************************
  * @file    TargetPlatform.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version V4.0.0
  * @date    20-Sep-2019
  * @brief   Initialization of the Target Platform
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#include <stdio.h>
#include "TargetFeatures.h"
#include "HWAdvanceFeatures.h"
#include "main.h"

/*bitmap*/
#include "screen.bmp.h"

/* Imported variables ---------------------------------------------------------*/

/* Exported variables ---------------------------------------------------------*/
TargetFeatures_t TargetBoardFeatures;

/* Local defines -------------------------------------------------------------*/

/* Local function prototypes --------------------------------------------------*/
static void MX_GPIO_Init(void);
static void Init_MEM1_Sensors(void);
static void Enable_MEM1_Sensors(void);

static uint32_t GetPage(uint32_t Address);
static uint32_t GetBank(uint32_t Address);

/**
  * @brief  Initialize all the Target platform's Features
  * @retval None
  */
void InitTargetPlatform(void)
{
  BSP_OSPI_NOR_Init_t sOSPI_NOR_Init;
  LCD_UTILS_Drv_t LcdDrv;
  TS_Init_t       TsInit;
#ifdef ALLMEMS1_ENABLE_PRINTF
  COM_InitTypeDef ComInit;
#endif

  /* Initialize LED */
  BSP_LED_Init( LED10 );

#ifdef ALLMEMS1_ENABLE_PRINTF
  /* Configure COM port */
  ComInit.BaudRate   = 115200;
  ComInit.WordLength = COM_WORDLENGTH_8B;
  ComInit.StopBits   = COM_STOPBITS_1;
  ComInit.Parity     = COM_PARITY_NONE;
  ComInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &ComInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif

  /* Initialize the external memory access */
  sOSPI_NOR_Init.InterfaceMode = BSP_OSPI_NOR_OPI_MODE;
  sOSPI_NOR_Init.TransferRate  = BSP_OSPI_NOR_DTR_TRANSFER;
  if (BSP_OSPI_NOR_Init(0, &sOSPI_NOR_Init) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  BSP_OSPI_NOR_EnableMemoryMappedMode(0);

  /* Initialize the LCD */
  if (BSP_LCD_Init(0, LCD_ORIENTATION_PORTRAIT) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Set UTIL_LCD functions */
  LcdDrv.DrawBitmap  = BSP_LCD_DrawBitmap;
  LcdDrv.FillRGBRect = BSP_LCD_FillRGBRect;
  LcdDrv.DrawHLine   = BSP_LCD_DrawHLine;
  LcdDrv.DrawVLine   = BSP_LCD_DrawVLine;
  LcdDrv.FillRect    = BSP_LCD_FillRect;
  LcdDrv.GetPixel    = BSP_LCD_ReadPixel;
  LcdDrv.SetPixel    = BSP_LCD_WritePixel;
  LcdDrv.GetXSize    = BSP_LCD_GetXSize;
  LcdDrv.GetYSize    = BSP_LCD_GetYSize;
  LcdDrv.SetLayer    = BSP_LCD_SetActiveLayer;
  LcdDrv.GetFormat   = BSP_LCD_GetFormat;
  UTIL_LCD_SetFuncDriver(&LcdDrv);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);

  /* Set the display on */
  if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Draw bitmap */
  UTIL_LCD_DrawBitmap(0, 0, (uint8_t *)ble565_bmp);

  /* Initialize the TouchScreen */
  TsInit.Width       = 240;
  TsInit.Height      = 240;
  TsInit.Orientation = TS_ORIENTATION_PORTRAIT;
  TsInit.Accuracy    = 10;
  if (BSP_TS_Init(0, &TsInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Configure TS interrupt */
  if (BSP_TS_EnableIT(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  ALLMEMS1_PRINTF("\r\nSTMicroelectronics %s:\r\n"
         "\tVersion %c.%c.%c\r\n"
         "\tSTM32L562E-DK board"
         "\r\n\n",
         ALLMEMS1_PACKAGENAME,
         ALLMEMS1_VERSION_MAJOR,ALLMEMS1_VERSION_MINOR,ALLMEMS1_VERSION_PATCH);

  /* Reset all the Target's Features */
  memset(&TargetBoardFeatures, 0, sizeof(TargetFeatures_t));

  /* Discovery and Initialize all the MEMS Target's Features */
  Init_MEM1_Sensors();

  Enable_MEM1_Sensors();

  ALLMEMS1_PRINTF("\r\n");
}

/** @brief Initialize all the MEMS1 sensors
 * @param None
 * @retval None
 */
static void Init_MEM1_Sensors(void)
{
  /* Accelero & Gyro */
  if (BSP_MOTION_SENSOR_Init(ACCELERO_INSTANCE, MOTION_ACCELERO | MOTION_GYRO)==BSP_ERROR_NONE){
    ALLMEMS1_PRINTF("\tOK Accelero Sensor\n\r");
    ALLMEMS1_PRINTF("\tOK Gyroscope Sensor\n\r");
  } else {
    ALLMEMS1_PRINTF("\tError Accelero Sensor\n\r");
    ALLMEMS1_PRINTF("\tError Gyroscope Sensor\n\r");
    while(1);
  }

  /* Set Accelerometer Full Scale to 2G */
  Set2GAccelerometerFullScale();
  /* For accelero HW features */
  InitHWFeatures();

  /* Enable interruption LSM6DS0_INT_PIN  */
  MX_GPIO_Init();
}

/** @brief Enable all the MEMS1 sensors
 *  @param None
 *  @retval None
 */
static void Enable_MEM1_Sensors(void)
{
  /* Accelero */
  if(BSP_MOTION_SENSOR_Enable(ACCELERO_INSTANCE, MOTION_ACCELERO)==BSP_ERROR_NONE)
    ALLMEMS1_PRINTF("\tEnabled Accelero Sensor\n\r");

  /* Gyro */
  if(BSP_MOTION_SENSOR_Enable(GYRO_INSTANCE, MOTION_GYRO)==BSP_ERROR_NONE)
    ALLMEMS1_PRINTF("\tEnabled Gyroscope Sensor\n\r");

}

/** @brief Initialize GPIO
 *  @param None
 *  @retval None
 */
static void MX_GPIO_Init(void)
{
  /* Enable interruption LSM6DS0_INT_PIN  */
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOF_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0x04, 0x00);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

/**
  * @brief  This function switches on the LED
  * @param  None
  * @retval None
  */
void LedOnTargetPlatform(void)
{
  BSP_LED_On( LED10 );
  TargetBoardFeatures.LedStatus=1;
}

/**
  * @brief  This function switches off the LED
  * @param  None
  * @retval None
  */
void LedOffTargetPlatform(void)
{
  BSP_LED_Off( LED10 );
  TargetBoardFeatures.LedStatus=0;
}

/** @brief  This function toggles the LED
  * @param  None
  * @retval None
  */
void LedToggleTargetPlatform(void)
{
  BSP_LED_Toggle( LED10 );
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}

/**
  * @brief  Gets the bank of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The bank of a given address
  */
static uint32_t GetBank(uint32_t Addr)
{
  uint32_t bank = 0;

  /* No Bank swap */
  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    bank = FLASH_BANK_1;
  }
  else
  {
    bank = FLASH_BANK_2;
  }

  return bank;
}

/**
 * @brief User function for Erasing the MDM on Flash
 * @param None
 * @retval uint32_t Success/NotSuccess [1/0]
 */
uint32_t UserFunctionForErasingFlash(void) {
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;
  uint32_t Success=1;

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks       = GetBank(MDM_FLASH_ADD);
  EraseInitStruct.Page        = GetPage(MDM_FLASH_ADD);
  EraseInitStruct.NbPages     = 2;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK){
    /* Error occurred while sector erase.
      User can add here some code to deal with this error.
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
    Success=0;
    Error_Handler();
  }

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  return Success;
}

/**
 * @brief User function for Saving the MDM  on the Flash
 * @param void *InitMetaDataVector Pointer to the MDM beginning
 * @param void *EndMetaDataVector Pointer to the MDM end
 * @retval uint32_t Success/NotSuccess [1/0]
 */
uint32_t UserFunctionForSavingFlash(void *InitMetaDataVector,void *EndMetaDataVector)
{
  uint32_t Success=1;

  /* Store in Flash Memory */
  uint32_t Address = MDM_FLASH_ADD;
  uint64_t *WriteIndex;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();
  for(WriteIndex =((uint64_t *) InitMetaDataVector); WriteIndex<((uint64_t *) EndMetaDataVector); WriteIndex++) {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address,*WriteIndex) == HAL_OK){
      Address = Address + 8;
    } else {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error
         FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
      Error_Handler();
      Success =0;
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
   to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  return Success;
}

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
