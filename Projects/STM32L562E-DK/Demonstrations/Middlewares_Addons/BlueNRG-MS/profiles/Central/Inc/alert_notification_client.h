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
*   FILENAME        -  alert_notification_client.h
*
*   COMPILER        -  EWARM
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      25/02/2015
*   $Revision$:  First version
*   $Author$:    
*   Comments:    Alert Notification Client Profile Central role Header File
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

/******************************************************************************
* Macro Declarations
******************************************************************************/
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

#define ANC_FOUND_ALL_CHARACS		(SUPPORTED_NEW_ALERT_CATEGORY_CHAR_FOUND|\
                                         NEW_ALERT_CHAR_FOUND|\
                                         SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_FOUND|\
                                         UNREAD_ALERT_STATUS_CHAR_FOUND|\
                                         CONTROL_POINT_CHAR_FOUND)

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
#define CATEGORY_ID_ALL                         (0xFF)

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
#define COMMAND_NOT_SUPPORTED 	                 (0xA0)
   
/* Profile States */
#define ANC_UNINITIALIZED                               0x0000
#define ANC_CONNECTED_IDLE                              0x0001
#define ANC_INITIALIZED                                 0x0002
#define ANC_DEVICE_DISCOVERY                            0x0003
#define ANC_START_CONNECTION                            0x0004
#define ANC_CONNECTED                                   0x0005

#define ANC_SERVICE_DISCOVERY                           0x0006

#define ANC_CHARAC_DISCOVERY                            0x0007 
#define ANC_WAIT_CHARAC_DISCOVERY                       0x0008

#define ANC_CONN_PARAM_UPDATE_REQ                       0x0009
#define ANC_DISCONNECTION                               0x000A

#define ANC_READ_NEW_ALERT_CATEGORY		        0x000B
#define ANC_WAIT_READ_NEW_ALERT_CATEGORY		0x000C

#define ANC_READ_UNREAD_ALERT_STATUS_CATEGORY	        0x000D
#define ANC_WAIT_READ_UNREAD_ALERT_STATUS_CATEGORY	0x000E

#define ANC_ENABLE_NEW_ALERT_NOTIFICATION                0x000F  
#define ANC_WAIT_ENABLE_NEW_ALERT_NOTIFICATION           0x0010 
#define ANC_ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION      0x0011  
#define ANC_WAIT_ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION 0x0012 

#define ANC_WRITE_CONTROL_POINT                          0x0013  
#define ANC_WAIT_WRITE_CONTROL_POINT                     0x0014

#define READ_CHARACTERISTIC_MAX_SIZE                    30   //TBR: value to be tuned 
#define PRIMARY_SERVICES_ARRAY_SIZE                     50
#define CHARAC_OF_SERVICE_ARRAY_SIZE                    100

#define SUPPORTED_NEW_ALERT_CATEGORY_CHAR_SIZE		2
#define NEW_ALERT_CHAR_SIZE				20
#define SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_SIZE	2
#define UNREAD_ALERT_STATUS_CHAR_SIZE		        2 
#define ALERT_NOTIFICATION_CONTROL_POINT_CHAR_SIZE	2

/**
 * @brief Error during the service discovery procedure
 */
#define ANC_ERROR_IN_SERVICE_DISCOVERY       0x01

/**
 * @brief Error during the disconnection procedure
 */
#define ANC_ERROR_IN_DISCONNECTION           0x02

/**
 * @brief Error during the characteristic discovery procedure
 */
#define ANC_ERROR_IN_CHARAC_DISCOVERY        0x03

/**
 * @brief Error during the characteristic descriptor discovery procedure
 */
#define ANC_ERROR_IN_CHARAC_DESC_DISCOVERY   0x04

/**
 * @brief Error during the Write Procedure
 */
#define ANC_ERROR_IN_WRITE_PROCEDURE         0x05

/**
 * @brief Error during the enable of indication 
 */
//#define ANC_ERROR_IN_ENABLE_INDICATION       0x06
 
  
/**
 * @brief Error during the enable of notification
 */
