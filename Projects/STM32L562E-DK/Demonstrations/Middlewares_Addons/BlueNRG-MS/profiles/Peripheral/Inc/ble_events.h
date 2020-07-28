/*******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  ble_events.h
*
*   COMPILER        -  EWARM (gnu gcc)
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/03/2012
*   $Revision$:  first version
*   $Author$:    
*   Comments:    
*
********************************************************************************
*
*  File Description 
*  ---------------------
*  This file defines the events which are given to the application by the profiles
* All events are defined in sequential order as it is possible to have more than one
* profile running simultaneously
*
*******************************************************************************/

#ifndef _BLE_EVENTS_H_
#define _BLE_EVENTS_H_

/* application events from ble main profile 
 * The error codes for the profile specific events
 * are defined in the profile specific header files
 */
#define EVT_MP_BLUE_INITIALIZED           		(0x01)
#define EVT_MP_CONNECTION_COMPLETE              (0x02)
#define EVT_MP_PASSKEY_REQUEST					(0x03)
#define EVT_MP_PAIRING_COMPLETE					(0x04)
#define EVT_MP_DISCONNECTION_COMPLETE			(0x05)
#define EVT_MP_ADVERTISE_ERROR					(0x06)
#define EVT_MP_ADVERTISING_TIMEOUT				(0x07)

/**
 * events to the application from heart rate sensor(HRS)
 */ 
#define HRS_EVT_BASE							(0x08)
#define EVT_HRS_INITIALIZED					    (HRS_EVT_BASE)
#define EVT_HRS_CHAR_UPDATE_CMPLT			    (HRS_EVT_BASE+1)/* error code indicating success or failure */
#define EVT_HRS_MANUFAC_STRING_UPD_CMPLT		(HRS_EVT_BASE+2)
#define EVT_HRS_RESET_ENERGY_EXPENDED			(HRS_EVT_BASE+3)

/**
 * events to the application from Proximity Reporter(PR)
 */ 
#define PR_EVT_BASE								(HRS_EVT_BASE+4)
#define EVT_PR_INITIALIZED						(PR_EVT_BASE)
#define EVT_PR_LINK_LOSS_ALERT					(PR_EVT_BASE+1)/* different alert values */
#define EVT_PR_PATH_LOSS_ALERT					(PR_EVT_BASE+2)/* different alert values */
/**
 * events to the application from Proximity Monitor(PM)
 */ 
#define PM_EVT_BASE								(PR_EVT_BASE+3)
#define EVT_PM_INITIALIZED						(PM_EVT_BASE)
#define EVT_PM_DISCOVERY_CMPLT				    (PM_EVT_BASE+1)
#define EVT_PM_LINK_LOSS_ALERT					(PM_EVT_BASE+2)/* different alert values */
#define EVT_PM_PATH_LOSS_ALERT					(PM_EVT_BASE+3)/* different alert values */

/**
 * events to the application from Time Server(TS)
 */ 
#define TS_EVT_BASE								(PM_EVT_BASE+4)
#define EVT_TS_INITIALIZED						(TS_EVT_BASE)
#define EVT_TS_CHAR_UPDATE_CMPLT				(TS_EVT_BASE+1)
#define EVT_TS_START_REFTIME_UPDATE				(TS_EVT_BASE+2)
#define EVT_TS_STOP_REFTIME_UPDATE				(TS_EVT_BASE+3)
#define EVT_TS_CURTIME_READ_REQ					(TS_EVT_BASE+4)

/**
 * events to the application from Time Client(TC)
 */ 
#define TC_EVT_BASE								(TS_EVT_BASE+5)
#define EVT_TC_REQUIREMENTS_NOT_MET				(TC_EVT_BASE) /* error codes CT_CHARACTERISTIC_NOT_FOUND, CT_SERVICE_NOT_FOUND */
#define EVT_TC_DISCOVERY_CMPLT					(TC_EVT_BASE+1)
#define EVT_TC_CUR_TIME_VAL_RECEIVED			(TC_EVT_BASE+2)
#define EVT_TC_INITIALIZED						(TC_EVT_BASE+3)
#define EVT_TC_READ_CUR_TIME_CHAR			    (TC_EVT_BASE+4)
#define EVT_TC_READ_REF_TIME_INFO_CHAR			(TC_EVT_BASE+5)
#define EVT_TC_READ_LOCAL_TIME_INFO_CHAR	    (TC_EVT_BASE+6)
#define EVT_TC_READ_TIME_WITH_DST_CHAR			(TC_EVT_BASE+6)
#define EVT_TC_READ_TIME_UPDATE_STATE_CHAR		(TC_EVT_BASE+6)

/**
 * events to the application from HID device
 */ 
#define HID_EVT_BASE							(TC_EVT_BASE+7)
#define EVT_HID_INITIALIZED						(HID_EVT_BASE)
#define EVT_HID_UPDATE_CMPLT			        (HID_EVT_BASE+1)/* with error code */
#define EVT_BATT_LEVEL_READ_REQ					(HID_EVT_BASE+2)
/**
 * events to the application from Battery(BAT)
 */ 
#define BAT_EVT_BASE							(HID_EVT_BASE+3)
#define EVT_BATTERY_LEVEL_UPDATE_CMPLT			(BAT_EVT_BASE)

/**
 * events to the application related to Device Information service(DIS)
 */ 
