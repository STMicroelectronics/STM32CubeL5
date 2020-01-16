/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    HASH/HASH_SHA1_DMA_TrustZone/Secure/Src/main.c
  * @author  MCD Application Team
  * @brief   Secure main application
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Non-secure Vector table to jump to (internal Flash Bank2 here)             */
/* Caution: address must correspond to non-secure internal Flash where is     */
/*          mapped in the non-secure vector table                             */
#define VTOR_TABLE_NS_START_ADDR  0x08040000UL
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* HASH handler declaration */
HASH_HandleTypeDef     HashHandle;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void NonSecure_Init(void);
static void SystemIsolation_Config(void);
static void SystemClock_Config(void); /* provided as example if secure sets clocks */
void Error_Handler(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* SAU/IDAU, FPU and interrupts secure/non-secure allocation setup done */
  /* in SystemInit() based on partition_stm32l552xx.h file's definitions. */

  /* USER CODE BEGIN 1 */

  /* Secure/Non-secure Memory and Peripheral isolation configuration */
  SystemIsolation_Config();

  /* Enable SecureFault handler (HardFault is default) */
  SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk;

  /* STM32L5xx **SECURE** HAL library initialization:
       - Secure Systick timer is configured by default as source of time base,
         but user can eventually implement his proper time base source (a general
         purpose timer for example or other time source), keeping in mind that
         Time base duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined
         and handled in milliseconds basis.
       - Low Level Initialization
     */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* Enable Instruction cache (default 2-ways set associative cache) */
  if(HAL_ICACHE_Enable() != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Configure the System clock to have a frequency of 110 MHz */
  SystemClock_Config();

  /* USER CODE END SysInit */


  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* All IOs are by default allocated to secure */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_All, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_All, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOC, GPIO_PIN_All, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOD, GPIO_PIN_All, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_All, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOF, GPIO_PIN_All, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOG, GPIO_PIN_All, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOH, GPIO_PIN_All, GPIO_PIN_NSEC);

  /* Leave the GPIO clocks enabled to let non-secure having I/Os control */

  /* HASH SHA-1 Handle init */
  HashHandle.Init.DataType = HASH_DATATYPE_8B;
  if (HAL_HASH_Init(&HashHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /*************** Setup and jump to non-secure *******************************/

  NonSecure_Init();

  /* Non-secure software does not return, this code is not executed */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

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

/* USER CODE BEGIN 4 */
/**
  * @brief  System Isolation Configuration by GTZC
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
    Error_Handler();
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
    Error_Handler();
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
  if (HAL_GTZC_TZSC_ConfigPeriphAttributes(GTZC_PERIPH_HASH,
                                           GTZC_TZSC_PERIPH_SEC|GTZC_TZSC_PERIPH_NPRIV) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Enable illegal access interrupts in TZIC for any non-secure application */
  /* access to TZSC/TZIC/MPCBB/MPCWM registers */
  if (    (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_MPCBB2_REG)   != HAL_OK)
       || (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_SRAM2)        != HAL_OK)
       || (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_MPCBB1_REG)   != HAL_OK)
       || (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_SRAM1)        != HAL_OK)
       || (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_OCTOSPI1_MEM) != HAL_OK)
       || (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_FMC_MEM)      != HAL_OK)
       || (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_TZIC)         != HAL_OK)
       || (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_TZSC)         != HAL_OK))
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Clear all illegal access pending interrupts in TZIC */
  if (HAL_GTZC_TZIC_ClearFlag(GTZC_PERIPH_ALL) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Enable GTZC TZIC secure interrupt */
  HAL_NVIC_SetPriority(GTZC_IRQn, 0, 0); /* Highest priority level */
  HAL_NVIC_ClearPendingIRQ(GTZC_IRQn);
  HAL_NVIC_EnableIRQ(GTZC_IRQn);
}

/* USER CODE END 4 */

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
    Error_Handler();
  }

  /* AHB prescaler divider at 1 as second step */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  /* LED3 on */
  BSP_LED_Init(LED3);
  BSP_LED_On(LED3);

  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
