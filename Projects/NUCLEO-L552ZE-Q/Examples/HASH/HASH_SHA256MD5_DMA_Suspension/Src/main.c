/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define HIGH_PRIO_INPUT_TAB_SIZE   ((uint32_t) 261)      /* The size of the input data "aInput_HighPrio" */
#define LOW_PRIO_LARGE_INPUT_TAB_SIZE  ((uint32_t) 3797) /* The size of the input data "aInput_LowPrio_Large" */
#define KEY_TAB_SIZE               ((uint32_t) 261)      /* The size of the key */

#define HASHTimeout        0xFF

#define MD5_DIGEST_LENGTH    16
#define SHA256_DIGEST_LENGTH 32
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
HASH_HandleTypeDef hhash;
DMA_HandleTypeDef hdma_hash_in;

/* USER CODE BEGIN PV */
__ALIGN_BEGIN uint8_t aInput_HighPrio[HIGH_PRIO_INPUT_TAB_SIZE] __ALIGN_END =
                        {0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,
                         0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                         0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                         0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                         0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                         0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                         0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                         0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                         0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                         0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                         0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                         0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                         0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                         0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                         0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                         0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                         0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                         0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                         0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                         0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                         0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                         0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                         0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                         0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                         0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                         0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                         0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                         0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                         0x32,0x20,0x2a,0x2a,0x2a};

__ALIGN_BEGIN uint8_t aInput_LowPrio_Large[LOW_PRIO_LARGE_INPUT_TAB_SIZE] __ALIGN_END =                         
                        {0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,
                         0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                         0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                         0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                         0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                         0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                         0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                         0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                         0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                         0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                         0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                         0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                         0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                         0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                         0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                         0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                         0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                         0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                         0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                         0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                         0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                         0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                         0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                         0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                         0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                         0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                         0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                         0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                         0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                         0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                         0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                         0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                         0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                         0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                         0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                         0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                         0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                         0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                         0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                         0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                         0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                         0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                         0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                         0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                         0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                         0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                         0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                         0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                         0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                         0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                         0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                         0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                         0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                         0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                         0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                         0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                         0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                         0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                         0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                         0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                         0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                         0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                         0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                         0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                         0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                         0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                         0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                         0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                         0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                         0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                         0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                         0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                         0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                         0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                         0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                         0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                         0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                         0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                         0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                         0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                         0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                         0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                         0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                         0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                         0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                         0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                         0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                         0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                         0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                         0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                         0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                         0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                         0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                         0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                         0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                         0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                         0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                         0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                         0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                         0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                         0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                         0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                         0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                         0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                         0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                         0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                         0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                         0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                         0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                         0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                         0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                         0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                         0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                         0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                         0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                         0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                         0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                         0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                         0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                         0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                         0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                         0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                         0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                         0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                         0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                         0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                         0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                         0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                         0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                         0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                         0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                         0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                         0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                         0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                         0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                         0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                         0x32,0x20,0x2a,0x2a,0x2a};

__ALIGN_BEGIN uint8_t aKey[KEY_TAB_SIZE] __ALIGN_END =
                        {0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,
                         0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                         0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                         0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                         0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                         0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                         0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                         0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                         0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                         0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                         0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                         0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                         0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                         0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                         0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                         0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                         0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                         0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                         0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                         0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                         0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                         0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                         0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                         0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                         0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                         0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                         0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                         0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                         0x32,0x20,0x2a,0x2a,0x2a};

__ALIGN_BEGIN static uint8_t aSHA256Digest[SHA256_DIGEST_LENGTH] __ALIGN_END;
__ALIGN_BEGIN static uint8_t aMD5Digest[MD5_DIGEST_LENGTH] __ALIGN_END;

__ALIGN_BEGIN static uint8_t aExpectSHA256Digest[SHA256_DIGEST_LENGTH] __ALIGN_END =
                             {0x31, 0x48, 0x37, 0x63, 0x4e, 0xf8, 0x06, 0xfb,
                              0x09, 0x28, 0x4b, 0xea, 0x85, 0x82, 0xe0, 0x1b,
                              0xab, 0x12, 0xe7, 0xa4, 0x63, 0xe2, 0x03, 0xc0,
                              0xa7, 0x0a, 0xa1, 0x10, 0x3c, 0x5b, 0x8d, 0xbe};
__ALIGN_BEGIN static uint8_t aExpectMD5Digest[MD5_DIGEST_LENGTH] __ALIGN_END =
                             {0x87, 0x7e, 0xbb, 0xbf, 0xc8, 0xa9, 0xc0, 0x85,
                              0x78, 0xa2, 0xc3, 0x31, 0xd6, 0x7e, 0xc7, 0x0e};

uint8_t Storage_Buffer[57 * 4] = {0};   /* 57 32-bit words are saved */

