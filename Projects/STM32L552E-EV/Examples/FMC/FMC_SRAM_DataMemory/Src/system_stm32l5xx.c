/**
  ******************************************************************************
  * @file    system_stm32l5xx.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
  *
  *   This file provides two functions and one global variable to be called from
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32l5xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick
  *                                  timer or configure other parameters.
  *
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *   After each device reset the MSI (4 MHz) is used as system clock source.
  *   Then SystemInit() function is called, in "startup_stm32l5xx.s" file, to
  *   configure the system clock before to branch to main program.
  *
  *   This file configures the system clock as follows:
  *=============================================================================
  *-----------------------------------------------------------------------------
  *        System Clock source                    | MSI
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 4000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                               | 4000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                         | 1
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                         | 1
  *-----------------------------------------------------------------------------
  *        PLL_SRC                                | No clock
  *-----------------------------------------------------------------------------
  *        PLL_M                                  | 1
  *-----------------------------------------------------------------------------
  *        PLL_N                                  | 8
  *-----------------------------------------------------------------------------
  *        PLL_P                                  | 7
  *-----------------------------------------------------------------------------
  *        PLL_Q                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL_R                                  | 2
  *-----------------------------------------------------------------------------
  *        PLLSAI1_SRC                            | NA
  *-----------------------------------------------------------------------------
  *        PLLSAI1_M                              | NA
  *-----------------------------------------------------------------------------
  *        PLLSAI1_N                              | NA
  *-----------------------------------------------------------------------------
  *        PLLSAI1_P                              | NA
  *-----------------------------------------------------------------------------
  *        PLLSAI1_Q                              | NA
  *-----------------------------------------------------------------------------
  *        PLLSAI1_R                              | NA
  *-----------------------------------------------------------------------------
  *        PLLSAI2_SRC                            | NA
  *-----------------------------------------------------------------------------
  *        PLLSAI2_M                              | NA
  *-----------------------------------------------------------------------------
  *        PLLSAI2_N                              | NA
  *-----------------------------------------------------------------------------
  *        PLLSAI2_P                              | NA
  *-----------------------------------------------------------------------------
  *        Require 48MHz for USB FS,              | Disabled
  *        SDIO and RNG clock                     |
  *-----------------------------------------------------------------------------
  *=============================================================================
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Apache License, Version 2.0,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/Apache-2.0
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup STM32L5xx_System
  * @{
  */

/** @addtogroup STM32L5xx_System_Private_Includes
  * @{
  */

#include "stm32l5xx.h"

/**
  * @}
  */

/** @addtogroup STM32L5xx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32L5xx_System_Private_Defines
  * @{
  */

#if !defined  (HSE_VALUE)
  #define HSE_VALUE    16000000U /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (MSI_VALUE)
  #define MSI_VALUE    4000000U  /*!< Value of the Internal oscillator in Hz*/
#endif /* MSI_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    16000000U /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */


/************************* Miscellaneous Configuration ************************/
/*!< Uncomment the following line if you need to use external SRAM as data memory  */
#define DATA_IN_ExtSRAM 

/* Note: Following vector table addresses must be defined in line with linker
         configuration. */
/*!< Uncomment the following line if you need to relocate the vector table
     anywhere in Flash or Sram, else the vector table is kept at the automatic
     remap of boot address selected */
/* #define USER_VECT_TAB_ADDRESS */

#if defined(USER_VECT_TAB_ADDRESS)
/*!< Uncomment the following line if you need to relocate your vector Table
     in Sram else user remap will be done in Flash. */
/* #define VECT_TAB_SRAM */

#if defined(VECT_TAB_SRAM)
#define VECT_TAB_BASE_ADDRESS   SRAM1_BASE      /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x200. */
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x200. */
#else
#define VECT_TAB_BASE_ADDRESS   FLASH_BASE      /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x200. */
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x200. */
#endif /* VECT_TAB_SRAM */
#endif /* USER_VECT_TAB_ADDRESS */

/******************************************************************************/
/**
  * @}
  */

