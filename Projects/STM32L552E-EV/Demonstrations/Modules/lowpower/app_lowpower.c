/**
  ******************************************************************************
  * @file    app_lowpower.c
  * @author  MCD Application Team
  * @brief   Low power application implementation.
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
#ifndef __APP_LOWPOWER_C
#define __APP_LOWPOWER_C

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "app_lowpower.h"
#include "main.h"
#include "stm32l5xx_hal.h"
#include "stm32l552e_eval.h"
#include "stm32l552e_eval_lcd.h"
#include "stm32l552e_eval_io.h"
#include "stm32l552e_eval_idd.h"
#include "stm32l552e_eval_sd.h"
#include "stm32l552e_eval_bus.h"
#include "utils.h"
#include "k_config.h"
#include "k_module.h"
#include "k_menu.h"
#include "k_window.h"
#include "k_storage.h"

//#define DEBUG

/* Private typedef ----------------------------------------------------------*/
#define IDD_VALUE_STRING_SIZE 4
#define IDD_UNIT_STRING_SIZE 2

typedef struct
{
  char value[IDD_VALUE_STRING_SIZE + 1];  /* 3 significant digit only */
  char unit[IDD_UNIT_STRING_SIZE + 1];    /* 2 letters */
  uint32_t value_na;                      /* value in nano amps */
  uint8_t error_code;                     /* 0 means no error else see mfx documentation
                                             In case of error please retry */
} Iddvalue_t;


/* Private constants ----------------------------------------------------------*/
/* Wake up source */
#define WAKEUP_GPIO 0UL
#define WAKEUP_RTC  1UL
/* Private function prototypes -----------------------------------------------*/
KMODULE_RETURN _LowPowerDemoExec(void);
KMODULE_RETURN _LowPowerDemoConfig(void);
KMODULE_RETURN _LowPowerDemoUnConfig(void);

void LowPowerIddMeasureInit(uint32_t InterruptMode);
uint32_t LowPowerIddGetValue(void);
static void Idd_Convert(uint32_t Value, Iddvalue_t * idd);

void LowPowerDemo(void);
void LowPowerUserAction(uint8_t sel);
void LowPowerStandbyWakeupPin(void);
void LowPowerStandbyRTCAlarm(void);
void LowPowerStopEXTI(void);
void LowPowerStopRTCAlarm(void);
void LowPowerSleepEXTI(void);
void LowPowerSleepRTCAlarm(void);
void LowPowerRunSmpsBypass(void);
void LowPowerRunSmpsHighPower(void);
void LowPowerRunSmpsLowPower(void);

static void LowPowerHandleAlarm(void);

void SystemPeripheralClockDisable(uint32_t WakeupSource);
void SystemPeripheralClockEnable(uint32_t WakeupSource);
void GPIOoptimStandby(uint32_t WakeupSource);
void while1Aligned64(void);
void SystemClock_24MHz(void);

/* Private Variable ----------------------------------------------------------*/
/* standby mode menu */
const tMenuItem StandbyModeMenuItems[] =
{
    {"Wakeup pin"    , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerStandbyWakeupPin, LowPowerUserAction, NULL, NULL, NULL  },
    {"RTC Alarm"     , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerStandbyRTCAlarm,  LowPowerUserAction, NULL, NULL, NULL  },
    {"Return"        ,  0,  0,    SEL_EXIT, MODULE_LOWPOWER, NULL, NULL, NULL, NULL, NULL  }
};

const tMenu StandbyModeMenu = {
  "Standby mode", StandbyModeMenuItems, countof(StandbyModeMenuItems), TYPE_TEXT, 1, 1
};

/* sleep mode menu */
const tMenuItem SleepModeMenuItems[] =
{
    {"EXTI pin"     , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerSleepEXTI,     LowPowerUserAction, NULL, NULL, NULL },
    {"RTC Alarm"    , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerSleepRTCAlarm, LowPowerUserAction, NULL, NULL, NULL  },
    {"Return"       ,  0,  0,    SEL_EXIT, MODULE_LOWPOWER, NULL, NULL, NULL, NULL }
};

const tMenu SleepModeMenu = {
  "Sleep mode", SleepModeMenuItems, countof(SleepModeMenuItems), TYPE_TEXT, 1, 1
};

