/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_Semaphore/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

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
#define semtstSTACK_SIZE      configMINIMAL_STACK_SIZE
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* Definitions for SEM_Thread1 */
osThreadId_t SEM_Thread1Handle;
const osThreadAttr_t SEM_Thread1_attributes = {
  .name = "SEM_Thread1",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};
/* Definitions for SEM_Thread2 */
osThreadId_t SEM_Thread2Handle;
const osThreadAttr_t SEM_Thread2_attributes = {
  .name = "SEM_Thread2",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512 * 4
};
/* Definitions for osSemaphore */
osSemaphoreId_t osSemaphoreHandle;
const osSemaphoreAttr_t osSemaphore_attributes = {
  .name = "osSemaphore"
};
/* USER CODE BEGIN PV */
__IO uint32_t OsStatus = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
void SemaphoreThread1(void *argument);
void SemaphoreThread2(void *argument);

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
  
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Initialize LEDs */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of osSemaphore */
  osSemaphoreHandle = osSemaphoreNew(1, 0, &osSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of SEM_Thread1 */
  SEM_Thread1Handle = osThreadNew(SemaphoreThread1, (void *) osSemaphoreHandle, &SEM_Thread1_attributes);

  /* creation of SEM_Thread2 */
  SEM_Thread2Handle = osThreadNew(SemaphoreThread2, (void *) osSemaphoreHandle, &SEM_Thread2_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 55;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
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
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_SemaphoreThread1 */
/**
  * @brief  Function implementing the SEM_Thread1 thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_SemaphoreThread1 */
void SemaphoreThread1(void *argument)
{
  /* USER CODE BEGIN 5 */
  uint32_t count = 0;
  osSemaphoreId_t semaphore = (osSemaphoreId_t) argument;
  /* Infinite loop */
  for (;;)
  {

    if (semaphore != NULL)
    {
	  OsStatus = osSemaphoreAcquire(semaphore, osWaitForever);
      /* Try to obtain the semaphore */
      if (OsStatus == osOK)
      {
        count = osKernelGetTickCount() + 5000;

        /* Toggle LED1 every 200 ms for 5 seconds */
        while (count > osKernelGetTickCount())
        {
          /* Toggle LED1 */
          BSP_LED_Toggle(LED1);

          /* Delay 200 ms */
          osDelay(200);
        }

        /* Turn off LED1*/
        BSP_LED_Off(LED1);
        /* Release the semaphore */
        OsStatus = osSemaphoreRelease(semaphore);

        /* Suspend ourseleves to execute thread 2 (lower priority)  */
        OsStatus = osThreadSuspend(SEM_Thread1Handle);
      }
    }
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_SemaphoreThread2 */
/**
* @brief Function implementing the SEM_Thread2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SemaphoreThread2 */
void SemaphoreThread2(void *argument)
{
  /* USER CODE BEGIN SemaphoreThread2 */
  uint32_t count = 0;
  osSemaphoreId_t semaphore = (osSemaphoreId_t) argument;
  /* Infinite loop */
  for (;;)
  {
    if (semaphore != NULL)
    {
      /* Try to obtain the semaphore */
      OsStatus = osSemaphoreAcquire(semaphore, osWaitForever);
      {
        /* Resume Thread 1 (higher priority)*/
        OsStatus = osThreadResume(SEM_Thread1Handle);

        count = osKernelGetTickCount() + 5000;

        /* Toggle LED2 every 200 ms for 5 seconds*/
        while (count > osKernelGetTickCount())
        {
          BSP_LED_Toggle(LED2);

          osDelay(200);
        }

        /* Turn off LED2 */
        BSP_LED_Off(LED2);

        /* Release the semaphore to unblock Thread 1 (higher priority)  */
        OsStatus = osSemaphoreRelease(semaphore);
      }
    }
  }
  /* USER CODE END SemaphoreThread2 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
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
  {}
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
