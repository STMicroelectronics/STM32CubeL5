/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_MPU/Src/main.c
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint32_t address;
  uint32_t handle;
  uint32_t psp;
} FaultInfo_t;

/* Private define ------------------------------------------------------------*/
#if defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
extern uint32_t Image$$ER_IROM_FREERTOS_SYSTEM_CALLS$$Base;
extern uint32_t Image$$ER_IROM_FREERTOS_SYSTEM_CALLS$$Limit;

extern uint32_t Image$$ER_IROM_UNPRIVILEGED$$Base;
extern uint32_t Image$$ER_IROM_UNPRIVILEGED$$Limit;

/* Memory map needed for MPU setup. Must match the one defined in
 * the scatter-loading file (Project.sct). */
const uint32_t * __FLASH_segment_start__ = ( uint32_t * ) 0x08000000;
const uint32_t * __FLASH_segment_end__ = ( uint32_t * ) 0x080FFFFF;
const uint32_t * __SRAM_segment_start__ = ( uint32_t * ) 0x20000000;
const uint32_t * __SRAM_segment_end__ = ( uint32_t * ) 0x2003FFFF;

const uint32_t * __privileged_functions_start__ = ( uint32_t * ) 0x08000000;
const uint32_t * __privileged_functions_end__ = ( uint32_t * ) 0x08007FFF;
const uint32_t * __privileged_sram_start__ = ( uint32_t * ) 0x20000000;
const uint32_t * __privileged_sram_end__ = ( uint32_t * ) 0x20007FFF;

const uint32_t * __unprivileged_flash_start__ = ( uint32_t * ) &( Image$$ER_IROM_UNPRIVILEGED$$Base );
const uint32_t * __unprivileged_flash_end__ = ( uint32_t * ) &( Image$$ER_IROM_UNPRIVILEGED$$Limit );

const uint32_t * __syscalls_flash_start__ = ( uint32_t * ) &( Image$$ER_IROM_FREERTOS_SYSTEM_CALLS$$Base );
const uint32_t * __syscalls_flash_end__ = ( uint32_t * ) &( Image$$ER_IROM_FREERTOS_SYSTEM_CALLS$$Limit );
#endif

#define NUMBER_OF_TASKS (2)

/* Return address offset within stack frame */
#define RETURN_ADDRESS_OFFSET (6U)

/* Mask used to identify whether instruction is 16 bits or 32 bits */
#define INSTRUCTION_32BIT_Msk (0xE000)

/* Example config */
/* Number of faults a task can cause before it gets deleted */
#define EXAMPLE_FAULT_COUNT_THRESHOLD (3)

#define EXAMPLE_SHARED_MEMORY_SIZE (512U)

/* A queue to hold fault information */
static QueueHandle_t fault_queue;

/* Define an array to track fault count */
static uint8_t fault_count[NUMBER_OF_TASKS];

/* Define stack spaces for unprivileged tasks.
 * These must meet MPU alignment and size requirements. */
static StackType_t Task1Stack[ 2 * configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
static StackType_t Task2Stack[ 2 * configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );

/* Define shared memory */
static uint8_t sharedMemory[ EXAMPLE_SHARED_MEMORY_SIZE ] __attribute__( ( aligned( 32 ) ) );
static TaskHandle_t MainTaskHandle;
static TaskHandle_t TaskHandles[NUMBER_OF_TASKS];

static void Task1(void *argument);
static void Task2(void *argument);

/* Task parameters for unprivileged tasks */
static const TaskParameters_t TaskParameters[NUMBER_OF_TASKS] =
{
  {
      .pvTaskCode     = Task1,
      .pcName         = "Task1",
      .usStackDepth   = 2 * configMINIMAL_STACK_SIZE,
      .pvParameters   = NULL,
      .uxPriority     = tskIDLE_PRIORITY,
      .puxStackBuffer = Task1Stack,
      .xRegions       =   {
                              { sharedMemory      , EXAMPLE_SHARED_MEMORY_SIZE,
                              tskMPU_REGION_READ_ONLY | tskMPU_REGION_EXECUTE_NEVER },
                              { LED4_GPIO_PORT, EXAMPLE_SHARED_MEMORY_SIZE,
                              tskMPU_REGION_READ_WRITE | tskMPU_REGION_EXECUTE_NEVER },

                              { 0 ,  0,  0 },
                          }
  },
  {
      .pvTaskCode     = Task2,
      .pcName         = "Task2",
      .usStackDepth   = 2 * configMINIMAL_STACK_SIZE,
      .pvParameters   = NULL,
      .uxPriority     = tskIDLE_PRIORITY,
      .puxStackBuffer = Task2Stack,
      .xRegions       =   {
                              { sharedMemory      , EXAMPLE_SHARED_MEMORY_SIZE,
                              tskMPU_REGION_READ_ONLY | tskMPU_REGION_EXECUTE_NEVER },
                              { LED5_GPIO_PORT, EXAMPLE_SHARED_MEMORY_SIZE,
                              tskMPU_REGION_READ_WRITE | tskMPU_REGION_EXECUTE_NEVER },
                              { 0 ,  0,  0 },
                          }
  }
};

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_USART3_UART_Init(void);
static void MX_GPIO_Init(void);
static void MainTask(void *argument);

/* USER CODE BEGIN PFP */
#if defined(__ARMCC_VERSION) || defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif
/* USER CODE END PFP */

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
 /* Initialize configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();

  /* Initialize LEDs */
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);

  /* For better performances, enable the instruction cache in 1-way direct mapped mode */
  HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY);
  HAL_ICACHE_Enable();

  printf("FreeRTOS MPU Application\r\n");

  /* Init scheduler */
  osKernelInitialize();

  fault_queue = xQueueCreate(5U, sizeof(FaultInfo_t));

  if(fault_queue == NULL)
  {
     Error_Handler();
  }
  /* USER CODE END RTOS_QUEUES */

  /* USER CODE BEGIN RTOS_THREADS */
  if(xTaskCreate(MainTask, "MainTask", 4 * configMINIMAL_STACK_SIZE, NULL,
                 (configMAX_PRIORITIES -1) |portPRIVILEGE_BIT  ,
                 &MainTaskHandle) != pdPASS)
  {
     Error_Handler();
  }

  /* Create unprivileged threads */
  if(xTaskCreateRestricted(&TaskParameters[0], &TaskHandles[0] )!= pdPASS)
  {
     Error_Handler();
  }
  if(xTaskCreateRestricted(&TaskParameters[1], &TaskHandles[1] )!= pdPASS)
  {
     Error_Handler();
  }

  /* Start the scheduler. */
  vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler */
  for (;;);

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

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_ODD;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    while(1);
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    while(1);
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    while(1);
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    while(1);
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
}