HASH_HandleTypeDef hhash_saved;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DMA_Init(void);
static void MX_ICACHE_Init(void);
static void MX_HASH_Init(void);
/* USER CODE BEGIN PFP */
static void HigherPriorityBlock_DMA_Processing(void);
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
  uint32_t process_ongoing = 1;     /* low priority block processing flag         */
  uint32_t suspension_trigger = 1;  /* low priority block suspension request flag */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Configure LEDs */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_ICACHE_Init();
  MX_HASH_Init();
  /* USER CODE BEGIN 2 */
  /****************************************************************************/
  /*                                                                          */
  /*              DMA-based HASH processing suspension/resumption             */
  /*                                                                          */
  /****************************************************************************/

  /* Set low priority block processing and suspension request flags */
  process_ongoing = 1;
  suspension_trigger = 1;
  
  /*************************************************/
  /*                                               */
  /* Start SHA256 processing of low priority block */
  /*                                               */ 
  /*************************************************/   
  do
  {
  
    if (HAL_HASHEx_SHA256_Start_DMA(&hhash, aInput_LowPrio_Large, LOW_PRIO_LARGE_INPUT_TAB_SIZE) != HAL_OK)
    {
      Error_Handler();
    }

    /* While the HASH process in on-going, a suspension request occurs
      (it may be triggered by an interruption in user code). In this example, 
      the suspension request is triggered by the flag suspension_trigger */
    if (suspension_trigger == 1)
    {
      /* Request to suspend the low priority HASH processing */
      if (HAL_HASH_DMAFeed_ProcessSuspend(&hhash) != HAL_OK)
      {
        Error_Handler();
      }   
      /* In this example, no more suspension will be requested for the low priority
          block SHA-25 processing */          
      suspension_trigger = 0; 
    }      
    
    
    /* Wait for the HASH handle to exit BUSY state */    
    while ((HAL_HASH_GetState(&hhash) != HAL_HASH_STATE_SUSPENDED)
           && (HAL_HASH_GetState(&hhash) != HAL_HASH_STATE_READY)); 
    
    
    /* If State is READY, low priority block processing is over,
       if State is SUSPENDED, high priority block is processed and
       low priority block processing is resumed afterwards. */              
    if (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_READY)
    {
      process_ongoing = 0;
    } 
    else if (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_SUSPENDED)
    {
      /* Context saving operation */
      HAL_HASH_ContextSaving(&hhash, Storage_Buffer);
      
      /* Save low priority block HASH handle parameters */
      hhash_saved = hhash;
      
      /* Process high priority block (in DMA mode) */      
      HigherPriorityBlock_DMA_Processing();
      
      /* Check high priority block processing result */      
      if (memcmp(aMD5Digest, aExpectMD5Digest, MD5_DIGEST_LENGTH) != 0)
      {
        Error_Handler();
      }
      
      /* Context restoring operation */
      HAL_HASH_ContextRestoring(&hhash, Storage_Buffer);
      hhash = hhash_saved;
    } 
    else
    {
      Error_Handler();
    }    
    
  } while (process_ongoing);       
  
  /* Wrap-up low priority block processing with DMA transfer in reading the computed digest */
  if (HAL_HASHEx_SHA256_Finish(&hhash, aSHA256Digest, HASHTimeout) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Check low priority block processing result */    
  if(memcmp(aSHA256Digest, aExpectSHA256Digest, SHA256_DIGEST_LENGTH) != 0)
  {
    Error_Handler();
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
  * @brief HASH Initialization Function
  * @param None
  * @retval None
  */
static void MX_HASH_Init(void)
{

  /* USER CODE BEGIN HASH_Init 0 */

  /* USER CODE END HASH_Init 0 */

  /* USER CODE BEGIN HASH_Init 1 */

  /* USER CODE END HASH_Init 1 */
  hhash.Init.DataType = HASH_DATATYPE_8B;
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN HASH_Init 2 */

  /* USER CODE END HASH_Init 2 */

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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel7_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel7_IRQn);

}

/* USER CODE BEGIN 4 */
/**
  * @brief High priority block HMAC-MD5 digest computation 
  * @note This function is executed when the low priority block processing
  *        has been suspended and its context saved (DMA mode)   
  * @note High priority block is input buffer aInput_HighPrio, digest is 
  *       saved in aMD5Digest.             
  * @param  None
  * @retval None
  */
static void HigherPriorityBlock_DMA_Processing(void)
{

  /****************************************************************************/

  /* Use same HASH handle as low priority block,
     set only required parameters, other parameters remain the same
     (e.g. DataType is still set to HASH_DATATYPE_8B */
  hhash.Init.pKey = aKey;
  hhash.Init.KeySize = KEY_TAB_SIZE;
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }

  /* High priority block processing */
  if (HAL_HMAC_MD5_Start_DMA(&hhash, aInput_HighPrio, HIGH_PRIO_INPUT_TAB_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
  while (HAL_HASH_GetState(&hhash) == HAL_HASH_STATE_BUSY);

  if (HAL_HASH_MD5_Finish(&hhash, aMD5Digest, HASHTimeout) != HAL_OK)
  {
    Error_Handler();
  }

  return;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Turn LED_RED on */
  BSP_LED_On(LED_RED);
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
  while (1)
  {
  } 
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
