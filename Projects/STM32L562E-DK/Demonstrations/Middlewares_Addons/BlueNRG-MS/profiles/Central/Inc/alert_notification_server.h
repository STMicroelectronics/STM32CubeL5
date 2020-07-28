/*******************************************************************************/
/*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2015 STMicroelectronics International NV
*
*   FILENAME        -  alert_notification_server.h
*
*   COMPILER        -  EWARM
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      02/02/2015
*   $Revision$:  First version
*   $Author$:    
*   Comments:    Alert Notification Server Profile Central role Header File
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

/******************************************************************************
* Macro Declarations
******************************************************************************/

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
#define ENABLE_NEW_ALERT_NOTIFICATION		 (0x00)
#define ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION	 (0x01)
#define DISABLE_NEW_ALERT_NOTIFICATION		 (0x02)
#define DISABLE_UNREAD_ALERT_STATUS_NOTIFICATION (0x03)
#define NOTIFY_NEW_ALERT_IMMEDIATELY		 (0x04)
#define NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY	 (0x05)

#define ALERT_CHAR_MAX_TEXT_FIELD_LEN		 (18)
   
/* alert notification profile specific error code */
#define COMMAND_NOT_SUPPORTED 		               (0xA0)
   
/* Profile States */
#define ANS_UNINITIALIZED                               0x0000
#define ANS_CONNECTED_IDLE                              0x0001
#define ANS_INITIALIZED                                 0x0002
#define ANS_DEVICE_DISCOVERY                            0x0003
#define ANS_START_CONNECTION                            0x0004
#define ANS_CONNECTED                                   0x0005
#define ANS_CONN_PARAM_UPDATE_REQ                       0x0006
#define ANS_NOTIFY_NEW_ALERT_IMMEDIATELY 		0x0007
#define ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY 	0x0008

#define SUPPORTED_NEW_ALERT_CATEGORY_CHAR_SIZE		2
#define NEW_ALERT_CHAR_SIZE				20
#define SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_SIZE	2
#define UNREAD_ALERT_STATUS_CHAR_SIZE		        2 
#define ALERT_NOTIFICATION_CONTROL_POINT_CHAR_SIZE	2

/******************************************************************************
* Type definitions
******************************************************************************/
/* Alert Notification Server Central Information */
typedef struct ansInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** TX output power */
  uint8_t txPower;
  /** Alert category */
  uint8_t * alertCategory;
  /** Unread Alert Category */
  uint8_t * unreadAlertCategory;
} ansInitDevType;

typedef struct ansSecurityS {
  /** Set the IO capabilities of the device.*/
  uint8_t ioCapability;
  /** Man in the middle mode */
  uint8_t mitm_mode;
  /** Out of Band authentication enabled */
  uint8_t oob_enable;
  /** Out of Band data */
  uint8_t oob_data[16];
  /** Bonding Mode flag */
  uint8_t bonding_mode;
  /** Use fixed pin during pairing process */
  uint8_t use_fixed_pin;
  /** Fixed pin to be used during pairing if 
      MIMT protection is enabled. 
      Any random value between 0 to 999999 
  */
  uint32_t fixed_pin;
} ansSecurityType;

