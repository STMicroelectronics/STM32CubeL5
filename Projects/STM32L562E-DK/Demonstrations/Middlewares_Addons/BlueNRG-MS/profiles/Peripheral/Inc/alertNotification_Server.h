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
*   FILENAME        -  alertNotification_Server.h
*
*   COMPILER        -  EWARM
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      31/07/2014
*   $Revision$:  Second version
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
#ifndef _ALERT_NOTIFICATION_SERVER_H_
#define _ALERT_NOTIFICATION_SERVER_H_

/*******************************************************************************
* Macro
*******************************************************************************/
/* alert category bitmasks first octet */
#define CATEGORY_SIMPLE_ALERT 			(0x01)
#define CATEGORY_EMAIL 				(0x02)
#define CATEGORY_NEWS 				(0x04)
#define CATEGORY_CALL 				(0x08)
#define CATEGORY_MISSED_CALL 			(0x10)
#define CATEGORY_SMS_MMS 			(0x20)
#define CATEGORY_VOICE_MAIL 			(0x40)
#define CATEGORY_SCHEDULE 			(0x80)

/* alert category bitmasks second octet */
#define CATEGORY_HIGH_PRIORITIZED_ALERT		(0x01)
#define CATEGORY_INSTANT_MESSAGE		(0x02)

/* category ID to be written to the control point
 * characteristic
 */
typedef uint8_t tCategoryID;
#define CATEGORY_ID_SIMPLE_ALERT 		(0x00)
#define CATEGORY_ID_EMAIL 			(0x01)
#define CATEGORY_ID_NEWS 			(0x02)
#define CATEGORY_ID_CALL 			(0x03)
#define CATEGORY_ID_MISSED_CALL 		(0x04)
#define CATEGORY_ID_SMS_MMS 			(0x05)
#define CATEGORY_ID_VOICE_MAIL 			(0x06)
#define CATEGORY_ID_SCHEDULE 			(0x07)
#define CATEGORY_ID_HIGH_PRIORITIZED_ALERT	(0x08)
#define CATEGORY_ID_INSTANT_MESSAGE		(0x09)

/* command ID to be written to the control point
 * characteristic
 */
typedef uint8_t tCommandID;
#define ENABLE_NEW_ALERT_NOTIFICATION		 (0x00)
#define ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION	 (0x01)
#define DISABLE_NEW_ALERT_NOTIFICATION		 (0x02)
#define DISABLE_UNREAD_ALERT_STATUS_NOTIFICATION (0x03)
#define NOTIFY_NEW_ALERT_IMMEDIATELY		 (0x04)
#define NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY	 (0x05)

#define ALERT_CHAR_MAX_TEXT_FIELD_LEN		 (18)

typedef struct _tTextInfo
{
  uint8_t len;
  uint8_t *str;
}tTextInfo;
/*******************************************************************************
* Functions
*******************************************************************************/
/**
 * ANS_Client_Init
 * 
 * @param[in] alertCategory : bitmask of the categories supported
 *            for the new alert characteristic. This has to be 2 bytes long
 * @param[in] alertCategory : bitmask of the categories supported
 *            for the unread alert status characteristic. This has to be 2 bytes long
 * @param[in] applcb : the callback function to be called by the
 *            proifle to notify the application of the profile specific events
 * 
 * Initializes the alert notification profile in the server role
 * 
 * @return BLE_STATUS_SUCCESS if the initialization was successful
 */ 
tBleStatus ANS_Init(uint8_t* alertCategory,
		    uint8_t* unreadAlertCategory,
		    BLE_CALLBACK_FUNCTION_TYPE applcb);

/**
 * ANS_Advertize
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
tBleStatus ANS_Advertize(uint8_t useWhitelist);

/**
 * ANS_Update_New_Alert_Category
 * 
 * @param[in] len : length of the category field. It has to be 0 or 1.
 * @param[in] category : bitmask of the categories supported. The bitmasks
 * are split across 2 octets and hav the meaning as described in the
 * bluetooth assigned numbers documentation
 * 
 * updates the alert category characteristic with the new bitmask.
 * The supported categories have to be constant when in connection
 * 
 * @return BLE_STATUS_SUCCESS if the update is successfully started
 * BLE_STATUS_INVALID_PARAMS if bitmask for non existent category is set
 */ 
tBleStatus ANS_Update_New_Alert_Category(uint8_t len,uint8_t* category);

/**
 * ANS_Update_Unread_Alert_Category
 * 
 * @param[in] len : length of the category field. It has to be 0 or 1.
 * @param[in] category : bitmask of the categories supported. The bitmasks
 * are split across 2 octets and hav the meaning as described in the
 * bluetooth assigned numbers documentation
 * 
 * updates the unread alert status category characteristic with the new bitmask.
 * The supported categories have to be constant when in connection
 * 
 * @return BLE_STATUS_SUCCESS if the update is successfully started
 * BLE_STATUS_INVALID_PARAMS if bitmask for non existent category is set
 */ 
tBleStatus ANS_Update_Unread_Alert_Category(uint8_t len,uint8_t* category);

/**
 * ANS_Update_New_Alert
 * 
 * @param[in] categoryID : ID of the category for which 
 *            the alert status has to be updated. 
 * @param[in] alertCount : alert count for the category specified.
 * The application has to maintain the count of new alerts. When
 * this function is called, the previous value of the alert will
 * be overwritten. 
 * @param[in] textInfo : Textual information corresponding to the alert.
 * For Example, in case of missed call category, the textual information
 * could be the name of the caller. If the alert count is more than 1, then
 * the textual information should correspond to the newest alert
 * 
 * updates the number of new alerts for the category specified
 * in the new alert characteristic
 * 
 * @return if the category ID specified is not valid, or the text
 * information is longer than18 octets, then BLE_STATUS_INVALID_PARAMS 
 * is returned. On successful write, BLE_STATUS_SUCCESS is returned
 */ 
tBleStatus ANS_Update_New_Alert(tCategoryID categoryID,uint8_t alertCount,tTextInfo textInfo);

/**
 * ANS_Update_Unread_Alert_Status
 * 
 * @param[in] categoryID : ID of the category for which 
 *            the alert status has to be updated. 
 * @param[in] alertCount : alert count for the category specified.
 * The application has to maintain the count of unread alerts. When
 * this function is called, the previous value of the alert will
 * be overwritten. 
 * 
 * updates the number of unread alerts for the category specified
 * in the unread alert status characteristic
 * 
 * @return if the category ID specified is not valid,
 * then BLE_STATUS_INVALID_PARAMS is returned.
 * On successful write, BLE_STATUS_SUCCESS is returned
 */ 
tBleStatus ANS_Update_Unread_Alert_Status(tCategoryID categoryID,uint8_t alertCount);

/**
 * ANSProfile_StateMachine
 * 
 * @param[in] None
 * 
 * ANS profile's state machine: to be called on application main loop. 
 */ 
tBleStatus ANSProfile_StateMachine(void);

#endif