/** @addtogroup STM32L5xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32L5xx_System_Private_Variables
  * @{
  */
  /* The SystemCoreClock variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
  uint32_t SystemCoreClock = 4000000U;

  const uint8_t  AHBPrescTable[16] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U};
  const uint8_t  APBPrescTable[8] =  {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};
  const uint32_t MSIRangeTable[16] = {100000U,   200000U,   400000U,   800000U,  1000000U,  2000000U, \
                                      4000000U, 8000000U, 16000000U, 24000000U, 32000000U, 48000000U, \
                                      0U,       0U,       0U,        0U};  /* MISRAC-2012: 0U for unexpected value */
/**
  * @}
  */

/** @addtogroup STM32L5xx_System_Private_FunctionPrototypes
  * @{
  */

#if defined (DATA_IN_ExtSRAM)
  static void SystemInit_ExtMemCtl(void); 
#endif /* DATA_IN_ExtSRAM */

/**
  * @}
  */

/** @addtogroup STM32L5xx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system.
  * @retval None
  */

void SystemInit(void)
{
  /* Configure the Vector Table location -------------------------------------*/
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
#endif

  /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  SCB->CPACR |= ((3UL << 20U)|(3UL << 22U));  /* set CP10 and CP11 Full Access */
#endif

#if defined (DATA_IN_ExtSRAM)
  SystemInit_ExtMemCtl(); 
#endif /* DATA_IN_ExtSRAM */
}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *
  * @note   - The system frequency computed by this function is not the real
  *           frequency in the chip. It is calculated based on the predefined
  *           constant and the selected clock source:
  *
  *           - If SYSCLK source is MSI, SystemCoreClock will contain the MSI_VALUE(*)
  *
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(**)
  *
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(***)
  *
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(***)
  *             or HSI_VALUE(*) or MSI_VALUE(*) multiplied/divided by the PLL factors.
  *
  *         (*) MSI_VALUE is a constant defined in stm32l5xx_hal.h file (default value
  *             4 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.
  *
  *         (**) HSI_VALUE is a constant defined in stm32l5xx_hal.h file (default value
  *              16 MHz) but the real value may vary depending on the variations
  *              in voltage and temperature.
  *
  *         (***) HSE_VALUE is a constant defined in stm32l5xx_hal.h file (default value
  *              8 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  *
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  uint32_t tmp, msirange, pllvco, pllsource, pllm, pllr;

  /* Get MSI Range frequency--------------------------------------------------*/
  if((RCC->CR & RCC_CR_MSIRGSEL) == 0U)
  { /* MSISRANGE from RCC_CSR applies */
    msirange = (RCC->CSR & RCC_CSR_MSISRANGE) >> 8U;
  }
  else
  { /* MSIRANGE from RCC_CR applies */
    msirange = (RCC->CR & RCC_CR_MSIRANGE) >> 4U;
  }
  /*MSI frequency range in HZ*/
  msirange = MSIRangeTable[msirange];

  /* Get SYSCLK source -------------------------------------------------------*/
  switch (RCC->CFGR & RCC_CFGR_SWS)
  {
    case 0x00:  /* MSI used as system clock source */
      SystemCoreClock = msirange;
      break;

    case 0x04:  /* HSI used as system clock source */
      SystemCoreClock = HSI_VALUE;
      break;

    case 0x08:  /* HSE used as system clock source */
      SystemCoreClock = HSE_VALUE;
      break;

    case 0x0C:  /* PLL used as system clock  source */
      /* PLL_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE/ PLLM) * PLLN
         SYSCLK = PLL_VCO / PLLR
         */
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);
      pllm = ((RCC->PLLCFGR & RCC_PLLCFGR_PLLM) >> 4U) + 1U ;

      switch (pllsource)
      {
        case 0x02:  /* HSI used as PLL clock source */
          pllvco = (HSI_VALUE / pllm);
          break;

        case 0x03:  /* HSE used as PLL clock source */
          pllvco = (HSE_VALUE / pllm);
          break;

        default:    /* MSI used as PLL clock source */
          pllvco = (msirange / pllm);
          break;
      }
      pllvco = pllvco * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 8U);
      pllr = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >> 25U) + 1U) * 2U;
      SystemCoreClock = pllvco/pllr;
      break;

    default:
      SystemCoreClock = msirange;
      break;
  }
  /* Compute HCLK clock frequency --------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4U)];
  /* HCLK clock frequency */
  SystemCoreClock >>= tmp;
}

