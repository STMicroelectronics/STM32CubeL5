/**
  ******************************************************************************
  * @file    Templates/TrustZoneEnabled/Secure/Src/main.c
  * @author  MCD Application Team
  * @brief   Main secure program body
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

/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */
extern funcptr_NS pSecureErrorCallback;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Non-secure Vector table to jump to (internal Flash Bank2 here)             */
/* Caution: address must correspond to non-secure internal Flash where is     */
/*          mapped in the non-secure vector table                             */
#define VTOR_TABLE_NS_START_ADDR  0x08040000UL


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void NonSecure_Init(void);
static void SystemIsolation_Config(void);
/* static void SystemClock_Config(void);  provided as example if secure sets clocks */

/**
  * @brief  Main program
  * @retval None
  */
int main(void)
{
  /* SAU/IDAU, FPU and Interrupts secure/non-secure allocation settings  */
  /* already done in SystemInit() thanks to partition_stm32l552xx.h file */

  /* STM32L5xx **SECURE** HAL library initialization:
       - Secure Systick timer is configured by default as source of time base,
         but user can eventually implement his proper time base source (a general
         purpose timer for example or other time source), keeping in mind that
         Time base duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined
         and handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Secure application may configure the System clock here */
  /* SystemClock_Config(); */

  /* For better performances, enable the instruction cache in 1-way direct mapped mode */
  HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY);
  HAL_ICACHE_Enable();

  /* Secure/Non-secure Memory and Peripheral isolation configuration */
  SystemIsolation_Config();

  /* Enable SecureFault handler (HardFault is default) */
  SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk;

  /* Add your secure application code here prior to non-secure initialization
     */

	/* All IOs are by default allocated to secure */

  /* Release PB.03 I/O for LED4 control on non-secure */
  /* __HAL_RCC_GPIOB_CLK_ENABLE(); */
  /* HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_3, GPIO_PIN_NSEC); */

	/* Leave the GPIO clocks enabled to let non-secure having I/Os control */

  /* Secure SysTick should rather be suspended before calling non-secure  */
  /* in order to avoid wake-up from sleep mode entered by non-secure      */
  /* The Secure SysTick shall be resumed on non-secure callable functions */
  HAL_SuspendTick();

  /*************** Setup and jump to non-secure *******************************/

  NonSecure_Init();

  /* Non-secure software does not return, this code is not executed */
  while (1) {
  }
}

/**
  * @brief  GTZC TZIC interrupt callback.
  * @param  PeriphId Peripheral identifier triggering the illegal access.
  *         This parameter can be a value of @ref GTZC_TZSC_TZIC_PeriphId
  * @retval None.
  */
void HAL_GTZC_TZIC_Callback(uint32_t PeriphId)
{
  funcptr_NS callback_NS; // non-secure callback function pointer

  /* Prevent unused argument(s) compilation warning */
  UNUSED(PeriphId);

  if(pSecureErrorCallback != (funcptr_NS)NULL)
  {
   /* return function pointer with cleared LSB */
   callback_NS = (funcptr_NS)cmse_nsfptr_create(pSecureErrorCallback);

   callback_NS();
  }
  else
  {
    /* Something went wrong in test case */
    while(1);
  }
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Non-secure call function
  *         This function is responsible for Non-secure initialization and switch
  *         to non-secure state
  * @retval None
  */
static void NonSecure_Init(void)
{
  funcptr_NS NonSecure_ResetHandler;

  SCB_NS->VTOR = VTOR_TABLE_NS_START_ADDR;

  /* Set non-secure main stack (MSP_NS) */
  __TZ_set_MSP_NS((*(uint32_t *)VTOR_TABLE_NS_START_ADDR));

  /* Get non-secure reset handler */
  NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)((VTOR_TABLE_NS_START_ADDR) + 4U)));

  /* Start non-secure state software application */
  NonSecure_ResetHandler();
}

/**
  * @brief  System Isolation Configuration
  *         This function is responsible for Memory and Peripheral isolation
  *         for secure and non-secure application parts
  * @retval None
  */
