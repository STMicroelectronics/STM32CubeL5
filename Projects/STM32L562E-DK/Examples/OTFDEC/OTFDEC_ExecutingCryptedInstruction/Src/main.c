/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    OTFDEC/OTFDEC_ExecutingCryptedInstruction/Src/main.c
  * @author  MCD Application Team
  * @brief    This example describes how to execute an encrypted instruction located
  *           on the OCTOSPI external flash using the OTFDEC peripheral.
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
#include "hal_octospi_utility.h"
#if defined(__GNUC__)
#include <stdio.h>
#include <stdlib.h>
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef uint32_t (*Customized_f)(uint32_t a,uint32_t b);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PAGE_SIZE                        0x100
#define BUFFER_SIZE                      0x100
#define START_ADRESS_OCTOSPI1            0x90000000
#define START_ADRESS_OTFDEC1_REGION2     0x90003000
#define END_ADRESS_OTFDEC1_REGION2       0x90003FFF
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

OSPI_HandleTypeDef hospi1;

OTFDEC_HandleTypeDef hotfdec1;

/* USER CODE BEGIN PV */
OTFDEC_RegionConfigTypeDef Config = {0};
Customized_f Decrypt_add15;
uint32_t Result_Decrypt_Add15 = 0;
uint32_t Key[4]={0xA9876543, 0x210FEDCB, 0xA9876543, 0x210FEDCB};
/* Ciphered instructions */
uint32_t Crypt_add[4] = {0x0ECE000A, 0x369DEFC3, 0x641DB7B1, 0x3F10275E};

uint32_t a=10;
uint32_t b=20;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
static void MX_GPIO_Init(void);
static void MX_OCTOSPI1_Init(void);
static void MX_OTFDEC1_Init(void);
/* USER CODE BEGIN PFP */
static uint8_t Ref_CRC_computation(uint32_t * keyin);

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
extern void initialise_monitor_handles(void);
#endif
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
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
  initialise_monitor_handles();	/*rtt*/
  printf("Semihosting Test...\n\r"); 
