/**
  ******************************************************************************
  * @file    test_protections.c
  * @author  MCD Application Team
  * @brief   Test Protections module.
  *          This file provides set of firmware functions to manage Test Protections
  *          functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
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
#include "mpu_armv8m_drv.h"
#include "string.h"
#include "stm32l5xx_hal.h"
#include "test_protections.h"
#include "com.h"
#include "common.h"
#include "region_defs.h"

/** @addtogroup TEST_PROTECTIONS Test protections
  * @{
  */

/** @defgroup  TEST_PROTECTIONS_Private_Defines Private Defines
  * @{
  */

/** @defgroup  TEST_PROTECTIONS_Private_Variables Private Variables
  * @{
  */


/* Automatic tests : list of tests (Address, Message, Operation */
/* Flash programming by 64 bits */
#define SHIFT_FLASH_WRITE (8U-1U)

const TestProtection_t aProtectTests[] =
{

  {NS_RAM_ALIAS(NS_TOTAL_RAM_SIZE), "Data Secure Start", TEST_READ_RAM, NOT_APPLICABLE},
  {NS_RAM_ALIAS(NS_TOTAL_RAM_SIZE + S_DATA_SIZE), "Data Secure End", TEST_READ_RAM, NOT_APPLICABLE},
  {S_ROM_ALIAS(S_IMAGE_PRIMARY_AREA_OFFSET), "Code Secure Start", TEST_READ_FLASH, NOT_APPLICABLE},
  {S_ROM_ALIAS(S_IMAGE_PRIMARY_AREA_OFFSET) + FLASH_S_PARTITION_SIZE - 1, "Code Secure END(veneer)", TEST_READ_FLASH, NOT_APPLICABLE},
  { (uint32_t) &((RNG_TypeDef *)RNG_BASE)->SR, "RNG IP SR", TEST_READ_PERIPH, RNG_TEST},
  { (uint32_t) &((RNG_TypeDef *)RNG_BASE)->DR, "RNG IP DR", TEST_READ_PERIPH, RNG_TEST},
  { (uint32_t) &(TAMP->BKP0R), "BACKUP REG 0", TEST_READ_PERIPH, NOT_APPLICABLE },
  { ((uint32_t) &(TAMP->BKP0R) + 28U), "BACKUP REG 7", TEST_READ_PERIPH, NOT_APPLICABLE},
  {0x00000000, "Execution successful", TEST_END}
};

/* Automatic test : list of operation */
const uint8_t aTestOperation[][20] =
{
  "write 8 bytes",
  "read 1 byte",
  "erase 512 bytes",
  "write 4 bytes",
  "read 1 byte",
  "execute",
  "read 4 bytes",
  "end"
};

/**
  * @}
  */

/** @defgroup  TEST_PROTECTIONS_Private_Functions Private Functions
  * @{
  */
static void RDP_REGRESSION_Run(void);
static void TEST_PROTECTIONS_PrintTestingMenu(void);
static void set_periph(PeriphTest, uint32_t on);


/**
  * @}
  */

/** @defgroup  TEST_PROTECTIONS_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup  TEST_PROTECTIONS_Control_Functions Control Functions
  * @{
  */

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  Testnumber giving the test that generate a reset
  * @retval None.
  */

void TEST_PROTECTIONS_Run(void)
{
  uint8_t key = 0U;
  uint8_t exit = 0U;

  /*-1- Print Main Menu message*/
  TEST_PROTECTIONS_PrintTestingMenu();

  while (exit == 0U)
  {
    key = 0U;

    /* Clean the input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)
      {
        case '1' :
          TEST_PROTECTIONS_Run_SecUserMem();
          break;
        case '2' :
          RDP_REGRESSION_Run();
        case 'x' :
          exit = 1U;
          break;

        default:
          printf("Invalid Number !\r");
          break;
      }

      /* Print Main Menu message */
      TEST_PROTECTIONS_PrintTestingMenu();
    }
  }
}

