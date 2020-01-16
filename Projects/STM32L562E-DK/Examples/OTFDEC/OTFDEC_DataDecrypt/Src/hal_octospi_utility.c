/**
  ******************************************************************************
  * @file    OTFDEC/OTFDEC_DataDecrypt/Src/hal_octospi_utility.c
  * @author  MCD Application Team
  * @brief   This file provides helper to initialize OCTOSPI/externalFlash.
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

/* Includes ------------------------------------------------------------------*/
#include "hal_octospi_utility.h"
#include "macronix.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern OSPI_HandleTypeDef hospi1;

/* Private functions ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void OSPI_ResetMemory(OSPI_HandleTypeDef *hospi);
static void OSPI_WriteEnable(OSPI_HandleTypeDef *hospi);
static void OSPI_AutoPollingMemReady(OSPI_HandleTypeDef *hospi);
static void OSPI_OctalDtrModeCfg(OSPI_HandleTypeDef *hospi);

/**
  * @brief  This function configure the external memory.
  * @param  None
  * @retval None
  */
uint32_t OSPI_Config(void)
{
  OSPI_ResetMemory(&hospi1);
  OSPI_OctalDtrModeCfg(&hospi1);

  return 0;
}

/**
  * @brief  This function write data to the external memory.
  * @param  pBuffer Memory address of the source buffer
  * @param  FlashAddress Address in the external memory
  * @param  Size Size of the buffer to be transferred
  * @retval None
  */
uint32_t OSPI_Write(uint32_t * pBuffer, uint32_t FlashAddress, uint32_t Size)
{
  OSPI_RegularCmdTypeDef  sCommand;
  uint32_t Size_temp = 0;

  if (Size == 0)
  {
    Error_Handler();
  }
  else
  {
    Size_temp = Size * 4;  /* Size converted in bytes */
  }

  /* 1- Enable write operations ----------------------------------------- */
  OSPI_WriteEnable(&hospi1);

  /* 2- Erasing Sequence ------------------------------------------------ */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = OCTAL_SECTOR_ERASE_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_ENABLE;
  sCommand.Address            = FlashAddress;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_8_LINES;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_ENABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_ENABLE;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Reconfigure OSPI to automatic polling mode to wait for end of erase */
  OSPI_AutoPollingMemReady(&hospi1);

  while (Size_temp)
  {
    /* 3- Enable write operations ----------------------------------------- */
    OSPI_WriteEnable(&hospi1);

    /* 4- Writing Sequence ------------------------------------------------ */
    sCommand.Instruction = OCTAL_PAGE_PROG_CMD;
    sCommand.Address     = FlashAddress;
    sCommand.DataMode    = HAL_OSPI_DATA_8_LINES;
    sCommand.NbData      = (Size_temp > OSPI_PAGE_SIZE)? OSPI_PAGE_SIZE: Size_temp;

    if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_OSPI_Transmit(&hospi1, (uint8_t *)pBuffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }

    if (Size_temp > OSPI_PAGE_SIZE)
    {
      Size_temp -= OSPI_PAGE_SIZE;
      pBuffer = pBuffer + OSPI_PAGE_SIZE/4; /* "/4" because pBuffer is of type "uint32_t *" */
      FlashAddress = FlashAddress + OSPI_PAGE_SIZE;
    }
    else
    {
      Size_temp = 0;
    }

    /* Reconfigure OSPI to automatic polling mode to wait for end of program */
    OSPI_AutoPollingMemReady(&hospi1);

  }

  return 0;
}

/**
  * @brief  This function set memory mapped mode.
  * @param  None
  * @retval None
  */
