/**
  ******************************************************************************
  * @file    iddmeasure.c
  * @author  MCD Application Team
  * @brief   Power measurement functions
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
#define __IDDMEASURE_C

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iddmeasure.h"

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#define PWR_GPIO_BIT_ALL (PWR_GPIO_BIT_0 | PWR_GPIO_BIT_1 |PWR_GPIO_BIT_2 |PWR_GPIO_BIT_3 |PWR_GPIO_BIT_4 |PWR_GPIO_BIT_5 |PWR_GPIO_BIT_6 |PWR_GPIO_BIT_7 |PWR_GPIO_BIT_8 |PWR_GPIO_BIT_9 |PWR_GPIO_BIT_10 |PWR_GPIO_BIT_11 |PWR_GPIO_BIT_12 |PWR_GPIO_BIT_13 |PWR_GPIO_BIT_14 |PWR_GPIO_BIT_15)

#define DEBUG_ON                0
#define RTC_WAKEUP              0 /* RTC Wakeup from standby and shutdown instead of Energy meter */
#define OPTIM_STANDBY_SHUTDOWN  1
#define IDD_SIGNATURE           1


#define RTC_CLOCK_SOURCE_LSE

#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  (0x7F)
#define RTC_SYNCH_PREDIV   (0x00FF)
#endif

/* Private function prototypes -----------------------------------------------*/
static void Idd_RunEnter(void);
static void Idd_SleepEnter(void);
static void Idd_LprEnter(void);
static void Idd_LprSleepEnter(void);
static void Idd_StopEnter(void);
static void Idd_StandbyEnter(void);

#if (RTC_WAKEUP == 1)
static void Idd_SetRtcWakeup(void);
#else
static void Idd_WakeUpPinConfig(void);
#endif
static void Idd_Convert(uint32_t Value, IddValue_t * idd);

#if (IDD_SIGNATURE == 1)
static void Idd_SaveSignature(void);
#endif
void IddMeasureInit(void);
void IddMeasureDeInit(void);
void while1Aligned64(void);

void SystemLowClock_Config(void);
void SystemHardwareDeInit(uint32_t mode);
void SystemPeripheralClockEnable(void);
void SystemPeripheralClockDisable(uint32_t rtc_on);
void Error_Handler(void);

extern RTC_HandleTypeDef RtcHandle;
IddState_t IddMeasureState = IDD_STATE_IDLE;
FlagStatus IddInitialized = RESET;
FlagStatus LCDInitialized = SET;
uint32_t IddTestIndex = 0;
IddAppli_t IddTest[IDD_TEST_NB]=
{
  {Idd_RunEnter, "Run 110MHz", IDD_RUN},
  {Idd_SleepEnter, "Sleep 110MHz", IDD_SLEEP},
  {Idd_LprEnter, "LP Run 2MHz", IDD_LPR_2MHZ},
  {Idd_LprSleepEnter, "LP Sleep", IDD_LPR_SLEEP},
  {Idd_StopEnter,  "Stop2", IDD_STOP2},
  {Idd_StandbyEnter, "Standby", IDD_STANDBY},
};

#if (OPTIM_STANDBY_SHUTDOWN == 1)
typedef enum
{
  NOT_VALID = 0,
  PU,
  PD, /* update LAST_GPIO_TYPE if you add a mode */
}gpioMode_t;
#define LAST_GPIO_TYPE PD

typedef struct
{
  gpioMode_t gpioMode;
  uint32_t port;
  uint32_t pin;
} bestConfig_t;

static const bestConfig_t bestConfigStandbyShutdown[] = {
//    {PU, PWR_GPIO_B, PWR_GPIO_BIT_6},  /* I2C1 PU */
//    {PU, PWR_GPIO_B, PWR_GPIO_BIT_7},  /* I2C1 PU */
//    {PU, PWR_GPIO_B, PWR_GPIO_BIT_10}, /* LPUART1 RX */
//    {PD, PWR_GPIO_C, PWR_GPIO_BIT_13}, /* PC 13*/
    {PU, PWR_GPIO_G, PWR_GPIO_BIT_13}, /* LED GREEN */
    {PU, PWR_GPIO_D, PWR_GPIO_BIT_3},  /* LED RED */
};