/* RunSmpsHighPower mode menu */
const tMenuItem RunSmpsModeItems[] =
{
    {"Bypass 110MHz"        , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerRunSmpsBypass,     LowPowerUserAction, NULL, NULL, NULL },
    {"High Power 110MHz"    , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerRunSmpsHighPower,  LowPowerUserAction, NULL, NULL, NULL },
    {"Low Power 24MHz"      , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerRunSmpsLowPower,   LowPowerUserAction, NULL, NULL, NULL },
    {"Return"               ,  0,  0,    SEL_EXIT, MODULE_LOWPOWER, NULL, NULL, NULL, NULL }
};

const tMenu RunSmpsModeMenu = {
  "SMPS mode", RunSmpsModeItems, countof(RunSmpsModeItems), TYPE_TEXT, 1, 1
};

/* stop mode menu */
const tMenuItem StopModeMenuItems[] =
{
    {"EXTI pin"     , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerStopEXTI,     LowPowerUserAction, NULL, NULL, NULL },
    {"RTC Alarm"    , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerStopRTCAlarm, LowPowerUserAction, NULL, NULL, NULL  },
    {"Return"       ,  0,  0,    SEL_EXIT, MODULE_LOWPOWER, NULL, NULL, NULL, NULL }
};

const tMenu StopModeMenu = {
  "Stop mode", StopModeMenuItems, countof(StopModeMenuItems), TYPE_TEXT, 1, 1
};


/* Main menu */
const tMenuItem LowPowerMenuItems[] =
{
    {"RUN SMPS mode"             , 14, 30, SEL_SUBMENU, MODULE_LOWPOWER, NULL, NULL, (const tMenu*)&RunSmpsModeMenu, NULL, NULL  },
    {"SLEEP mode"                , 14, 30, SEL_SUBMENU, MODULE_LOWPOWER, NULL, NULL, (const tMenu*)&SleepModeMenu, NULL, NULL  },
    {"STOP mode"                 , 14, 30, SEL_SUBMENU, MODULE_LOWPOWER, NULL, NULL, (const tMenu*)&StopModeMenu, NULL, NULL  },
    {"STANDBY mode"              , 14, 30, SEL_SUBMENU, MODULE_LOWPOWER, NULL, NULL, (const tMenu*)&StandbyModeMenu, NULL, NULL  },
    {"Return"                    ,  0,  0, SEL_EXIT, MODULE_NONE, NULL, NULL, NULL, NULL }
};

const tMenu LowpowerMenu = {
  "Low power", LowPowerMenuItems, countof(LowPowerMenuItems), TYPE_TEXT, 1, 1};

/* used to exit application */
static __IO uint8_t user_event=0;
static __IO uint8_t user_action=0;

/* Private typedef -----------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
const K_ModuleItem_Typedef ModuleLowPower =
{
  MODULE_LOWPOWER,
  _LowPowerDemoConfig,
  _LowPowerDemoExec,
  _LowPowerDemoUnConfig,
  NULL
};

/* RTC Handle variable */
extern RTC_HandleTypeDef RtcHandle;

/* Idd variable */
__IO uint32_t IddOnGoing;

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
  //while(1);
  while(IddOnGoing);
}



/**
  * @brief  Initialize the Idd Measurement application
  * @param  None.
  * @note   None.
  * @retval None.
  */
void LowPowerIddMeasureInit(uint32_t InterruptMode)
{
  if(BSP_IDD_Init(0) < 0)
  {
    Error_Handler();
  }

  if (InterruptMode != 0)
  {
    if(BSP_IDD_EnableIT(0) < 0)
    {
      Error_Handler();
    }
  }
  else
  {
    if(BSP_IDD_DisableIT(0) < 0)
    {
      Error_Handler();
    }
  }

  if(BSP_IDD_StartMeasurement(0) < 0)
  {
    Error_Handler();
  }
  return;
}

/**
  * @brief  Print Idd Measurement application
  * @param  None.
  * @note   None.
  * @retval None.
  */
uint32_t LowPowerIddGetValue(void)
{
  uint32_t IddValue;

  if(BSP_IDD_ClearIT(0) < 0)
  {
    Error_Handler();
  }

  if(BSP_IDD_GetValue(0, &IddValue) < 0)
  {
    Error_Handler();
  }

  return IddValue;
}

/**
  * @brief  DeInitialize the Idd Measurement application
  * @param  None.
  * @note   None.
  * @retval None.
  */
void LowPowerIddMeasureDeInit(void)
{
  if(BSP_IDD_DisableIT(0) < 0)
  {
    Error_Handler();
  }

  if(BSP_IDD_DeInit(0) < 0)
  {
    Error_Handler();
  }
  return;
}

