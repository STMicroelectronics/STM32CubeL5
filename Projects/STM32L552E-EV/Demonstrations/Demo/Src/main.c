/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   This file provides main program functions
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
#include <string.h>
#include "main.h"
#include "utils.h"
#include "stm32l552e_eval.h"
#include "stm32l552e_eval_lcd.h"
#include "stm32l552e_eval_idd.h"
#include "stm32l552e_eval_sd.h"
#include "k_config.h"
#include "k_window.h"
#include "k_demo.h"
#include "k_storage.h"
#include "k_widgets.h"
#include "k_menu.h"
#include "app_lowpower.h"
#include "app_imagesbrowser.h"
#include "app_main.h"
#include "app_audio.h"
#include "app_calendar.h"
#include "app_filesbrowser.h"
#include "app_ucpd.h"

/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

static void     BootCheckAndExec(void);

/* Private variables ---------------------------------------------------------*/
uint32_t RccBootFlags = 0;
uint32_t PwrSBFlag = 0;

LCD_UTILS_Drv_t LCD_Driver;

/* Exported variables --------------------------------------------------------*/
RTC_HandleTypeDef RtcHandle = {0};

/* Exported functions --------------------------------------------------------*/
/**
* @brief  Main program
* @retval int
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

  /* Enable Instruction cache:
     1-way cache set for low power purpose
   */
  if(HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  if(HAL_ICACHE_Enable() != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Configure the system clock @ 110 MHz (PLL ON) */
  SystemClock_Config();

  /* LED init */
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);

  /* Enable RTC & backup domain access */
  RTC_Config();

  /* Check Boot Reason */
  BootCheckAndExec();

  /* Start the demo thread */
  kDemo_Start();

  /* we should not reach here */
  while(1);
}

/** @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {
  case SD_DETECT_PIN :
    BSP_SD_Init(0);
    kStorage_SdDetection(BSP_SD_IsDetected(0));
    break;
  case JOY_DOWN :
  case JOY_UP :
  case JOY_SEL :
  case JOY_RIGHT :
  case JOY_LEFT :
  case BUTTON_WAKEUP :
    kMenu_EventHandler(GPIO_Pin);
    break;
  default :
    Error_Handler();
    break;
  }
}

/** @brief Demonstration kernel initialization
  * @retval None
  */
void kDemo_Initialization(void)
{
  uint32_t pXSize;
 uint32_t iddValue;

  /* Initialize the SD */
  if (BSP_SD_Init(0) == BSP_ERROR_PERIPH_FAILURE)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Initialize the LCD */
  if (BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE) != BSP_ERROR_NONE)
  {
    /* Initialization Error */
    Error_Handler();
  }

  LCD_Driver.DrawBitmap  = BSP_LCD_DrawBitmap;
  LCD_Driver.FillRGBRect = BSP_LCD_FillRGBRect;
  LCD_Driver.DrawHLine   = BSP_LCD_DrawHLine;
  LCD_Driver.DrawVLine   = BSP_LCD_DrawVLine;
  LCD_Driver.FillRect    = BSP_LCD_FillRect;
  LCD_Driver.GetPixel    = BSP_LCD_ReadPixel;
  LCD_Driver.SetPixel    = BSP_LCD_WritePixel;
  LCD_Driver.GetXSize    = BSP_LCD_GetXSize;
  LCD_Driver.GetYSize    = BSP_LCD_GetYSize;
  LCD_Driver.SetLayer    = BSP_LCD_SetActiveLayer;
  LCD_Driver.GetFormat   = BSP_LCD_GetFormat;

  UTIL_LCD_SetFuncDriver(&LCD_Driver); /* SetFunc before setting device */
  UTIL_LCD_SetDevice(0);            /* SetDevice after funcDriver is set */

  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set the display on */
  if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Check if we return from standby */
  if(PwrSBFlag != 0)
  {
    /* Init MFX communication */
    if(BSP_IDD_Init(0) < 0)
    {
      Error_Handler();
    }

    /* Get MFX IDD value */
    iddValue = LowPowerIddGetValue();

    /* Deint Measure through MFX */
    LowPowerIddMeasureDeInit();

    /* Display info */
    LowPowerExitDisplay(STANDBY, iddValue);
  }

  /* Check the SD presence else wait until SD insertion */
  if(BSP_SD_IsDetected(0) != SD_PRESENT)
  {
    kWindow_Error("sd not present\n...\nwait sd insertion\n");
    while(BSP_SD_IsDetected(0) != SD_PRESENT);
    BSP_SD_Init(0);
  }

  /* FatFs initialisation */
  if(kStorage_Init() != KSTORAGE_NOERROR)
  {
    kWindow_Error("FatFs initialization failed\nplease check the SD card\n");
    Error_Handler();
  }

  if(RccBootFlags != 0)
  {
    /* Display the demonstration window */
    UTIL_LCD_SetFont(&UTIL_LCD_DEFAULT_FONT);
    UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
    BSP_LCD_GetXSize(0, &pXSize);
    UTIL_LCD_FillRect(0, 0, pXSize, UTIL_LCD_DEFAULT_FONT.Height*2, UTIL_LCD_COLOR_ST_PINK);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_DisplayStringAt( 0,              0,      (uint8_t *)EVAL_BOARD, CENTER_MODE);
    UTIL_LCD_DisplayStringAt( 0,  UTIL_LCD_DEFAULT_FONT.Height  , (uint8_t *)"DEMONSTRATION", CENTER_MODE);

    kStorage_OpenFileDrawPixel(((UTIL_LCD_DEFAULT_FONT.Height*2) + 1), ((pXSize - 170) / 2), (uint8_t *)"STFILES/STLogo.bmp");

    /* Let time user to see information */
    HAL_Delay(2000);
  }

  /* Module Initialization */
  kModule_Init();

  /* control the resources */
  if(kModule_CheckResource() != KMODULE_OK)
  {
    kWindow_Error("resource file\nmissing\nplease update\nsd card content\n");
    while(1);
  }
}

