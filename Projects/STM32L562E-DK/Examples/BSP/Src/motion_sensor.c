/**
  ******************************************************************************
  * @file    motion_sensor.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the motion sensor feature in the 
  *          BSP driver
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
#include "main.h"
#include "stdio.h"
#include "string.h"

/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void MotionSensor_SetHint(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Motion sensor demo
  * @param  None
  * @retval None
  */
void MotionSensor_demo(void)
{
  BSP_MOTION_SENSOR_Axes_t         axes;
  BSP_MOTION_SENSOR_AxesRaw_t      axesRaw;
  uint8_t string[50] = {0};

  MotionSensor_SetHint();

  /*************************/
  /* Test of Accelerometer */
  /*************************/

  /* Initialize function */
  if (BSP_MOTION_SENSOR_Init(0, MOTION_ACCELERO) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Enable function */
  if (BSP_MOTION_SENSOR_Enable(0, MOTION_ACCELERO) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  
  /* GetAxes function */
  UTIL_LCD_DisplayStringAt(20, 100, (uint8_t*) "Accelerometer axes", LEFT_MODE);
  while (UserButtonPressed == RESET)
  {
    if (BSP_MOTION_SENSOR_GetAxes(0, MOTION_ACCELERO, &axes) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    if (BSP_MOTION_SENSOR_GetAxesRaw(0, MOTION_ACCELERO, &axesRaw) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    sprintf((char*)string, "X = %ld      ", (signed long)axes.x);
    UTIL_LCD_DisplayStringAt(20, 115, string, LEFT_MODE);
    sprintf((char*)string, "Y = %ld      ", (signed long)axes.y);
    UTIL_LCD_DisplayStringAt(20, 130, string, LEFT_MODE);
    sprintf((char*)string, "Z = %ld      ", (signed long)axes.z);
    UTIL_LCD_DisplayStringAt(20, 145, string, LEFT_MODE);
    sprintf((char*)string, "Xraw = %d      ", axesRaw.x);
    UTIL_LCD_DisplayStringAt(20, 160, string, LEFT_MODE);
    sprintf((char*)string, "Yraw = %d      ", axesRaw.y);
    UTIL_LCD_DisplayStringAt(20, 175, string, LEFT_MODE);
    sprintf((char*)string, "Zraw = %d      ", axesRaw.z);
    UTIL_LCD_DisplayStringAt(20, 190, string, LEFT_MODE);
    HAL_Delay(500);
  }
  HAL_Delay(1000);
  UserButtonPressed = RESET;

  /* Disable function */
  if (BSP_MOTION_SENSOR_Disable(0, MOTION_ACCELERO) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* De-initialize function */
  if (BSP_MOTION_SENSOR_DeInit(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /*********************/
  /* Test of Gyroscope */
  /*********************/

  MotionSensor_SetHint();

  /* Initialize function */
  if (BSP_MOTION_SENSOR_Init(0, MOTION_GYRO) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Enable function */
  if (BSP_MOTION_SENSOR_Enable(0, MOTION_GYRO) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  
  /* GetAxes function */
  UTIL_LCD_DisplayStringAt(20, 100, (uint8_t*) "Gyroscope axes", LEFT_MODE);
  while (UserButtonPressed == RESET)
  {
    if (BSP_MOTION_SENSOR_GetAxes(0, MOTION_GYRO, &axes) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    if (BSP_MOTION_SENSOR_GetAxesRaw(0, MOTION_GYRO, &axesRaw) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    sprintf((char*)string, "X = %ld      ", (signed long)axes.x);
    UTIL_LCD_DisplayStringAt(20, 115, string, LEFT_MODE);
    sprintf((char*)string, "Y = %ld      ", (signed long)axes.y);
    UTIL_LCD_DisplayStringAt(20, 130, string, LEFT_MODE);
    sprintf((char*)string, "Z = %ld      ", (signed long)axes.z);
    UTIL_LCD_DisplayStringAt(20, 145, string, LEFT_MODE);
    sprintf((char*)string, "Xraw = %d      ", axesRaw.x);
    UTIL_LCD_DisplayStringAt(20, 160, string, LEFT_MODE);
    sprintf((char*)string, "Yraw = %d      ", axesRaw.y);
    UTIL_LCD_DisplayStringAt(20, 175, string, LEFT_MODE);
    sprintf((char*)string, "Zraw = %d      ", axesRaw.z);
    UTIL_LCD_DisplayStringAt(20, 190, string, LEFT_MODE);
    HAL_Delay(500);
  }
  HAL_Delay(1000);
  UserButtonPressed = RESET;

  /* Disable function */
  if (BSP_MOTION_SENSOR_Disable(0, MOTION_GYRO) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  
  /* De-initialize function */
  if (BSP_MOTION_SENSOR_DeInit(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /***************/
  /* End of test */
  /***************/
}

/**
  * @brief  Display motion sensor demo hint
  * @param  None
  * @retval None
  */
static void MotionSensor_SetHint(void)
{
  /* Clear the LCD */ 
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  
  /* Set Audio Demo description */
  UTIL_LCD_FillRect(0, 0, 240, 80, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE); 
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"Motion Sensor", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"Move the board.", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t *)"Press User push-button", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"for next step", CENTER_MODE);
  
  UTIL_LCD_DrawRect(10, 90, 220, 140, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 91, 218, 138, UTIL_LCD_COLOR_BLUE);

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE); 
}

/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