typedef struct ansConnDevS {
  /** The master shall use the fastScanInterval and fastScanWindow parameters for the duration
   *  set in this variable. After this duration the master should switch to the reducedPowerScanInterval
   *  and reducedPowerScanWindow to reduce the power consumption
   */
  uint32_t fastScanDuration;
  /** Fast time interval from when the Controller started its last LE scan until it begins
   *  the subsequent LE scan. The scan interval should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   *  This scan interval has a duration of 30 sec.
   */
  uint16_t fastScanInterval;
  /** Amount of time for the duration of the LE Fast scan. Scan_Window shall be less than
   *  or equal to Scan_Interval. The scan window should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   *  This scan window ha a duration of 30 sec.
   */
  uint16_t fastScanWindow;
  /** Reduced power time interval from when the Controller started its last LE scan until it begins
   *  the subsequent LE scan. The scan interval should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   *  This scan interval will be used after 30 s to reduce the power consumption.
   */
  uint16_t reducedPowerScanInterval;
  /** Amount of time for the duration of the LE reduced power scan. Scan_Window shall be less than
   *  or equal to Scan_Interval. The scan window should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   *  This scan interval will be used after 30 s to reduce the power consumption.
   */
  uint16_t reducedPowerScanWindow;
  /** Type of the peer address (0=PUBLIC_ADDR, 1=RANDOM_ADDR). */
  uint8_t peer_addr_type;
  /** Address of the peer device with which a connection has to be established. */
  uint8_t *peer_addr;
  /** Type of our address used during advertising (0=PUBLIC_ADDR, 1=RANDOM_ADDR). */
  uint8_t own_addr_type;
  /** Minimum value for the connection event interval. This shall be less than or
   *  equal to Conn_Interval_Max.\n
   *  Range: 0x0006 to 0x0C80\n
   *  Time = N x 1.25 msec\n
   *  Time Range: 7.5 msec to 4 seconds
   */
  uint16_t conn_min_interval;
  /** Maximum value for the connection event interval. This shall be greater than or
   *  equal to Conn_Interval_Min.\n
   *  Range: 0x0006 to 0x0C80\n
   *  Time = N x 1.25 msec\n
   *  Time Range: 7.5 msec to 4 seconds
   */
  uint16_t conn_max_interval;
  /** Slave latency for the connection in number of connection events.\n
   *  Range: 0x0000 to 0x01F4
   */
  uint16_t conn_latency;
  /** Supervision timeout for the LE Link.\n
   *  Range: 0x000A to 0x0C80\n
   *  Time = N x 10 msec\n
   *  Time Range: 100 msec to 32 seconds
   */
  uint16_t supervision_timeout;
  /** Minimum length of connection needed for the LE connection.\n
   *  Range: 0x0000 - 0xFFFF\n
   *  Time = N x 0.625 msec.
   */
  uint16_t min_conn_length;
  /** Maximum length of connection needed for the LE connection.\n
   *  Range: 0x0000 - 0xFFFF\n
   *  Time = N x 0.625 msec.
   */
  uint16_t max_conn_length;
} ansConnDevType;

typedef uint8_t tCategoryID;
typedef uint8_t tCommandID;

typedef struct _tTextInfo
{
  uint8_t len;
  uint8_t *str;
}tTextInfo;

typedef struct ansDevDiscS {
  /** Type of our address used during advertising (PUBLIC_ADDR, RANDOM_ADDR). */
  uint8_t own_addr_type;
  /** Time interval from when the Controller started its last LE scan until it begins
   *  the subsequent LE scan. The scan interval should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   */
  uint16_t scanInterval;
  /** Amount of time for the duration of the LE scan. The scan window shall be less than
   *  or equal to scan interval. The scan window should be a number in the range
   *  0x0004 to 0x4000. This corresponds to a time range 2.5 msec to 10240 msec.
   *  For a number N, Time = N x 0.625 msec.
   */
  uint16_t scanWindow;
} ansDevDiscType;

typedef struct ansConnUpdateParamS {
  /** This is the identifier which associates the request to the
   * response. The same identifier has to be returned by the upper
   * layer in the command Master_ConnectionUpdateParamResponse().
   */
  uint8_t  identifier; 
  /** Minimum value for the connection event interval. This shall be less than or
   *  equal to interval_max.\n
   *  Range: 0x0006 to 0x0C80\n
   *  Time = N x 1.25 msec\n
   *  Time Range: 7.5 msec to 4 seconds
   */  
  uint16_t interval_min;
  /** Maximum value for the connection event interval. This shall be greater than or
   *  equal to interval_min.\n
   *  Range: 0x0006 to 0x0C80\n
   *  Time = N x 1.25 msec\n
   *  Time Range: 7.5 msec to 4 seconds
   */
  uint16_t interval_max;
  /** Slave latency for the connection in number of connection events.\n
   *  Range: 0x0000 to 0x01F4
   */
  uint16_t slave_latency;
  /** Supervision timeout for the LE Link.\n
   *  Range: 0x000A to 0x0C80\n
   *  Time = N x 10 msec\n
   *  Time Range: 100 msec to 32 seconds
   */
  uint16_t timeout_mult;
} ansConnUpdateParamType;

