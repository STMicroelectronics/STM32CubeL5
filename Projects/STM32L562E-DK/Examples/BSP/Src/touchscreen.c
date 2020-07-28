/**
  ******************************************************************************
  * @file    touchscreen.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the touchscreen driver.
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
#define  CIRCLE_RADIUS        20
/* Private macro -------------------------------------------------------------*/
#define  CIRCLE_XPOS(i)       (i * 45)
#define  CIRCLE_YPOS(i)       (240 - CIRCLE_RADIUS - 80)
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Touchscreen_SetHint(void);
static void Touchscreen_DrawBackground (uint8_t state);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Touchscreen Demo
  * @param  None
  * @retval None
  */
void Touchscreen_demo(void)
{
  uint16_t   x, y;
  uint8_t    state = 0;
  uint8_t    text[30];
  uint8_t    radius;
  uint8_t    radius_previous = 0;
  TS_MultiTouch_State_t TsMultipleState = {0};

  Touchscreen_SetHint();

  Touchscreen_DrawBackground(state);
  
  while (1)
  {
    /* Check in interrupt in touch screen the touch status and coordinates    */
      /* if touch occurred                                                      */
    if (TouchDetected == SET)
    {
      TouchDetected = RESET;
      
      if (BSP_TS_Get_MultiTouchState(0, &TsMultipleState) != BSP_ERROR_NONE)
      {
        Error_Handler();
      }
      if(TsMultipleState.TouchDetected >= 1)
      {
        /* Get X and Y position of the touch post calibrated */
        x = TsMultipleState.TouchX[0];
        y = TsMultipleState.TouchY[0];

        UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
        sprintf((char*)text, "Nb touch detected = %lu", (unsigned long)TsMultipleState.TouchDetected);
        UTIL_LCD_DisplayStringAt(15, 185, (uint8_t *)&text, LEFT_MODE);

        /* Display 1st touch detected coordinates */
        sprintf((char*)text, "1[%lu,%lu]    ", (unsigned long)x, (unsigned long)y);
        UTIL_LCD_DisplayStringAt(15, 200, (uint8_t *)&text, LEFT_MODE);
        
        if (TsMultipleState.TouchDetected >= 2)  /* Display 2nd touch detected coordinates if applicable */
        {
          sprintf((char*)text, "2[%lu,%lu]    ", (unsigned long)TsMultipleState.TouchX[1], (unsigned long)TsMultipleState.TouchY[1]);
          UTIL_LCD_DisplayStringAt(15, 215, (uint8_t *)&text, LEFT_MODE);
        }
        else
        {
          sprintf((char*)text, "              ");
          UTIL_LCD_DisplayStringAt(15, 215, (uint8_t *)&text, LEFT_MODE);
        }
        
        /* Weight not supported so radius maximum */
        radius = CIRCLE_RADIUS;

        if ((y > (CIRCLE_YPOS(1) - CIRCLE_RADIUS)) &&
            (y < (CIRCLE_YPOS(1) + CIRCLE_RADIUS)))
        {

          if ((x > (CIRCLE_XPOS(1) - CIRCLE_RADIUS)) &&
              (x < (CIRCLE_XPOS(1) + CIRCLE_RADIUS)))
          {
            if ((radius != radius_previous) || (state != 1))
            {
              if (state != 1) /* Erase previous filled circle */
              {
                Touchscreen_DrawBackground(state);
              }
              UTIL_LCD_FillCircle(CIRCLE_XPOS(1), CIRCLE_YPOS(1), radius, UTIL_LCD_COLOR_BLUE);
              radius_previous = radius;
              state = 1;
            }
          }
          if ((x > (CIRCLE_XPOS(2) - CIRCLE_RADIUS)) &&
              (x < (CIRCLE_XPOS(2) + CIRCLE_RADIUS)))
          {
            if ((radius != radius_previous) || (state != 2))
            {
              if (state != 2) /* Erase previous filled circle */
              {
                Touchscreen_DrawBackground(state);
              }
              UTIL_LCD_FillCircle(CIRCLE_XPOS(2), CIRCLE_YPOS(2), radius, UTIL_LCD_COLOR_RED);
              radius_previous = radius;
              state = 2;
            }
          }

          if ((x > (CIRCLE_XPOS(3) - CIRCLE_RADIUS)) &&
              (x < (CIRCLE_XPOS(3) + CIRCLE_RADIUS)))
          {
            if ((radius != radius_previous) || (state != 4))
            {
              if (state != 4) /* Erase previous filled circle */
              {
                Touchscreen_DrawBackground(state);
              }
              UTIL_LCD_FillCircle(CIRCLE_XPOS(3), CIRCLE_YPOS(3), radius, UTIL_LCD_COLOR_YELLOW);
              radius_previous = radius;
              state = 4;
            }
          }

          if ((x > (CIRCLE_XPOS(4) - CIRCLE_RADIUS)) &&
              (x < (CIRCLE_XPOS(4) + CIRCLE_RADIUS)))
          {
            if ((radius != radius_previous) || (state != 8))
            {
              if (state != 8) /* Erase previous filled circle */
              {
                Touchscreen_DrawBackground(state);
              }
              UTIL_LCD_FillCircle(CIRCLE_XPOS(4), CIRCLE_YPOS(3), radius, UTIL_LCD_COLOR_GREEN);
              radius_previous = radius;
              state = 8;
            }
          }
        }
      } /* of if(TS_State.touchDetected) */
    }

    if (UserButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      HAL_Delay(500);
      UserButtonPressed = RESET;
      
      return;
    }
  }
}

