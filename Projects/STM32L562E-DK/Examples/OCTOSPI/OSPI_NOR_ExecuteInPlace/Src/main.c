/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    OSPI/OSPI_NOR_ExecuteInPlace/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to configure and use OctoSPI through
  *          the STM32L5xx HAL API.
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
#if defined(__ARMCC_VERSION)
extern uint32_t Load$$OSPI_NOR$$Base;
extern uint32_t Load$$OSPI_NOR$$Length;
#elif defined(__ICCARM__)
#pragma section =".ospi_nor"
#pragma section =".ospi_nor_init"
#elif defined(__GNUC__)
extern uint32_t _ospi_nor_init_base;
extern uint32_t _ospi_nor_init_length;
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

OSPI_HandleTypeDef hospi1;
DMA_HandleTypeDef hdma_octospi1;

/* USER CODE BEGIN PV */
__IO uint8_t CmdCplt, TxCplt;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_OCTOSPI1_Init(void);
/* USER CODE BEGIN PFP */
static void OSPI_WriteEnable(OSPI_HandleTypeDef *hospi);
static void OSPI_AutoPollingMemReady(OSPI_HandleTypeDef *hospi);
static void OSPI_OctalModeCfg(OSPI_HandleTypeDef *hospi);
static void GpioToggle(void);
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
  OSPI_RegularCmdTypeDef sCommand;
  OSPI_MemoryMappedTypeDef sMemMappedCfg;
  __IO uint32_t ospi_addr = 0;
  uint8_t *flash_addr;
  __IO uint8_t step = 0;
  uint32_t max_size, size, nb_sectors_to_erase;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  BSP_LED_Init(LED10);
  BSP_LED_Init(LED9);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_ICACHE_Init();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_OCTOSPI1_Init();
  /* USER CODE BEGIN 2 */

  /* Caution: Despite instruction cache is enabled, no need for cache invalidation here */
  /*          since no previous fetch (read/execute) instructions were executed on      */
  /*          a different memory content                                                */

  /* Configure the memory in octal mode ------------------------------------- */
  OSPI_OctalModeCfg(&hospi1);

  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  flash_addr = 0;
  size = 0;

#if defined(__ARMCC_VERSION)
  max_size = (uint32_t)(&Load$$OSPI_NOR$$Length);
#elif defined(__ICCARM__)
  max_size = __section_size(".ospi_nor_init");
#elif defined(__GNUC__)
  max_size = (uint32_t)((uint8_t *)(&_ospi_nor_init_length));
