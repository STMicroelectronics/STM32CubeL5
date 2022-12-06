/**
  ******************************************************************************
  * @file    MetaDataManager_Config.h 
  * @author  System Research & Applications Team - Catania Lab.
  * @version V4.0.0
  * @date    20-Sep-2019
  * @brief   Meta Data Manager Config Header File
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
  
/* Define to prevent recursive inclusion -------------------------------------*/  
#ifndef _META_DATA_MANAGER_CONFIG_H_
#define _META_DATA_MANAGER_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ---------------------------------------------------- */
#include <stdlib.h>

#include "stm32l5xx_hal.h"
#include "stm32l562e_discovery.h"
#include "ALLMEMS1_config.h"

#define MDM_PRINTF ALLMEMS1_PRINTF

#ifdef __cplusplus
}
#endif

#endif /* _META_DATA_MANAGER_CONFIG_H_ */