#define NB_BEST_CONFIG_STANDBY_SHUTDOWN (sizeof(bestConfigStandbyShutdown) / sizeof(bestConfig_t))
void GPIOoptimStandbyShutdown(void);
#endif /* OPTIM_STANDBY_SHUTDOWN */


/* Private typedef -----------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

#if (IDD_SIGNATURE == 1)
/**
* @brief  Automatically restore index if signature is TRUE
* @retval Boolean 0 = FALSE; 1 = TRUE.
*/
uint32_t Idd_IsSignature(void)
{
  uint32_t bkup;

  __HAL_RCC_PWR_CLK_ENABLE();
  /* Enable RTC back-up registers access */
  HAL_PWR_EnableBkUpAccess();

  bkup = READ_REG(TAMP->BKP28R);

  if (bkup != 0)
  {
    /* Restore index */
    IddTestIndex = bkup - 1;
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
* @brief  clear signature
* @retval None
*/
void Idd_ClearSignature(void)
{
  __HAL_RCC_PWR_CLK_ENABLE();
  /* Enable RTC back-up registers access */
  HAL_PWR_EnableBkUpAccess();

  /* Clear signature */
  WRITE_REG(TAMP->BKP28R, 0);
}

/**
* @brief  Save index and set signature
* @retval None
*/
static void Idd_SaveSignature(void)
{
  __HAL_RCC_PWR_CLK_ENABLE();
  /* Enable RTC back-up registers access */
  HAL_PWR_EnableBkUpAccess();

  WRITE_REG( TAMP->BKP28R, IddTestIndex + 1);
  WRITE_REG( TAMP->BKP26R, 0x08018000);
}
#endif

/**
* @brief  Initialize the Idd Measurement application
* @retval None
*/
void IddMeasureInit(void)
{
  /* Initialize Idd measurement component */
  if(IddInitialized != SET)
  {
    IddInitialized = SET;

    /*## IDD Initialization ############################*/
    if(BSP_IDD_Init(0) < 0)
    {
      Error_Handler();
    }

    if(BSP_IDD_EnableIT(0) < 0)
    {
      Error_Handler();
    }
  }
  return;
}

/**
* @brief  DeInitialize the Idd Measurement application
* @retval None
*/
void IddMeasureDeInit(void)
{
  IddInitialized = RESET;
  return;
}

/**
* @brief  Get the Idd Measurement state
* @retval IddState_t  Idd Measurement state
*/
IddState_t Idd_GetState()
{
  return IddMeasureState;
}

/**
* @brief  Execute the Idd Measurement
* @param IddIndex  Idd measurement index
* @retval None
*/
void Idd_ExecuteAction(uint32_t IddIndex)
{
  if(IddTest[IddIndex].action != IDD_GETIDD_AFTER_RESET)
  {
    IddMeasureInit();

    /* Start measurement campaign */
    if(BSP_IDD_StartMeasurement(0) < 0)
    {
      Error_Handler();
    }

#if (IDD_SIGNATURE == 1)
    Idd_SaveSignature();
#endif

    IddMeasureState = IDD_STATE_MEASURE_ON_GOING;

    /* Unconfigure HW resources */
    SystemHardwareDeInit(IddIndex);

    /* if function pointer exists, execute corresponding low power action */
    if(IddTest[IddIndex].IddEnter != NULL)
    {
      IddTest[IddIndex].IddEnter();
    }
  }
}

/**
* @brief  Get the Idd Measurement value
* @retval pIddValue  Pointer to Idd Measurement value structure
*/
void Idd_GetValue(IddValue_t *pIddValue)
{
  uint32_t IddReadValue;

  IddMeasureInit();

  /* check if idd interrupt that occurred is measurement done or error */
  if(IddMeasureState == IDD_STATE_MEASURE_ERROR)
  {
    pIddValue->value[0] = 0;
    pIddValue->unit[0] = 0;
    pIddValue->value_na = 0;
  }
  else
  {
    pIddValue->error_code = 0;
    /* Get Idd value */
    IddReadValue = 0;
    if(BSP_IDD_GetValue(0, &IddReadValue) < 0)
    {
      Error_Handler();
    }

    /* Fill value in Nano amps */
    pIddValue->value_na = 10 * IddReadValue;
    /* Convert Idd value in order to display it on LCD glass */
    Idd_Convert(IddReadValue, pIddValue);
  }
  /* Power mode */
  pIddValue->power_mode = (IddAction_t )IddTestIndex;
}

/**
* @brief  Enter MCU in Run mode @24 Mhz.
* @retval None
*/
static void Idd_RunEnter(void)
{
  /* Suspend HAL tick irq */
  HAL_SuspendTick();

  /* System peripheral clock disable, rtc on */
  SystemPeripheralClockDisable(0);

  /* Create and start an external thread to execute while(1) */
  while1Aligned64();
}

/**
* @brief  while(1) aligned on 64 bits
* @param  argument: None.
* @retval None
*/
#if defined ( __ICCARM__ )
#pragma location="While1Section"
#pragma optimize=no_inline
void while1Aligned64(void)
#else
__attribute__((section(".While1Section")))
void while1Aligned64(void) __attribute__((noinline));
void while1Aligned64(void)
#endif
{
  /* while (1) Must be aligned 64 bit to fit with the flash read size*/
  while(1);
}

/**
* @brief  Enter MCU in Sleep mode.
* @retval None
*/
static void Idd_SleepEnter(void)
{
  /* Suspend HAL tick irq */
  HAL_SuspendTick();

  /* System peripheral clock disable, rtc on */
  SystemPeripheralClockDisable(0);

  /* Enable Flash power down mode during Sleep mode     */
  /* (uncomment this line if power consumption figures  */
  /*  must be measured with Flash still on in Low Power */
  /*  Sleep mode)                                       */
  __HAL_FLASH_SLEEP_POWERDOWN_ENABLE();

  /* Enable PWR clock enable */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Enter in Sleep mode, Main Regulator ON */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

/**
* @brief  Enter MCU in Low Power Run mode.
* @retval None
*/
static void Idd_LprEnter(void)
{
  /* go to 2MHz PLL Off */
  SystemLowClock_Config();

  /* Suspend HAL tick irq */
  HAL_SuspendTick();

  /* System peripheral clock disable, rtc on */
  SystemPeripheralClockDisable(0);

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_PWREx_EnableLowPowerRunMode();

  while1Aligned64();
}

/**
* @brief  Enter MCU in Low Power Sleep mode.
* @retval None
*/
static void Idd_LprSleepEnter(void)
{
  /* go to 2MHz PLL Off */
  SystemLowClock_Config();

  /* Suspend HAL tick irq */
  HAL_SuspendTick();

  /* System peripheral clock disable, rtc on */
  SystemPeripheralClockDisable(0);

  /* Enable Flash power down mode during Sleep mode     */
  /* (uncomment this line if power consumption figures  */
  /*  must be measured with Flash still on in Low Power */
  /*  Sleep mode)                                       */
  __HAL_FLASH_SLEEP_POWERDOWN_ENABLE();

  /* Enable PWR clock enable */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Enter in Sleep mode, Low Power Regulator ON */
  HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

/**
* @brief  Enter MCU in Stop mode (STOP2).
* @retval None
*/
static void Idd_StopEnter(void)
{
  /* Ensure that MSI is wake-up system clock */
  HAL_RCCEx_WakeUpStopCLKConfig(RCC_STOP_WAKEUPCLOCK_MSI);

  /* Enable PWR clock enable */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Suspend HAL tick interrupt */
  HAL_SuspendTick();

  /* System peripheral clock disable, rtc on */
  SystemPeripheralClockDisable(0);

  /* Enable PWR clock enable */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Enter in Low power Stop 2 */
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
}

/**
* @brief  Enter MCU in Standby mode.
* @retval None
*/
static void Idd_StandbyEnter(void)
{
  /* Configure wake up */
#if (RTC_WAKEUP == 1)
  Idd_SetRtcWakeup();
#else
  Idd_WakeUpPinConfig();
#endif

  /* System peripheral clock disable, rtc on */
  SystemPeripheralClockDisable(0);

#if (OPTIM_STANDBY_SHUTDOWN == 1)
  /* Configure the relevant I/O in pull-up or pull-down mode */
  GPIOoptimStandbyShutdown();
#endif

  /* Enable PWR clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Enter STANDBY mode */
  HAL_PWR_EnterSTANDBYMode();
}

#if (RTC_WAKEUP == 1)
static void Idd_SetRtcWakeup()
{
  RtcHandle.Instance = RTC;                  /**< Define instance */
  /**
  * Set the Asynchronous prescaler
  */
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24 /* RTC_HOURFORMAT_12 */; /**< need to be initialized to not corrupt the RTC_CR register */
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH; /**< need to be initialized to not corrupt the RTC_CR register */
  RtcHandle.Init.OutPutType  = RTC_OUTPUT_TYPE_OPENDRAIN; /**< need to be initialized to not corrupt the RTC_CR register */
  HAL_RTC_Init(&RtcHandle);

  /**
  * Bypass the shadow register
  */
  HAL_RTCEx_EnableBypassShadow(&RtcHandle);

  /* Disable Write Protection */
  __HAL_RTC_WRITEPROTECTION_DISABLE(&RtcHandle) ;

  /* Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE))
  Wakeup Time = Wakeup Time Base * WakeUpCounter
  = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE)) * WakeUpCounter
  ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

  To configure the wake up timer to TIMERSERVER_TICK_VALUE
  Wakeup Time Base = 16 /(~32.000KHz) = 0.5 ms
  4000 = 0.5 ms  * WakeUpCounter
  Wakeup Time =  4000 * 2 */
  HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, 4000 * 2, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0);
}

#else /* (RTC_WAKEUP == 1) */

/**
* @brief  Configure the Wake up pin to exit power modes.
* @retval None
*/
static void Idd_WakeUpPinConfig(void)
{
  /* Enable PWR clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Idd component measurement Interrupt pin is on PC13 which can be
  configured as wake up pin source. Use it to wake up main MCU */
  /* Disable all used wakeup sources: WKUP pin */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);

  /* Clear wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);

  /* Enable wakeup pin WKUP2 */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_HIGH);
}
#endif /* (RTC_WAKEUP == 1) */

/**
* @brief  Convert value to display correct amper unit.
* @retval None
*/
static void Idd_Convert(uint32_t Value, IddValue_t * idd)
{
  float TempIddDisplay = 0;
  idd->value[0]=0;
  idd->unit[0]=0;

  TempIddDisplay = (float) Value;

  if (TempIddDisplay < 1000){  /* Value in nano amps */
    sprintf(idd->value, "%.0f", TempIddDisplay);
    sprintf(idd->unit, "nA");
  }else{  /* Value in micro amps */
    TempIddDisplay = TempIddDisplay / 1000;
    if (TempIddDisplay < 10){
      sprintf(idd->value, "%.2f", TempIddDisplay);
      sprintf(idd->unit, "uA");
    }else if (TempIddDisplay < 100){
      sprintf(idd->value, "%.1f", TempIddDisplay);
      sprintf(idd->unit, "uA");
    }else if (TempIddDisplay < 1000){
      sprintf(idd->value, "%.0f", TempIddDisplay);
      sprintf(idd->unit, "uA");
    }else{ /* Value in milli Amp */
      TempIddDisplay = TempIddDisplay/1000;
      if (TempIddDisplay < 10){
        sprintf(idd->value, "%.2f", TempIddDisplay);
        sprintf(idd->unit, "mA");
      }else if (TempIddDisplay < 100){
        sprintf(idd->value, "%.1f", TempIddDisplay);
        sprintf(idd->unit, "mA");
      }else if (TempIddDisplay < 1000){
        sprintf(idd->value, "%.0f", TempIddDisplay);
        sprintf(idd->unit, "mA");
      }
    }
  }
}

/**
* @brief  System Clock Configuration in Low Power run mode
*         The system Clock is configured as follow :
*            System Clock source            = MSI
*            SYSCLK(Hz)                     = 2000000
*            HCLK(Hz)                       = 2000000
*            AHB Prescaler                  = 1
*            APB1 Prescaler                 = 1
*            APB2 Prescaler                 = 1
*            MSI Frequency(Hz)              = 2000000
*            Flash Latency(WS)              = 0
* @retval None
*/
void SystemLowClock_Config(void)
{
  /* oscillator and clocks configs */
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  uint32_t flatency = 0;

  /* Retrieve clock parameters */
  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &flatency );

  /* switch SYSCLK to MSI in order to disable PLL */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* turn off PLL and set MSI to 2Mhz */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;   /* 2 Mhz */
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Adapt voltage scaling to low power run */
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  if(HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Enter Low Power Run mode */
  HAL_PWREx_EnableLowPowerRunMode();

  /* Disable Power Control clock once PWR registers are updated */
  __HAL_RCC_PWR_CLK_DISABLE();

}


/**
* @brief  System Low Power Configuration
* @param  Mode Purpose of HW deinitialization (IDD mode)
* @retval None
*/
void SystemHardwareDeInit(uint32_t mode)
{
  GPIO_InitTypeDef GPIO_InitStructure;

#if(TRACE != 0)
  /* Initialize the VCOM interface */
  vcom_deInit();
#endif

  /* Deinit software part */
  if (BSP_IDD_DeInit(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  if (BSP_LCD_DisplayOff(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  if (BSP_LCD_DeInit(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  /* Enable PWR  peripheral Clock */
  __HAL_RCC_PWR_CLK_ENABLE();

#if (DEBUG_ON == 1)
  HAL_DBGMCU_EnableDBGSleepMode();
  HAL_DBGMCU_EnableDBGStopMode();
#endif

  HAL_PWREx_EnableVddIO2();

  HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle);
  HAL_RTCEx_DeactivateCalibrationOutPut(&RtcHandle);


  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /* Set all GPIO in analog state to reduce power consumption                 */
  /* Note: Debug using ST-Link is not possible during the execution of this   */
  /*       example because communication between ST-link and the device       */
  /*       under test is done through UART. All GPIO pins are disabled (set   */
  /*       to analog input mode) including  UART I/O pins.           */
  GPIO_InitStructure.Pin = GPIO_PIN_All;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;

#if (DEBUG_ON == 1)
  /* keep swdio swclk */
  GPIO_InitStructure.Pin = (GPIO_InitStructure.Pin & ~(GPIO_PIN_13 | GPIO_PIN_14));
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.Pin = GPIO_PIN_All;
#else
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

#if (DEBUG_ON == 1)
  /* keep swo */
  GPIO_InitStructure.Pin = GPIO_PIN_All & ~(GPIO_PIN_3);
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.Pin = GPIO_PIN_All;
#else
  GPIO_InitStructure.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif


  /* Keep wakeup pin except for standby */
  if(IddTestIndex != IDD_STANDBY)
  {
    GPIO_InitStructure.Pin &= ~GPIO_PIN_13; /* PC13 */
  }
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();

  HAL_PWREx_DisableVddIO2();
  HAL_PWREx_DisableVddUSB();
  HAL_PWREx_DisableSRAM2ContentRetention();
  HAL_PWREx_DisableBatteryCharging();
  HAL_PWR_DisableBkUpAccess();
}

/**
* @brief  Enable System peripheral clocks
* @retval None
*/
void SystemPeripheralClockEnable(void)
{
  /* Enable minimum System peripheral clocks */
  /* others will be enabled via BSP layers   */
}

/**
* @brief  Disable System peripheral clocks
* @param  rtc_on : 1 = keep rtc;  0 = don't keep rtc
* @retval None
*/
void SystemPeripheralClockDisable(uint32_t rtc_on)
{
  RCC_OscInitTypeDef oscinitstruct = {0};

  /* All Clock off instead of in all modes */
  if (rtc_on == 1)
  {
    oscinitstruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSI48;
  }
  else
  {
    oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSI48;
    oscinitstruct.LSEState   = RCC_LSE_OFF;
  }

  oscinitstruct.LSIState     = RCC_LSI_OFF;
  oscinitstruct.HSEState     = RCC_HSE_OFF;
  oscinitstruct.HSIState     = RCC_HSI_OFF;
  oscinitstruct.HSI48State   = RCC_HSI48_OFF;
  oscinitstruct.PLL.PLLState = RCC_PLL_NONE;

  if(HAL_RCC_OscConfig(&oscinitstruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Disable remaining clocks */
  __HAL_RCC_FLASH_CLK_DISABLE();
  __HAL_RCC_SYSCFG_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_I2C1_CLK_DISABLE();
  __HAL_RCC_SPI1_CLK_DISABLE();
  __HAL_RCC_DFSDM1_CLK_DISABLE();
  __HAL_RCC_DMA1_CLK_DISABLE();
  __HAL_RCC_DMA2_CLK_DISABLE();
  __HAL_RCC_TIM6_CLK_DISABLE();
  __HAL_RCC_FMC_CLK_DISABLE();
  __HAL_RCC_PWR_CLK_DISABLE();
  __HAL_RCC_SAI1_CLK_DISABLE();
  __HAL_RCC_DFSDM1_CLK_DISABLE();
  __HAL_RCC_OSPI1_CLK_DISABLE();
  __HAL_RCC_SDMMC1_CLK_DISABLE();
  __HAL_RCC_LPUART1_CLK_DISABLE();
#if (RTC_WAKEUP != 1)
  __HAL_RCC_RTC_DISABLE();
  __HAL_RCC_RTCAPB_CLK_DISABLE();
#endif
  __HAL_RCC_CRC_CLK_DISABLE();


  RCC->AHB1SMENR  = 0x0;
  RCC->AHB2SMENR  = 0x0;
  RCC->AHB3SMENR  = 0x0;
  RCC->APB1SMENR1 = 0x0;
  RCC->APB1SMENR2 = 0x0;
  RCC->APB2SMENR  = 0x0;
}

/**
* @brief  Configure the relevant I/O in pull-up or pull-down mode
* @param  None
* @retval None
*/
#if (OPTIM_STANDBY_SHUTDOWN == 1)
void GPIOoptimStandbyShutdown(void)
{
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnablePullUpPullDownConfig();
  /***** Best config for all GPIO according to bestConfigStandbyShutdown[] ******/
  for(uint32_t i = 0; i<NB_BEST_CONFIG_STANDBY_SHUTDOWN; i++ )
  {
    if(bestConfigStandbyShutdown[i].gpioMode == PD) {
      HAL_PWREx_EnableGPIOPullDown(bestConfigStandbyShutdown[i].port, bestConfigStandbyShutdown[i].pin);
    }

    if(bestConfigStandbyShutdown[i].gpioMode == PU) {
      HAL_PWREx_EnableGPIOPullUp(bestConfigStandbyShutdown[i].port, bestConfigStandbyShutdown[i].pin);
    }
  }
  __HAL_RCC_PWR_CLK_DISABLE();
}
#endif


/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

#undef __IDDMEASURE_C

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