#if defined (DATA_IN_ExtSRAM)
/**
  * @brief  Setup the external memory controller.
  *         Called in startup_stm32l5xx.s before jump to main.
  *         This function configures the external memories (SRAM)
  *         This SRAM will be used as program data memory (including heap and stack).
  * @param  None
  * @retval None
  */
void SystemInit_ExtMemCtl(void)
{
  __IO uint32_t tmp = 0x00;

  /*-- GPIOs Configuration -----------------------------------------------------*/
  /* Enable GPIOD, GPIOE, GPIOF and GPIOG interface clock */
  RCC->AHB2ENR       |= 0x00000078;
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN);
  
  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0]      = 0xC0CC00CC;
  GPIOD->AFR[1]      = 0xCCCCCCCC;
  /* Configure PDx pins in Alternate function mode */  
  GPIOD->MODER       = 0xAAAA8A0A;
  /* Configure PDx pins speed */  
  GPIOD->OSPEEDR     = 0xFFFFCF0F;
  /* Configure PDx pins Output type to push-pull */  
  GPIOD->OTYPER      = 0x00000000;
  /* No pull-up, pull-down for PDx pins */ 
  GPIOD->PUPDR       = 0x00000000;

  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]      = 0xC00CC0CC;
  GPIOE->AFR[1]      = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */ 
  GPIOE->MODER       = 0xAAAA828A;
  /* Configure PEx pins speed */ 
  GPIOE->OSPEEDR     = 0xFFFFC3CF;
  /* Configure PEx pins Output type to push-pull */  
  GPIOE->OTYPER      = 0x00000000;
  /* No pull-up, pull-down for PEx pins */ 
  GPIOE->PUPDR       = 0x00000000;

  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]      = 0x00CCCCCC;
  GPIOF->AFR[1]      = 0xCCCC0000;
  /* Configure PFx pins in Alternate function mode */   
  GPIOF->MODER       = 0xAA000AAA;
  /* Configure PFx pins speed */ 
  GPIOF->OSPEEDR     = 0xFF000FFF;
  /* Configure PFx pins Output type to push-pull */  
  GPIOF->OTYPER      = 0x00000000;
  /* No pull-up, pull-down for PFx pins */ 
  GPIOF->PUPDR       = 0x00000000;

  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]      = 0x00CCCCCC;
  GPIOG->AFR[1]      = 0x00000000;
  /* Configure PGx pins in Alternate function mode */ 
  GPIOG->MODER       = 0x00000AAA;
  /* Configure PGx pins speed */ 
  GPIOG->OSPEEDR     = 0x00000FFF;
  /* Configure PGx pins Output type to push-pull */  
  GPIOG->OTYPER      = 0x00000000;
  /* No pull-up, pull-down for PGx pins */ 
  GPIOG->PUPDR       = 0x00000000;
  
  /* Enable PWR interface clock */
  RCC->APB1ENR1      |= 0x10000000;
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_PWREN);

  /* IOSV bit MUST be set to access GPIO port G[2:15] */
  PWR->CR2           |= 0x00000200;

  /*-- FMC Configuration --------------------------------------------------*/
  /* Enable the FMC interface clock */
  RCC->AHB3ENR       |= 0x00000001;
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

  /* Configure and enable Bank1_SRAM1 */
  FMC_Bank1_R->BTCR[0] = 0x00001091;
  FMC_Bank1_R->BTCR[1] = 0x0FF001F2;

  (void)(tmp); 
}
#endif /* DATA_IN_ExtSRAM */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
