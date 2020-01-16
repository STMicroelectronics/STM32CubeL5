/**
  ******************************************************************************
  * @file    lcd.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use LCD drawing features.
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
#define LCD_FEATURES_NUM        3

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Feature;

/* Private function prototypes -----------------------------------------------*/
static void LCD_SetHint(void);
static void LCD_Show_Feature(uint8_t feature);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  LCD demo
  * @param  None
  * @retval None
  */
void LCD_demo(void)
{
  LCD_SetHint();
  LCD_Feature = 0;
  LCD_Show_Feature (LCD_Feature);

  while (1)
  {
    if (WakeupButtonPressed == SET)
    {
      /* Add delay to avoid rebound and reset it status */
      HAL_Delay(500);
      WakeupButtonPressed = RESET;

      if (++LCD_Feature < LCD_FEATURES_NUM)
      {
        LCD_Show_Feature(LCD_Feature);
      }
      else
      {
        return;
      }
    }
  }
}

/**
  * @brief  Display LCD demo hint
  * @param  None
  * @retval None
  */
static void LCD_SetHint(void)
{
  /* Clear the LCD */
  GUI_Clear(GUI_COLOR_WHITE);

  /* Set LCD Demo description */
  GUI_FillRect(0, 0, 320, 80, GUI_COLOR_BLUE);
  GUI_SetTextColor(GUI_COLOR_WHITE);
  GUI_SetBackColor(GUI_COLOR_BLUE);
  GUI_SetFont(&Font24);
  GUI_DisplayStringAt(0, 0, (uint8_t *)"LCD", CENTER_MODE);
  GUI_SetFont(&Font12);
  GUI_DisplayStringAt(0, 30, (uint8_t *)"This example shows the different", CENTER_MODE);
  GUI_DisplayStringAt(0, 45, (uint8_t *)"LCD Features, use Wakeup push-button", CENTER_MODE);
  GUI_DisplayStringAt(0, 60, (uint8_t *)"to display next page", CENTER_MODE);

  GUI_DrawRect(10, 90, 300, 140, GUI_COLOR_BLUE);
  GUI_DrawRect(11, 91, 298, 138, GUI_COLOR_BLUE);
}

/**
  * @brief  Show LCD Features
  * @param  feature : feature index
  * @retval None
  */
static void LCD_Show_Feature(uint8_t feature)
{
  Point Points[] = {{20, 150}, {80, 150}, {80, 200}};
  Point Points2[] = {{100, 150}, {160, 150}, {160, 200}};

  GUI_SetBackColor(GUI_COLOR_WHITE);
  GUI_FillRect(12, 92, 296, 136, GUI_COLOR_WHITE);
  GUI_SetTextColor(GUI_COLOR_BLACK);

  switch (feature)
  {
    case 0:
      /* Text Feature */

      GUI_DisplayStringAt(14, 100, (uint8_t *)"Left aligned Text", LEFT_MODE);
      GUI_DisplayStringAt(0, 115, (uint8_t *)"Center aligned Text", CENTER_MODE);
      GUI_DisplayStringAt(14, 130, (uint8_t *)"Right aligned Text", RIGHT_MODE);
      GUI_SetFont(&Font24);
      GUI_DisplayStringAt(14, 180, (uint8_t *)"Font24", LEFT_MODE);
      GUI_SetFont(&Font20);
      GUI_DisplayStringAt(140, 180, (uint8_t *)"Font20", LEFT_MODE);
      GUI_SetFont(&Font16);
      GUI_DisplayStringAt(240, 184, (uint8_t *)"Font16", LEFT_MODE);
      break;

    case 1:

      /* Draw misc. Shapes */
      GUI_DrawRect(20, 100, 60 , 40, GUI_COLOR_BLACK);
      GUI_FillRect(100, 100, 60 , 40, GUI_COLOR_BLACK);

      GUI_DrawCircle(200, 120, 20, GUI_COLOR_GRAY);
      GUI_FillCircle(280, 120, 20, GUI_COLOR_GRAY);

      GUI_DrawPolygon(Points, 3, GUI_COLOR_GREEN);
      GUI_FillPolygon(Points2, 3, GUI_COLOR_GREEN);

      GUI_DrawEllipse(200, 170, 30, 20, GUI_COLOR_RED);
      GUI_FillEllipse(270, 170, 30, 20, GUI_COLOR_RED);

      GUI_DrawHLine(20, 210, 64, GUI_COLOR_BLACK);
      GUI_DrawLine (170, 220, 300, 190, GUI_COLOR_BLACK);
      GUI_DrawLine (170, 190, 300, 220, GUI_COLOR_BLACK);
      break;

    case 2:
      /* Draw Bitmap */
      GUI_DrawBitmap(20, 100, (uint8_t *)stlogo);
      HAL_Delay(100);

      GUI_DrawBitmap(120, 100, (uint8_t *)stlogo);
      HAL_Delay(100);

      GUI_DrawBitmap(220, 100, (uint8_t *)stlogo);
      HAL_Delay(100);

      GUI_DrawBitmap(20, 160, (uint8_t *)stlogo);
      HAL_Delay(100);

      GUI_DrawBitmap(120, 160, (uint8_t *)stlogo);
      HAL_Delay(100);

      GUI_DrawBitmap(220, 160, (uint8_t *)stlogo);
      HAL_Delay(100);
      break;
    default :
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