/**
  * @brief  Convert value to display correct amper unit.
  * @param  None
  * @retval None
  */
static void Idd_Convert(uint32_t Value, Iddvalue_t * idd)
{
  float TempIddDisplay = 0;
  idd->value[0]=0;
  idd->unit[0]=0;

  TempIddDisplay = (float) Value * 10;

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
  * @brief  EXTI line detection callbacks.
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void BSP_IDD_Callback(uint32_t Instance)
{
  IddOnGoing = 0;
}

/**
  * @brief  Configure the Lowpower application
  * @param  None.
  * @note   run and display information about the lowpower feature.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _LowPowerDemoConfig(void)
{
  /*#### Disable all used wakeup sources ####*/
  HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);
  /* Disable all previous wake up interrupt */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);

  /*#### Clear all related wakeup flags ####*/
  /* Clear the Alarm interrupt pending bit */
  __HAL_RTC_ALARM_CLEAR_FLAG(&RtcHandle,RTC_FLAG_ALRBF);
  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2 | PWR_FLAG_SB);

    return KMODULE_OK;
}

/**
  * @brief  un-Configure the Lowpower application
  * @param  None.
  * @note   run and display information about the lowpower feature.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _LowPowerDemoUnConfig(void)
{
  return KMODULE_OK;
}

/**
  * @brief  Run the Lowpower application
  * @param  None.
  * @note   run and display information about the lowpower feature.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _LowPowerDemoExec(void)
{
  /* Prepare Execute the main MMI of lowpower application */
  kMenu_Execute(LowpowerMenu);
  return KMODULE_OK;
}

/**
  * @brief  Get User action
  * @param  sel   User selection (JOY_SEL,...)
  * @note   This example is the only way to get user information.
  * @retval None
  */
void LowPowerUserAction(uint8_t sel)
{
  if (user_action == 0)
  {
    user_action = 1;
    user_event = sel;
  }
}

/**
  * @brief  Run the Lowpower Standby mode Wakeup pin
  * @param  None.
  * @note   run and display information about the lowpower feature.
  * @retval None.
  */