#endif

  nb_sectors_to_erase = (max_size / OSPI_SECTOR_SIZE) + 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    switch(step)
    {
      case 0:
        CmdCplt = 0;
        
        /* Enable write operations ------------------------------------------ */
        OSPI_WriteEnable(&hospi1);

        /* Erasing Sequence ------------------------------------------------- */
        sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
        sCommand.Instruction   = OCTAL_SECTOR_ERASE_CMD;
        sCommand.AddressMode   = HAL_OSPI_ADDRESS_8_LINES;
        sCommand.Address       = ospi_addr;
        sCommand.DataMode      = HAL_OSPI_DATA_NONE;
        sCommand.DummyCycles   = 0;

        if (HAL_OSPI_Command_IT(&hospi1, &sCommand) != HAL_OK)
        {
          Error_Handler();
        }

        step++;
        break;

      case 1:
        if(CmdCplt != 0)
        {
          CmdCplt = 0;

          /* Configure automatic polling mode to wait for end of erase ------ */  
          OSPI_AutoPollingMemReady(&hospi1);

          nb_sectors_to_erase --;
          if(nb_sectors_to_erase != 0)
          {
            ospi_addr += OSPI_SECTOR_SIZE;
          }
          else
          {
            /* Initialize the variables for the data writing ---------------- */
            ospi_addr = 0;
#if defined(__ARMCC_VERSION)
            flash_addr = (uint8_t *)(&Load$$OSPI_NOR$$Base);
#elif defined(__ICCARM__)
            flash_addr = (uint8_t *)(__section_begin(".ospi_nor_init"));
#elif defined(__GNUC__)
            flash_addr = (uint8_t *)(&_ospi_nor_init_base);
#endif
            
            /* Copy only one page if the section is bigger */
            if (max_size > OSPI_PAGE_SIZE)
            {
              size = OSPI_PAGE_SIZE;
            }
            else
            {
              size = max_size;
            }
          }
          step++;
        }
        break;
        
      case 2:
        TxCplt = 0;

        if(nb_sectors_to_erase != 0)
        {
          step = 0;
        }
        else
        {
          /* Enable write operations ---------------------------------------- */
          OSPI_WriteEnable(&hospi1);
        
          /* Writing Sequence ----------------------------------------------- */
          sCommand.Instruction = OCTAL_PAGE_PROG_CMD;
          sCommand.DataMode    = HAL_OSPI_DATA_8_LINES;
          sCommand.NbData      = size;

          if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
          {
            Error_Handler();
          }

          if (HAL_OSPI_Transmit_DMA(&hospi1, flash_addr) != HAL_OK)
          {
            Error_Handler();
          }

          step++;
        }
      break;

      case 3:
        if(TxCplt != 0)
        {
          TxCplt = 0;
        
          /* Configure automatic polling mode to wait for end of program ----- */  
          OSPI_AutoPollingMemReady(&hospi1);
        
          ospi_addr += size;
          flash_addr += size;
        
          /* Check if a new page writing is needed */
          if (ospi_addr < max_size)
          {
            /* Update the remaining size if it is less than the page size */
            if ((ospi_addr + size) > max_size)
            {
              size = max_size - ospi_addr;
            }
            step = 2;
          }
          else
          {
            /* Memory-mapped mode configuration ------------------------------- */
            sCommand.OperationType = HAL_OSPI_OPTYPE_WRITE_CFG;
            sCommand.Instruction   = OCTAL_PAGE_PROG_CMD;
            sCommand.DataMode      = HAL_OSPI_DATA_8_LINES;
            sCommand.NbData        = 1; 
            sCommand.DQSMode       = HAL_OSPI_DQS_ENABLE;

            if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
            {
              Error_Handler();
            }

            sCommand.OperationType = HAL_OSPI_OPTYPE_READ_CFG;
            sCommand.Instruction   = OCTAL_IO_READ_CMD;
            sCommand.DummyCycles   = DUMMY_CLOCK_CYCLES_READ;
            sCommand.DQSMode       = HAL_OSPI_DQS_DISABLE;

            if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
            {
              Error_Handler();
            }

            sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_DISABLE;

            if (HAL_OSPI_MemoryMapped(&hospi1, &sMemMappedCfg) != HAL_OK)
            {
              Error_Handler();
            }

            step++;
          }
        }
      break;

      case 4:
        /* Execute the code from OSPI NOR memory -------------------------- */
        GpioToggle();
      break;

      default :
        Error_Handler();
    }
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
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

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
  * @brief  Command completed callback.
  * @param  hospi: OSPI handle
  * @retval None
  */
