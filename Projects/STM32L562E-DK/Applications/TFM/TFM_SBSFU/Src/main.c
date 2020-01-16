/**
  ******************************************************************************
  * @file    Templates/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body
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
#include "uart_stdout.h"
#include "tfm_low_level_security.h"
#include "mpu_armv8m_drv.h"
#include "region_defs.h"
/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */
void boot_clear_bl2_ram_area(void)
{

}
/**
  * @brief This function configures the source of the time base:
  *        The time source is configured to have 1ms time base with a dedicated
  *        Tick interrupt priority.
  * @note This function overwrites the one declared as __weak in HAL.
  *       In this implementation, nothing is done.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  /* load 1Hz for timeout 1 second */
  uint32_t ticks = SystemCoreClock ;
  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_ENABLE_Msk;
  return HAL_OK;
}
/**
  * @brief Provide a tick value in millisecond.
  * @note This function overwrites the one declared as __weak in HAL.
  *       In this implementation, time is counted without using SysTick timer interrupts.
  * @retval tick value
  */
uint32_t HAL_GetTick(void)
{
  static uint32_t m_uTick = 0U;
  static uint32_t t1 = 0U, tdelta = 0U;
  uint32_t t2;

  t2 =  SysTick->VAL;

  if (t2 <= t1)
  {
    tdelta += t1 - t2;
  }
  else
  {
    tdelta += t1 + SysTick->LOAD - t2;
  }

  if (tdelta > (SystemCoreClock / (1000U)))
  {
    tdelta = 0U;
    m_uTick ++;
  }

  t1 = t2;
  return m_uTick;
}
/** @addtogroup Templates
  * @{
  */
