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
*   FILENAME        -  glucose_collector_racp.h
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
*   Comments:    glucose collector RACP notifications value header file 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the glucose collector RACP notifications values
* 
******************************************************************************/


/******************************************************************************
 * Include Files
******************************************************************************/

#ifndef _GLUCOSE_COLLECTOR_RACP_H_
#define _GLUCOSE_COLLECTOR_RACP_H_

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
#include <glucose_collector_racp_CB.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/

/* Max size of a single glucose notification */
#define RACP_SINGLE_NOTIFICATION_MAX_SIZE      (17 + 3) //TBR 3 more bytes for handling up to 3 additional bytes if any)
/******************************************************************************
* Type definitions
******************************************************************************/

/* typedef used for storing the received glucose notifications (measurement, 
   measurement context)for a single RACP procedure  */
typedef struct _tRACPNotifications{
  uint16_t attr_handle; 
  uint8_t data_length; 
  uint8_t data_value[RACP_SINGLE_NOTIFICATION_MAX_SIZE]; 
} tRACPNotifications;
/******************************************************************************
* Imported Variable
******************************************************************************/
/* Storage of received Glucose Sensor  Notifications (measurement, measurement context) 
   for a single RACP procedure */
tRACPNotifications glucose_collector_notifications_for_racp[RACP_MAX_EXPECTED_NOTIFICATIONS_NUMBER];

/******************************************************************************
* Function Declarations
******************************************************************************/

/****** Callbacks ************************************************************/

#endif /* _GLUCOSE_COLLECTOR_RACP_H_ */