#define ANC_ERROR_IN_ENABLE_NOTIFICATION     0x07
   
/**
 * @brief Error during the connection procedure procedure
 */
#define ANC_ERROR_IN_CONNECTION              0x08

/**
 * @brief Error during the read  procedure
 */
#define ANC_ERROR_IN_READ                    0x09



/******************************************************************************
* Type definitions
******************************************************************************/
/* Alert Notification Client Central Information */
typedef struct ancInitDevS {
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
} ancInitDevType;

typedef struct ancSecurityS {
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
} ancSecurityType;

typedef struct ancConnDevS {
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
} ancConnDevType;

typedef struct ancDevDiscS {
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
} ancDevDiscType;

typedef struct ancConnUpdateParamS {
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
} ancConnUpdateParamType;


typedef uint8_t tCategoryID;
typedef uint8_t tCommandID;

typedef struct _tTextInfo
{
  uint8_t len;
  uint8_t *str;
}tTextInfo;

typedef struct _tNewAlert
{
  tCategoryID categoryID;
  uint8_t numNewAlert;
  tTextInfo textStringInfo;
}tNewAlert;

typedef struct _tUnreadAlertStatus
{
  tCategoryID categoryID;
  uint8_t unreadCount;
}tUnreadAlertStatus;

/* Alert Notification  Client Central Context  */
typedef struct alertNotificationClientContextS {
  /**
   * state of the alert notification client
   * central state machine
   */ 
  uint32_t state;
  //uint8_t disconnected;
  uint8_t public_addr[6];
  uint16_t connHandle;
  uint8_t numpeerServices;
  
  uint8_t ANSPresent; 
  
  uint8_t peerServices[PRIMARY_SERVICES_ARRAY_SIZE];
  
  /**
   * handle of the alert notification service
   */ 
  uint16_t alertServiceValueHandle;
	
  /**
   * handle of the Supported new alert category characteristic value 
   */ 
  uint16_t newAlertCategoryValueHandle;
	
  /**
   * handle of the new alert characteristic value
   */ 
  uint16_t newAlertValueHandle;
	
  /**
   * handle of the supported unread alert category characteristic value
   */ 
  uint16_t unreadAlertCategoryValueHandle;
	
  /**
   * handle of the unread alert status characteristic value
   */ 
  uint16_t unreadAlertStatusValueHandle;
	
  /**
   * handle of the alert notification control point characteristic value
   */ 
  uint16_t ctrlPointCharValueHandle;
    
  /**
   * bitmask of the alert categories supported for the new alert.
   * Value of Supported New Alert Category characteristic
   */ 
  uint8_t alertCategory[2];       
  
  /* New Alert characteristic value*/
  tNewAlert newAlert;
	
  /**
   * bitmask of the alert categories supported for the unread alert status.
   * Value of Supported Unread Alert Status Category characteristic
   */ 
  uint8_t unreadAlertCategory[2]; 
  
  /* New Alert characteristic value*/
  tUnreadAlertStatus unreadAlertStatus;

  uint8_t numCharac;
  uint8_t charac[CHARAC_OF_SERVICE_ARRAY_SIZE];
  uint16_t uuid_searched;
 
  uint8_t data[READ_CHARACTERISTIC_MAX_SIZE]; 
  uint16_t dataLen;
  
  uint8_t fullConf;
  
} alertNotificationClientContext_Type;

/******************************************************************************
* Imported Variable
******************************************************************************/
/* alert notification client central profile context */
extern alertNotificationClientContext_Type alertNotificationClientContext; 

extern BOOL bReadSupportedUnreadAlert;
/*******************************************************************************
* Functions
*******************************************************************************/
/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref ancInitDevType)
 * @retval Status of call
 */
