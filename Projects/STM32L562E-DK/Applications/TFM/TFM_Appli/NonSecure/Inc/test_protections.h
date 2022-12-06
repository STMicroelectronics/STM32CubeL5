/**
  ******************************************************************************
  * @file    test_protections.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Test Protections functionalities.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_PROTECTIONS_H
#define TEST_PROTECTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"



/** @addtogroup TEST_PROTECTIONS Protections test example
  * @{
  */
/** @defgroup  SFU_TEST_Exported_Constant Exported Constant
  * @{
  */
#define TEST_PROTECTION_MASK 0xdead0000
/* Magic number saved in DR0 backup register */
#define MAGIC_TEST_DONE          0xAAA2
#define MAGIC_TEST_INIT          0xAAA3

#define PROTECT_MSG_LEN (32U)           /* message to be displayed length           */

typedef enum
{
  TEST_WRITE_FLASH = 0U,                /* Writing test in FLASH */
  TEST_READ_FLASH,                      /* Reading test in FLASH */
  TEST_ERASE_FLASH,                     /* Erasing test in FLASH */
  TEST_WRITE_RAM,                       /* Writing test in RAM */
  TEST_READ_RAM,                        /* Reading test in RAM */
  TEST_EXECUTE,                         /* Execution function test */
  TEST_READ_PERIPH,                     /* Read 4 bytes in periph */
  TEST_END,                             /* Last test : Success ! */
} TestType;

typedef enum
{
  TEST_IN_PROGRESS = 0U,                /* Test in progress */
  TEST_COMPLETED,                       /* Test ended : success */
  TEST_ERROR,                           /* Test aborted : error */
} TestStatus;

typedef enum
{
  RNG_TEST,
  NOT_APPLICABLE,
} PeriphTest;

/**
  * @}
  */

/** @defgroup  SFU_TEST_Exported_Types Exported Types
  * @{
  */

typedef struct
{
  uint32_t address;                     /* address to be tested */
  uint8_t msg[PROTECT_MSG_LEN];         /* message to be displayed when testing */
  TestType type;                        /* type of test */
  PeriphTest periph;
} TestProtection_t;

/**
  * @}
  */

/** @addtogroup  TEST_PROTECTION_Exported_Functions
  * @{
  */
void TEST_PROTECTIONS_Run_SecUserMem(void);
void TEST_PROTECTIONS_Run(void);


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* TEST_PROTECTIONS_H */
