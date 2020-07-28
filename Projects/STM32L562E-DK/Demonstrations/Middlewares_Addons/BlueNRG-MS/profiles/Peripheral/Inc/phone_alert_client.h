/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2014 STMicroelectronics International NV
*
*   FILENAME        -  phonealert_client.h
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
* ---------------------------
*
*   $Date$:      28/07/2014
*   $Revision$:  Second version
*   $Author$:    
*   Comments:    Redesigned the Phone Alert Client Profile according to 
*                unified BlueNRG DK framework 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header for the phone alert client profile
******************************************************************************/
#ifndef _PHONE_ALERT_CLIENT_H_
#define _PHONE_ALERT_CLIENT_H_

/**********************************************************************************
* Macros
**********************************************************************************/
/* value definitions for ringer setting characteristic */
#define RINGER_SILENT		(0x00)
#define RINGER_NORMAL		(0x01)

/* value definitions for ringer control point characteristic */
#define SILENT_MODE		(0x01)
#define MUTE_ONCE		(0x02)
#define CANCEL_SILENT_MODE	(0x03)

/* value definitions for alert status characteristic 
 * If the bits specified below are not set, it
 * implies that the corresponding state is not
 * active
 */
#define RINGER_STATE_ACTIVE		(0x01)
#define VIBRATOR_STATE_ACTIVE		(0x02)
#define DISPLAY_ALERT_ACTIVE		(0x04)

/* error codes for phone client */
#define PHONE_ALERT_SERVICE_NOT_FOUND		(0x01)
#define PHONE_ALERT_STATUS_CHARAC_NOT_FOUND     (0x02)
#define RINGER_CNTRL_POINT_CHARAC_NOT_FOUND     (0x03)
#define RINGER_SETTING_CHARAC_NOT_FOUND         (0x04)
#define PHONE_ALERT_STATUS_DESC_NOT_FOUND       (0x05)
#define RINGER_CNTRL_POINT_DESC_NOT_FOUND       (0x06)
#define RINGER_SETTING_DESC_NOT_FOUND           (0x07)

/**********************************************************************************
* Function Prototypes
**********************************************************************************/
/**
 * PAC_Init
 * 
 * @param[in] FindMeTargetcb : callback function to be called
 *           by the profile to notify the application of events
 * 
 * Initializes the Phone Alert Status profile for client role
 */ 
tBleStatus PAC_Init(BLE_CALLBACK_FUNCTION_TYPE phoneAlertClientcb);

/**
 * PAC_Add_Device_To_WhiteList
 * 
 * @param[in] bdAddr : address of the peer device
 *            that has to be added to the whitelist
 */ 
tBleStatus PAC_Add_Device_To_WhiteList(uint8_t* bdAddr);
					
/**
 * PAC_Advertize
 * 
 * This function puts the device into
 * discoverable mode if it is in the
 * proper state to do so
 */ 
tBleStatus PAC_Advertize(void);

/**
 * PAC_Configure_Ringer
 * 
 * @param[in] ringerMode : the ringer mode to be set\n
 *            SILENT_MODE	    (0x01)\n
 *            MUTE_ONCE   	    (0x02)\n
 *            CANCEL_SILENT_MODE    (0x03)
 * 
 * Starts a write without response GATT procedure
 * to write the ringer mode command to the phone alert server
 * 
 * @return returns BLE_STATUS_SUCCESS if the parameters
 * are valid and the procedure has been started else
 * returns error codes
 */ 
tBleStatus PAC_Configure_Ringer(uint8_t ringerMode);

/**
 * PAC_Read_AlertStatus
 * 
 * When this function is called by the application,
 * the profile starts a gatt procedure to read the
 * characteristic value. The value read will be retuned
 * via the event to the application.
 * 
 * @return returns BLE_STATUS_SUCCESS if the procedure
 *         was started successfully
 */ 
tBleStatus PAC_Read_AlertStatus(void);

/**
 * PAC_Read_RingerSetting
 * 
 * When this function is called by the application,
 * the profile starts a gatt procedure to read the
 * characteristic value. The value read will be returned
 * via the event to the application.
 * 
 * @return returns BLE_STATUS_SUCCESS if the procedure
 *         was started successfully
 */ 
tBleStatus PAC_Read_RingerSetting(void);

/**
 * PACProfile_StateMachine
 * 
 * @param[in] None
 * 
 * PAC profile's state machine: to be called on application main loop. 
 */ 
tBleStatus PACProfile_StateMachine(void);

/**
 * PAC_Disable_ALert_Status_Notification
 * 
 * @param[in] None
 * 
 * Disable the Alert Status Notification
 */ 
void PAC_Disable_ALert_Status_Notification(void);

/**
 * PAC_Disable_Ringer_Status_Notification
 * 
 * @param[in] None
 * 
 * Disable the Ringer Status Notification
 */ 
void PAC_Disable_Ringer_Status_Notification(void);

#endif