uint32_t OSPI_MemoryMap(void)
{
  OSPI_RegularCmdTypeDef sCommand;
  OSPI_MemoryMappedTypeDef sMemMappedCfg;

  if (HAL_OSPI_SetFifoThreshold(&hospi1, 8) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize the command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_WRITE_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = OCTAL_PAGE_PROG_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_ENABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_8_LINES;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_ENABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_8_LINES;
  sCommand.NbData             = 1; 
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_ENABLE;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_ENABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sCommand.OperationType = HAL_OSPI_OPTYPE_READ_CFG;
  sCommand.Instruction   = OCTAL_IO_DTR_READ_CMD;
  sCommand.DummyCycles   = DUMMY_CLOCK_CYCLES_READ;

  if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_DISABLE;

  if (HAL_OSPI_MemoryMapped(&hospi1, &sMemMappedCfg))
  {
    Error_Handler();
  }

  return 0;
}

/**
  * @brief  This function reset the OSPI memory.
  * @param  hospi OSPI handle
  * @retval None
  */
static void OSPI_ResetMemory(OSPI_HandleTypeDef *hospi)
{
  uint8_t id[20];
  OSPI_RegularCmdTypeDef  sCommand;

  /* Initialize the reset enable command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  sCommand.Instruction = RESET_ENABLE_CMD;
  /* Send the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Send the reset memory command */
  sCommand.Instruction = RESET_MEMORY_CMD;
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_Delay(MEMORY_REG_WRITE_DELAY);

  sCommand.Instruction = READ_ID_CMD;
  sCommand.NbData      = 20;
  sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_OSPI_Receive(hospi, id, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (id[0] != 0xC2)
  {
    /* The memory was not in SPI, so the reset has not been taken in account */
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_8_LINES;
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_16_BITS;
    sCommand.DataMode        = HAL_OSPI_DATA_NONE;
    
    sCommand.Instruction = OCTAL_RESET_ENABLE_CMD;
    /* Send the command */
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }
    
    /* Send the reset memory command */
    sCommand.Instruction = OCTAL_RESET_MEMORY_CMD;
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }
    
    HAL_Delay(MEMORY_REG_WRITE_DELAY);
    
    sCommand.Instruction     = READ_ID_CMD;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.NbData          = 20;
    sCommand.DataMode        = HAL_OSPI_DATA_1_LINE;
    
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }
    
    if (HAL_OSPI_Receive(hospi, id, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }
    
    if (id[0] != 0xC2)
    {
      /* The memory was not in SOPI, so the reset has not been taken in account */
      sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
      sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
      sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_ENABLE;
      sCommand.DataMode           = HAL_OSPI_DATA_NONE;
      
      sCommand.Instruction = OCTAL_RESET_ENABLE_CMD;
      /* Send the command */
      if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        Error_Handler();
      }
      
      /* Send the reset memory command */
      sCommand.Instruction = OCTAL_RESET_MEMORY_CMD;
      if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        Error_Handler();
      }
      
      HAL_Delay(MEMORY_REG_WRITE_DELAY);
      
      sCommand.Instruction        = READ_ID_CMD;
      sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
      sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
      sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
      sCommand.NbData             = 20;
      sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
      
      if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        Error_Handler();
      }
      
      if (HAL_OSPI_Receive(hospi, id, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        Error_Handler();
      }
      
      if (id[0] != 0xC2)
      {
        /* The memory was not in DOPI, so the reset has not been taken in account */
        /* Error because the memory is in an unknown configuration */
        Error_Handler();
      }
    }
  }
}
/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hospi OSPI handle
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
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_ENABLE;
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
  sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_ENABLE;
  sCommand.DataMode       = HAL_OSPI_DATA_8_LINES;
  sCommand.DataDtrMode    = HAL_OSPI_DATA_DTR_ENABLE;
  sCommand.NbData         = 2; 
  sCommand.DummyCycles    = DUMMY_CLOCK_CYCLES_READ_REG;
  sCommand.DQSMode        = HAL_OSPI_DQS_ENABLE;

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
  * @param  hospi OSPI handle
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
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_ENABLE;
  sCommand.Address            = 0x0;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_8_LINES;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_ENABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_8_LINES;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_ENABLE;
  sCommand.NbData             = 2; 
  sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_READ_REG;
  sCommand.DQSMode            = HAL_OSPI_DQS_ENABLE;
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
  * @brief  This function configure the memory in Octal DTR mode.
  * @param  hospi OSPI handle
  * @retval None
  */
static void OSPI_OctalDtrModeCfg(OSPI_HandleTypeDef *hospi)
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

  reg = CR2_DTR_OPI_ENABLE;

  if (HAL_OSPI_Transmit(hospi, &reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Wait that the configuration is effective and check that memory is ready */
  HAL_Delay(MEMORY_REG_WRITE_DELAY);
  
  /* Wait that the memory is ready ---------------------------------- */
  OSPI_AutoPollingMemReady(hospi);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