/* Alert Notification  Server Central Context  */
typedef struct alertNotificationServerContextS {
  /**
   * state of the alert notification server
   * central state machine
   */ 
  uint32_t state;
  uint8_t public_addr[6];
  uint16_t connHandle;
  
  /**
   * handle of the alert notification service
   */ 
  uint16_t alertServiceHandle;
	
  /**
   * handle of the supported new alert category
   * characteristic
   */ 
  uint16_t newAlertCategoryHandle;
	
  /**
   * handle of the new alert characteristic
   */ 
  uint16_t newAlertHandle;
	
  /**
   * handle of the supported unread alert
   * category characteristic
   */ 
  uint16_t UnreadAlertCategoryHandle;
	
  /**
   * handle of the unread alert status
   * characteristic
   */ 
  uint16_t unreadAlertStatusHandle;
	
  /**
   * handle of the alert notification
   * control point characteristic
   */ 
  uint16_t ctrlPointCharHandle;
    
  /**
   * bitmask of the alert categories 
   * supported for the new alert
   */ 
  uint8_t alertCategory[2];
	
  /**
   * bitmask of the alert categories 
   * supported for the unread alert status
   */ 
  uint8_t unreadAlertCategory[2];
	
  /**
   * this is set to 1 when the control point
   * is written by the peer to enable new
   * alert notifications
   */ 
  uint8_t notifyNewAlert[10];
	
  /**
   * this is set to 1 when the control point
   * is written by the peer to enable unread
   * alert status notifications
   */ 
  uint8_t notifyUnreadAlert[10];
  
  //uint8_t fullConf;

} alertNotificationServerContext_Type;

/******************************************************************************
* Imported Variable
******************************************************************************/
/* alert notification server central profile context */
extern alertNotificationServerContext_Type alertNotificationServerContext; 

/*******************************************************************************
* Functions
*******************************************************************************/
/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref ansInitDevType)
 * @retval Status of call
 */
