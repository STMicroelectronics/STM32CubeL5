/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
* File Name          : healt_thermometer_collector_config.h
* Author             : AMS - AAS, RF Application Team
* Version            : V1.0.0
* Date               : 09-February-2015
* Description        : Master Configuration parameters
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BLOOD_PRESSURE_COLLECTOR_CONFIG_H
#define __BLOOD_PRESSURE_COLLECTOR_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "master_basic_profile.h"

/******************** PTS PERIPHERAL ADDRESS *******************/ 
#define PTS_PERIPHERAL_ADDRESS {0x44, 0x06, 0x06, 0xdc, 0x1b, 0x00} // {0x97, 0x2F, 0x07, 0xdc, 0x1b, 0x00} {0x44, 0x06, 0x06, 0xdc, 0x1b, 0x00}   /* Address of my PTS dongle */

/* Expected Peer Address if any */
#define PEER_ADDRESS {0xdd, 0x00, 0x00, 0xE1, 0x80, 0x02};  /* Expected Blood Pressure Sensor Address */                              

/******************** Device Init Parameters *******************/

#define BPC_PUBLIC_ADDRESS         {0x23, 0x02, 0x00, 0xE1, 0x80, 0x02} //{0x22, 0x02, 0x00, 0xE1, 0x80, 0x02}//{0xFC, 0x12, 0x00, 0xE1, 0x80, 0x02} //{0x22, 0x02, 0x00, 0xE1, 0x80, 0x02}
#define BPC_DEVICE_NAME            {'B', 'L', 'P', '_', 'C'}
#define BPC_TX_POWER_LEVEL         4


/******************** Device Security Parameters *******************/   

#define BPC_IO_CAPABILITY    IO_CAP_KEYBOARD_DISPLAY
#define BPC_MITM_MODE        MITM_PROTECTION_REQUIRED
#define BPC_OOB_ENABLE       OOB_AUTH_DATA_ABSENT
#define BPC_USE_FIXED_PIN    USE_FIXED_PIN_FOR_PAIRING
#define BPC_FIXED_PIN        111111 
#define BPC_BONDING_MODE     BONDING


/******************** Device Discovery Procedure Parameters *******************/
   
#define BPC_LIM_DISC_SCAN_INT  0x30 // 30 ms 
#define BPC_LIM_DISC_SCAN_WIND 0x30 // 30 ms


/******************** Device Connection Parameters *******************/

#define BPC_FAST_SCAN_DURATION 30000           // 30 sec
#define BPC_FAST_SCAN_INTERVAL 48              // 30 ms = 48 * 0.625 ms
#define BPC_FAST_SCAN_WINDOW   48              // 30 ms = 48 * 0.625 ms
#define BPC_REDUCED_POWER_SCAN_INTERVAL 4096   // 2.56 sec = 4096 * 0.625 ms
#define BPC_REDUCED_POWER_SCAN_WINDOW   18     // 11.25 ms = 18 * 0.625 ms
#define BPC_FAST_MIN_CONNECTION_INTERVAL 40    // 50 ms = 40 * 1.25 ms 
#define BPC_FAST_MAX_CONNECTION_INTERVAL 56    // 70 ms = 56 * 1.25 ms
#define BPC_FAST_CONNECTION_LATENCY 0
#define BPC_SUPERVISION_TIMEOUT 20             // 200 ms = 20 x 10 ms
                                               // Supervision_timeout > 2 * (1+ Connection_Latency) * Connection_Interval (min value is 15) 
#define BPC_MIN_CONN_LENGTH 5                  // 3.125 ms = 5 * 0.625 msec
#define BPC_MAX_CONN_LENGTH 5                  // 3.125 ms = 5 * 0.625 msec

  
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 

#endif /*__BLOOD_PRESSURE_COLLECTOR_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