#define DIS_EVT_BASE									  (BAT_EVT_BASE+1)
#define EVT_DIS_SYSTEM_ID_UPDATE_CMPLT                    (DIS_EVT_BASE)
#define EVT_DIS_MODEL_NUM_UPDATE_CMPLT                    (DIS_EVT_BASE+1)
#define EVT_DIS_SERIAL_NUM_UPDATE_CMPLT                   (DIS_EVT_BASE+2)
#define EVT_DIS_FIRMWARE_STRING_UPDATE_CMPLT              (DIS_EVT_BASE+3)
#define EVT_DIS_HARDWARE_STRING_UPDATE_CMPLT              (DIS_EVT_BASE+4)
#define EVT_DIS_SOFTWARE_STRING_UPDATE_CMPLT              (DIS_EVT_BASE+5)
#define EVT_DIS_MANUFACTURER_NAME_STRING_UPDATE_CMPLT     (DIS_EVT_BASE+6)
#define EVT_DIS_IEEE_CERTIFICATION_UPDATE_CMPLT           (DIS_EVT_BASE+7)
#define EVT_DIS_PNP_ID_UPDATE_CMPLT                       (DIS_EVT_BASE+8)

/**
 * events to the application from Scan Parameters Servcie(SCAN)
 */ 
#define SCAN_EVT_BASE									  (DIS_EVT_BASE+9)
#define EVT_SCAN_REFRESH_UPDATE_CMPLT                     (SCAN_EVT_BASE)


/**
 * events to the application from Find Me target profile(FMT)
 */ 
#define FMT_EVT_BASE				(SCAN_EVT_BASE+1)
#define EVT_FMT_INITIALIZED			(FMT_EVT_BASE)
#define EVT_FMT_ALERT				(FMT_EVT_BASE+1)

/**
 * events to the application from Find Me Locator Profile(FML)
 */ 
#define FML_EVT_BASE					(FMT_EVT_BASE+2)
#define EVT_FML_INITIALIZED				(FML_EVT_BASE)
#define EVT_FML_DISCOVERY_CMPLT	        (FML_EVT_BASE+1)/* error codes ALERT_CHARACTERISTIC_NOT_FOUND, IMM_ALERT_SERVICE_NOT_FOUND, BLE_STATUS_SUCCESS */

/**
 * events to the application from Phone Alert Status Client
 */ 
#define PAC_EVT_BASE					(FML_EVT_BASE+2)
#define EVT_PAC_INITIALIZED				(PAC_EVT_BASE)
#define EVT_PAC_DISCOVERY_CMPLT	        (PAC_EVT_BASE+1)
/* This  event is returned when the application has previously
 * requested to read the alert status characteristic or a
 * notification is received 
 */ 
#define EVT_PAC_ALERT_STATUS			(PAC_EVT_BASE+2)
#define EVT_PAC_RINGER_SETTING			(PAC_EVT_BASE+3)

/**
 * events to the application from blood pressure profile
 */
#define BPS_EVT_BASE					(PAC_EVT_BASE+4)
#define EVT_BPS_INITIALIZED				(BPS_EVT_BASE)
#define EVT_BPS_BPM_CHAR_UPDATE_CMPLT 	(BPS_EVT_BASE+1)
#define EVT_BPS_ICP_CHAR_UPDATE_CMPLT 	(BPS_EVT_BASE+2)
#define EVT_BPS_IDLE_CONNECTION_TIMEOUT (BPS_EVT_BASE+3)

/**
 * events to the application from the health thermometer profile
 */
#define HT_EVT_BASE					    			(BPS_EVT_BASE+4)
#define EVT_HT_INITIALIZED							(HT_EVT_BASE)
#define EVT_HT_TEMPERATURE_CHAR_UPDATE_CMPLT 		(HT_EVT_BASE+1)
#define EVT_HT_INTERMEDIATE_TEMP_CHAR_UPDATE_CMPLT  (HT_EVT_BASE+2)
#define EVT_HT_TEMP_TYPE_CHAR_UPDATE_CMPLT 		    (HT_EVT_BASE+3)
#define EVT_HT_MEASUREMENT_INTERVAL_UPDATE_CMPLT	(HT_EVT_BASE+4)
#define EVT_HT_MEASUREMENT_INTERVAL_RECEIVED		(HT_EVT_BASE+5)
#define EVT_HT_IDLE_CONNECTION_TIMEOUT 			    (HT_EVT_BASE+6)

/**
 * events to the application from the alert notification profile - client
 */ 
#define ANC_EVT_BASE					    		(HT_EVT_BASE+7)
#define EVT_ANC_INITIALIZED							(ANC_EVT_BASE)
#define EVT_ANC_DISCOVERY_CMPLT						(ANC_EVT_BASE+1)/* with error codes */
#define EVT_ANC_NEW_ALERT_RECEIVED					(ANC_EVT_BASE+2)
#define EVT_ANC_UNREAD_ALERT_STATUS_RECEIVED		(ANC_EVT_BASE+3)

/**
 * events to the application from the alert notification profile - server
 */ 
#define ANS_EVT_BASE					    		(ANC_EVT_BASE+4)
#define EVT_ANS_INITIALIZED							(ANS_EVT_BASE)


/**
 * events to the application from the glucose profile.
 */
#define GL_EVT_BASE					     (ANS_EVT_BASE+1)
#define EVT_GL_INITIALIZED			             (GL_EVT_BASE)
#define EVT_GL_IDLE_CONNECTION_TIMEOUT 			     (GL_EVT_BASE+1)

#endif