void kModule_Init(void)
{
   kModule_Add(MODULE_MAIN_APP    , ModuleAppMain);
   kModule_Add(MODULE_VIEWBMP     , ModuleImageBrowser);
   kModule_Add(MODULE_FILESBRO    , ModuleFilesBrowser);
   kModule_Add(MODULE_LOWPOWER    , ModuleLowPower);
   kModule_Add(MODULE_AUDIO       , ModuleAudio);
   kModule_Add(MODULE_CALENDAR    , ModuleCalendar);
   kModule_Add(MODULE_UCPD        , ModuleUcpd);
}

void kDemo_UnInitialization(void)
{
  /* Nothing to do */
}

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File pointer to the source file name
* @param  Line assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line
  number,ex: printf("Wrong parameters value: file %s on line %d\r\n",
  file, line) */

  /* Infinite loop */
  while (1)
  {}
}

#endif

/* Private functions ---------------------------------------------------------*/

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

  /* Enable MSI Oscillator and activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
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



/**
  * @brief  Configures the RTC & backup domain access
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
  /* Enable Power Clock*/
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Allow Access to RTC Backup domaine */
  HAL_PWR_EnableBkUpAccess();

  /* Set the RTC time base to 1s */
  RtcHandle.Instance = RTC;
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = 0x7Fu;
  RtcHandle.Init.SynchPrediv = 0x00FFu;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Set Calendar Ultra-Low power mode */
  if (HAL_RTCEx_SetLowPowerCalib(&RtcHandle, RTC_LPCAL_SET) != HAL_OK)
  {
    Error_Handler();
  }
}

void RTC_UnConfig(void)
{
  if(HAL_RTC_DeInit(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}



/**
  * @brief  Check boot reason
  * @retval None
  */
static void BootCheckAndExec(void)
{
  /* Get reset flags */
  RccBootFlags = RCC->CSR & (RCC_CSR_PINRSTF | RCC_CSR_BORRSTF | RCC_CSR_SFTRSTF
                             | RCC_CSR_OBLRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_LPWRRSTF);

  /* Clear reset flags */
  __HAL_RCC_CLEAR_RESET_FLAGS();

  /* Get Standby flag */
  PwrSBFlag = __HAL_PWR_GET_FLAG(PWR_FLAG_SB);

  /* Clear Standby flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
}


/**
  * @brief  Error Handler.
  * @retval None
  */
void Error_Handler(void)
{
  while(1)
  {
    HAL_Delay(100);
    BSP_LED_Toggle(LED4);
    BSP_LED_Toggle(LED5);
  }
}

/**
  * @brief  BSP Joystick Callback.
  * @param  JoyPin Specifies the pin connected EXTI line
  * @retval None
  */
void BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  if (UcpdDemoRunning == 1)
  {
    UcpdJoyCallback(JOY, JoyPin);
  }
  else
  {
    switch(JoyPin)
    {
    case JOY_DOWN :
    case JOY_UP :
    case JOY_SEL :
    case JOY_RIGHT :
    case JOY_LEFT :
      kMenu_EventHandler(JoyPin);
      break;
    default :
      break;
    }
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