/* exported variables ---------------------------------------------------------*/
RTC_HandleTypeDef RTCHandle;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/
static void mpc_init_cfg(void);
static void sau_and_idau_cfg(void);
static void mpu_init_cfg(void);
static void lock_bl2_shared_area(void);
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*  unsecure bottom of SRAM1 for error_handler */
  mpc_init_cfg();

  /* Set MPU to forbidd execution outside of not muteable code  */
  /* initialialize not secure MPU to forbidd execution on Flash /SRAM */
  mpu_init_cfg();

  /* Enable SAU to gain access to flash area non secure for write/read */
  sau_and_idau_cfg();


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

  /* enable SRAM2 retention low power mode */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableSRAM2ContentRetention();
 
  /*  */
  __HAL_RCC_RTC_DISABLE();
  /* Initialise the RTC */
  RTCHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
  RTCHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
  RTCHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
  RTCHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
  RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RTCHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
  RTCHandle.Instance            = RTC;
  RTCHandle.State = HAL_RTC_STATE_RESET;
  if (HAL_RTC_Init(&RTCHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure the System clock to have a frequency of 110 MHz */
  SystemClock_Config();


  /* Add your application code here
     */
  /* lock top of SRAM2 in secure */
  lock_bl2_shared_area();
  /*  Init for log */
#ifdef TFM_DEV_MODE
  stdio_init();
#endif
  /*  Check static protection */
  TFM_LL_SECU_CheckStaticProtections();
  /* Apply Run time Protection */
  TFM_LL_SECU_ApplyRunTimeProtections();
  /* Middleware function no return */
  (void)bl2_main();
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
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* To avoid undershoot due to maximum frequency, select PLL as system clock source */
  /* with AHB prescaler divider 2 as first step */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* AHB prescaler divider at 1 as second step */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}
/**
  * @brief  Memory Config Init
  * @param  None
  * @retval None
  */
static void  mpc_init_cfg(void)
{
#ifdef TFM_ERROR_HANDLER_NON_SECURE
  /*  unsecure only one block in SRAM1 */
  GTZC_MPCBB1_S->VCTR[0] = 0xfffffffe;
#endif
}

/**
  * @brief  Sau idau init
  * @param  None
  * @retval None
  */
static void sau_and_idau_cfg(void)
{
  /* Disable SAU */
  TZ_SAU_Disable();
  /* Configures SAU regions non-secure to gain access to SRAM1 non secure   */
#ifdef TFM_ERROR_HANDLER_NON_SECURE
  SAU->RNR  = 0;
  SAU->RBAR = (SRAM1_BASE_NS & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((SRAM1_BASE_NS + 0xff) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
#endif
  /* Configure RSS table */
  SAU->RNR  = 1;
  SAU->RBAR = (0x0BF90000 & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((0x0BF90000 + 0xffff) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
#ifdef TFM_ERROR_HANDLER_NON_SECURE
  /* Allow non secure access to Flash non Secure peripheral for regression */
  SAU->RNR  = 2;
  SAU->RBAR = ((uint32_t)FLASH_NS & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = (((uint32_t)FLASH_NS + 0xffff) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
#endif
  /* Allow non secure Flash base access */
  SAU->RNR  = 3;
  SAU->RBAR = ((uint32_t)FLASH_BASE_NS & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = (((uint32_t)FLASH_BASE_NS + FLASH_AREA_3_OFFSET + FLASH_AREA_3_SIZE) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
  /* Force memory writes before continuing */
  __DSB();
  /* Flush and refill pipeline with updated permissions */
  __ISB();
  /* Enable SAU */
  TZ_SAU_Enable();
}
/**
  * @brief  mpu init
  * @param  None
  * @retval None
  */
static void mpu_init_cfg(void)
{
#ifdef TFM_BOOT_MPU_PROTECTION

  struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };
  struct mpu_armv8m_dev_t dev_mpu_ns = { MPU_BASE_NS};
  struct mpu_armv8m_region_cfg_t region_cfg;
  /* Secure MPU */
  /* background region is enabled , secure execution on unsecure flash is not possible*/
  /* but non secure execution on unsecure flash is possible , non secure mpu is used to protect execution */
  /* since SAU is enabled later to gain access to non secure flash */
  /* Forbid execuction outside of flash write protected area  */
  /* descriptor 0 is set execute readonly before jumping in Secure application */
  region_cfg.region_nr = 0;
  region_cfg.region_base = FLASH_BASE_S + FLASH_AREA_0_OFFSET ;
  region_cfg.region_limit = FLASH_BASE_S + FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  region_cfg.region_nr = 1;
  region_cfg.region_base = FLASH_BASE_S + FLASH_AREA_PERSO_OFFSET + FLASH_AREA_BL2_SIZE;
  region_cfg.region_limit = FLASH_BASE_S + FLASH_AREA_0_OFFSET -1;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* Forbid execuction on full SRAM area */
  region_cfg.region_nr = 2;
  region_cfg.region_base = SRAM1_BASE_S ;
  region_cfg.region_limit = SRAM1_BASE_S + TOTAL_RAM_SIZE;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* forbid secure peripheral execution */
  region_cfg.region_nr = 3;

  region_cfg.region_base = PERIPH_BASE_S;
  region_cfg.region_limit = PERIPH_BASE_S + 0xFFFFFFF;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DEVICE_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_MAIR_ATTR_DEVICE_IDX;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* enable secure MPU */
  mpu_armv8m_enable(&dev_mpu_s, PRIVILEGED_DEFAULT_ENABLE,
                    HARDFAULT_NMI_ENABLE);
  /* forbid execution on non secure FLASH /RAM in case of jump in non secure */
  /* Non Secure MPU  background all access in priviligied */
  /* reduced execution to all flash during control */
  region_cfg.region_nr = 0;
  region_cfg.region_base = FLASH_BASE_NS + FLASH_AREA_1_OFFSET;
  region_cfg.region_limit = FLASH_BASE_NS + FLASH_AREA_1_OFFSET + FLASH_AREA_1_SIZE;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_ns, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  region_cfg.region_nr = 1;
  region_cfg.region_base = FLASH_BASE_NS + FLASH_AREA_2_OFFSET;
  region_cfg.region_limit = FLASH_BASE_NS + FLASH_AREA_3_OFFSET + FLASH_AREA_3_SIZE;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_ns, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* Forbid execuction on full SRAM area */
  region_cfg.region_nr = 2;
#ifdef TFM_ERROR_HANDLER_NON_SECURE
   region_cfg.region_base = SRAM1_BASE_NS + (~MPU_RBAR_BASE_Msk)+1;
#else
  region_cfg.region_base = SRAM1_BASE_NS ;
#endif
  region_cfg.region_limit = SRAM1_BASE_NS + TOTAL_RAM_SIZE;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
  if (mpu_armv8m_region_enable(&dev_mpu_ns, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* enable non secure MPU */
  mpu_armv8m_enable(&dev_mpu_ns, PRIVILEGED_DEFAULT_ENABLE,
                    HARDFAULT_NMI_ENABLE);
#endif /* TFM_BOOT_MPU_PROTECTION */
}


/**
  * @brief  lock bl2 shared sram in secure
  * @param  None
  * @retval None
  */
static void lock_bl2_shared_area(void)
{
  MPCBB_ConfigTypeDef MPCBB_desc;
  /* assumption shared area in SRAM2 in the last 8 Kbytes super block */
  /*  This area in SRAM 2 is updated BL2 and can be lock to avoid any changes */
  if (
    (BOOT_TFM_SHARED_DATA_BASE >= S_RAM_ALIAS(_SRAM2_TOP - GTZC_MPCBB_SUPERBLOCK_SIZE))
    && (BOOT_TFM_SHARED_DATA_SIZE <= GTZC_MPCBB_SUPERBLOCK_SIZE))
  {

    __HAL_RCC_GTZC_CLK_ENABLE();
    if (HAL_GTZC_MPCBB_GetConfigMem(SRAM2_BASE, &MPCBB_desc) != HAL_OK)
    {
      Error_Handler();
    }
    MPCBB_desc.AttributeConfig.MPCBB_LockConfig_array[0] |= 0x00000080;
    if (HAL_GTZC_MPCBB_ConfigMem(SRAM2_BASE, &MPCBB_desc) != HAL_OK)
    {
      Error_Handler();
    }
  }
  else Error_Handler();
}

void mpu_config_appli(void)
{
#ifdef TFM_BOOT_MPU_PROTECTION
   struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };
   struct mpu_armv8m_dev_t dev_mpu_ns ={ MPU_BASE_NS};
   struct mpu_armv8m_region_cfg_t region_cfg;
  /* region 0 is now enable for execution */
  region_cfg.region_nr = 0;
  region_cfg.region_base = FLASH_BASE_S + FLASH_AREA_0_OFFSET ;
  region_cfg.region_limit = FLASH_BASE_S + FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_OK;
  if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
  /* region 0 is now enable for execution */
  region_cfg.region_nr = 0;
  region_cfg.region_base = FLASH_BASE_NS + FLASH_AREA_1_OFFSET;
  region_cfg.region_limit = FLASH_BASE_NS + FLASH_AREA_1_OFFSET + FLASH_AREA_1_SIZE;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_OK;
   if (mpu_armv8m_region_enable(&dev_mpu_ns, &region_cfg) != MPU_ARMV8M_OK)
  {
    Error_Handler();
  }
#endif /* TFM_BOOT_MPU_PROTECTION */
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
#ifdef TFM_ERROR_HANDLER_NON_SECURE
#define WHILE_1_OPCODE 0xe7fe
  typedef void (*nsfptr_t) (void) __attribute__((cmse_nonsecure_call));
  nsfptr_t nsfptr = (nsfptr_t)(SRAM1_BASE_NS + 1);
  __IO uint16_t *pt = (uint16_t *)SRAM1_BASE_NS;
  /*  copy while(1) instruction */
  *pt = WHILE_1_OPCODE;
  /* Flush and refill pipeline  */
  __DSB();
  __ISB();
  /*  call non secure while(1) */
  nsfptr();
#else /*  TFM_ERROR_HANDLER_NON_SECURE */
  /* it is customizeable */
  while (1);
#endif  /*  TFM_ERROR_HANDLER_NON_SECURE */
}
#if defined(__ARMCC_VERSION)
/* reimplement the function to reach Error Handler */
void __aeabi_assert(const char *expr, const char *file, int line)
{
#ifdef TFM_DEV_MODE
  printf("assertion \" %s \" failed: file %s %d\n", expr, file, line);
#endif
  Error_Handler();
}
#endif  /*  __ARMCC_VERSION */
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
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
