/**
  ******************************************************************************
  * @file    app_hcr.h
  * @author  MCD Application Team
  * @brief   Header for app_hcr.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_HCR_H
#define APP_HCR_H

#undef GLOBAL
#ifdef __APP_HCR_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Includes ------------------------------------------------------------------*/
/*hal*/
#include "stm32l5xx_hal.h"

/*discovery*/
#include "stm32l562e_discovery.h"
#include "stm32l562e_discovery_lcd.h"
#include "stm32l562e_discovery_ts.h"

/*std*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*bitmap*/
#include "phone565.bmp.h"
#include "mail565.bmp.h"
#include "chat565.bmp.h"
#include "play565.bmp.h"
#include "stm32cubeai565.bmp.h"

/*new LCD API*/
#include "stm32_lcd.h"

/*misc*/
#include "utility.h"

/*ai*/
#include "network.h"
#include "network_data.h"

/*ts  cal*/
#if 0  // PLF
#include "k_calibration.h"
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct _logoDisplay {
        char*     logo_string;
        uint8_t*  logo_buffer;
        uint32_t  x_pos;
        uint32_t  y_pos;
}logoDisplay;

/* Exported constants --------------------------------------------------------*/
#define LOGO_END { NULL, NULL }

#define PATCH_SIZE 28
#define FB_SIZE 60

#define PEN_POINT_SIZE 7
#define TOUCH_TIMEOUT 700

#define AI_MNETWORK_NUMBER (1)

#define USE_UART_DEBUG 0

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

int HCR_demo(void);

/* External variables --------------------------------------------------------*/

#endif /* APP_HCR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