/**
  * @brief Test Access on Secure Area
  * @param None.
  * @retval None.
  */
void TEST_PROTECTIONS_Run_SecUserMem(void)
{
  uint32_t test_idx;
  uint32_t flashErrCode = 0;
  uint64_t pattern = 0U;
  uint32_t page_error = 0U;
  FLASH_EraseInitTypeDef p_erase_init;
  TestStatus status = TEST_IN_PROGRESS;
  __IO uint8_t tmp;
  __IO uint32_t tmp_reg;
  int32_t i;
  void (*func)(void);

  if ((TestNumber & TEST_PROTECTION_MASK) == 0)
  {
    TestNumber = TEST_PROTECTION_MASK;
  }

  do
  {
    /* slow down execution */

    HAL_Delay(100);
    test_idx = TestNumber & 0xffff;
    if (test_idx > (sizeof(aProtectTests) / sizeof(TestProtection_t) -1))
    {
      TestNumber = 0;
      return;
    }
    /*  update to next test  */
    TestNumber = TEST_PROTECTION_MASK | ((test_idx + 1) & 0xffff);
    printf("\r\n= [TEST] %s @ %s %08x", \
           aTestOperation[aProtectTests[test_idx].type], aProtectTests[test_idx].msg, (int)aProtectTests[test_idx].address);
    switch (aProtectTests[test_idx].type)

    {
      /* Trying to write 64bits in FLASH : WRP flag is set for WRP or PCROP protected area
         or reset generated if under FWALL or MPU protection */
      case TEST_WRITE_FLASH :
        HAL_FLASH_Unlock();
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, aProtectTests[test_idx].address, pattern);
        HAL_FLASH_Lock();
        flashErrCode = HAL_FLASH_GetError();
        printf("\r\nflash error code: %x",(int)flashErrCode);
        if ((flashErrCode & HAL_FLASH_ERROR_WRP) == 0U)
        {
          status = TEST_ERROR;
        }

        /*
         * OBs protection test: MPU generates a MEMORY FAULT and a RESET
         * So the test is FAILED if we reach this line
         */
        if (0 == strncmp("OBs @", (const char *)aProtectTests[test_idx].msg, 5))
        {
          status = TEST_ERROR;
        }
        /* else not an OB protection test so probably not an error */
        break;

      /* Trying to read in FLASH : RDP set in case of PCROP protected area
         or reset generated if under FWALL or MPU protection */
      case TEST_READ_FLASH :

        tmp = *(uint8_t *)(aProtectTests[test_idx].address);


        HAL_Delay(1);        /* ensure Flag is set */

        /*
         * When Isolation activated : RESET should be generated
         * So the test is FAILED if we reach this line
         */
        if (0 == strncmp("Isolated", (const char *)aProtectTests[test_idx].msg, 8))
        {
          status = TEST_ERROR;
        }

        break;

      /* Trying to erase 512 bytes in FLASH : WRP flag set for WRP or PCROP protected area
         or reset generated if under FWALL or MPU protection */
      case TEST_ERASE_FLASH :
        HAL_FLASH_Unlock();
        p_erase_init.Banks       = FLASH_BANK_1;
        p_erase_init.TypeErase   = FLASH_TYPEERASE_PAGES;
        p_erase_init.Page        = (aProtectTests[test_idx].address) / 0x800;
        p_erase_init.NbPages     = 1;
        HAL_FLASHEx_Erase(&p_erase_init, &page_error);
        HAL_FLASH_Lock();
        if ((HAL_FLASH_GetError() & HAL_FLASH_ERROR_WRP) == 0U)
        {
          status = TEST_ERROR;
        }
        break;

      /* Trying to write in RAM : reset generated if under FWALL or MPU protection */
      case TEST_WRITE_RAM :
        *(uint32_t *)aProtectTests[test_idx].address = 0x00000000;
        status = TEST_ERROR;
        break;

      /* Trying to read in RAM : reset generated if under FWALL or MPU protection */
      case TEST_READ_RAM :

        tmp = *(uint8_t *)aProtectTests[test_idx].address;

        printf(" value : %d ", tmp);
        status = TEST_ERROR;
        break;

      /* Tryning to execute code : reset generated if under FWALL or MPU protection */
      case TEST_EXECUTE :
        func = (void(*)(void))(aProtectTests[test_idx].address);
        func();
        status = TEST_ERROR;
        break;
      case TEST_READ_PERIPH :
        set_periph(aProtectTests[test_idx].periph, 1);
        for (i = 0; i < 10; i++)
        {
          tmp_reg = *((__IO uint32_t *)aProtectTests[test_idx].address);
          if (tmp_reg != 0)
          {
            break;
          }
        }
        /* error or reading 0 */
        if (tmp_reg != 0)
        {
          status = TEST_ERROR;
          printf(" value : %x ",(int)tmp_reg);
        }
        set_periph(aProtectTests[test_idx].periph, 0);
        break;
      /* End of execution */
      case TEST_END :
        status = TEST_COMPLETED;
        printf("\r\n TEST_PROTECTIONS_Run_SecUserMem : Passed");
        break;

      default :
        break;
    }
  } while (status == TEST_IN_PROGRESS) ;

  if (status == TEST_ERROR)
  {
    printf("\r\n TEST_PROTECTIONS_Run_SecUserMem : Failed");
  }
  else if (status == TEST_COMPLETED)
  {
    /* reset for next run (need a RESET of the platform to restart a session) */
    printf("\b\b\b\b\b\b\b\b");
    TestNumber = 0;            /* no more test in progress */
  }
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup  TEST_PROTECTION_Private_Functions Private Functions
  * @{
  */
/**

  * @brief Set Device in RDP regression state
  * @param None.
  * @retval None.
  */
static void RDP_REGRESSION_Run(void)
{
  struct mpu_armv8m_dev_t dev_mpu_ns = { MPU_BASE };
  struct mpu_armv8m_region_cfg_t region_cfg;

  /* Allow execution in privileged ns data region */
  region_cfg.region_nr = 2U;
  region_cfg.region_base = NS_DATA_START+(~MPU_RBAR_BASE_Msk+1);
  region_cfg.region_limit = NS_DATA_LIMIT;
  region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
  region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_ONLY;
  region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
  region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_OK;
  mpu_armv8m_region_enable(&dev_mpu_ns, &region_cfg);

  __disable_irq();
  {
#define WHILE_1_OPCODE 0xe7fe
    typedef void (*nsfptr_t)(void);
    nsfptr_t nsfptr = (nsfptr_t)(SRAM1_BASE_NS + 1);
    __IO uint16_t *pt = (uint16_t *)SRAM1_BASE_NS;
    /*  copy while(1) instruction */
    *pt = WHILE_1_OPCODE;
    /* Flush and refill pipeline  */
    __DSB();
    __ISB();
    printf("\r\n Device ready for regression : \r\n");
    printf("\r\n - Connect STM32CubeProgrammer mode=HotPlug");
    printf("\r\n - Perform RDP Regression ");
    printf("\r\n ");

    /*  call should not return  */
    nsfptr();
  }
}

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_PrintTestingMenu(void)
{
  printf("\r\n=================== Test Menu ============================\r\n\n");
  printf("  Test Protection : NonSecure try to access to Secure --- 1\r\n\n");
  printf("  RDP Regression ---------------------------------------- 2\r\n\n");
  printf("  Previous Menu ----------------------------------------- x\r\n\n");
}

/**
  * @brief  Enable periph clock require to perform test
  * @param  periph to test.
  * @retval enable 1 , disable 0.
  */
static void set_periph(PeriphTest periph, uint32_t on)
{

  switch (periph)
  {
    case NOT_APPLICABLE:
      break;
    case RNG_TEST:
      if (on)
      {
        __HAL_RCC_RNG_CLK_ENABLE();
      }
      else
        __HAL_RCC_RNG_CLK_DISABLE();
      break;
    default:
      break;
  }

}
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
