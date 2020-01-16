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
#define  CIRCLE_RADIUS        30
/* Private macro -------------------------------------------------------------*/
#define  CIRCLE_XPOS(i)       (i * 64)
#define  CIRCLE_YPOS(i)       (240 - CIRCLE_RADIUS - 60)
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
  TS_State_t TS_State;

  Touchscreen_SetHint();

  Touchscreen_DrawBackground(state);
  
  while (1)
  {
    /* Check in interrupt in touch screen the touch status and coordinates    */
      /* if touch occurred                                                      */
    if (TouchDetected == SET)
    {
      TouchDetected = RESET;
      
      if (BSP_TS_GetState(0, &TS_State) != BSP_ERROR_NONE)
      {
        Error_Handler();
      }
      if(TS_State.TouchDetected)
      {
        /* Get X and Y position of the touch post calibrated */
        x = TS_State.TouchX;
        y = TS_State.TouchY;

        GUI_SetBackColor(GUI_COLOR_WHITE);
        GUI_SetTextColor(GUI_COLOR_BLACK);
        sprintf((char*)text, "Nb touch detected = %lu", (unsigned long)TS_State.TouchDetected);
        GUI_DisplayStringAt(15, 200, (uint8_t *)&text, LEFT_MODE);

        /* Display 1st touch detected coordinates */
        sprintf((char*)text, "1[%d,%d]    ", x, y);
        GUI_DisplayStringAt(15, 215, (uint8_t *)&text, LEFT_MODE);
        
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
              GUI_FillCircle(CIRCLE_XPOS(1), CIRCLE_YPOS(1), radius, GUI_COLOR_BLUE);
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
              GUI_FillCircle(CIRCLE_XPOS(2), CIRCLE_YPOS(2), radius, GUI_COLOR_RED);
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
              GUI_FillCircle(CIRCLE_XPOS(3), CIRCLE_YPOS(3), radius, GUI_COLOR_YELLOW);
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
              GUI_FillCircle(CIRCLE_XPOS(4), CIRCLE_YPOS(3), radius, GUI_COLOR_GREEN);
              radius_previous = radius;
              state = 8;
            }
          }
        }
      } /* of if(TS_State.touchDetected) */
    }

    if (WakeupButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      HAL_Delay(500);
      WakeupButtonPressed = RESET;
      
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
  GUI_Clear(GUI_COLOR_WHITE);

  /* Set Touchscreen Demo description */
  GUI_FillRect(0, 0, 320, 80, GUI_COLOR_BLUE);
  GUI_SetTextColor(GUI_COLOR_WHITE);
  GUI_SetBackColor(GUI_COLOR_BLUE);
  GUI_SetFont(&Font24);
  GUI_DisplayStringAt(0, 0, (uint8_t *)"Touchscreen", CENTER_MODE);
  GUI_SetFont(&Font12);
  GUI_DisplayStringAt(0, 30, (uint8_t *)"Please use the Touchscreen to fill the", CENTER_MODE);
  GUI_DisplayStringAt(0, 45, (uint8_t *)"colored circles", CENTER_MODE);
  GUI_DisplayStringAt(0, 60, (uint8_t *)"Only 1 finger touch coordinates are displayed", CENTER_MODE);

  /* Set the LCD Text Color */
  GUI_DrawRect(10, 90, 300, 140, GUI_COLOR_BLUE);
  GUI_DrawRect(11, 91, 298, 138, GUI_COLOR_BLUE);

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
      GUI_FillCircle(CIRCLE_XPOS(1), CIRCLE_YPOS(1), CIRCLE_RADIUS, GUI_COLOR_BLUE);
      GUI_FillCircle(CIRCLE_XPOS(2), CIRCLE_YPOS(2), CIRCLE_RADIUS, GUI_COLOR_RED);
      GUI_FillCircle(CIRCLE_XPOS(3), CIRCLE_YPOS(3), CIRCLE_RADIUS, GUI_COLOR_YELLOW);
      GUI_FillCircle(CIRCLE_XPOS(4), CIRCLE_YPOS(4), CIRCLE_RADIUS, GUI_COLOR_GREEN);

      GUI_FillCircle(CIRCLE_XPOS(1), CIRCLE_YPOS(1), CIRCLE_RADIUS - 2, GUI_COLOR_WHITE);
      GUI_FillCircle(CIRCLE_XPOS(2), CIRCLE_YPOS(2), CIRCLE_RADIUS - 2, GUI_COLOR_WHITE);
      GUI_FillCircle(CIRCLE_XPOS(3), CIRCLE_YPOS(3), CIRCLE_RADIUS - 2, GUI_COLOR_WHITE);
      GUI_FillCircle(CIRCLE_XPOS(4), CIRCLE_YPOS(4), CIRCLE_RADIUS - 2, GUI_COLOR_WHITE);
      break;

    case 1:
      GUI_FillCircle(CIRCLE_XPOS(1), CIRCLE_YPOS(1), CIRCLE_RADIUS, GUI_COLOR_BLUE);
      GUI_FillCircle(CIRCLE_XPOS(1), CIRCLE_YPOS(1), CIRCLE_RADIUS - 2, GUI_COLOR_WHITE);
      break;

    case 2:
      GUI_FillCircle(CIRCLE_XPOS(2), CIRCLE_YPOS(2), CIRCLE_RADIUS, GUI_COLOR_RED);
      GUI_FillCircle(CIRCLE_XPOS(2), CIRCLE_YPOS(2), CIRCLE_RADIUS - 2, GUI_COLOR_WHITE);
      break;

    case 4:
      GUI_FillCircle(CIRCLE_XPOS(3), CIRCLE_YPOS(3), CIRCLE_RADIUS, GUI_COLOR_YELLOW);
      GUI_FillCircle(CIRCLE_XPOS(3), CIRCLE_YPOS(3), CIRCLE_RADIUS - 2, GUI_COLOR_WHITE);
      break;

    case 8:
      GUI_FillCircle(CIRCLE_XPOS(4), CIRCLE_YPOS(4), CIRCLE_RADIUS, GUI_COLOR_GREEN);
      GUI_FillCircle(CIRCLE_XPOS(4), CIRCLE_YPOS(4), CIRCLE_RADIUS - 2, GUI_COLOR_WHITE);
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
