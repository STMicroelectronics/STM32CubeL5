/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the STM32L562E-DK BSP Drivers
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license SLA0044,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        http://www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "string.h"

/* Exported variables --------------------------------------------------------*/
__IO FlagStatus TouchDetected     = RESET;

RTC_HandleTypeDef RtcHandle;

/* Private typedef -----------------------------------------------------------*/

typedef struct
{
  void   (*DemoFunc)(void);
  uint8_t DemoName[20];
} DemoTypedef;

typedef enum
{
  NONE = 0,
  RIGHT,
  LEFT,
  SELECT,
  UP,
  DOWN,
  EXIT
} Event_TypeDef;

/* Private define ------------------------------------------------------------*/
#define RIGHT_POS_X_THRESHOLD_MIN 200UL
#define RIGHT_POS_X_THRESHOLD_MAX 239UL   /* 240-1 */
#define RIGHT_POS_Y_THRESHOLD_MIN  40UL
#define RIGHT_POS_Y_THRESHOLD_MAX 199UL   /* 240-1-40 */
#define LEFT_POS_X_THRESHOLD_MIN    0UL
#define LEFT_POS_X_THRESHOLD_MAX   39UL   /* 40-1 */
#define LEFT_POS_Y_THRESHOLD_MIN   40UL
#define LEFT_POS_Y_THRESHOLD_MAX  199UL   /* 240-1-40 */
#define SELECT_POS_X_THRESHOLD_MIN    40UL
#define SELECT_POS_X_THRESHOLD_MAX   199UL   /* 200-1 */
#define SELECT_POS_Y_THRESHOLD_MIN    40UL
#define SELECT_POS_Y_THRESHOLD_MAX   199UL   /* 240-1-40 */
#define EXIT_POS_X_THRESHOLD_MIN     200UL
#define EXIT_POS_Y_THRESHOLD_MIN     220UL



/* Private macro -------------------------------------------------------------*/
#define COUNT_OF_EXAMPLE(x)    (sizeof(x)/sizeof(BSP_DemoTypedef))

/* Private variables ---------------------------------------------------------*/
TS_State_t DemoTS;
Event_TypeDef DemoEvent = NONE;

/* Components Initialization Status */
FlagStatus ButtonInitialized = RESET;
FlagStatus LcdInitialized = RESET;
FlagStatus LedInitialized = RESET;
FlagStatus TsInitialized  = RESET;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void SystemClock_ConfigFromSTOP(void);
static void SystemHardwareInit(void);
static void Display_DemoDescription(IddAppli_t *pIdd);
static void Display_StartMeasure(IddAppli_t *pIdd);
static void Display_IddResult(IddAppli_t *pIdd);
static void Display_IddMode(IddAppli_t *pIdd);
static void Display_Error(void);

static Event_TypeDef DemoGetEvent(TS_State_t *TS_State);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
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

  /* System common Hardware components initialization (Leds, button, joystick and LCD) */
  SystemHardwareInit();

  __HAL_RCC_PWR_CLK_ENABLE();
  /* Enable RTC back-up registers access */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_RTCAPB_CLK_ENABLE();

  /* Test if return from IDD measurement, update index */
  if (Idd_IsSignature())
  {
    /* Clear signature */
    Idd_ClearSignature();

    /* Inform the menu that we need to display the IDD measurement module */
    Display_IddResult(&IddTest[IddTestIndex]);
  }
  else
  {
    /* Display demo description */
    Display_DemoDescription(&IddTest[IddTestIndex]);
  }

  /* Infinite loop */
  while (1)
  {
    if (TouchDetected == SET)
    {
      /* Get Touch area */
      if (BSP_TS_GetState(0, &DemoTS) == BSP_ERROR_NONE)
      {
        DemoEvent = DemoGetEvent(&DemoTS);
      }
      HAL_Delay(250);
      TouchDetected = RESET;
    }

    if (DemoEvent != NONE)
    {
      switch(DemoEvent)
      {
      case SELECT:
        Display_StartMeasure(&IddTest[IddTestIndex]);
        Idd_ExecuteAction(IddTestIndex);
        break;
      case RIGHT:
        if (IddTestIndex < (IDD_TEST_NB-1))
        {
          IddTestIndex++;
        }
        else
        {
          IddTestIndex = 0;
        }
        break;
      case LEFT:
        if (DemoEvent == LEFT)
        {
          if (IddTestIndex > 0)
          {
            IddTestIndex--;
          }
          else
          {
            IddTestIndex = (IDD_TEST_NB-1);
          }
        }
        break;
      case EXIT:
        /* Exit */
        HAL_NVIC_SystemReset();
        break;
      default:
        break;
      }

      Display_DemoDescription(&IddTest[IddTestIndex]);
      DemoEvent = NONE;
    }

    /* Enter STOP mode */
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);

    /* ... device is STOP mode ... */

    /* Return from STOP mode, re-enable the SYSCLCK to 110MHz */
    SystemClock_ConfigFromSTOP();
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
  *            PLL_P                          = 7
  *            Flash Latency(WS)              = 5
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

/**
  * @brief  System Clock Configuration from STOP mode
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
  * @retval None
  */
void SystemClock_ConfigFromSTOP(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Activate PLL */
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
    Error_Handler();
  }

  /* To avoid undershoot due to maximum frequency, select PLL as system clock source */
  /* with AHB prescaler divider 2 as first step */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }

  /* AHB prescaler divider at 1 as second step */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  System Hardware initialization
  * @retval None
  */