void HAL_OSPI_CmdCpltCallback(OSPI_HandleTypeDef *hospi)
{
  CmdCplt++;
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  hospi: OSPI handle
  * @retval None
  */
 void HAL_OSPI_TxCpltCallback(OSPI_HandleTypeDef *hospi)
{
  TxCplt++; 
}

/**
  * @brief  Transfer Error callback.
  * @param  hospi: OSPI handle
  * @retval None
  */
void HAL_OSPI_ErrorCallback(OSPI_HandleTypeDef *hospi)
{
  Error_Handler();
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hospi: OSPI handle
  * @retval None
  */
static void OSPI_WriteEnable(OSPI_HandleTypeDef *hospi)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_AutoPollingTypeDef sConfig;

  /* Enable write operations ------------------------------------------ */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = OCTAL_WRITE_ENABLE_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure automatic polling mode to wait for write enabling ---- */  
  sCommand.Instruction    = OCTAL_READ_STATUS_REG_CMD;
  sCommand.Address        = 0x0;
  sCommand.AddressMode    = HAL_OSPI_ADDRESS_8_LINES;
  sCommand.AddressSize    = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.DataMode       = HAL_OSPI_DATA_8_LINES;
  sCommand.DataDtrMode    = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData         = 1; 
  sCommand.DummyCycles    = DUMMY_CLOCK_CYCLES_READ_REG;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Match         = WRITE_ENABLE_MATCH_VALUE;
  sConfig.Mask          = WRITE_ENABLE_MASK_VALUE;
  sConfig.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
  sConfig.Interval      = AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hospi: OSPI handle
  * @retval None
  */
static void OSPI_AutoPollingMemReady(OSPI_HandleTypeDef *hospi)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_AutoPollingTypeDef sConfig;


  /* Configure automatic polling mode to wait for memory ready ------ */  
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = OCTAL_READ_STATUS_REG_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Address            = 0x0;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_8_LINES;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_8_LINES;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = 1; 
  sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_READ_REG;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
  
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Match         = MEMORY_READY_MATCH_VALUE;
  sConfig.Mask          = MEMORY_READY_MASK_VALUE;
  sConfig.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
  sConfig.Interval      = AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function configure the memory in Octal mode.
  * @param  hospi: OSPI handle
  * @retval None
  */
static void OSPI_OctalModeCfg(OSPI_HandleTypeDef *hospi)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_AutoPollingTypeDef sConfig;
  uint8_t reg;

  /* Enable write operations ---------------------------------------- */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = WRITE_ENABLE_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure automatic polling mode to wait for write enabling ---- */  
  sCommand.Instruction = READ_STATUS_REG_CMD;
  sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;
  sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData      = 1; 

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Match         = WRITE_ENABLE_MATCH_VALUE;
  sConfig.Mask          = WRITE_ENABLE_MASK_VALUE;
  sConfig.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
  sConfig.Interval      = AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Write Configuration register 2 (with new dummy cycles) --------- */
  sCommand.Instruction    = WRITE_CFG_REG_2_CMD;
  sCommand.Address        = CONFIG_REG2_ADDR3;  
  sCommand.AddressMode    = HAL_OSPI_ADDRESS_1_LINE;
  sCommand.AddressSize    = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  reg = CR2_DUMMY_CYCLES_66MHZ;

  if (HAL_OSPI_Transmit(hospi, &reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait that the memory is ready ---------------------------------- */
  sCommand.Instruction = READ_STATUS_REG_CMD;
  sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Match = MEMORY_READY_MATCH_VALUE;
  sConfig.Mask  = MEMORY_READY_MASK_VALUE;

  if (HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable write operations ---------------------------------------- */
  sCommand.Instruction = WRITE_ENABLE_CMD;
  sCommand.DataMode    = HAL_OSPI_DATA_NONE;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure automatic polling mode to wait for write enabling ---- */  
  sCommand.Instruction = READ_STATUS_REG_CMD;
  sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Match = WRITE_ENABLE_MATCH_VALUE;
  sConfig.Mask  = WRITE_ENABLE_MASK_VALUE;

  if (HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Write Configuration register 2 (with octal mode) --------------- */  
  sCommand.Instruction = WRITE_CFG_REG_2_CMD;
  sCommand.Address     = CONFIG_REG2_ADDR1;  
  sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
      
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  reg = CR2_STR_OPI_ENABLE;

  if (HAL_OSPI_Transmit(hospi, &reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Wait that the configuration is effective and check that memory is ready */
  HAL_Delay(MEMORY_REG_WRITE_DELAY);
  
  /* Wait that the memory is ready ---------------------------------- */
  OSPI_AutoPollingMemReady(hospi);
}

/**
  * @brief  Toggle the GPIOs
  * @param  None
  * @retval None
  */
#if defined(__ICCARM__)
static void GpioToggle(void) @ ".ospi_nor"
#elif defined(__ARMCC_VERSION) || defined(__GNUC__)
static void __attribute__((section(".ospi_nor"), noinline)) GpioToggle(void)
#endif
{
  BSP_LED_Toggle(LED10);
  /* Insert delay 100 ms */
  HAL_Delay(100);
  BSP_LED_Toggle(LED9);
  /* Insert delay 100 ms */
  HAL_Delay(100);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  BSP_LED_On(LED_RED);

  /* User may add here some code to deal with this error */
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
