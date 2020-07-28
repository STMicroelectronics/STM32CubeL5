/*******************************************************************************/
/*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2014 STMicroelectronics International NV
*
*   FILENAME        -  alertNotification_Client.h
*
*   COMPILER        -  EWARM
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      30/07/2014
*   $Revision$:  first version
*   $Author$:    
*   Comments:    Alert Notification Profile Header File
*
********************************************************************************
*
*  File Description 
*  ---------------------
*  
*
*******************************************************************************/
#ifndef _ALERT_NOTIFICATION_CLIENT_H_
#define _ALERT_NOTIFICATION_CLIENT_H_

/*******************************************************************************
* Macro
*******************************************************************************/
/* error codes for ANC_DISCOVERY_CMPLT
 * event
 */
#define ALERT_NOTIFICATION_SERVICE_NOT_FOUND		(0x01)
#define SUPPORTED_NEW_ALERT_CATEGORY_CHAR_NOT_FOUND 	(0x02)
#define NEW_ALERT_CHAR_NOT_FOUND 			(0x04)
#define SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_NOT_FOUND 	(0x08)
#define UNREAD_ALERT_STATUS_CHAR_NOT_FOUND 		(0x10)
#define CONTROL_POINT_CHAR_NOT_FOUND 			(0x20)
#define NEW_ALERT_CHAR_DESC_NOT_FOUND 			(0x40)
#define UNREAD_ALERT_STATUS_DESC_NOT_FOUND 		(0x41)
#define NO_CATEGORY_SUPPORTED_IN_ALERT_CHAR 		(0x42)
#define NO_CATEGORY_SUPPORTED_IN_UNREAD_ALERT_CHAR 	(0x43)

/* bitmasks for the characteristics discovered */
#define ALERT_NOTIFICATION_SERVICE_FOUND		(0x01)
#define SUPPORTED_NEW_ALERT_CATEGORY_CHAR_FOUND 	(0x02)
#define NEW_ALERT_CHAR_FOUND 				(0x04)
#define SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_FOUND 	(0x08)
#define UNREAD_ALERT_STATUS_CHAR_FOUND 			(0x10)
#define CONTROL_POINT_CHAR_FOUND 			(0x20)

#define ANC_FOUND_ALL_CHARACS				(SUPPORTED_NEW_ALERT_CATEGORY_CHAR_FOUND|\
							 NEW_ALERT_CHAR_FOUND|\
							 SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_FOUND|\
							 UNREAD_ALERT_STATUS_CHAR_FOUND|\
							 CONTROL_POINT_CHAR_FOUND)

/* alert category bitmasks first octet */
#define CATEGORY_SIMPLE_ALERT 				(0x01)
#define CATEGORY_EMAIL 					(0x02)
#define CATEGORY_NEWS 					(0x04)
#define CATEGORY_CALL 					(0x08)
#define CATEGORY_MISSED_CALL 				(0x10)
#define CATEGORY_SMS_MMS 				(0x20)
#define CATEGORY_VOICE_MAIL 				(0x40)
#define CATEGORY_SCHEDULE 				(0x80)

/* alert category bitmasks second octet */
#define CATEGORY_HIGH_PRIORITIZED_ALERT		        (0x01)
#define CATEGORY_INSTANT_MESSAGE			(0x02)

/* category ID to be written to the control point
 * characteristic
 */
typedef uint8_t tCategoryID;
#define CATEGORY_ID_SIMPLE_ALERT 			(0x00)
#define CATEGORY_ID_EMAIL 				(0x01)
#define CATEGORY_ID_NEWS 				(0x02)
#define CATEGORY_ID_CALL 				(0x03)
#define CATEGORY_ID_MISSED_CALL 			(0x04)
#define CATEGORY_ID_SMS_MMS 				(0x05)
#define CATEGORY_ID_VOICE_MAIL 				(0x06)
#define CATEGORY_ID_SCHEDULE 				(0x07)
#define CATEGORY_ID_HIGH_PRIORITIZED_ALERT		(0x08)
#define CATEGORY_ID_INSTANT_MESSAGE			(0x09)
#define CATEGORY_ID_ALL                                 (0xFF)

/* command ID to be written to the control point
 * characteristic
 */
typedef uint8_t tCommandID;
#define ENABLE_NEW_ALERT_NOTIFICATION			(0x00)
#define ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION	        (0x01)
#define DISABLE_NEW_ALERT_NOTIFICATION			(0x02)
#define DISABLE_UNREAD_ALERT_STATUS_NOTIFICATION        (0x03)
#define NOTIFY_NEW_ALERT_IMMEDIATELY			(0x04)
#define NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY	        (0x05)

/*******************************************************************************
* Functions
*******************************************************************************/
/**
 * ANP_Client_Init
 * 
 * @param[in] applcb : the callback function to be called by the
 *            proifle to notify the application of the profile specific events
 * 
 * Initializes the alert notification profile in the client role
 * 
 * @return BLE_STATUS_SUCCESS if the initialization was successful
 */ 
tBleStatus ANC_Client_Init(BLE_CALLBACK_FUNCTION_TYPE applcb);

/**
 * ANC_Advertize
 * 
 * @param[in] useWhiteList : This should be set to 
 *            true if the user is trying to establish
 *            a connection with a previously bonded
 *            device. 
 * 
 * If the useWhiteList is set to TRUE, then the device
 * is configured to use the whitelist which is configured
 * with bonded devices at the time of initialization else
 * limited discoverable mode is entered to connect to any
 * of the avialable devices
 */ 
tBleStatus ANC_Advertize(uint8_t useWhitelist);

/**
 * ANC_Write_Control_Point
 * 
 * @param[in] command : ID of the command to be sent
 * @param[in] category : ID of the category which has to be
 *            affected by the command. A value of 0xFF 
 *            will affect all categories 
 * 
 * Writes the control point characteristic with the command
 * and category specified. 
 * 
 * @return if the category ID specified is a category not
 * supported by the peer server or the categry ID or command ID
 * is out of range, then BLE_STATUS_INVALID_PARAMS is returned.
 * On successful write, BLE_STATUS_SUCCESS is returned
 */ 
tBleStatus ANC_Write_Control_Point(tCommandID command,tCategoryID category);

/**
 * ANC_Enable_Disable_New_Alert_Notification
 * 
 * @param[in] enable : enables the notifications for the
 * new alert characteristic. After enabling this the control
 * point characteristic should also be written with the command
 * and category in order to get alerts from the peer
 * 
 * @return BLE_STATUS_SUCCESS if the input parameter is valid
 * and the command is sent else BLE_STATUS_INVALID_PARAMS
 */         
tBleStatus ANC_Enable_Disable_New_Alert_Notification(uint16_t enable);

/**
 * ANC_Enable_Disable_Unread_Alert_Status_Notification
 * 
 * @param[in] enable : enables the notifications for the
 * unread alert status characteristic. After enabling this the control
 * point characteristic should also be written with the command
 * and category in order to get alerts from the peer
 * 
 * @return BLE_STATUS_SUCCESS if the input parameter is valid
 * and the command is sent else BLE_STATUS_INVALID_PARAMS
 */         
tBleStatus ANC_Enable_Disable_Unread_Alert_Status_Notification(uint16_t enable);

/**
 * ANCProfile_StateMachine
 * 
 * @param[in] None
 * 
 * ANC profile's state machine: to be called on application main loop. 
 */ 
tBleStatus ANCProfile_StateMachine(void);

#endif
