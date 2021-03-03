/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_Mutexes/Src/main.c
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
#define mutexSHORT_DELAY     ((uint32_t) 20)
#define mutexNO_DELAY        ((uint32_t) 0)
#define mutexTWO_TICK_DELAY  ((uint32_t) 2)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* Definitions for MutHigh */
osThreadId_t MutHighHandle;
const osThreadAttr_t MutHigh_attributes = {
  .name = "MutHigh",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 512
};
/* Definitions for MutMedium */
osThreadId_t MutMediumHandle;
const osThreadAttr_t MutMedium_attributes = {
  .name = "MutMedium",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512
};
/* Definitions for MutLow */
osThreadId_t MutLowHandle;
const osThreadAttr_t MutLow_attributes = {
  .name = "MutLow",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512
};
/* Definitions for osMutex */
osMutexId_t osMutexHandle;
const osMutexAttr_t osMutex_attributes = {
  .name = "osMutex"
};
/* USER CODE BEGIN PV */

/* Variables used to detect and latch errors */
__IO uint32_t HighPriorityThreadCycles = 0, MediumPriorityThreadCycles = 0, LowPriorityThreadCycles = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
void MutexHighPriorityThreadr(void *argument);
void MutexMediumPriorityThread(void *argument);
void MutexLowPriorityThread(void *argument);

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
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED6);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of osMutex */
  osMutexHandle = osMutexNew(&osMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of MutHigh */
  MutHighHandle = osThreadNew(MutexHighPriorityThreadr, NULL, &MutHigh_attributes);

  /* creation of MutMedium */
  MutMediumHandle = osThreadNew(MutexMediumPriorityThread, NULL, &MutMedium_attributes);

  /* creation of MutLow */
  MutLowHandle = osThreadNew(MutexLowPriorityThread, NULL, &MutLow_attributes);

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

/* USER CODE BEGIN Header_MutexHighPriorityThreadr */
/**
  * @brief  Function implementing the MutHigh thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_MutexHighPriorityThreadr */
void MutexHighPriorityThreadr(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Just to remove compiler warning */
  (void) argument;
  /* Infinite loop */
  for (;;)
  {
    /* The first time through the mutex will be immediately available, on
    subsequent times through the mutex will be held by the low priority thread
    at this point and this Take will cause the low priority thread to inherit
    the priority of this tadhr.  In this case the block time must be
    long enough to ensure the low priority thread will execute again before the
    block time expires.  If the block time does expire then the error
    flag will be set here */
    if (osMutexAcquire(osMutexHandle, mutexTWO_TICK_DELAY) != osOK)
    {
      /* Toggle LED5 to indicate error */
      BSP_LED_Toggle(LED5);
    }

    /* Ensure the other thread attempting to access the mutex
    are able to execute to ensure they either block (where a block
    time is specified) or return an error (where no block time is
    specified) as the mutex is held by this task */
    osDelay(mutexSHORT_DELAY);

    /* We should now be able to release the mutex .
    When the mutex is available again the medium priority thread
    should be unblocked but not run because it has a lower priority
    than this thread.  The low priority thread should also not run
    at this point as it too has a lower priority than this thread */
    if (osMutexRelease(osMutexHandle) != osOK)
    {
      /* Toggle LED5 to indicate error */
      BSP_LED_Toggle(LED5);
    }

    /* Keep count of the number of cycles this thread has performed */
    HighPriorityThreadCycles++;
    BSP_LED_Toggle(LED4);

    /* Suspend ourselves to the medium priority thread can execute */
    osThreadSuspend(MutHighHandle);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_MutexMediumPriorityThread */
/**
* @brief Function implementing the MutMedium thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MutexMediumPriorityThread */
void MutexMediumPriorityThread(void *argument)
{
  /* USER CODE BEGIN MutexMediumPriorityThread */
  uint32_t MaxMediumPriorityThreadCycles = 0;
  /* Just to remove compiler warning */
  (void) argument;

  /* Infinite loop */
  for (;;)
  {
    /* This thread will run while the high-priority thread is blocked, and the
    high-priority thread will block only once it has the mutex - therefore
    this call should block until the high-priority thread has given up the
    mutex, and not actually execute past this call until the high-priority
    thread is suspended */
    if (osMutexAcquire(osMutexHandle, osWaitForever) == osOK)
    {
      if (osThreadGetState(MutHighHandle) != osThreadBlocked)
      {
        /* Did not expect to execute until the high priority thread was
        suspended.
        Toggle LED5 to indicate error */
        BSP_LED_Toggle(LED5);
      }
      else
      {
        /* Give the mutex back before suspending ourselves to allow
        the low priority thread to obtain the mutex */
        if (osMutexRelease(osMutexHandle) != osOK)
        {
          /* Toggle LED5 to indicate error */
          BSP_LED_Toggle(LED5);
        }
        osThreadSuspend(MutMediumHandle);
      }
    }
    else
    {
      /* We should not leave the osMutexWait() function
      until the mutex was obtained.
      Toggle LED5 to indicate error */
      BSP_LED_Toggle(LED5);
    }
    
    MaxMediumPriorityThreadCycles = MediumPriorityThreadCycles + 1;
    
    /* The High and Medium priority threads should be in lock step */
    if (HighPriorityThreadCycles != MaxMediumPriorityThreadCycles)
    {
      /* Toggle LED5 to indicate error */
      BSP_LED_Toggle(LED5);
    }

    /* Keep count of the number of cycles this task has performed so a
    stall can be detected */
    MediumPriorityThreadCycles++;
  }
  /* USER CODE END MutexMediumPriorityThread */
}

/* USER CODE BEGIN Header_MutexLowPriorityThread */
/**
* @brief Function implementing the MutLow thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MutexLowPriorityThread */
void MutexLowPriorityThread(void *argument)
{
  /* USER CODE BEGIN MutexLowPriorityThread */
  /* Just to remove compiler warning */
  (void) argument;

  /* Infinite loop */
  for (;;)
  {
    /* Keep attempting to obtain the mutex.  We should only obtain it when
    the medium-priority thread has suspended itself, which in turn should only
    happen when the high-priority thread is also suspended */
    if (osMutexAcquire(osMutexHandle, mutexNO_DELAY) == osOK)
    {
      /* Is the haigh and medium-priority threads suspended? */
      if ((osThreadGetState(MutHighHandle) != osThreadBlocked) || (osThreadGetState(MutMediumHandle) != osThreadBlocked))
      {
        /* Toggle LED5 to indicate error */
        BSP_LED_Toggle(LED5);
      }
      else
      {
        /* Keep count of the number of cycles this task has performed
        so a stall can be detected */
        LowPriorityThreadCycles++;

        /* We can resume the other tasks here even though they have a
        higher priority than the this thread. When they execute they
        will attempt to obtain the mutex but fail because the low-priority 
        thread is still the mutex holder.  this thread will then inherit 
        the higher priority.  The medium-priority thread will block indefinitely
        when it attempts to obtain the mutex, the high-priority thread will only
        block for a fixed period and an error will be latched if the
        high-priority thread has not returned the mutex by the time this
        fixed period has expired */
        osThreadResume(MutMediumHandle);
        osThreadResume(MutHighHandle);

        /* Release the mutex, disinheriting the higher priority again */
        if (osMutexRelease(osMutexHandle) != osOK)
        {
          /* Toggle LED5 to indicate error */
          BSP_LED_Toggle(LED5);
        }
      }
    }

#if configUSE_PREEMPTION == 0
    {
      taskYIELD();
    }
#endif
  }
  /* USER CODE END MutexLowPriorityThread */
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