static void SystemIsolation_Config(void)
{
  uint32_t index;
  MPCBB_ConfigTypeDef MPCBB_desc;

  /* Enable GTZC peripheral clock */
  __HAL_RCC_GTZC_CLK_ENABLE();

  /* -------------------------------------------------------------------------*/
  /*                   Memory isolation configuration                         */
  /* Initializes the memory that secure application books for non secure      */
  /* -------------------------------------------------------------------------*/

  /* -------------------------------------------------------------------------*/
  /* Internal RAM */
  /* The booking is done in both IDAU/SAU and GTZC MPCBB */

  /* GTZC MPCBB setup */
  /* based on non-secure RAM memory area starting from 0x20018000         */
  /* 0x20018000 is the start address of second SRAM1 half                 */
  /* Internal SRAM is secured by default and configured by block          */
  /* of 256bytes.                                                         */
  /* Non-secure block-based memory starting from 0x20018000 means         */
  /* 0x18000 / (256 * 32) = 12 super-blocks for secure block-based memory */
  /* and remaining super-blocks set to 0 for all non-secure blocks        */
  MPCBB_desc.SecureRWIllegalMode = GTZC_MPCBB_SRWILADIS_ENABLE;
  MPCBB_desc.InvertSecureState = GTZC_MPCBB_INVSECSTATE_NOT_INVERTED;
  MPCBB_desc.AttributeConfig.MPCBB_LockConfig_array[0] = 0x00000000U;  /* Locked configuration */
  for(index=0; index<12; index++)
  {
    /* Secure blocks */
    MPCBB_desc.AttributeConfig.MPCBB_SecConfig_array[index] = 0xFFFFFFFFU;
  }
  for(index=12; index<24; index++)
  {
    /* Non-secure blocks */
    MPCBB_desc.AttributeConfig.MPCBB_SecConfig_array[index] = 0x00000000U;
  }

  if (HAL_GTZC_MPCBB_ConfigMem(SRAM1_BASE, &MPCBB_desc) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Internal SRAM2 is set as non-secure and configured by block          */
  /* Non-secure block-based memory starting from 0x20030000 means         */
  /* 0x10000 / (256 * 32) = 8 super-blocks set to 0 for non-secure        */
  /* block-based memory                                                   */
  for(index=0; index<8; index++)
  {
    /* Non-secure blocks */
    MPCBB_desc.AttributeConfig.MPCBB_SecConfig_array[index] = 0x00000000U;
  }

  if (HAL_GTZC_MPCBB_ConfigMem(SRAM2_BASE, &MPCBB_desc) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* -------------------------------------------------------------------------*/
  /* Internal Flash */
  /* The booking is done in both IDAU/SAU and FLASH interface */

  /* Setup done based on Flash dual-bank mode described with 1 area per bank  */
  /* Non-secure Flash memory area starting from 0x08040000 (Bank2)            */
  /* Flash memory is secured by default and modified with Option Byte Loading */
  /* Insure SECWM2_PSTRT > SECWM2_PEND in order to have all Bank2 non-secure  */

  /* -------------------------------------------------------------------------*/
  /* External OctoSPI memory */
  /* The booking is done in both IDAU/SAU and GTZC MPCWM interface */

  /* Default secure configuration */
  /* Else need to use HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes() */

  /* -------------------------------------------------------------------------*/
  /* External NOR/FMC memory */
  /* The booking is done in both IDAU/SAU and GTZC MPCWM interface */

  /* Default secure configuration */
  /* Else need to use HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes() */

  /* -------------------------------------------------------------------------*/
  /* External NAND/FMC memory */
  /* The booking is done in both IDAU/SAU and GTZC MPCWM interface */

  /* Default secure configuration */
  /* Else need to use HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes() */


  /* -------------------------------------------------------------------------*/
  /*                   Peripheral isolation configuration                     */
  /* Initializes the peripherals and features that secure application books   */
  /* for secure (RCC, PWR, RTC, EXTI, DMA, OTFDEC, etc..) or leave them to    */
  /* non-secure (GPIO (secured by default))                                   */
  /* -------------------------------------------------------------------------*/

  /* Clear all illegal access flags in TZIC */
  if(HAL_GTZC_TZIC_ClearFlag(GTZC_PERIPH_ALL) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Enable all illegal access interrupts in TZIC */
  if(HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_ALL) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Enable GTZC TZIC secure interrupt */
  HAL_NVIC_SetPriority(GTZC_IRQn, 0, 0); /* Highest priority level */
  HAL_NVIC_ClearPendingIRQ(GTZC_IRQn);
  HAL_NVIC_EnableIRQ(GTZC_IRQn);
}

#if 0   /* provided as example if secure sets clocks */
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
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source with transition state */
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
#endif /* 0 */

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

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
