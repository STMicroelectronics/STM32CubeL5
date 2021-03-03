/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
__IO uint32_t endOfProcess = 0;
uint8_t buffer[256] = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_PKA_Init(void);
/* USER CODE BEGIN PFP */
void     LED_On(void);
void     LED_Blinking(uint32_t Period);
static uint32_t Buffercmp(const uint8_t* pBuffer1,const uint8_t* pBuffer2, uint32_t BufferLength);
__IO uint32_t *PKA_Memcpy_u8_to_u32(__IO uint32_t dst[], const uint8_t src[], uint32_t n);
uint8_t *PKA_Memcpy_u32_to_u8(uint8_t dst[], __IO const uint32_t src[], uint32_t n);
void PKA_load_ciphering_parameter(void);
void PKA_load_unciphering_parameter(void);
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
  /* USER CODE BEGIN 1 */
  uint32_t result = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);

  /* System interrupt init*/

  /** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
  */
  LL_PWR_DisableUCPDDeadBattery();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_PKA_Init();
  /* USER CODE BEGIN 2 */
  /* Set mode to ECDSA signature generation in interrupt mode */
  LL_PKA_SetMode(PKA, LL_PKA_MODE_MONTGOMERY_PARAM_MOD_EXP);
  LL_PKA_EnableIT_ADDRERR(PKA);
  LL_PKA_EnableIT_RAMERR(PKA);
  LL_PKA_EnableIT_PROCEND(PKA);
  
  /*   FROM PLAINTEXT TO CIPHERTEXT   */
  
  /* Loads the input buffers to PKA RAM */
  PKA_load_ciphering_parameter();
  
  /* Launch the computation in interrupt mode */
  LL_PKA_Start(PKA);
  
  /* Wait for the interrupt callback */
  while(endOfProcess != 1);
  endOfProcess = 0;
  
  /* Retrieve the result and output buffer */
  PKA_Memcpy_u32_to_u8(buffer, &PKA->RAM[PKA_MODULAR_EXP_OUT_SM_ALGO_ACC1], rsa_pub_2048_modulus_len / 4);
  
  /* Compare to expected results */
  result = Buffercmp(buffer, ciphertext_bin, ciphertext_bin_len);
  if (result != 0)
  {
    LED_Blinking(LED_BLINK_ERROR);
  } 
  
  /*   FROM CIPHERTEXT TO PLAINTEXT   */
  
  /* Loads the input buffers to PKA RAM */  
  PKA_load_unciphering_parameter();
  
  /* Launch the computation in interrupt mode */
  LL_PKA_Start(PKA);
  
  /* Wait for the interrupt callback */
  while(endOfProcess != 1);
  endOfProcess = 0;
  
  /* Retrieve the result and output buffer */
  PKA_Memcpy_u32_to_u8(buffer, &PKA->RAM[PKA_MODULAR_EXP_OUT_SM_ALGO_ACC1], rsa_pub_2048_modulus_len / 4);
  
  /* Compare to expected results */
  result = Buffercmp(buffer, plaintext_bin, plaintext_bin_len);
  if (result != 0)
  {
    LED_Blinking(LED_BLINK_ERROR);
  }  
  
  LED_On();
  /* USER CODE END 2 */

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
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5)
  {
  }

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
  LL_RCC_MSI_Enable();

   /* Wait till MSI is ready */
  while(LL_RCC_MSI_IsReady() != 1)
  {
  }

  LL_RCC_MSI_EnableRangeSelection();
  LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);
  LL_RCC_MSI_SetCalibTrimming(0);
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 55, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_EnableDomain_SYS();
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

   /* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
   LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Insure 1µs transition state at intermediate medium speed clock based on DWT*/
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;
  while(DWT->CYCCNT < 100);

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_Init1msTick(110000000);

  LL_SetSystemCoreClock(110000000);
}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */
  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief PKA Initialization Function
  * @param None
  * @retval None
  */
