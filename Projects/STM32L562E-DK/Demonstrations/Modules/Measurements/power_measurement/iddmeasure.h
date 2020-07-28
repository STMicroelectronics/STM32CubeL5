/**
  ******************************************************************************
  * @file    iddmeasure.h
  * @author  MCD Application Team
  * @brief   Idd Measurement functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license SLA0044,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        http://www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IDD_MEASURE_H
#define __IDD_MEASURE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stddef.h>

/** @addtogroup  IDD_MEASURE_MODULE
  * @{
  */

/** @defgroup  IDD_MEASURE
  * @brief Idd Measure routines
  * @{
  */

/* Exported defines -----------------------------------------------------------*/
#define IDD_VALUE_STRING_SIZE 4
#define IDD_UNIT_STRING_SIZE 2

/* Exported typedef -----------------------------------------------------------*/
typedef enum
{
  IDD_RUN = 0,
  IDD_SLEEP,
  IDD_LPR_2MHZ,
  IDD_LPR_SLEEP,
  IDD_STOP2,
  IDD_STANDBY,
  IDD_GETIDD_AFTER_RESET, /* Keep IDD_GETIDD_AFTER_RESET at the end of the enum list */
} IddAction_t;

#define IDD_TEST_NB  IDD_GETIDD_AFTER_RESET

typedef struct
{
  char value[IDD_VALUE_STRING_SIZE + 1];  /* 3 significant digit only */
  char unit[IDD_UNIT_STRING_SIZE + 1];    /* 2 letters */
  uint32_t value_na;                      /* value in nano amps */
  uint8_t error_code;                     /* 0 means no error else see mfx documentation
                                             In case of error please retry */
  IddAction_t power_mode;                 /* Power mode tested */
} IddValue_t;


typedef struct
{
  void   (*IddEnter)(void);
  char modeStr[20];
  IddAction_t action;
} IddAppli_t;

typedef enum
{
  IDD_STATE_IDLE = 0,
  IDD_STATE_MEASURE_ON_GOING = 1,
  IDD_STATE_MEASURE_OK = 2,
  IDD_STATE_MEASURE_ERROR = 3,
} IddState_t;

/* Exported macros ------------------------------------------------------------*/
/* Exported variables ---------------------------------------------------------*/
extern uint32_t   IddTestIndex;
extern IddAppli_t IddTest[];

/* Exported functions ---------------------------------------------------------*/
void Idd_ExecuteAction(uint32_t IddIndex);
void Idd_GetValue(IddValue_t *pIddValue);
IddState_t Idd_GetState(void);
uint32_t Idd_IsSignature(void); /* Automatically restore index if TRUE */
void Idd_ClearSignature(void);


/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __IDD_MEASURE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

