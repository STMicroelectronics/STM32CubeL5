/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
* File Name          : time_server_config.h
* Author             : AMS - VMA, RF Application Team
* Version            : V1.0.0
* Date               : 26-January-2015
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
#ifndef __TIMER_SERVER_CONFIG_H_
#define __TIMER_SERVER_CONFIG_H_

/* Includes ------------------------------------------------------------------*/
#include "master_basic_profile.h"

/******************** PTS PERIPHERAL ADDRESS *******************/

#define PTS_PERIPHERAL_ADDRESS {0x44, 0x06, 0x06, 0xdc, 0x1b, 0x00} //{0x00, 0x1B, 0xDC, 0x06, 0x06, 0x44} 

/* Expected Peer Address if any */
#define PEER_ADDRESS {0xfd, 0x00, 0x25, 0xec, 0x02, 0x04} //TBR

/******************** Device Init Parameters *******************/

#define TIME_SERVER_PUBLIC_ADDRESS          {0x22, 0x02, 0x00, 0xE1, 0x80, 0x02}
#define TIME_SERVER_DEVICE_NAME             {'T', 'I', 'P', 'S', '_', 'C'}
#define TIME_SERVER_TX_POWER_LEVEL          4

/******************** Device Security Parameters *******************/   

#define TIME_SERVER_IO_CAPABILITY    IO_CAP_KEYBOARD_DISPLAY
#define TIME_SERVER_MITM_MODE        MITM_PROTECTION_REQUIRED
#define TIME_SERVER_OOB_ENABLE       OOB_AUTH_DATA_ABSENT
#define TIME_SERVER_USE_FIXED_PIN    USE_FIXED_PIN_FOR_PAIRING
#define TIME_SERVER_FIXED_PIN        111111 
#define TIME_SERVER_BONDING_MODE     BONDING

/******************** Device Discovery Procedure Parameters *******************/
   
#define TIME_SERVER_LIM_DISC_SCAN_INT  0x30 // 30 ms 
#define TIME_SERVER_LIM_DISC_SCAN_WIND 0x30 // 30 ms 


/******************** Device Connection Parameters *******************/

#define TIME_SERVER_FAST_SCAN_DURATION 30000           // 30 sec
#define TIME_SERVER_FAST_SCAN_INTERVAL 48              // 30 ms
#define TIME_SERVER_FAST_SCAN_WINDOW   48              // 30 ms
#define TIME_SERVER_REDUCED_POWER_SCAN_INTERVAL 4096   // 2.56 sec
#define TIME_SERVER_REDUCED_POWER_SCAN_WINDOW   18     // 11.25 ms
#define TIME_SERVER_FAST_MIN_CONNECTION_INTERVAL 40    // 50 ms
#define TIME_SERVER_FAST_MAX_CONNECTION_INTERVAL 56    // 70 ms
#define TIME_SERVER_FAST_CONNECTION_LATENCY 0
#define TIME_SERVER_SUPERVISION_TIMEOUT 20             // 200 ms
#define TIME_SERVER_MIN_CONN_LENGTH 5                  // 3.125 ms
#define TIME_SERVER_MAX_CONN_LENGTH 5                  // 3.125 ms


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 

#endif /*__TIMER_SERVER_CONFIG_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