static void MX_PKA_Init(void)
{

  /* USER CODE BEGIN PKA_Init 0 */

  /* USER CODE END PKA_Init 0 */

  /* Peripheral clock enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_PKA);

  /* PKA interrupt Init */
  NVIC_SetPriority(PKA_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(PKA_IRQn);

  /* USER CODE BEGIN PKA_Init 1 */

  /* USER CODE END PKA_Init 1 */
  LL_PKA_Enable(PKA);
  /* USER CODE BEGIN PKA_Init 2 */
  /* Check that PKA is correctly enabled. PKA RAM is erased at power on and at system reset.
     During RAM erasing, content of PKA registers can't be modified. */
  while (LL_PKA_IsEnabled(PKA) != 1U)
  {
    LL_PKA_Enable(PKA);
  }
  /* USER CODE END PKA_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG);
  LL_PWR_EnableVddIO2();

  /**/
  LL_GPIO_SetOutputPin(GPIOG, LL_GPIO_PIN_12);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Load into PKA RAM the ciphering parameters.
  * @param  None
  * @retval None
  */
void PKA_load_ciphering_parameter(void)
{
  /* Get the number of bit per operand */
  PKA->RAM[PKA_MODULAR_EXP_IN_OP_NB_BITS] = rsa_pub_2048_modulus_len*8;

  /* Get the number of bit of the exponent */
  PKA->RAM[PKA_MODULAR_EXP_IN_EXP_NB_BITS] = rsa_pub_2048_publicExponent_len*8;

  /* Move the input parameters pOp1 to PKA RAM */
  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT_BASE], plaintext_bin, rsa_pub_2048_modulus_len);
  PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT_BASE + rsa_pub_2048_modulus_len / 4] = 0;

  /* Move the exponent to PKA RAM */
  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT], rsa_pub_2048_publicExponent, rsa_pub_2048_publicExponent_len);
  PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT + rsa_pub_2048_publicExponent_len / 4] = 0;

  /* Move the modulus to PKA RAM */
  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_MODULUS], rsa_pub_2048_modulus, rsa_pub_2048_modulus_len);
  PKA->RAM[PKA_MODULAR_EXP_IN_MODULUS + rsa_pub_2048_modulus_len / 4] = 0;
}

/**
  * @brief  Load into PKA RAM the unciphering parameters.
  * @param  None
  * @retval None
  */
void PKA_load_unciphering_parameter(void)
{
  /* Get the number of bit per operand */
  PKA->RAM[PKA_MODULAR_EXP_IN_OP_NB_BITS] = rsa_pub_2048_modulus_len*8;

  /* Get the number of bit of the exponent */
  PKA->RAM[PKA_MODULAR_EXP_IN_EXP_NB_BITS] = rsa_priv_2048_privateExponent_len*8;

  /* Move the input parameters pOp1 to PKA RAM */
  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT_BASE], ciphertext_bin, rsa_pub_2048_modulus_len);
  PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT_BASE + rsa_pub_2048_modulus_len / 4] = 0;

  /* Move the exponent to PKA RAM */
  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT], rsa_priv_2048_privateExponent, rsa_priv_2048_privateExponent_len);
  PKA->RAM[PKA_MODULAR_EXP_IN_EXPONENT + rsa_priv_2048_privateExponent_len / 4] = 0;

  /* Move the modulus to PKA RAM */
  PKA_Memcpy_u8_to_u32(&PKA->RAM[PKA_MODULAR_EXP_IN_MODULUS], rsa_priv_2048_modulus, rsa_pub_2048_modulus_len);
  PKA->RAM[PKA_MODULAR_EXP_IN_MODULUS + rsa_pub_2048_modulus_len / 4] = 0;
  
}


void PKA_ERROR_callback(void)
{
  LED_Blinking(LED_BLINK_ERROR);
}

void PKA_PROCEND_callback(void)
{
  endOfProcess = 1;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint32_t Buffercmp(const uint8_t* pBuffer1,const uint8_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief  Copy uint8_t array to uint32_t array to fit PKA number representation.
  * @param  dst Pointer to destination
  * @param  src Pointer to source
  * @param  n Number of u32 to be handled
  * @retval dst
  */
__IO uint32_t *PKA_Memcpy_u8_to_u32(__IO uint32_t dst[], const uint8_t src[], uint32_t n)
{
  const uint32_t *ptrSrc = (const uint32_t *) src;

  if (dst != 0)
  {
    for (uint32_t index = 0; index < n / 4; index++)
    {
      dst[index] = __REV(ptrSrc[n / 4 - index - 1]);
    }
  }
  return dst;
}

/**
  * @brief  Copy uint32_t array to uint8_t array to fit PKA number representation.
  * @param  dst Pointer to destination
  * @param  src Pointer to source
  * @param  n Number of u8 to be handled (must be multiple of 4)
  * @retval dst
  */
uint8_t *PKA_Memcpy_u32_to_u8(uint8_t dst[], __IO const uint32_t src[], uint32_t n)
{
  uint32_t *ptrDst = (uint32_t *) dst;
  if (dst != 0)
  {
    for (uint32_t index = 0; index < n; index++)
    {
      ptrDst[n - index - 1] = __REV(src[index]);
    }
  }
  return dst;
}

/**
  * @brief  Turn-on LED_GREEN.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED_GREEN on */
  LL_GPIO_ResetOutputPin(GPIOG, LL_GPIO_PIN_12);
}

/**
  * @brief  Set LED_GREEN to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Toggle LED_GREEN in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(GPIOG, LL_GPIO_PIN_12); 
    LL_mDelay(Period);
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Infinite loop */
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
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