tBleStatus ANC_Init(ancInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref ancDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the ANC_DeviceDiscovery_CB()
 */
tBleStatus ANC_DeviceDiscovery(ancDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref ancSecurityType)
 * @retval Status of call
 */
tBleStatus ANC_SecuritySet(ancSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * ANC_SecuritySet()  
 * @retval Status of call
 */
tBleStatus ANC_StartPairing(void);

/**
  * @brief Clear Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus ANC_Clear_Security_Database(void);

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback ANC_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus ANC_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref ancConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * ANC_ConnectionStatus_CB() 
 */
tBleStatus ANC_DeviceConnection(ancConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the ANC_ConnectionStatus_CB()
 */
tBleStatus ANC_DeviceDisconnection(void);

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
tBleStatus ANC_ConnectionParameterUpdateRsp(uint8_t accepted, ancConnUpdateParamType *param);

/**
 * @brief Starts the service discovery procedure
 * on the peer device
 * @retval Status of call
 * @note All the information of the service will be returned
 * on the ANC_ServicesDiscovery_CB()
 * This function MUST not be called if the application uses the ANC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus ANC_ServicesDiscovery(void);

/**
 * @brief Starts the characteristic discovery procedure
 * on the peer device
 * @param uuid_service UUID of the service to use to 
 * discover all the related characteristics
 * @retval Status of call
 * @note All the information of the service will be
 * returned on the ANC_CharacOfService_CB()
 * This function MUST not be called if the application uses the ANC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus ANC_DiscCharacServ(uint16_t uuid_service);

/**
 * @brief Run a profile state machine to execute all the central role procedure:
 * connection, service discovery, characteristic dicovery and 
 * peer device configuration procedure.
 * @param connParam Contains all the values used during the connection
 * procedure (see @ref ancConnDevType)
 * @retval Status of call
 * @note In the application main loop is REQUIRED to call the function 
 * ANC_StateMachine() to run the device internal state machine.
 * If some procedure fails, the failure code will be signaled on the 
 * ANC_FullConfError_CB().
 */
tBleStatus ANC_ConnConf(ancConnDevType connParam);

/**
 * @brief Find the device services
 * @param uuid_service  service uuid to be searched
 * @param [out] *start_handle pointer to service start handle
 * @param [out] *end_handle pointer to service end handle
 * @retval Status of call
 */
tBleStatus ANC_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle);
/**
 * @brief Run the profile central state machine. 
 * @param  None
 * @retval None
 */
void ANC_StateMachine(void);

/**
  * @brief Discovery New Alert Client Characteristic Descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus ANC_Start_New_Alert_Client_Char_Descriptor_Discovery(void);

/**
  * @brief Discovery Unread Alert Status Client Characteristic Descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus ANC_Start_Unread_Alert_Status_Client_Char_Descriptor_Discovery(void);

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
tBleStatus ANC_Enable_Disable_New_Alert_Notification(uint8_t enable);

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
tBleStatus ANC_Enable_Disable_Unread_Alert_Status_Notification(uint8_t enable);

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
 * @brief  It checks the write response status and error related to the 
 *         Alert Notification Control Point (ANCP) write request to reset energy expended      
 *         It should be called within the Profile event handler, on EVT_BLUE_GATT_ERROR_RESP, 
 *         EVT_BLUE_GATT_PROCEDURE_COMPLETE (TBR?)  
 * @param  err_code:  HRCP write response error code (from Heart Rate Sensor) 
 */
void ANC_CP_Check_Write_Response_Handler(uint8_t err_code);

/**
  * @brief Read Supported New Alert Category characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus ANC_Read_New_Alert_Category(void);

/**
  * @brief  Store & analyse the Characteristic Value for Supported New Alert Category Char
  * @param [in] None
  * @retval Error Code
  */
void ANC_Read_New_Alert_Category_CB(void);

/**
  * @brief Read Supported Unread Alert Status Category characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus ANC_Read_Unread_Alert_Status_Category(void);

/**
  * @brief  Store & analyse the Characteristic Value for Supported Unread Alert Status Category Char
  * @param [in] None
  * @retval Error Code
  */
void ANC_Read_Unread_Alert_Status_Category_CB(void);

/**
* @brief  It stores the New Alert Characteristics Notification
* @param  attr_handle: New Alert characteristic handle
* @param  data_lenght: New Alert characteristic value lenght
* @param  value: New Alert characteristic value 
*/
void ANC_New_Alert_Notification_CB(uint16_t attr_handle, uint8_t data_length,uint8_t * value);

/**
* @brief  It stores the Unread Alert Status Characteristics Notification
* @param  attr_handle: Unread Alert Status characteristic handle
* @param  data_lenght: Unread Alert Status characteristic value lenght
* @param  value: Unread Alert Status characteristic value 
*/
void ANC_Unread_Alert_Status_Notification_CB(uint16_t attr_handle, uint8_t data_length, uint8_t * value);


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
extern void ANC_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
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
extern void ANC_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

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
extern void ANC_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * ANC_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref ancConnUpdateParamType for more details)
 * @retval None
 */
extern void ANC_ConnectionParameterUpdateReq_CB(ancConnUpdateParamType *param);

/**
 * @brief This callback contains all the information of the characterisitcs of service discovery procedure
 * @param status Status of the characteristics of service discovery procedure
 * @param numCharac Number of characteristics found for that service on the peer device
 * @param charac Data of the characteristics of a service discovered.
 * The array will be a sequence of elements with the following format:\n
 * - 1 byte  UUID Type (1 = 16 bit, 2 = 128 bit)\n
 * - 2 bytes Characteristic Handle\n
 * - 1 bytes Characteristic Properties\n
 * - 2 bytes Characteristic Value Handle\n
 * - n bytes (2 or 16) Characteristics UUID\n
 * @retval None
 * @note This callback MUST not be called if the application uses the ANC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void ANC_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This callback contains all the information of the characterisitcs descriptor
 * of a service
 * @param status Status of the charcteristics descriptor discovery procedure
 * @param numCharac Number of characteristic descriptors of a service found on the peer device
 * @param charac Data of the characteristic descriptots discovered.
 * The array will be a sequence of elements with the following format:\n
 * - 1 byte Format handle data (1 = lists with 16 bit UUID, 2 = lists with 128 bit UUID)\n
 * - 2 bytes Characteristic descriptor Handle\n
 * - n bytes (2 or 16) UUID according the format field\n
 * @retval None
 * @note This callback MUST not be called if the application uses the ANC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void ANC_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This callback returns the read characteristic value of the 
 * connected peer device. Returns, also, the status of the read procedure
 * @param status Status of the read operation
 * @param data_len Length of the data returned
 * @param data Data read
 * @retVal None
 */
extern void ANC_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data);

