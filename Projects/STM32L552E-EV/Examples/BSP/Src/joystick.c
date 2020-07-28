/**
  ******************************************************************************
  * @file    joystick.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the joystick feature in the 
  *          STM32L552E-EV driver
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
static void Joystick_SetHint(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Joystick demo
  * @param  None
  * @retval None
  */
void Joystick_demo(void)
{ 
  static uint16_t xPtr = 12;
  static uint16_t yPtr = 92;
  static uint16_t old_xPtr = 12;
  static uint16_t old_yPtr = 92;  

  Joystick_SetHint();
  
  /* Initial cursor display */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
    
  UTIL_LCD_DisplayChar(xPtr, yPtr, 'X');   
  
  while (1)
  {
    /* Initial Joystick state set following MFX interrupt */
    if(JoyPinPressed != JOY_NONE)
    {
      /* Wait 1 ms to manage debounce */
      HAL_Delay(1);

      switch(JoyPinPressed)
      {
      case JOY_UP:
        if(yPtr > 92)
        {
          yPtr--;
        }
        break;     
      case JOY_DOWN:
        if(yPtr < 217)
        {
          yPtr++;
        }
        break;          
      case JOY_LEFT:
        if(xPtr > 12)
        {
          xPtr--;
        }
        break;         
      case JOY_RIGHT:
        if(xPtr < 301)
        {
          xPtr++;
        }
        break;                 
      default:
        break;           
      }
    
      if(JoyPinPressed == JOY_SEL)
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        UTIL_LCD_DisplayChar(xPtr, yPtr, 'X');
      }
      else
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
        UTIL_LCD_DisplayChar(old_xPtr, old_yPtr, 'X');
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
        UTIL_LCD_DisplayChar(xPtr, yPtr, 'X');

        old_xPtr = xPtr;
        old_yPtr = yPtr;
      }
      
      JoyPinPressed = JOY_NONE;
    }
    
    if(WakeupButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      HAL_Delay(500);
      WakeupButtonPressed = RESET;

      return;
    }
  }
}


/**
  * @brief  Display joystick demo hint
  * @param  None
  * @retval None
  */
static void Joystick_SetHint(void)
{
  /* Clear the LCD */ 
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  
  /* Set Joystick Demo description */
  UTIL_LCD_FillRect(0, 0, 320, 80, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE); 
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"Joystick", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"Please use the joystick to move", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t *)"the pointer inside the rectangle", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"Press Wakeup button to switch to next menu", CENTER_MODE);
  
  UTIL_LCD_DrawRect(10, 90, 300, 140, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 91, 298, 138, UTIL_LCD_COLOR_BLUE);
}
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