tBleStatus ANS_Init(ansInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref ansDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the ANS_DeviceDiscovery_CB()
 */
tBleStatus ANS_DeviceDiscovery(ansDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref ansSecurityType)
 * @retval Status of call
 */
tBleStatus ANS_SecuritySet(ansSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * ANS_SecuritySet()  
 * @retval Status of call
 */
tBleStatus ANS_StartPairing(void);

/**
  * @brief Clear Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus ANS_Clear_Security_Database();

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback ANS_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus ANS_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref ansConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * ANS_ConnectionStatus_CB() 
 */
tBleStatus ANS_DeviceConnection(ansConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the ANS_ConnectionStatus_CB()
 */
tBleStatus ANS_DeviceDisconnection(void);

/**
 * @brief This function shall be called by the master when it receives a
 * connection parameter update request from the peer device. If the master rejects these parameters can send the
 * response with the accepted parameter set to FALSE or ignore the request. This function shall be managed only if the 
 * master has already discovered all the services and characteristics.
 * If theese discovery procedure are ongoing, the master basic profile sends automatically the connection parameter response
 * with status rejected.
 * @param accepted TRUE if the peripheral peer request is accepted, FALSE otherwise.
 * @param param Connection parameter to update
 * @retval Status of the call.
 */
tBleStatus ANS_ConnectionParameterUpdateRsp(uint8_t accepted, ansConnUpdateParamType *param);

/**
 * @brief This function adds Alert Notiifcation Server service & related characteristics
 * @param[in]: None         
 * @retval Status of the call.
 */ 
tBleStatus ANS_Add_Services_Characteristics(void);

/**
 * @brief This function updates the alert category characteristic with the new bitmask.
 * The supported categories have to be constant when in connection.
 * @param[in] len : length of the category field. It has to be 0 or 1.
 * @param[in] category : bitmask of the categories supported. The bitmasks
 * are split across 2 octets and hav the meaning as described in the
 * bluetooth assigned numbers documentation
 * @return BLE_STATUS_SUCCESS if the update is successfully started
 * BLE_STATUS_INVALID_PARAMS if bitmask for non existent category is set
 */ 
tBleStatus ANS_Update_New_Alert_Category(uint8_t len,uint8_t* category);

/**
 * @brief This function updates the unread alert status category characteristic with the new bitmask.
 * The supported categories have to be constant when in connection
 * @param[in] len : length of the category field. It has to be 0 or 1.
 * @param[in] category : bitmask of the categories supported. The bitmasks
 * are split across 2 octets and hav the meaning as described in the
 * bluetooth assigned numbers documentation.
 * @return BLE_STATUS_SUCCESS if the update is successfully started
 * BLE_STATUS_INVALID_PARAMS if bitmask for non existent category is set
 */ 
tBleStatus ANS_Update_Unread_Alert_Category(uint8_t len,uint8_t* category);

/**
 * @brief This function updates the number of new alerts for the category specified
 * in the new alert characteristic.
 * @param[in] categoryID : ID of the category for which 
 *            the alert status has to be updated. 
 * @param[in] alertCount : alert count for the category specified.
 * The application has to maintain the count of new alerts. When
 * this function is called, the previous value of the alert will
 * be overwritten. 
 * @param[in] textInfo : Textual information corresponding to the alert.
 * For Example, in case of missed call category, the textual information
 * could be the name of the caller. If the alert count is more than 1, then
 * the textual information should correspond to the newest alert.
 * @return if the category ID specified is not valid, or the text
 * information is longer than18 octets, then BLE_STATUS_INVALID_PARAMS 
 * is returned. On successful write, BLE_STATUS_SUCCESS is returned
 */ 
tBleStatus ANS_Update_New_Alert(tCategoryID categoryID,uint8_t alertCount,tTextInfo textInfo);

/**
 * @brief This function updates the number of unread alerts for the category specified
 * in the unread alert status characteristic
 * @param[in] categoryID : ID of the category for which 
 *            the alert status has to be updated. 
 * @param[in] alertCount : alert count for the category specified.
 * The application has to maintain the count of unread alerts. When
 * this function is called, the previous value of the alert will
 * be overwritten. 
 * @return if the category ID specified is not valid,
 * then BLE_STATUS_INVALID_PARAMS is returned.
 * On successful write, BLE_STATUS_SUCCESS is returned
 */ 
tBleStatus ANS_Update_Unread_Alert_Status(tCategoryID categoryID,uint8_t alertCount);

/**
 * @brief This function writes the alert control point.
 * @param[in] *attVal : pointer to alert control point value 
 * @return:  status
 */ 
tBleStatus ANC_Handle_ControlPoint_Write(uint8_t *attVal);

/**
 * @brief Run the profile central state machine. 
 * @param  None
 * @retval None
 */
void ANS_StateMachine(void);

/***************  Public Function callback ******************/

/**
 * @brief This callback contains all the information of the device discovered
 * during the central proceudure
 * @param status Status of the device discovery procedure
 * @param addr_type Address type of the device discovered
 * @param addr Address of the device discovered
 * @param data_length Data length of the information from the discovered device
 * @param data Data of the discvered device
 * @param RSSI RSSI received during the device discovery procedure
 * @retval None
 */
extern void ANS_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
				   uint8_t data_length, uint8_t *data, 
				   uint8_t RSSI);

/**
 * @brief This callback contains all the information of the service discovery procedure
 * @param status Status of the service discovery procedure
 * @param numServices Nubmer of services discovered
 * @param services Data of the services discovered.
 *  * The array will be a sequence of elements with the following format:
 * - 1 byte  UUID Type (1 = 16 bit, 2 = 128 bit)
 * - 2 bytes Service Start Handle
 * - 2 bytes Service End Handle
 * - n bytes (2 or 16) Service UUID
 * @retval None
 */
extern void ANS_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

/**
 * @brief This callback contains the status of the connection procedure
 * @param connection_evt Connection event returned during the connection procedure.
 * Valid values are:\n
 * - CONNECTION_ESTABLISHED_EVT\n
 * - CONNECTION_FAILED_EVT\n
 * - DISCONNECTION_EVT\n
 * @param status Status of the event
 * @retval None
 */
extern void ANS_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * ANS_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref ansConnUpdateParamType for more details)
 * @retval None
 */
extern void ANS_ConnectionParameterUpdateReq_CB(ansConnUpdateParamType *param);

/**
 * @brief This function is called from the profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the ANS_SendPinCode() function to set the pin code.
 */
extern void ANS_PinCodeRequired_CB(void);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void ANS_Pairing_CB(uint16_t conn_handle, uint8_t status);

/**
 * @brief This callback is called when a  Attribute Modification event is received 
 * @param handle characteristic handle
 * @param length  length of received data
 * @param data_value array of received data
 * @retval None
 */
extern void ANC_Attribute_Modification_Received_CB(uint8_t handle, uint8_t length, uint8_t * data_value); 

/**
 * @brief This callback is called when a Permit control point event is received 
 * @param command_value pointer to control point command value
 * @retval None
 */
extern void ANC_Received_Control_Point_Command_CB(uint8_t * command_value);
#endif
