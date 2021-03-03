/**
  ******************************************************************************
  * @file    TouchSensing\TouchSensing_1touchkey\Src\main.c
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "stm32l552e_eval.h"
#include "stm32l552e_eval_lcd.h"
#include "stm32_lcd.h"

/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup TouchSensing_1touchkey
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Select to use or not the LCD (0=No, 1=Yes) */
#define USE_LCD (1)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* TSC handler declaration */
TSC_HandleTypeDef TscHandle;

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
static void Error_Handler(void);
static void Process_Sensors(tsl_user_status_t status);

/* Utility functions for the LCD */
#if USE_LCD > 0
static uint8_t LcdMakeLine(uint8_t *output, uint8_t *name, uint8_t *state, uint8_t *data);
static uint8_t *LcdState2String(TSL_StateId_enum_T State);
static void LcdValue2String(uint8_t *output, int16_t input);
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  tsl_user_status_t tsl_status;
#if USE_LCD > 0
  LCD_UTILS_Drv_t lcdDrv;
#endif

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
  HAL_ICACHE_Enable();

  /* Configure LEDs */
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);

  /* Configure the TSC peripheral */
  TscHandle.Instance                     = TSCx;
  TscHandle.Init.AcquisitionMode         = TSC_ACQ_MODE_NORMAL;
  TscHandle.Init.CTPulseHighLength       = TSC_CTPH_2CYCLES;
  TscHandle.Init.CTPulseLowLength        = TSC_CTPL_2CYCLES;
  TscHandle.Init.IODefaultMode           = TSC_IODEF_OUT_PP_LOW;
  TscHandle.Init.MaxCountInterrupt       = DISABLE;
  TscHandle.Init.MaxCountValue           = TSC_MCV_8191;
  TscHandle.Init.PulseGeneratorPrescaler = TSC_PG_PRESC_DIV32;
  TscHandle.Init.SpreadSpectrum          = DISABLE;
  TscHandle.Init.SpreadSpectrumDeviation = 127;
  TscHandle.Init.SpreadSpectrumPrescaler = TSC_SS_PRESC_DIV1;
  TscHandle.Init.SynchroPinPolarity      = TSC_SYNC_POLARITY_FALLING;
  /* All channel, shield and sampling IOs must be declared below */
  TscHandle.Init.ChannelIOs              = TSC_GROUP4_IO1;
  TscHandle.Init.SamplingIOs             = TSC_GROUP4_IO2 | TSC_GROUP2_IO4;
  TscHandle.Init.ShieldIOs               = TSC_GROUP2_IO3;

  if (HAL_TSC_Init(&TscHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Just to indicate the fw is alive... */
  BSP_LED_On(LED4);
  BSP_LED_On(LED5);
  BSP_LED_On(LED6);

  /* Configure LCD */
#if USE_LCD > 0
  /* Configure LCD */
  if (BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }


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

  /* Set UTIL_LCD functions */
  UTIL_LCD_SetFuncDriver(&lcdDrv);

  /* Clear screen */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set the display on */
  if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  UTIL_LCD_SetFont(&UTIL_LCD_DEFAULT_FONT);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKBLUE);

  UTIL_LCD_DisplayStringAt(0, 130, (uint8_t *)"STM32L552xx", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 150, (uint8_t *)"TouchSensing", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 170, (uint8_t *)"Example", CENTER_MODE);
#endif

  BSP_LED_Off(LED4);
  BSP_LED_Off(LED5);
  BSP_LED_Off(LED6);

  /* Initialize the STMTouch driver */
  tsl_user_Init();

  /* Infinite loop */
  while (1)
  {
    /* Execute STMTouch Driver state machine */
    tsl_status = tsl_user_Exec();
    if (tsl_status != TSL_USER_STATUS_BUSY)
    {
      Process_Sensors(tsl_status);
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

/**
  * @brief  Display sensors information on LEDs and LCD
  * @param  status TSL user status
  * @retval None
  */
void Process_Sensors(tsl_user_status_t status)
{
#if USE_LCD > 0
  uint8_t TheLine[20];
  uint8_t str_value[6]; /* sign + 4 digits + empty char */
#endif

#if USE_LCD > 0
  /* Display TS1 sensor information */
  LcdValue2String(str_value, MyTKeys[0].p_ChD->Delta);
  LcdMakeLine(TheLine, (uint8_t *)("TS1"), LcdState2String(MyTKeys[0].p_Data->StateId), str_value);
  UTIL_LCD_DisplayStringAt(0, 25, TheLine, CENTER_MODE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKBLUE);
#endif

  /* LED4 is ON when TS1 on board is touched */
  if (MyTKeys[0].p_Data->StateId == TSL_STATEID_DETECT)
  {
    BSP_LED_On(LED4);
  }
  else
  {
    BSP_LED_Off(LED4);
  }

  /* ECS information */
  switch (status)
  {
    case TSL_USER_STATUS_OK_ECS_OFF:
      BSP_LED_Off(LED6);
#if USE_LCD > 0
      UTIL_LCD_DisplayStringAt(0, 5, (uint8_t *)("ECS OFF"), CENTER_MODE);
#endif
      break;
    case TSL_USER_STATUS_OK_ECS_ON:
      BSP_LED_Toggle(LED6);
#if USE_LCD > 0
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
      UTIL_LCD_DisplayStringAt(0, 5, (uint8_t *)("ECS ON "), CENTER_MODE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKBLUE);
#endif
      break;
    default:
      break;
  }
}

#if USE_LCD > 0
/**
  * @brief Converts a value in string
  * @param[out] output String
  * @param[in]  input  Value (between -9999 and +9999)
  * @retval None
  */
void LcdValue2String(uint8_t *output, int16_t input)
{
  uint8_t *temp;
  temp = output;

  if ((input > 9999) || (input < -9999))
  {
    *temp = 0;
    return;
  }

  /* 1st char: sign */
  if (input < 0)
  {
    *temp = '-';
    input = -input;
  }
  else
  {
    *temp = '+';
  }

  temp++;

  /* 2nd char: first digit */
  if ((input > 999) && (input < 10000))
  {
    *temp = 48 + (input / 1000); /* First digit */
  }
  else
  {
    *temp = ' ';
  }

  temp++;

  /* 3rd char: second digit */
  if (input > 99)
  {
    *temp = 48 + ((input % 1000) / 100); /* Second digit */
  }
  else
  {
    *temp = ' ';
  }

  temp++;

  /* 4th char: third digit */
  if (input > 9)
  {
    *temp = 48 + ((input % 100) / 10); /* Third digit */
  }
  else
  {
    *temp = ' ';
  }

  temp++;

  /* 5th char: fourth digit */
  *temp = 48 + (input % 10); /* Fourth digit */

  temp++;

  /* 5th char: null value */
  *temp = 0;
}

/**
  * @brief Converts the sensor state ID in a string
  * @param[in] state Sensor state ID
  * @retval String Sensor state string
  */
uint8_t* LcdState2String(TSL_StateId_enum_T state)
{
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKBLUE);
  switch (state)
  {
    case TSL_STATEID_DEB_CALIB:
        return (uint8_t*)("DB_CALIB");
    case TSL_STATEID_CALIB:
        return (uint8_t*)("CALIB   ");
    case TSL_STATEID_RELEASE:
        return (uint8_t*)("RELEASE ");
    case TSL_STATEID_DEB_RELEASE_PROX:
    case TSL_STATEID_DEB_RELEASE_DETECT:
    case TSL_STATEID_DEB_RELEASE_TOUCH:
        return (uint8_t*)("DB_REL  ");
    case TSL_STATEID_PROX:
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_MAGENTA);
        return (uint8_t*)("PROX    ");
    case TSL_STATEID_DEB_PROX:
    case TSL_STATEID_DEB_PROX_DETECT:
    case TSL_STATEID_DEB_PROX_TOUCH:
        return (uint8_t*)("DB_PROX ");
    case TSL_STATEID_DETECT:
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
        return (uint8_t*)("DETECT  ");
    case TSL_STATEID_DEB_DETECT:
        return (uint8_t*)("DB_DET  ");
    case TSL_STATEID_TOUCH:
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
        return (uint8_t*)("TOUCH   ");
    case TSL_STATEID_ERROR:
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        return (uint8_t*)("ERROR   ");
    case TSL_STATEID_DEB_ERROR_CALIB:
    case TSL_STATEID_DEB_ERROR_RELEASE:
    case TSL_STATEID_DEB_ERROR_PROX:
    case TSL_STATEID_DEB_ERROR_DETECT:
    case TSL_STATEID_DEB_ERROR_TOUCH:
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        return (uint8_t*)("DB_ERR  ");
    case TSL_STATEID_OFF:
        return (uint8_t*)("OFF     ");
    default:
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        return (uint8_t*)("UNKNOWN ");
  }
}

/**
  * @brief Create the string to be displayed
  * @param[out] output Output string
  * @param[in]  name   Sensor name (3 characters)
  * @param[in]  state  Sensor state (8 characters)
  * @param[in]  data   Sensor data (-9999 to +9999)
  * @retval uint8_t Number of characters in the output string
  */
uint8_t LcdMakeLine(uint8_t *output, uint8_t *name, uint8_t *state, uint8_t *data)
{
  uint8_t index = 0;
  uint8_t index2;

  /* Sensor name: 3 characters */
  for (index2 = 0; index2 < 3; index2++)
  {
    output[index++] = name[index2];
  }
  output[index++] = ' ';

  /* Sensor state: 8 characters */
  for (index2 = 0; index2 < 8; index2++)
  {
    output[index++] = state[index2];
  }
  output[index++] = ' ';

  /* Sensor data */
  for (index2 = 0; data[index2] != 0; index2++)
  {
    output[index++] = data[index2];
  }

  output[index++] = 0;

  return index;
}
#endif /* if USE_LCD > 0 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while (1)
  {
    BSP_LED_Toggle(LED5);
    HAL_Delay(200);
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
    BSP_LED_Toggle(LED5);
    HAL_Delay(100);
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