#endif
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_ICACHE_Init();
  MX_GPIO_Init();
  MX_OCTOSPI1_Init();
  MX_OTFDEC1_Init();
  /* USER CODE BEGIN 2 */
  /* Config OCTOSPI */
  OSPI_Config() ;

  /* Write Ciphered Data */
  OSPI_Write(Crypt_add, START_ADRESS_OTFDEC1_REGION2- START_ADRESS_OCTOSPI1, 4);

  /* Activate memory mapping */
  OSPI_MemoryMap();

  /* Enable all interruptions */
  __HAL_OTFDEC_ENABLE_IT(&hotfdec1, OTFDEC_ALL_INT);

  /* Set OTFDEC Mode */
  if (HAL_OTFDEC_RegionSetMode(&hotfdec1, OTFDEC_REGION2, OTFDEC_REG_MODE_INSTRUCTION_OR_DATA_ACCESSES) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set OTFDEC Key */
  if (HAL_OTFDEC_RegionSetKey(&hotfdec1, OTFDEC_REGION2, Key) != HAL_OK)
  {
    Error_Handler();
   }

  /* Second CRC check. Superfluous at this point since done by default in HAL_OTFDEC_RegionSetKey()
     but shown in this example for illustration purposes */
  if ((uint8_t)(HAL_OTFDEC_RegionGetKeyCRC( &hotfdec1, OTFDEC_REGION2 )) !=  Ref_CRC_computation(Key))
  {
    Error_Handler();
  }

  /* Configure then activate OTFDEC decryption */
  Config.Nonce[0]     = 0x11111111;
  Config.Nonce[1]     = 0x55555555;
  Config.StartAddress = START_ADRESS_OTFDEC1_REGION2;
  Config.EndAddress   = END_ADRESS_OTFDEC1_REGION2;
  Config.Version      = 0xA5A5;
  if (HAL_OTFDEC_RegionConfig(&hotfdec1, OTFDEC_REGION2, &Config, OTFDEC_REG_CONFIGR_LOCK_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Executing opcode of arithmetic addition  */
  Decrypt_add15 = (Customized_f)(START_ADRESS_OTFDEC1_REGION2+1);
  Result_Decrypt_Add15 = Decrypt_add15(a,b);

  /* Output arithmetic addition result on debugger Terminal I/O using printf function */
  printf("%s%lu%s%lu%s\n"," When a and b are respectively equal to ", (unsigned long)a," and ", (unsigned long)b,",");
  printf("%s%lu\n"," a+b+15 computed by ciphered instructions is: ", (unsigned long)Result_Decrypt_Add15);
  printf("\n\r** Test finished successfully. ** \n\r");

  /* Turn on appropriate LED after checking the result */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
  if ((a+b+15) != Result_Decrypt_Add15)
  {
    BSP_LED_On(LED_RED);
  }
  else
  {
    BSP_LED_On(LED_GREEN);
  }
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

/**
  * @brief OCTOSPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OCTOSPI1_Init(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */
  /* OCTOSPI1 parameter configuration*/
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 4;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MACRONIX;
  hospi1.Init.DeviceSize = 26;
  hospi1.Init.ChipSelectHighTime = 2;
  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
  hospi1.Init.ClockPrescaler = 2;
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE;
  hospi1.Init.ChipSelectBoundary = 0;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_USED;
  hospi1.Init.Refresh = 0;
  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */

  /* USER CODE END OCTOSPI1_Init 2 */

}

/**
  * @brief OTFDEC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OTFDEC1_Init(void)
{

  /* USER CODE BEGIN OTFDEC1_Init 0 */

  /* USER CODE END OTFDEC1_Init 0 */

  /* USER CODE BEGIN OTFDEC1_Init 1 */

  /* USER CODE END OTFDEC1_Init 1 */
  hotfdec1.Instance = OTFDEC1;
  if (HAL_OTFDEC_Init(&hotfdec1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OTFDEC1_Init 2 */

  /* USER CODE END OTFDEC1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
/**
  * @brief  OTFDEC Key CRC computation
  * @note   Source code of the Key CRC computation. Function is used to compute
  *         the expected CRC and compare it with that provided by the Peripheral
  *         in KEYCRC bitfield after loading the key in OTFDEC_RxKEYR registers.
  * @param  keyin OTFDEC key
  * @retval 8-bit long CRC
  */
static uint8_t Ref_CRC_computation(uint32_t * keyin)
{
  uint8_t CRC7_POLY = 0x7;
  uint32_t key_strobe[4] = {0xAA55AA55, 0x3, 0x18, 0xC0};
  uint8_t  i, j, k, crc = 0x0;
  uint32_t  keyval;

  for (j = 0; j < 4; j++)
  {
    keyval = *(keyin+j);
    if (j == 0)
    {
      keyval ^= key_strobe[0];
    }
    else
    {
      keyval ^= (key_strobe[j] << 24) | (crc << 16) | (key_strobe[j] << 8) | crc;
    }

    for (i = 0, crc = 0; i < 32; i++)
    {
      k = (((crc >> 7) ^ ((keyval >> (31-i))&0xF))) & 1;
      crc <<= 1;
      if (k)
      {
        crc ^= CRC7_POLY;
       }
    }

    crc^=0x55;
  }

  return crc;

}

/**
  * @brief OTFDEC error callback.
  * @param  hotfdec_region pointer to a OTFDEC_Region_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @retval None
  */
void HAL_OTFDEC_ErrorCallback(OTFDEC_HandleTypeDef *hotfdec)
{

  if ((hotfdec->ErrorCode & OTFDEC_ISR_XONEIF) == OTFDEC_ISR_XONEIF)
  {
    /* Disable Execute-Only Execute-Never error interrupt */
    CLEAR_BIT( hotfdec->Instance->IER, OTFDEC_IER_XONEIE);

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
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
