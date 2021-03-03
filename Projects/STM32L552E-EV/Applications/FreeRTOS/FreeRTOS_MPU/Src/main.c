/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_MPU/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Callback function prototypes */
extern void vApplicationIdleHook (void);
extern void vApplicationTickHook (void);
#if configCHECK_FOR_STACK_OVERFLOW
extern void vApplicationStackOverflowHook (TaskHandle_t xTask, signed char *pcTaskName);
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if defined(__ICCARM__)
extern uint32_t * __FLASH_segment_start__;
extern uint32_t * __FLASH_segment_end__;
extern uint32_t * __privileged_functions_start__;
extern uint32_t * __privileged_functions_end__ ;
extern uint32_t * __unprivileged_flash_start__;
extern uint32_t * __unprivileged_flash_end__;
extern uint32_t * __SRAM_segment_start__;
extern uint32_t * __SRAM_segment_end__;
extern uint32_t * __privileged_sram_start__;
extern uint32_t * __privileged_sram_end__;
extern uint32_t * __unprivileged_sram_start__;
extern uint32_t * __unprivileged_sram_end__;
#elif defined(__ARMCC_VERSION)
const uint32_t * __FLASH_segment_start__ = ( uint32_t * ) 0x08000000UL;
const uint32_t * __FLASH_segment_end__ = ( uint32_t * ) 0x0807FFFFUL;
const uint32_t * __privileged_functions_start__ = ( uint32_t * ) 0x08000000UL;
const uint32_t * __privileged_functions_end__ = ( uint32_t * ) 0x08001CFFUL;
const uint32_t * __syscalls_flash_start__ = ( uint32_t * ) 0x08001D00UL;
const uint32_t * __syscalls_flash_end__ = ( uint32_t * ) 0x0801EFFUL;
const uint32_t * __unprivileged_flash_start__ = ( uint32_t * ) 0x08001F00UL;
const uint32_t * __unprivileged_flash_end__ = ( uint32_t * ) 0x0807FFFFUL;
const uint32_t * __SRAM_segment_start__ = ( uint32_t * ) 0x20000000UL;
const uint32_t * __SRAM_segment_end__ = ( uint32_t * ) 0x2003FFFFUL;
const uint32_t * __privileged_sram_start__ = ( uint32_t * ) 0x20000000UL;
const uint32_t * __privileged_sram_end__ = ( uint32_t * ) 0x200007FFUL;
const uint32_t * __unprivileged_sram_start__ = ( uint32_t * ) 0x20000800UL;
const uint32_t * __unprivileged_sram_end__ = ( uint32_t * ) 0x2002FFFFUL;
#else /* __GNUC__ */
extern uint32_t __FLASH_segment_start__[];
extern uint32_t __FLASH_segment_end__[];
extern uint32_t __privileged_functions_start__[];
extern uint32_t __privileged_functions_end__[];
extern uint32_t __unprivileged_flash_start__[];
extern uint32_t __unprivileged_flash_end__[];
extern uint32_t __SRAM_segment_start__[];
extern uint32_t __SRAM_segment_end__[];
extern uint32_t __privileged_sram_start__[];
extern uint32_t __privileged_sram_end__[];
extern uint32_t __unprivileged_sram_start__[];
extern uint32_t __unprivileged_sram_end__[];
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void ReadTask(void *argument);
static void WriteTask(void *argument);

int8_t VAR = 0 ;
uint8_t PB_Pressed = 0;
static void SystemClock_Config(void);
TaskHandle_t hReadTask, hWriteTask;
#define CYCLE_RESET            ( 2 )

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
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

  /* For better performances, enable the instruction cache in 1-way direct mapped mode */
  HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY);
  HAL_ICACHE_Enable();

  /* Initialize button */
  BSP_PB_Init(BUTTON_TAMPER, BUTTON_MODE_EXTI);

  /* Initialize LEDs */
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);

  xTaskCreate( ReadTask, "Task1", 512, NULL,( 3 | portPRIVILEGE_BIT ), &hReadTask );
  xTaskCreate( WriteTask, "Task2", 512, NULL,( 3 | portPRIVILEGE_BIT ), &hWriteTask );

  /* Start the scheduler. */
  vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler */
  for (;;);

}

/**
  * @brief  EXTI line detection callbacks
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  PB_Pressed = 1;
}

/**
  * @brief  ReadTask thread
  * @param  thread not used
  * @retval None
  */
static void ReadTask(void *argument)
{
  (void) argument;

  for (;;)
  {
    if (VAR == 0)
    {
      BSP_LED_Off(LED4);
    }
    else if (VAR == 1)
    {
      BSP_LED_On(LED4);
    }
    else
    {
      BSP_LED_Off(LED4);
      while(1)
      {
        BSP_LED_Toggle(LED5);
        vTaskDelay(500);
      }
    }
  }
}

/**
  * @brief  WriteTask thread
  * @param  argument not used
  * @retval None
  */
static void WriteTask(void *argument)
{
  (void) argument;

  for (;;)
  {
    if (PB_Pressed == 1)
    {
      VAR = -1;
      portRESET_PRIVILEGE();
      break;
    }
    else
    {
      if (VAR == 0)
      {
        VAR = 1;
        vTaskDelay(500);
      }
      else
      {
        VAR = 0;
        vTaskDelay(500);
      }
    }
  }
  while(1)
  {

  }
}

void vApplicationIdleHook( void )
{
  volatile const uint32_t *pul;
  volatile uint32_t ulReadData;

  /* The idle task, and therefore this function, run in Supervisor mode and
  can therefore access all memory.  Try reading from corners of flash and
  RAM to ensure a memory fault does not occur.

  Start with the edges of the privileged data area. */
  pul = __privileged_sram_start__;
  ulReadData = *pul;
  pul = __privileged_sram_end__ - 1;
  ulReadData = *pul;

  /* Next the standard SRAM area. */
  pul = __unprivileged_sram_end__ - 1;
  ulReadData = *pul;

  /* And the standard Flash area - the start of which is marked for
  privileged access only. */
  pul = __privileged_functions_start__;
  ulReadData = *pul;
  pul = __unprivileged_flash_end__ - 1;
  ulReadData = *pul;

  /* Reading off the end of Flash or SRAM space should cause a fault.
  Uncomment one of the following two pairs of lines to test. */

  /* pul = __FLASH_segment_end__ + 4;
  ulReadData = *pul; */

  /* pul = __SRAM_segment_end__ + 1;
  ulReadData = *pul; */

  ( void ) ulReadData;
}

void vApplicationTickHook( void )
{
  static uint32_t ulCallCount = 0;
  const uint32_t ulCallsBetweenSends = pdMS_TO_TICKS( 5000 );

  /* If configUSE_TICK_HOOK is set to 1 then this function will get called
  from each RTOS tick.  It is called from the tick interrupt and therefore
  will be executing in the privileged state. */

  ulCallCount++;

  /* Is it time to print out the pass/fail message again? */
  if( ulCallCount >= ulCallsBetweenSends )
  {
    ulCallCount = 0;
  }
}

#if configCHECK_FOR_STACK_OVERFLOW
void vApplicationStackOverflowHook( TaskHandle_t pxTask, signed char *pcTaskName )
{
  /* If configCHECK_FOR_STACK_OVERFLOW is set to either 1 or 2 then this
  function will automatically get called if a task overflows its stack. */
  ( void ) pxTask;
  ( void ) pcTaskName;
  for( ;; );
}
#endif

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

  /* Enable MSI Oscillator and activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
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
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
