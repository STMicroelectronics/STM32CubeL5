/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the STM32L562E-DK BSP Drivers
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_hcr.h"

/* Exported variables --------------------------------------------------------*/
__IO FlagStatus UserButtonPressed = RESET;
__IO FlagStatus TouchDetected     = RESET;

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

/* Components Initialization Status */
FlagStatus LcdInitialized = RESET;
FlagStatus TsInitialized  = RESET;

TS_Init_t TsInit;   /* Set as global for new TS init */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void SystemHardwareInit(void);
static void crcInit(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* STM32L5xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 3
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the System clock to have a frequency of 110 MHz */
  SystemClock_Config();

  /* Enable Instruction cache (default 2-ways set associative cache) */
  if(HAL_ICACHE_Enable() != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* CRC init function */
  crcInit();

  /* System common Hardware components initialization (LCD, TS) */
  SystemHardwareInit();

  /* App Execution    */
  HCR_demo();

  /* we should not reach here */
  while(1);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 110000000
  *            HCLK(Hz)                       = 110000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 55
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable voltage range 0 for frequency above 80 Mhz */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0);
  __HAL_RCC_PWR_CLK_DISABLE();

  /* MSI Oscillator enabled at reset (4Mhz), activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 55;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* To avoid undershoot due to maximum frequency, select PLL as system clock source */
  /* with AHB prescaler divider 2 as first step */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* AHB prescaler divider at 1 as second step */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

static void crcInit(void)
{
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;

  __HAL_RCC_CRC_CLK_ENABLE();

  HAL_CRC_Init(&hcrc);
}

/**
  * @brief  System Power Configuration at Boot
  * @param  None
  * @retval None
  */
static void SystemHardwareInit(void)
{
  BSP_OSPI_NOR_Init_t sOSPI_NOR_Init;

  sOSPI_NOR_Init.InterfaceMode = BSP_OSPI_NOR_OPI_MODE;
  sOSPI_NOR_Init.TransferRate  = BSP_OSPI_NOR_DTR_TRANSFER;
  if (BSP_OSPI_NOR_Init(0, &sOSPI_NOR_Init) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  BSP_OSPI_NOR_EnableMemoryMappedMode(0);

  /* Initialize the LCD */
  if (LcdInitialized != SET)
  {
    LCD_UTILS_Drv_t lcdDrv;

    /* Initialize the LCD */
    if (BSP_LCD_Init(0, LCD_ORIENTATION_PORTRAIT) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }

    /* Set UTIL_LCD functions */
    lcdDrv.DrawBitmap  = BSP_LCD_DrawBitmap;
    lcdDrv.FillRGBRect = BSP_LCD_FillRGBRect;
    lcdDrv.DrawHLine   = BSP_LCD_DrawHLine;
    lcdDrv.DrawVLine   = BSP_LCD_DrawVLine;
    lcdDrv.FillRect    = BSP_LCD_FillRect;
    lcdDrv.GetPixel    = BSP_LCD_ReadPixel;
    lcdDrv.SetPixel    = BSP_LCD_WritePixel;
    lcdDrv.GetXSize    = BSP_LCD_GetXSize;
    lcdDrv.GetYSize    = BSP_LCD_GetYSize;
    lcdDrv.SetLayer    = BSP_LCD_SetActiveLayer;
    lcdDrv.GetFormat   = BSP_LCD_GetFormat;
    UTIL_LCD_SetFuncDriver(&lcdDrv);

    /* Clear the LCD */
    UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);

    /* Set the display on */
    if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }

    LcdInitialized = SET;
  }

  /* Initialize the TouchScreen */
  if (TsInitialized != SET)
  {
    /* Initialize the TouchScreen */
    TsInit.Width       = 240;
    TsInit.Height      = 240;
    TsInit.Orientation = TS_ORIENTATION_PORTRAIT;
    TsInit.Accuracy    = 0;
    if (BSP_TS_Init(0, &TsInit) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }

    TsInitialized = SET;
  }

#if defined(DEBUG)
  {
    COM_InitTypeDef COM_Init;

    /* Initialize COM init structure */
    COM_Init.BaudRate   = 115200;
    COM_Init.WordLength = COM_WORDLENGTH_8B;
    COM_Init.StopBits   = COM_STOPBITS_1;
    COM_Init.Parity     = COM_PARITY_NONE;
    COM_Init.HwFlowCtl  = COM_HWCONTROL_NONE;

    if (BSP_COM_Init(COM1, &COM_Init) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
  }
#endif
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