/**
  * @brief Function implementing the MainTask thread.
  *        It is responsible for montitoring other threads.
  * @param argument: Not used
  * @retval None
  */
void MainTask(void *argument)
{
  UNUSED(argument);

  FaultInfo_t fault;

  for(;;)
  {
    xQueueReceive(fault_queue, &fault, portMAX_DELAY);

    printf("\tTask name: %s\n", pcTaskGetName((TaskHandle_t) fault.handle));

    printf("------------------------------\n");

    /* Handle MPU exceptions */
    for(int i = 0; i < NUMBER_OF_TASKS; ++i)
    {
        /* Delete task if it has caused too many faults*/
        if(fault_count[i] == EXAMPLE_FAULT_COUNT_THRESHOLD)
        {
          vTaskDelete((TaskHandle_t)fault.handle);

          printf("%s has caused %d faults. It was deleted.\n",
                  pcTaskGetName(TaskHandles[i]),
                  EXAMPLE_FAULT_COUNT_THRESHOLD);
        }
    }
  }
}

/**
  * @brief Function implementing the Task1 thread.
  * @param argument: Not used
  * @retval None
  */
void Task1(void *argument)
{
  UNUSED(argument);

  uint8_t isFirstTime = 1;

  for(;;)
  {
    HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
    vTaskDelay(pdMS_TO_TICKS(1000));
    if(isFirstTime)
    {
      isFirstTime = 0;

      /* Try to perform an illegal write */
      sharedMemory[EXAMPLE_SHARED_MEMORY_SIZE - 1] = 4U;
    }
  }
}

/**
  * @brief Function implementing the Task2 thread.
  * @param argument: Not used
  * @retval None
  */
void Task2(void *argument)
{
  UNUSED(argument);

  vTaskDelay(pdMS_TO_TICKS(1000));
  for(;;)
  {
    HAL_GPIO_TogglePin(LED5_GPIO_PORT, LED5_PIN);
    vTaskDelay(pdMS_TO_TICKS(2000));

    /* Try to perform an illegal write */
    sharedMemory[EXAMPLE_SHARED_MEMORY_SIZE/2] = 4U;
  }
}

/* We need to use the original FreeRTOS xTaskGetCurrentTaskHandle
 * function instead of the one defined in mpu_wrapper.c */
#undef xTaskGetCurrentTaskHandle
TaskHandle_t xTaskGetCurrentTaskHandle( void );

/**
  * @brief MemManage fault recovery
  * @retval None
  */
void MemManage_Recover(void)
{
  FaultInfo_t current_fault;

  uint8_t nextInstructionOffset = 2U;
  uint32_t* returnAddress;
  uint16_t instruction;

  /* Log the offending thread */
  current_fault.address  = SCB->MMFAR;
  current_fault.handle =  (uint32_t)xTaskGetCurrentTaskHandle();
  current_fault.psp = (uint32_t)__get_PSP();

  /*
   * Clear DACCVIOL sticky bit in the MMFSR subregister.
   * If not cleared, subsequent fault addresses will not be stored in
   * the MMFAR register.*/
  SET_BIT(SCB->CFSR,SCB_CFSR_DACCVIOL_Msk);

  for(int i = 0; i < NUMBER_OF_TASKS; ++i)
    {
      if((TaskHandle_t)current_fault.handle == TaskHandles[i])
      {
        /* Increment fault count for task */
        ++fault_count[i];

        /* Display info about fault */
        printf("MemManage fault occurred\r\n");
        printf("\tAddress: 0x%X\n", (unsigned int) current_fault.address);
        printf("\tCurrent Task Handle: %p\n", xTaskGetCurrentTaskHandle());

        returnAddress = (uint32_t*)current_fault.psp + RETURN_ADDRESS_OFFSET;

        /* Identify if the instruction which caused the issue
         *is 16 or 32 bit wide */
        instruction = *(uint16_t*)(*(uint32_t*)returnAddress);
        if((instruction & INSTRUCTION_32BIT_Msk) == INSTRUCTION_32BIT_Msk)
        {
            nextInstructionOffset = 4U;
        }
        else
        {
            nextInstructionOffset = 2U;
        }

        /* Move to next instruction */
        *returnAddress += nextInstructionOffset;

        break;
      }
    }

  /* Send Fault info to Main task */
  xQueueSendToBackFromISR(fault_queue, &current_fault, NULL);

  /* Current task yields. This forces a context switch and guarantees that
   * the Main thread will be able to take action if needed before the current
   * thread continues. */
  portYIELD_WITHIN_API();
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  printf("Critical Error occurred \r\n");
  while (1)
  {
    BSP_LED_Toggle(LED6);
    HAL_Delay(500);
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