/**
 * @brief This function is called from the profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the ANC_SendPinCode() function to set the pin code.
 */
extern void ANC_PinCodeRequired_CB(void);

/**
 * @brief This callack returns the status of the characteristic enable notification procedure on the
 * connected peer device
 * @param status Status of the procedure 
 * @retval None
 * @note This callback MUST not be called if the application uses the ANC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void ANC_EnableNotification_CB(uint8_t status);

/**
 * @brief This callback is called when an error occurs during the full configuration and connection.
 * The full configuration and connection procedure is run with the ANC_ConnConf() function
 * call.
 * @param error_type Error type (see the application profile error definition)
 * @param code Error code
 * @retval None
 */
extern void ANC_FullConfError_CB(uint8_t error_type, uint8_t code);

/**
 * @brief This callback is called when an Intermediate Cuff Pressure notification is received
 * @param handle characteristic handle
 * @param length  length of received data
 * @param data_value array of received data
 * @retval None
 */
extern void ANC_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void ANC_Pairing_CB(uint16_t conn_handle, uint8_t status);

/**
 * @brief This callback is called when as response to a ANCP write procedure
 * is received just to inform user application (error messages are already
 * raised by device code)
 * @param error_code Error type 
 * @retval None
 */
extern void ANC_CP_Write_Response_CB(uint8_t err_code); 

#endif