void LowPowerStandbyWakeupPin(void)
{
  kWindow_Popup("STANDBY EXTI", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\n\nPlease wait...\n",   \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );
  HAL_Delay(1000);

  /* Int Measure through MFX */
  LowPowerIddMeasureInit(1);

  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
  HAL_Delay(10);

  /* Optimize GPIO consumption (analog) for standby mode */
  GPIOoptimStandby(WAKEUP_GPIO);

  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  /* Standby mode */
  HAL_PWR_EnterSTANDBYMode();

  /* Following code should never be reach as MCU reset when exitting from standby */
  while(1);
}

/**
  * @brief  Run the Lowpower Standby mode RTC Alarm
  * @param  None.
  * @note   run and display information about the lowpower feature.
  * @retval None.
  */
void LowPowerStandbyRTCAlarm(void)
{
  kWindow_Popup("STANDBY Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\nset delay time\n",                          \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* set the alarm */
  LowPowerHandleAlarm();

  kWindow_Popup("STANDBY Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,  \
                "\n\nPlease wait...\n",   \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );
  HAL_Delay(1000);

  /* Int Measure through MFX */
  LowPowerIddMeasureInit(0);

  /* Optimize GPIO consumption (analog) for standby mode */
  GPIOoptimStandby(WAKEUP_RTC);

  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  /* Enter StandBy mode */
  HAL_PWR_EnterSTANDBYMode();

  /* Following code should never be reach as MCU reset when exitting from standby */
  while(1);
}


/**
  * @brief  Enter in sleep mode and wake up by a MFX pin
  * @param  None
  * @note   This example enter in sleep mode.
  * @retval None
  */
void LowPowerSleepEXTI(void)
{
  uint32_t iddValue;
  kWindow_Popup("SLEEP EXTI", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\n\nPlease wait...\n",   \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );
  HAL_Delay(1000);

  /* Int Measure through MFX */
  LowPowerIddMeasureInit(1);

  /*************************************************************/
  /*  Disable all peripheral clock during SLEEP mode           */
  /*  and put GPIO in analog mode to optimize power consumtion */
  /*************************************************************/
  SystemPeripheralClockDisable(WAKEUP_GPIO);

  /* Suspend HAL tick irq */
  HAL_SuspendTick();

  /* Enable Flash power down mode during Sleep mode      */
  /* (comment this line if power consumption figures     */
  /*  must be measured with Flash still on in Sleep mode */
  __HAL_FLASH_SLEEP_POWERDOWN_ENABLE();

  /*********************/
  /* Enter Sleep Mode   */
  /*********************/
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  /* Resume HAL tick irq */
  HAL_ResumeTick();

  /********************************************************/
  /*  Restore peripheral clock and Gpio configuration     */
  /*  which have been disabled during SLEEP mode           */
  /********************************************************/
  SystemPeripheralClockEnable(WAKEUP_GPIO);

  /* Get Measure through MFX */
  iddValue = LowPowerIddGetValue();

  /* Deint Measure through MFX */
  LowPowerIddMeasureDeInit();

  /* Display info */
  LowPowerExitDisplay(SLEEP, iddValue);
}

/**
  * @brief  Enter in run smps bypass mode and wake up by a MFX pin
  * @param  None
  * @note   This example enter in run smps bypass mode  mode.
  * @retval None
  */
void LowPowerRunSmpsBypass(void)
{
  uint32_t iddValue;
  kWindow_Popup("RUN SMPS BYPASS", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\n\nPlease wait...\n",   \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );
  HAL_Delay(1000);

  /* Int Measure through MFX */
  LowPowerIddMeasureInit(1);

  /*************************************************************/
  /*  Disable all peripheral clock during RUN SMPS BYPASS mode           */
  /*  and put GPIO in analog mode to optimize power consumtion */
  /*************************************************************/
  SystemPeripheralClockDisable(WAKEUP_GPIO);

  /* Suspend HAL tick irq */
  HAL_SuspendTick();

  __HAL_RCC_PWR_CLK_ENABLE();
  if (HAL_PWREx_SMPS_SetMode(PWR_SMPS_BYPASS) != HAL_OK)
  {
    while (1);
  }
  /*********************/
  /* Enter Run SMS Bypass Mode   */
  /*********************/
  IddOnGoing = 1;
  while1Aligned64();

  __HAL_RCC_PWR_CLK_ENABLE();
  if (HAL_PWREx_SMPS_SetMode(PWR_SMPS_HIGH_POWER) != HAL_OK)
  {
    while (1);
  }

  /* Resume HAL tick irq */
  HAL_ResumeTick();

  /********************************************************/
  /*  Restore peripheral clock and Gpio configuration     */
  /*  which have been disabled during RUN SMPS BYPASS mode */
  /********************************************************/
  SystemPeripheralClockEnable(WAKEUP_GPIO);

  /* Get Measure through MFX */
  iddValue = LowPowerIddGetValue();

  /* Deint Measure through MFX */
  LowPowerIddMeasureDeInit();

  /* Display info */
  LowPowerExitDisplay(RUN_SMPS_BYPASS, iddValue);
}

/**
  * @brief  Enter in run smps low power mode and wake up by a MFX pin
  * @param  None
  * @note   This example enter in run smps low power mode.
  * @retval None
  */
void LowPowerRunSmpsLowPower(void)
{
  uint32_t iddValue;
  kWindow_Popup("RUN SMPS LOW POWER", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\n\nPlease wait...\n",   \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );
  HAL_Delay(1000);

  /* Int Measure through MFX */
  LowPowerIddMeasureInit(1);

  /*************************************************************/
  /*  Disable all peripheral clock during RUN SMPS LOW POWER mode */
  /*  and put GPIO in analog mode to optimize power consumtion */
  /*************************************************************/
  SystemPeripheralClockDisable(WAKEUP_GPIO);

  /* Suspend HAL tick irq */
  HAL_SuspendTick();

  /* Set CLock to 24 Mhz */
  __HAL_RCC_PWR_CLK_ENABLE();
  SystemClock_24MHz();

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2) != HAL_OK)
  {
    while(1);
  }

  /*********************/
  /* Enter Run SMS LowPower Mode   */
  /*********************/
  /* SMPS step down converter in high-power mode */
  if (HAL_PWREx_SMPS_SetMode(PWR_SMPS_LOW_POWER) != HAL_OK)
  {
    while (1);
  }

  IddOnGoing = 1;
  while1Aligned64();

  /* Resume HAL tick irq */
  HAL_ResumeTick();

  /* Set SMSP HP Mode */
  __HAL_RCC_PWR_CLK_ENABLE();
  if (HAL_PWREx_SMPS_SetMode(PWR_SMPS_HIGH_POWER) != HAL_OK)
  {
    while (1);
  }

  /* Voltage scale 0 */
  __HAL_RCC_PWR_CLK_ENABLE();
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    while(1);
  }

  /* Set clock to 110 MHz */
  SystemClock_Config();

  /********************************************************/
  /*  Restore peripheral clock and Gpio configuration     */
  /*  which have been disabled during RUN SMPS LOW POWER mode */
  /********************************************************/
  SystemPeripheralClockEnable(WAKEUP_GPIO);

  /* Get Measure through MFX */
  iddValue = LowPowerIddGetValue();

  /* Deint Measure through MFX */
  LowPowerIddMeasureDeInit();

  /* Display info */
  LowPowerExitDisplay(RUN_SMPS_LP, iddValue);
}

/**
  * @brief  Enter in run smps high power mode and wake up by a MFX pin
  * @param  None
  * @note   This example enter in run smps high power mode.
  * @retval None
  */
void LowPowerRunSmpsHighPower(void)
{
  uint32_t iddValue;
  kWindow_Popup("RUN SMPS HIGH POWER", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\n\nPlease wait...\n",   \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );
  HAL_Delay(1000);

  /* Int Measure through MFX */
  LowPowerIddMeasureInit(1);

  /*************************************************************/
  /*  Disable all peripheral clock during RUN SMPS HIGH POWER mode */
  /*  and put GPIO in analog mode to optimize power consumtion */
  /*************************************************************/
  SystemPeripheralClockDisable(WAKEUP_GPIO);

  /* Suspend HAL tick irq */
  HAL_SuspendTick();

  /* SMPS step down converter in high-power mode */
  __HAL_RCC_PWR_CLK_ENABLE();
  if (HAL_PWREx_SMPS_SetMode(PWR_SMPS_HIGH_POWER) != HAL_OK)
  {
    while (1);
  }
  /*********************/
  /* Enter Run SMS HighPower Mode   */
  /*********************/
  IddOnGoing = 1;
  while1Aligned64();

  /* Resume HAL tick irq */
  HAL_ResumeTick();

  /********************************************************/
  /*  Restore peripheral clock and Gpio configuration     */
  /*  which have been disabled during RUN SMPS HIGH POWER mode */
  /********************************************************/
  SystemPeripheralClockEnable(WAKEUP_GPIO);

  /* Get Measure through MFX */
  iddValue = LowPowerIddGetValue();

  /* Deint Measure through MFX */
  LowPowerIddMeasureDeInit();

  /* Display info */
  LowPowerExitDisplay(RUN_SMPS_HP, iddValue);
}

/**
  * @brief  Enter in sleep mode and exit by an alarm
  * @param  None
  * @note   This example enter in sleep mode.
  * @retval None
  */
void LowPowerSleepRTCAlarm(void)
{
  uint32_t iddValue;
  kWindow_Popup("SLEEP Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\nset delay time\n",                          \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* set the alarm */
  LowPowerHandleAlarm();

  kWindow_Popup("SLEEP Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,  \
                "\n\nPlease wait...\n",   \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );
  HAL_Delay(1000);

  /* Int Measure through MFX */
  LowPowerIddMeasureInit(0);

  /*************************************************************/
  /*  Disable all peripheral clock during SLEEP mode           */
  /*  and put GPIO in analog mode to optimize power consumtion */
  /*************************************************************/
  SystemPeripheralClockDisable(WAKEUP_RTC);

  /* Suspend HAL tick irq */
  HAL_SuspendTick();

  /* Enable Flash power down mode during Sleep mode       */
  /* (comment this line if power consumption figures      */
  /*  must be measured with Flash still on in Sleep mode) */
  __HAL_FLASH_SLEEP_POWERDOWN_ENABLE();

  /*******************/
  /* Enter Sleep Mode */
  /*******************/
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  /* Resume HAL tick irq */
  HAL_ResumeTick();

  /********************************************************/
  /*  Restore peripheral clock and Gpio configuration     */
  /*  which have been disabled during SLEEP mode          */
  /********************************************************/
  SystemPeripheralClockEnable(WAKEUP_RTC);

  /* Get Measure through MFX */
  iddValue = LowPowerIddGetValue();

  /* Deint Measure through MFX */
  LowPowerIddMeasureDeInit();

  /* Deactivate RTC alaram */
  HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);

  /* Clear the Alarm interrupt pending bit */
  __HAL_RTC_ALARM_CLEAR_FLAG(&RtcHandle,RTC_FLAG_ALRBF);

  /* Display info */
  LowPowerExitDisplay(SLEEP, iddValue);
}

/**
  * @brief  Enter in stop mode and wake up by a MFX pin
  * @param  None
  * @note   This example enter in stop mode.
  * @retval None
  */
void LowPowerStopEXTI(void)
{
  uint32_t iddValue;
  kWindow_Popup("STOP EXTI", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\n\nPlease wait...\n",   \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );
  HAL_Delay(1000);

  /* Int Measure through MFX */
  LowPowerIddMeasureInit(1);

  /*************************************************************/
  /*  Disable all peripheral clock during STOP mode            */
  /*  and put GPIO in analog mode to optimize power consumtion */
  /*************************************************************/
  SystemPeripheralClockDisable(WAKEUP_GPIO);

  /*******************/
  /* Enter Stop Mode */
  /*******************/
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);

  /* Restore the clock configuration */
  SystemClock_Config();

  /********************************************************/
  /*  Restore peripheral clock and Gpio configuration     */
  /*  which have been disabled during STOP mode           */
  /********************************************************/
  SystemPeripheralClockEnable(WAKEUP_GPIO);

  /* Get Measure through MFX */
  iddValue = LowPowerIddGetValue();

  /* Deint Measure through MFX */
  LowPowerIddMeasureDeInit();

  /* Display info */
  LowPowerExitDisplay(STOP, iddValue);
}

/**
  * @brief  Enter in stop mode and exit by an alarm
  * @param  None
  * @note   This example enter in stop mode.
  * @retval None
  */
void LowPowerStopRTCAlarm(void)
{
  uint32_t iddValue;
  kWindow_Popup("STOP Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\nset delay time\n",                          \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* set the alarm */
  LowPowerHandleAlarm();

  kWindow_Popup("STOP Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,  \
                "\n\nPlease wait...\n",   \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );
  HAL_Delay(1000);

  /* Int Measure through MFX */
  LowPowerIddMeasureInit(0);

  /*************************************************************/
  /*  Disable all peripheral clock during STOP mode            */
  /*  and put GPIO in analog mode to optimize power consumtion */
  /*************************************************************/
  SystemPeripheralClockDisable(WAKEUP_RTC);

  /*******************/
  /* Enter Stop Mode */
  /*******************/
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);

  /* Restore the clock configuration */
  SystemClock_Config();

  /********************************************************/
  /*  Restore peripheral clock and Gpio configuration     */
  /*  which have been disabled during STOP mode           */
  /********************************************************/
  SystemPeripheralClockEnable(WAKEUP_RTC);

  /* Get Measure through MFX */
  iddValue = LowPowerIddGetValue();

  /* Deint Measure through MFX */
  LowPowerIddMeasureDeInit();

  /* Deactivate RTC alaram */
  HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);

  /* Clear the Alarm interrupt pending bit */
  __HAL_RTC_ALARM_CLEAR_FLAG(&RtcHandle,RTC_FLAG_ALRBF);

  /* Display info */
  LowPowerExitDisplay(STOP, iddValue);
}

/**
  * @brief  Get user info to setup an alarm
  * @note   This function wait user info to setup the alarm.
  * @retval None
  */
static void LowPowerHandleAlarm(void)
{
  enum {
    HOURS,
    MINUTES,
    SECONDS,
    END
  };

  RTC_DateTypeDef currentdate = {0};
  RTC_TimeTypeDef time = {0};
  RTC_TimeTypeDef currenttime = {0};
  RTC_AlarmTypeDef Alarm = {0};
  sFONT *font;
  uint8_t temp[16];
  uint8_t exit = 0;
  uint8_t index = SECONDS;
  uint8_t position;
  uint8_t statpos;
  uint32_t pXSize;

  /* get current font */
  font = UTIL_LCD_GetFont();
  BSP_LCD_GetXSize(0, &pXSize);
  statpos = (pXSize >> 1) - (4 * font->Width);

  /* Default alarm value 5 seconds */
  time.Seconds = 5;
  /* Get the alarm time from user */
  do
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
    sprintf((char *)temp, "%.2d:%.2d:%.2d", time.Hours, time.Minutes, time.Seconds);
    UTIL_LCD_DisplayStringAt(statpos, 5 * font->Height, temp, LEFT_MODE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);

    switch(index)
    {
    case HOURS :
      position = statpos;
      sprintf((char *)temp, "%.2d", time.Hours);
      break;
    case MINUTES:
      position = statpos + (3 * font->Width);
      sprintf((char *)temp, "%.2d", time.Minutes);
      break;
    case SECONDS :
      position = statpos + (6 * font->Width);
      sprintf((char *)temp, "%.2d", time.Seconds);
      break;
    }
    UTIL_LCD_DisplayStringAt(position, 5 * font->Height, temp, LEFT_MODE);

    user_event = JOY_NONE;
    user_action = 0;
    while(user_event == JOY_NONE);
    switch(user_event)
    {
    case JOY_UP :
      if(index == HOURS)
      {
        if( time.Hours == 23 ) time.Hours = 0;
        else
          time.Hours++;
      }

      if(index == MINUTES)
      {
        if(time.Minutes == 59 ) time.Minutes= 0;
        else
          time.Minutes++;
      }
      if(index == SECONDS)
      {
        if(time.Seconds == 59 ) time.Seconds =0;
        else
          time.Seconds++;
        if (time.Seconds < 5 ) time.Seconds = 5; /* Mini value 5 secondes */
      }
      break;
    case JOY_DOWN :
      if(index == HOURS)
      {
        if (time.Hours == 0 ) time.Hours = 23;
        else time.Hours--;
      }
      if(index == MINUTES)
      {
        if(time.Minutes == 0) time.Minutes=59;
        else
          time.Minutes--;
      }
      if(index == SECONDS)
      {
        if(time.Seconds == 0) time.Seconds = 59;
        else
          time.Seconds--;
        if (time.Seconds < 5 ) time.Seconds = 5; /* Mini value 5 secondes */
      }
      break;
    case JOY_RIGHT :
      if(index != SECONDS ) index++;
      break;
    case JOY_LEFT :
      if(index != HOURS ) index--;
      break;
    case JOY_SEL :
      exit = 1;
      break;
    }
  } while(exit == 0);


  HAL_RTC_GetDate(&RtcHandle, &currentdate, RTC_FORMAT_BIN);
  HAL_Delay(100);
  HAL_RTC_GetTime(&RtcHandle, &currenttime,  RTC_FORMAT_BIN);
  time.SubSeconds = currenttime.SubSeconds;
  if((time.Seconds + currenttime.Seconds) > 60 )  time.Minutes++;
  Alarm.AlarmTime.Seconds = ((time.Seconds + currenttime.Seconds) % 60);

  if((time.Minutes + currenttime.Minutes) > 60 )  time.Hours++;
  Alarm.AlarmTime.Minutes = ((time.Minutes + currenttime.Minutes) % 60);

  Alarm.AlarmTime.Hours = ((time.Hours + currenttime.Hours) % 24);

  /* Set the alarm */
  Alarm.Alarm = RTC_ALARM_B;
  Alarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
  Alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  Alarm.AlarmMask = RTC_ALARMMASK_NONE;
  Alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  Alarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
  if(HAL_RTC_SetAlarm_IT(&RtcHandle, &Alarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    while(1);
  }
}

/**
  * @brief  Display message when exitting from Stop/Standby mode
  * @param  Mode   Mode which MCU exit from. Could be STANDBY or STOP
  * @retval None
  */
void LowPowerExitDisplay(uint32_t Mode, uint32_t iddValue)
{
  char msg[120] = "Exit from\n";
  Iddvalue_t idd;

  if(Mode == RUN_SMPS_BYPASS)
  {
    strcat(msg, "RUN_SMPS_BYPASS\n");
  }
  else if(Mode == RUN_SMPS_HP)
  {
    strcat(msg, "RUN_SMPS_HP\n");
  }
  else if(Mode == RUN_SMPS_LP)
  {
    strcat(msg, "RUN_SMPS_LP\n");
  }
  else if(Mode == STANDBY)
  {
    strcat(msg, "STANDBY\n");
  }
  else if(Mode == SLEEP)
  {
    strcat(msg, "SLEEP\n");
  }
  else
  {
    strcat(msg, "STOP\n");
  }

  /* Convert Idd value in order to display it on LCD glass */
  Idd_Convert(iddValue, &idd);

  sprintf(msg + strlen(msg), "\nMeasured value\n%s %s\n", idd.value, idd.unit);
  sprintf(msg + strlen(msg), "\nPress JOY sel Exit\n");

  kWindow_Popup("Low Power", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                msg, UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* Restore Joystick in interrupt mode */
  BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);
  /* Wait action on joystick */
  while(BSP_JOY_GetState(JOY1) == JOY_NONE);
  while(BSP_JOY_GetState(JOY1) != JOY_NONE);
}

/**
  * @brief  Deactivate RTC alarm
  * @param  hrtc RTChandle
  * @retval None
  */
void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
  HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);
}


/**
  * @brief  Disable System peripheral clocks, and optimse GPIO consumption
  * @param  WakeupSource source for wakeup. Could be WAKEUP_RTC or WAKEUP_GPIO
  * @retval None
  */
#define LCD_BACKLIGHT_GPIO_PORT           GPIOA
#define LCD_BACKLIGHT_GPIO_PIN            GPIO_PIN_5


void SystemPeripheralClockDisable(uint32_t WakeupSource)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (WakeupSource == WAKEUP_GPIO) /* MFX */
  {
    RTC_UnConfig();
  }

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

  /* but put Backlight as default state */
  HAL_GPIO_DeInit(LCD_BACKLIGHT_GPIO_PORT, LCD_BACKLIGHT_GPIO_PIN);

  kStorage_DeInit();

  if (BSP_SD_DeInit(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  if (BSP_LED_DeInit(LED4) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  if (BSP_LED_DeInit(LED5) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  if (BSP_JOY_DeInit(JOY1, JOY_ALL) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  if (BSP_IO_DeInit(JOY1) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  while (BSP_I2C1_DeInit() == BSP_ERROR_NONE)
  {
    ;
  }

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pins : PE2 PE3 PE4 PE5
                           PE6 PE7 PE8 PE9
                           PE10 PE11 PE12 PE13
                           PE14 PE15 PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC13 PC14 PC15 PC0
                           PC1 PC2 PC3 PC6
                           PC7 PC8 PC9 PC10
                           PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0
                          |GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PF0 PF1 PF2 PF3
                           PF4 PF5 PF6 PF7
                           PF8 PF9 PF10 PF11
                           PF12 PF13 PF14 PF15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PH0 PH1 PH3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins :  PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 PA7
                           PA8 PA9 PA10 PA11
                           PA12 PA13 PA14 PA15 */
  if (WakeupSource == WAKEUP_GPIO) /* MFX */
  {
   GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
#ifdef DEBUG
                          |GPIO_PIN_12|GPIO_PIN_13;
#else
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
#endif
  }
  else
  {
     GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                            |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                            |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
#ifdef DEBUG
                          |GPIO_PIN_12|GPIO_PIN_13;
#else
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
#endif
  }

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB13 PB14 PB15
                           PB4 PB5 PB6 PB7
                           PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PG0 PG1 PG2 PG3
                           PG4 PG5 PG6 PG7
                           PG8 PG9 PG10 PG12
                           PG13 PG14 PG15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD11
                           PD12 PD13 PD14 PD15
                           PD0 PD1 PD2 PD3
                           PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/**
  * @brief  Restore System peripheral clocks and GPIO configuration
  * @param  wakeupsource source for wakeup. Could be WAKEUP_RTC or WAKEUP_GPIO
  * @retval None
  */



void SystemPeripheralClockEnable(uint32_t WakeupSource)
{
  if (WakeupSource == WAKEUP_GPIO) /* MFX */
  {
    RTC_Config();
  }

  if (BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Set the display on */
  if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Reinit  I2C to communicate w/ mfx */
  if(BSP_IDD_Init(0) < 0)
  {
    Error_Handler();
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
}

/**
  * @brief  Disable System peripheral clocks
  * @param  WakeupSource source for wakeup. Could be WAKEUP_RTC or WAKEUP_GPIO
  * @retval None
  */
void GPIOoptimStandby(uint32_t WakeupSource)
{
  GPIO_InitTypeDef GPIO_InitStructure;


  /* Enable ultra low power mode. */
  HAL_PWREx_EnableUltraLowPowerMode();

  /* Disable the Power Voltage Detector (PVD) */
  HAL_PWR_DisablePVD();

  /* Disable dead battery behavior */
  HAL_PWREx_DisableUCPDDeadBattery();

  if (WakeupSource == WAKEUP_GPIO)
  {
    RTC_UnConfig();
  }

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* Set all GPIO in analog state to reduce power consumption */
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;

  GPIO_InitStructure.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* Disable remaining clocks */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 24000000
  *            HCLK(Hz)                       = 24000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 24000000
  *            PLL_M                          =
  *            PLL_N                          =
  *            PLL_R                          =
  *            PLL_P                          =
  *            PLL_Q                          =
  *            Flash Latency(WS)              =
  * @param  None
  * @retval None
  */

void SystemClock_24MHz(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct,FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* MSI is enabled after System reset, update MSI to 24Mhz (RCC_MSIRANGE_9) */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

#undef __APP_LOWPOWER_C
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
