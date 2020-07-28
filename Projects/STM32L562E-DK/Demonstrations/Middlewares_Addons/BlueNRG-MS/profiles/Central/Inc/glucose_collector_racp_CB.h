/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2015 STMicroelectronics International NV
*
*   FILENAME        -  glucose_collector_racp_CB.h
*
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/07/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    glucose collector racp callbacks header file 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the glucose collector racp callbacks 
* 
******************************************************************************/


/******************************************************************************
 * Include Files
******************************************************************************/

#ifndef _GLUCOSE_COLLECTOR_RACP_CB_H_
#define _GLUCOSE_COLLECTOR_RACP_CB_H_

#include <host_config.h>
#include <hci.h>

#include "bluenrg_aci.h"
#include "bluenrg_gatt_server.h"
#include "hci_const.h"
#include "bluenrg_gap.h"
#include "sm.h"

#include <debug.h>
#include <ble_list.h>
#include <timer.h>
#include <uuid.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Max number of expected notifications recevied as consequence of a 
   single RACP procedure: To BE TUNED based on expected glucose 
   sensor max notifications*/ 
#define RACP_MAX_EXPECTED_NOTIFICATIONS_NUMBER 255
/******************************************************************************
* Type definitions
******************************************************************************/

/******************************************************************************
* Imported Variable
******************************************************************************/

/******************************************************************************
* Function Declarations
******************************************************************************/

/****** Callbacks ************************************************************/

/**
* @brief  It stores the Glucose Sensor  Notifications (measurement, measurement context) 
*         for a single RACP procedure 
* @param  attr_handle: glucose sensor characteristic handle
* @param  data_lenght: glucose sensor characteristic value lenght
* @param  value: glucose sensor characteristic value 
*/
void GL_Collector_RACP_Notifications_Storage(uint16_t attr_handle, uint8_t data_length,uint8_t * value);

/**
* @brief  Utility function: It performs a post processing of the received notifications 
* to a single RACP procedure. It is used on PTS validation for display notifications data values
* @param  None
*/
void GL_Collector_RACP_Notifications_PostProcessing(void);

#endif /* _GLUCOSE_COLLECTOR_RACP_CB_H_ */