static void SystemHardwareInit(void)
{
  /* Init LEDs  */
  if (LedInitialized != SET)
  {
    if (BSP_LED_Init(LED9) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    if (BSP_LED_Init(LED10) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    LedInitialized = SET;
  }

  /* Init User push-button in EXTI Mode */
  if (ButtonInitialized != SET)
  {
    if (BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    ButtonInitialized = SET;
  }

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
    TS_Init_t TsInit;

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

    TsInitialized = SET;
  }
}

/**
  * @brief  Display demo description message
  * @param  pIdd  Pointer to Idd Measurement structure
  * @retval None
  */
static void Display_DemoDescription(IddAppli_t *pIdd)
{
  /* Display IDD mode */
  Display_IddMode(pIdd);

  /* Set font */
  UTIL_LCD_SetFont(&Font24);

  UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"START", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)">", RIGHT_MODE);
  UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"<", LEFT_MODE);
}


/**
  * @brief  Display measurement result
  * @param  pIdd  Pointer to Idd Measurement structure
  * @retval None
  */
static void Display_IddResult(IddAppli_t *pIdd)
{
  IddValue_t idd = {0};
  char value[60];

  /* Get IDD value */
  Idd_GetValue(&idd);

  /* Display IDD mode */
  Display_IddMode(pIdd);

  sprintf(value, "%s %s", idd.value, idd.unit);

  /* Display IDD result */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_YELLOW);
  UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)value, CENTER_MODE);

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(0, 160, (uint8_t *)"RESTART", CENTER_MODE);

  UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)">", RIGHT_MODE);
  UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"<", LEFT_MODE);

}

/**
  * @brief  Display Idd measurement message
  * @param  pIdd  Pointer to Idd Measurement structure
  * @retval None
  */
static void Display_IddMode(IddAppli_t *pIdd)
{
  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);

  /* Set font */
  UTIL_LCD_SetFont(&Font24);

  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);

  /* Display LCD messages */
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"IDD", CENTER_MODE);

  /* Set font */
  UTIL_LCD_SetFont(&Font20);

  /* Display Idd mode */
  UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)pIdd->modeStr, CENTER_MODE);

  /* Add BACK display */
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_DisplayStringAt(0,215,(uint8_t*)"BACK",RIGHT_MODE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
}

/**
  * @brief  Display start of measurement message
  * @param  pIdd  Pointer to Idd Measurement structure
  * @retval None
  */
static void Display_StartMeasure(IddAppli_t *pIdd)
{
  /* Display IDD mode */
  Display_IddMode(pIdd);

  /* Set font */
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"Please wait...", CENTER_MODE);
}

/**
  * @brief  Display error message
  * @retval None
  */
static void Display_Error(void)
{
  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);

  /* Set font */
  UTIL_LCD_SetFont(&Font24);

  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);

  UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"Error...", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 110, (uint8_t *)"Power-off/on", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 160, (uint8_t *)"the board", CENTER_MODE);
}

/**
  * @brief  Get event from touch panel
  * @retval Event_TypeDef   event type
  */
static Event_TypeDef DemoGetEvent(TS_State_t *TS_State)
{
  Event_TypeDef Event = NONE;

  /* Exit selection */
  if((TS_State->TouchX > EXIT_POS_X_THRESHOLD_MIN) && (TS_State->TouchY > EXIT_POS_Y_THRESHOLD_MIN))
  {
    Event = EXIT;
  }
  /* Right selection */
  else if((TS_State->TouchX >= RIGHT_POS_X_THRESHOLD_MIN) && (TS_State->TouchX < RIGHT_POS_X_THRESHOLD_MAX))
  {
    if((TS_State->TouchY >= RIGHT_POS_Y_THRESHOLD_MIN) && (TS_State->TouchY < RIGHT_POS_Y_THRESHOLD_MAX))
    {
      Event = RIGHT;
    }
  }
  /* Left selection */
  else if((TS_State->TouchX < LEFT_POS_X_THRESHOLD_MAX))
  {
    if((TS_State->TouchY >= LEFT_POS_Y_THRESHOLD_MIN) && (TS_State->TouchY < LEFT_POS_Y_THRESHOLD_MAX))
    {
      Event = LEFT;
    }
  }
  /* Select selection */
  else if((TS_State->TouchX >= SELECT_POS_X_THRESHOLD_MIN) && (TS_State->TouchX < SELECT_POS_X_THRESHOLD_MAX))
  {
    if((TS_State->TouchY >= SELECT_POS_Y_THRESHOLD_MIN) && (TS_State->TouchY < SELECT_POS_Y_THRESHOLD_MAX))
    {
      Event = SELECT;
    }
  }

  return Event;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

  Display_Error();

  while(1)
  {
    /* Toggle LED9 */
    (void) BSP_LED_Toggle(LED9);
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
  /* Perform system reset */
  HAL_NVIC_SystemReset();
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

/**
  * @brief IDD detection callbacks.
  * @param Instance IDD instance
  * @retval None
  */
void BSP_IDD_Callback(uint32_t Instance)
{
  /* Perform reset to display IDD measurement */
  HAL_NVIC_SystemReset();
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file pointer to the source file name
  * @param  line assert_param error line source number
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
