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
*   FILENAME        -  phone_alert_status_service.h
*
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/16/2014
*   $Revision$:  first version
*   $Author$:    
*   Comments:    phone alert status service
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the phone alert status service, characteristics
* 
******************************************************************************/

#ifndef _PHONE_ALERT_STATUS_SERVICE_H_
#define _PHONE_ALERT_STATUS_SERVICE_H_

/******************************************************************************
* Macro Declarations
******************************************************************************/

/******** Phone alert status characteristics *********************************/

/* Alert status (uint8) characteristic value: 3 fields exposing the
   the alerting status of the device */
#define ALERT_STATUS_BIT_RINGER_STATE                 (0x01) /* bit 0 */
#define ALERT_STATUS_BIT_VIBRATOR_STATE               (0x02) /* bit 1 */
#define ALERT_STATUS_BIT_DISPLAY_ALERT_STATUS_STATE   (0x04) /* bit 2 */
/* NOTE: 0x3 - 0x7 bits are reserved for future use */ 

/* Current possible Alert Status combinations values */
#define ALERT_STATUS_RINGER_VIBRATOR_DISPLAY (ALERT_STATUS_BIT_RINGER_STATE | \
                                              ALERT_STATUS_BIT_VIBRATOR_STATE | \
                                              ALERT_STATUS_BIT_DISPLAY_ALERT_STATUS_STATE)
                                             
#define ALERT_STATUS_RINGER_VIBRATOR (ALERT_STATUS_BIT_RINGER_STATE | \
                                      ALERT_STATUS_BIT_VIBRATOR_STATE)
#define ALERT_STATUS_RINGER_DISPLAY (ALERT_STATUS_BIT_RINGER_STATE | \
                                     ALERT_STATUS_BIT_DISPLAY_ALERT_STATUS_STATE)
#define ALERT_STATUS_VIBRATOR_DISPLAY (ALERT_STATUS_BIT_VIBRATOR_STATE | \
                                       ALERT_STATUS_BIT_DISPLAY_ALERT_STATUS_STATE)

#define ALERT_STATUS_NO_ALERTS (0x0) /* no alerts on phone alert status server */

/* Alert status (uint8) characteristic: list of available Key values */ 
#define ALERT_STATUS_RINGER_STATE_NOT_ACTIVE           (0x00)
#define ALERT_STATUS_RINGER_STATE_ACTIVE               (0x01)
#define ALERT_STATUS_VIBRATE_NOT_ACTIVE                (0x00)
#define ALERT_STATUS_VIBRATE_ACTIVE                    (0x01)
#define ALERT_STATUS_DISPLAY_ALERT_STATUS_NOT_ACTIVE   (0x00)
#define ALERT_STATUS_DISPLAY_ALERT_STATUS_ACTIVE       (0x01) 

/* Ringer Setting (8bit) characteristic: list of available Key values */
#define ALERT_STATUS_RINGER_SETTING_SILENT             (0x00)
#define ALERT_STATUS_RINGER_SETTING_NORMAL             (0x01) 
/* NOTE: 0x2, 0xFF keys values are reserved for future use */

/* Ringer Control Point (uint8) characteristic: list of available Key values */
#define ALERT_STATUS_RINGER_CONTROL_POINT_SILENT_MODE        (0x01)
#define ALERT_STATUS_RINGER_CONTROL_POINT_MUTE_ONCE          (0x02)
#define ALERT_STATUS_RINGER_CONTROL_POINT_CANCEL_SILENT_MODE (0x03)
/* NOTE: 0x4 - 0xFF keys values are reserved for future use */

/******************************************************************************
* Type definitions
******************************************************************************/

/******************************************************************************
* Function Declarations
******************************************************************************/

#endif /* _PHONE_ALERT_STATUS_SERVICE_H_ */
