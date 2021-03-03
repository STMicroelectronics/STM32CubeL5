/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the STM32L552E-EV BSP Drivers
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
#include "stlogo.h"
#include "stdio.h"
#include "string.h"

/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Exported variables --------------------------------------------------------*/
__IO FlagStatus WakeupButtonPressed = RESET;
__IO JOYPin_TypeDef JoyPinPressed   = JOY_NONE;
__IO FlagStatus TouchDetected       = RESET;

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  void   (*DemoFunc)(void);
  uint8_t DemoName[50];
  uint32_t DemoIndex;
} BSP_DemoTypedef;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define COUNT_OF_EXAMPLE(x)    (sizeof(x)/sizeof(BSP_DemoTypedef))

/* Private variables ---------------------------------------------------------*/
uint8_t DemoIndex = 0;

BSP_DemoTypedef  BSP_examples[]=
{
  {Led_demo, "LED", 0},
  {Joystick_demo, "JOYSTICK", 0},
  {LCD_demo, "LCD", 0},
  {Touchscreen_demo, "TOUCHSCREEN", 0},
  {AudioPlay_demo, "AUDIO PLAY", 0},
  {AudioRecord_demo, "AUDIO RECORD", 0},
  {SD_demo, "mSD", 0},
  {SRAM_demo, "SRAM", 0},
  {OSPI_NOR_demo, "OSPI NOR", 0},
  {OSPI_RAM_demo, "OSPI RAM", 0}
};

/* Components Initialization Status */
FlagStatus ButtonInitialized = RESET;
FlagStatus JoyInitialized = RESET;
FlagStatus LcdInitialized = RESET;
FlagStatus LedInitialized = RESET;
FlagStatus TsInitialized  = RESET;


/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void SystemHardwareInit(void);
static void Display_DemoDescription(void);

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

  /* For better performances, enable the instruction cache in 1-way direct mapped mode */
  HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY);
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* System common Hardware components initialization (Leds, button, joystick and LCD) */
  SystemHardwareInit();

  /* Display demo description */
  Display_DemoDescription();

  /* Infinite loop */
  while (1)
  {
    if(WakeupButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      HAL_Delay(500);
      WakeupButtonPressed = RESET;

      BSP_examples[DemoIndex++].DemoFunc();

      if(DemoIndex >= COUNT_OF_EXAMPLE(BSP_examples))
      {
        DemoIndex = 0;
      }

      Display_DemoDescription();
    }
  }
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
  *            PLL_P                          = 2
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
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
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  * @brief  System Power Configuration at Boot
  * @param  None
  * @retval None
  */
static void SystemHardwareInit(void)
{
  /* Init LEDs  */
  if (LedInitialized != SET)
  {
    if (BSP_LED_Init(LED4) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    if (BSP_LED_Init(LED5) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    if (BSP_LED_Init(LED6) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    if (BSP_LED_Init(LED7) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    LedInitialized = SET;
  }

  /* Init Wakeup and tamper push-buttons in EXTI Mode */
  if (ButtonInitialized != SET)
  {
    if (BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_EXTI) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    ButtonInitialized = SET;
  }

  /* Init Joystick in interrupt mode */
  if (JoyInitialized != SET)
  {
    if (BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    JoyInitialized = SET;
  }


  /* Initialize the LCD */
  if (LcdInitialized != SET)
  {
    LCD_UTILS_Drv_t lcdDrv;

    /* Initialize the LCD */
    if (BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE) != BSP_ERROR_NONE)
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
    UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

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
    TS_Init_t TsInit;

    /* Initialize the TouchScreen */
    TsInit.Width       = 320;
    TsInit.Height      = 240;
    TsInit.Orientation = TS_ORIENTATION_LANDSCAPE;
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

    TsInitialized = SET;
  }
}

/**
  * @brief  Display main demo messages
  * @param  None
  * @retval None
  */
static void Display_DemoDescription(void)
{
  char desc[60];

  /* Set font */
  UTIL_LCD_SetFont(&Font24);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKBLUE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);

  /* Display LCD messages */
  UTIL_LCD_DisplayStringAt(0, 10, (uint8_t *)"STM32L552-EV BSP", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 35, (uint8_t *)"drivers example", CENTER_MODE);

  /* Draw Bitmap */
  UTIL_LCD_DrawBitmap(120, 65, (uint8_t *)stlogo);

  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 220, (uint8_t *)"Copyright (c) STMicroelectronics 2019", CENTER_MODE);

  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_FillRect(0, 150, 320, 50, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(0, 150, (uint8_t *)"Press Wakeup push-button", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 165, (uint8_t *)"to start :", CENTER_MODE);
  sprintf(desc,"%s example", BSP_examples[DemoIndex].DemoName);
  UTIL_LCD_DisplayStringAt(0, 180, (uint8_t *)desc, CENTER_MODE);
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
    /* Toggle LED5 */
    (void) BSP_LED_Toggle(LED5);
    HAL_Delay(500);
  }
}

/**
  * @brief  BSP Push Button callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None.
  */
void BSP_PB_Callback(Button_TypeDef Button)
{
  if (Button == BUTTON_WAKEUP)
  {
    WakeupButtonPressed = SET;
  }
}

/**
  * @brief  BSP Joystick Callback.
  * @param  JOY Joystick.
  *   This parameter can be JOY1
  * @param  JoyPins Specifies joystick pin.
  *   This parameter can be one of following parameters:
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  * @retval None.
  */
void BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  if (JOY == JOY1)
  {
    JoyPinPressed = JoyPin;
  }
}

/**
  * @brief  TS Callback.
  * @param  Instance TS Instance.
  * @retval None.
  */
void BSP_TS_Callback(uint32_t Instance)
{
  if (Instance == 0)
  {
    TouchDetected = SET;
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