/**
  * @brief  Display TS Demo Hint
  * @param  None
  * @retval None
  */
static void Touchscreen_SetHint(void)
{
  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set Touchscreen Demo description */
  UTIL_LCD_FillRect(0, 0, 240, 80, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"Touchscreen", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"Please use the Touchscreen to fill", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t *)"the colored circles. Up to 2", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"touch coordinates are displayed.", CENTER_MODE);

  UTIL_LCD_DrawRect(10, 90, 220, 140, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 91, 218, 138, UTIL_LCD_COLOR_BLUE);
}

/**
  * @brief  Draw Touchscreen Background
  * @param  state : touch zone state
  * @retval None
  */
static void Touchscreen_DrawBackground (uint8_t state)
{

  switch (state)
  {

    case 0:
      UTIL_LCD_FillCircle(CIRCLE_XPOS(1), CIRCLE_YPOS(1), CIRCLE_RADIUS, UTIL_LCD_COLOR_BLUE);
      UTIL_LCD_FillCircle(CIRCLE_XPOS(2), CIRCLE_YPOS(2), CIRCLE_RADIUS, UTIL_LCD_COLOR_RED);
      UTIL_LCD_FillCircle(CIRCLE_XPOS(3), CIRCLE_YPOS(3), CIRCLE_RADIUS, UTIL_LCD_COLOR_YELLOW);
      UTIL_LCD_FillCircle(CIRCLE_XPOS(4), CIRCLE_YPOS(4), CIRCLE_RADIUS, UTIL_LCD_COLOR_GREEN);

      UTIL_LCD_FillCircle(CIRCLE_XPOS(1), CIRCLE_YPOS(1), CIRCLE_RADIUS - 2, UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_FillCircle(CIRCLE_XPOS(2), CIRCLE_YPOS(2), CIRCLE_RADIUS - 2, UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_FillCircle(CIRCLE_XPOS(3), CIRCLE_YPOS(3), CIRCLE_RADIUS - 2, UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_FillCircle(CIRCLE_XPOS(4), CIRCLE_YPOS(4), CIRCLE_RADIUS - 2, UTIL_LCD_COLOR_WHITE);
      break;

    case 1:
      UTIL_LCD_FillCircle(CIRCLE_XPOS(1), CIRCLE_YPOS(1), CIRCLE_RADIUS, UTIL_LCD_COLOR_BLUE);
      UTIL_LCD_FillCircle(CIRCLE_XPOS(1), CIRCLE_YPOS(1), CIRCLE_RADIUS - 2, UTIL_LCD_COLOR_WHITE);
      break;

    case 2:
      UTIL_LCD_FillCircle(CIRCLE_XPOS(2), CIRCLE_YPOS(2), CIRCLE_RADIUS, UTIL_LCD_COLOR_RED);
      UTIL_LCD_FillCircle(CIRCLE_XPOS(2), CIRCLE_YPOS(2), CIRCLE_RADIUS - 2, UTIL_LCD_COLOR_WHITE);
      break;

    case 4:
      UTIL_LCD_FillCircle(CIRCLE_XPOS(3), CIRCLE_YPOS(3), CIRCLE_RADIUS, UTIL_LCD_COLOR_YELLOW);
      UTIL_LCD_FillCircle(CIRCLE_XPOS(3), CIRCLE_YPOS(3), CIRCLE_RADIUS - 2, UTIL_LCD_COLOR_WHITE);
      break;

    case 8:
      UTIL_LCD_FillCircle(CIRCLE_XPOS(4), CIRCLE_YPOS(4), CIRCLE_RADIUS, UTIL_LCD_COLOR_GREEN);
      UTIL_LCD_FillCircle(CIRCLE_XPOS(4), CIRCLE_YPOS(4), CIRCLE_RADIUS - 2, UTIL_LCD_COLOR_WHITE);
      break;

  }
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
