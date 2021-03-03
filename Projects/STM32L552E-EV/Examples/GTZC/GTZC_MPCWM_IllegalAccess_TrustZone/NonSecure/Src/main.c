/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    GTZC/GTZC_MPCWM_IllegalAccess_TrustZone/NonSecure/Src/main.c
  * @author  MCD Application Team
  * @brief   Non-secure main application
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
__IO uint32_t WakeUpButtonPressed = 0U;
__IO uint32_t TamperButtonPressed = 0U;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
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
  /* USER CODE BEGIN 1 */
  uint32_t i, temp_value;
  volatile uint32_t read_value;

  /* STM32L5xx **NON-SECURE** HAL library initialization:
       - Non-secure Systick timer is configured by default as source of time base,
         but user can eventually implement his proper time base source (a general
         purpose timer for example or other time source), keeping in mind that
         Time base duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined
         and handled in milliseconds basis.
       - Set NVIC Group Priority to 3
       - Low Level Initialization
     */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    while ((WakeUpButtonPressed == 0U) && (TamperButtonPressed == 0U))
    {
      /* read/write test in first half of memory (non-secure area), should be OK
       * Reminder: in secure sub-project
       * - FMC NOR memory size is 16Mb = 2MB = 16*128KB
       * - mapped address range is 0x60000000..0x60200000
       * - so half size is 8*128KB
       * - first half is set as non-secure (8*128KB) in initial config
       * - second half is set as secure (8*128KB) in initial config
       */
      for (i = 0U; i < (8U * GTZC_TZSC_MPCWM_GRANULARITY); i+=4)
      {
        read_value = *(uint32_t *)(FMC_BANK1 + i);
        temp_value = ~read_value;
        *(uint32_t *)(FMC_BANK1 + i) = temp_value;
        if( *(uint32_t *)(FMC_BANK1 + i) != temp_value)
        {
          SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure read/write access mismatch in non-secure SRAM");
          /* Infinite loop */
          while(1);
        }
      }

      /* on previous test success, blink green led */
      HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
      /* Insert delay 1000 ms */
      HAL_Delay(1000U);
      HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
      /* Insert delay 1000 ms */
      HAL_Delay(1000U);
    }

    if (WakeUpButtonPressed == 1U)
    {
      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure detection of BUTTON_WAKEUP");

      /* here we do a write operation from the non-secure on a secure area.
       * This is of course not allowed, and the result should be a Secure fault
       * event because it doesn't respect SAU rules (1st obstacle, before MPCWM)
       * This access is done at the beginning of the second half of memory, set
       * as secured in initial config.
       */
      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure Read test on Secure area of FMC NOR");
      read_value = 0xABABABAB;
      *(uint32_t *)(FMC_BANK1 + 8 * GTZC_TZSC_MPCWM_GRANULARITY) = ~read_value;

      /* In the secure fault interrupt handler (on secure sub-project), FMC NOR
       * memory security properties have been updated, in order to avoid an
       * infinite loop. So we need to restore initial properties, to continue
       * SW execution in good conditions.
       */
      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure requests to restore SAU/MPCWM2 initial configuration");
      SECURE_SAU_MPCWM2_SetInitConfig();

      /* Switch off RED LED after a 1s delay */
      if (HAL_GPIO_ReadPin(LED5_GPIO_Port, LED5_Pin) == GPIO_PIN_RESET)
      {
        HAL_Delay(1000U);
        SECURE_ToggleRedLed();
      }

      HAL_Delay(1000U);
      WakeUpButtonPressed = 0U;
    }
    else if (TamperButtonPressed == 1U)
    {
      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure detection of BUTTON_TAMPER");

      /* here we do a write operation from the non-secure world on an address in
       * a non-secure area from SAU point of view (so should be OK) but on a
       * secure area from MPCWM2 point of view. This should triggered GTZC_IRQn.
       * For this, we should misalign SAU and MPCWM configuration, do the access
       * and restore initial configurations.
       */
      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure requests misalignment between SAU and MPCWM2 configuration");
      SECURE_SAU_MPCWM2_SetTamperButtonErrorCaseConfig();
      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-Secure Write test on Secure area of FMC NOR");
      read_value = 0xBABABABA;
      *(uint32_t *)(FMC_BANK1 + 8 * GTZC_TZSC_MPCWM_GRANULARITY) = ~read_value;

      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure requests to restore SAU/MPCWM2 initial configuration");
      SECURE_SAU_MPCWM2_SetInitConfig();

      /* Switch off RED LED after a 1s delay */
      if (HAL_GPIO_ReadPin(LED5_GPIO_Port, LED5_Pin) == GPIO_PIN_RESET)
      {
        HAL_Delay(1000U);
        SECURE_ToggleRedLed();
      }

      HAL_Delay(1000U);
      TamperButtonPressed = 0U;
    }
    else
    {
      /* Do nothing as not expected */
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : BUTTON_WAKEUP_Pin */
  GPIO_InitStruct.Pin = BUTTON_WAKEUP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_WAKEUP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_TAMPER_Pin */
  GPIO_InitStruct.Pin = BUTTON_TAMPER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_TAMPER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED4_Pin */
  GPIO_InitStruct.Pin = LED4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LED4_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI13_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(EXTI13_IRQn);

}

/* USER CODE BEGIN 4 */
/**
  * @brief  EXTI line rising detection callback.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == BUTTON_WAKEUP_Pin)
  {
    WakeUpButtonPressed = 1U;
  }
  else if (GPIO_Pin == BUTTON_TAMPER_Pin)
  {
    TamperButtonPressed = 1U;
  }
  else
  {
    /* Unexpected case, do nothing */
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
  /* User can add his own implementation to report the HAL error return state */

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
