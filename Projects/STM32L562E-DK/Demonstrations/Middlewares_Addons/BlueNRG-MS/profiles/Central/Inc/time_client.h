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
*   FILENAME        -  time_client.h
*
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      26/02/2015
*   $Revision$:  first version
*   $Author$:    
*   Comments:    Time Client Profile Central role header file
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the Time Client application profioe central role
* 
******************************************************************************/

#ifndef _TIME_CLIENT_H_
#define _TIME_CLIENT_H_

/******************************************************************************
 * Include Files
******************************************************************************/
#include <host_config.h>
#include <debug.h>
#include <timer.h>
#include <uuid.h>
#include "time_profile_types.h"
/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Profiles States */
#define TIME_CLIENT_UNINITIALIZED                                  0x0000
#define TIME_CLIENT_CONNECTED_IDLE                                 0x0001
#define TIME_CLIENT_INITIALIZED                                    0x0002
#define TIME_CLIENT_DEVICE_DISCOVERY                               0x0003
#define TIME_CLIENT_START_CONNECTION                               0x0004
#define TIME_CLIENT_CONNECTED                                      0x0005

#define TIME_CLIENT_SERVICE_DISCOVERY                              0x0006

#define TIME_CLIENT_CTS_CHARAC_DISCOVERY                           0x0007    
#define TIME_CLIENT_WAIT_CTS_CHARAC_DISCOVERY                      0x0008   

#define TIME_CLIENT_NEXT_DSTS_CHARAC_DISCOVERY                     0x0009    
#define TIME_CLIENT_WAIT_NEXT_DSTS_CHARAC_DISCOVERY                0x000A 

#define TIME_CLIENT_RTUS_CHARAC_DISCOVERY                          0x000B    
#define TIME_CLIENT_WAIT_RTUS_CHARAC_DISCOVERY                     0x000C

#define TIME_CLIENT_SET_CURRENT_TIME_UPDATE_NOTIFICATION           0x000D   
#define TIME_CLIENT_WAIT_SET_CURRENT_TIME_UPDATE_NOTIFICATION      0x000E   

#define TIME_CLIENT_CONN_PARAM_UPDATE_REQ                          0x000F
#define TIME_CLIENT_DISCONNECTION                                  0x0010
  
#define TIME_CLIENT_WAIT_READ_CURRENT_TIME_VALUE                   0x0011                        
#define TIME_CLIENT_WAIT_READ_LOCAL_TIME_INFO                      0x0012
#define TIME_CLIENT_WAIT_READ_REFERENCE_TIME_INFO                  0x0013
#define TIME_CLIENT_WAIT_READ_NEXT_DST_CHANGE_TIME                 0x0014
#define TIME_CLIENT_WAIT_READ_SERVER_TIME_UPDATE_STATUS            0x0015

              
#define PRIMARY_SERVICES_ARRAY_SIZE 50
#define CHARAC_OF_SERVICE_ARRAY_SIZE 100 

#define READ_CHARACTERISTIC_MAX_SIZE        30  

#define CURRENT_TIME_READ_SIZE              10 
#define LOCAL_TIME_READ_SIZE                2
#define REFERENCE_TIME_READ_SIZE            4
#define NEXT_DST_CHANGE_TIME_READ_SIZE      8
#define SERVER_TIME_UPDATE_STATUS_READ_SIZE 2 


/*  Error Condition */

/**
 * @brief Error during the service discovery procedure
 */
#define TIPC_ERROR_IN_SERVICE_DISCOVERY       0x01

/**
 * @brief Error during the disconnection procedure
 */
#define TIPC_ERROR_IN_DISCONNECTION           0x02

/**
 * @brief Error during the characteristic discovery procedure
 */
#define TIPC_ERROR_IN_CHARAC_DISCOVERY        0x03

/**
 * @brief Error during the characteristic descriptor discovery procedure
 */
#define TIPC_ERROR_IN_CHARAC_DESC_DISCOVERY   0x04

/**
 * @brief Error during the Write Procedure
 */
#define TIPC_ERROR_IN_WRITE_PROCEDURE         0x05

/**
 * @brief Error during the enable of indication 
 */
#define TIPC_ERROR_IN_ENABLE_INDICATION       0x06
 
/**
 * @brief Error during the enable of notification
 */
#define TIPC_ERROR_IN_ENABLE_NOTIFICATION     0x07
   
/**
 * @brief Error during the connection procedure procedure
 */
#define TIPC_ERROR_IN_CONNECTION              0x08

/**
 * @brief Error during the read  procedure
 */
#define TIPC_ERROR_IN_READ                    0x09

/******************************************************************************
* Type definitions
******************************************************************************/

/* Profile Central Information */
typedef struct tipcInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** TX output power */
  uint8_t txPower;
} tipcInitDevType;

typedef struct tipcSecurityS {
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
} tipcSecurityType;

typedef struct tipcConnDevS {
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
} tipcConnDevType;

typedef struct tipcDevDiscS {
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
} tipcDevDiscType;

typedef struct tipcConnUpdateParamS {
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
} tipcConnUpdateParamType;

/* Profile Context variable */
typedef struct TimeClientContextS {  
  
  uint32_t state;
  uint8_t public_addr[6];
  uint16_t connHandle;
  uint8_t numpeerServices;
  
  uint8_t currentTimeServicePresent;           
  uint8_t nextDSTServicePresent;        
  uint8_t RTUServicePresent;               
  
  uint8_t peerServices[PRIMARY_SERVICES_ARRAY_SIZE];
    
  /**
   * handle of the current time characteristic value
   */ 
  uint16_t currentTimeCharValHandle;                  
      
  /**
   * handle of the local time information characteristic value
   */ 
  uint16_t localTimeInfoCharValHandle;                 
      
  /**
   * handle of the reference time information characteristic value
   */  
  uint16_t refTimeInfoCharValHandle;                       
      
  /**
   * handle of the time with DST characteristic
   */ 
  uint16_t timeWithDSTCharValHandle;                       
      
  /**
   * handle of the time update control point characteristic
   */ 
  uint16_t timeUpdateCtlPointCharValHandle;                
      
  /**
   * handle of the time update state characteristic
   */ 
  uint16_t timeUpdateStateCharValHandle;                
  
  uint8_t numCharac;
  uint8_t charac[CHARAC_OF_SERVICE_ARRAY_SIZE];
  uint16_t uuid_searched;
 
  uint8_t data[READ_CHARACTERISTIC_MAX_SIZE]; 
  uint16_t dataLen;
  
  tCurrentTime read_current_time_value;  
  tCurrentTime notified_current_time_value;  
  tLocalTimeInformation read_local_time_value;
  tReferenceTimeInformation read_reference_time_value;
  tTimeWithDST read_dst_change_time_value;
  tTimeUpdateStatus read_server_time_update_status_value;
  uint8_t fullConf;
  
} TimeClientContext_Type;   

/******************************************************************************
* Imported Variable
******************************************************************************/
/* time client context */
extern TimeClientContext_Type TimeClientContext;

/******************************************************************************
* Function Declarations
******************************************************************************/

/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref tipcInitDevType)
 * @retval Status of call
 */
tBleStatus TimeClient_Init(tipcInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref tipcDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the TimeClient_DeviceDiscovery_CB()
 */
tBleStatus TimeClient_DeviceDiscovery(tipcDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref tipcSecurityType)
 * @retval Status of call
 */
tBleStatus TimeClient_SecuritySet(tipcSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * TimeClient_SecuritySet()  
 * @retval Status of call
 */
tBleStatus TimeClient_StartPairing(void);

/**
  * @brief Clear time client Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus TimeClient_Clear_Security_Database(void);

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback TimeClient_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus TimeClient_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref tipcConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * TimeClient_ConnectionStatus_CB() 
 * This function MUST not be called if the application uses the TimeClient_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus TimeClient_DeviceConnection(tipcConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the TimeClient_ConnectionStatus_CB()
 */
tBleStatus TimeClient_DeviceDisconnection(void);

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
tBleStatus TimeClient_ConnectionParameterUpdateRsp(uint8_t accepted, tipcConnUpdateParamType *param);

/**
 * @brief Starts the service discovery procedure
 * on the peer device
 * @retval Status of call
 * @note All the information of the service will be returned
 * on the TimeClient_ServicesDiscovery_CB()
 * This function MUST not be called if the application uses the TimeClient_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus TimeClient_ServicesDiscovery(void);

/**
 * @brief Starts the characteristic discovery procedure
 * on the peer device
 * @param uuid_service UUID of the service to use to 
 * discover all the related characteristics
 * @retval Status of call
 * @note All the information of the service will be
 * returned on the TimeClient_CharacOfService_CB()
 * This function MUST not be called if the application uses the TimeClient_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus TimeClient_DiscCharacServ(uint16_t uuid_service);

/**
 * @brief Starts the characteristic descriptor discovery procedure
 * for current time  
 * @param None
 * @retval Status of call
 * @note All the information of the service will be
 * returned on the TimeClient_CharacOfService_CB()
 * This function MUST not be called if the application uses the TimeClient_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus TimeClient_Start_Current_Time_Characteristic_Descriptor_Discovery(void);


/**
 * @brief Run a profile state machine to execute all the central role procedure:
 * connection, service discovery, characteristic discovery and 
 * peer device configuration procedure.
 * @param connParam Contains all the values used during the connection
 * procedure (see @ref tipcConnDevType)
 * @retval Status of call
 * @note In the application main loop is REQUIRED to call the function 
 * TimeClient_StateMachine() to run the time client internal state machine.
 * If some procedure fails, the failure code will be signaled on the 
 * TimeClient_FullConfError_CB().
 */
tBleStatus TimeClient_ConnConf(tipcConnDevType connParam);

/**
 * @brief Run the profile internal state machine. This function is
 * required when the application executes the TimeClient_ConnConf()
 * @retval None
 */
void TimeClient_StateMachine(void);

/**
 * @brief Find the device services
 * @param uuid_service  service uuid to be searched
 * @param [out] *start_handle pointer to service start handle
 * @param [out] *end_handle pointer to service end handle
 * @retval Status of call
 */
tBleStatus TimeClient_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle);

/**
  * @brief Discovery Current Time characteristic descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus TimeClient_Start_Current_Time_Characteristic_Descriptor_Discovery(void);

/**
  * @brief Read Current Time Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus TimeClient_ReadCurrentTimeChar(void);

/**
  * @brief Read Local Time Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus TimeClient_ReadLocalTimeChar(void);

/**
  * @brief Read Next DST Change Time Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus TimeClient_ReadNextDSTChangeTimeChar(void);

/**
  * @brief Read Reference Time Info Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus TimeClient_ReadReferenceTimeInfoChar(void);

/**
  * @brief Read Server Time Update Status Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus TimeClient_ReadServerTimeUpdateStatusChar(void);

/**
  * @brief Set Current Time Characteristic Notification
  * @param [in] value (TRUE: enable characteristic notification 
  *                    FALSE: disable characteristic notification)
  * By default notification of any change in Time on Time server 
  * will remain disabled. If the application on Time client need
  * the time update notification, it need to enable the notification
  * using this function.Once enabled, the notification of time 
  * update on time server will remain enabled. if the application 
  * dont want to receive the notification any more, it need to 
  * disable the notification using this function. 
  * @retval tBleStatus: BLE status   
  */
tBleStatus TimeClient_Set_Current_Time_Char_Notification(BOOL value);

/**
  * @brief Starts a gatt write without response procedure to 
  * write the time update control point characteristic  
  * on the server. The information read will be passed 
  * to the application in the form of an event
  * 
  * @param[in] ctlValue: the value passed starts
  * a procedure to cancel or start a update process\n
  * @retval tBleStatus: BLE status   
  */ 
tBleStatus TimeClient_Update_Reference_Time_On_Server(uint8_t ctlValue);

/**
  * @brief Display the current time characteristics fields (year, data, ...).
  * @param[in] tCurrentTime: the current time value
  * @retval None 
  */ 
void TimeClient_DisplayCurrentTimeCharacteristicValue(tCurrentTime data_value);

/**
  * @brief Decode and store the read characteristic value 
  * @param[in] data_length: length of the read characteristic value
  * @param[in] data_value: pointer to read read characteristic value
  * @retval None 
  */ 
void TimeClient_Decode_Read_Characteristic_Value(uint8_t data_length, uint8_t *data_value);

/***************  time client Public Function callback ******************/

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
extern void TimeClient_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
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
 * @note This callback MUST not be called if the application uses the TimeClient_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void TimeClient_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

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
extern void TimeClient_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the time client receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * TimeClient_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref tipcConnUpdateParamType for more details)
 * @retval None
 */
extern void TimeClient_ConnectionParameterUpdateReq_CB(tipcConnUpdateParamType *param);

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
 * @note This callback MUST not be called if the application uses the TimeClient_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void TimeClient_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This callback contains all the information of the characteristic descriptor
 * of a service
 * @param status Status of the charcteristics descriptor discovery procedure
 * @param numCharac Number of characteristic descriptors of a service found on the peer device
 * @param charac Data of the characteristic descriptots discovered.
 * The array will be a sequence of elements with the following format:\n
 * - 1 byte Format handle data (1 = lists with 16 bit UUID, 2 = lists with 128 bit UUID)\n
 * - 2 bytes Characteristic descriptor Handle\n
 * - n bytes (2 or 16) UUID according the format field\n
 * @retval None
 * @note This callback MUST not be called if the application uses the TimeClient_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void TimeClient_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This callback returns the read characteristic value of the 
 * connected peer device. Returns, also, the status of the read procedure
 * @param status Status of the read operation
 * @param attr_handle attribute handle
 * @param data_len Length of the data returned
 * @param data Data read
 * @retVal None
 */
extern void TimeClient_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data);

/**
 * @brief This function is called from the time client profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the TimeClient_SendPinCode() function to set the pin code.
 */
extern void TimeClient_PinCodeRequired_CB(void);

/**
 * @brief This callack returns the status of the characteristic  enable notification procedure on the
 * connected peer device
 * @param status Status of the procedure 
 * @retval None
 * @note This callback MUST not be called if the application uses the TimeClient_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void TimeClient_EnableNotification_CB(uint8_t status);
/**
 * @brief This callback is called when an error occurs during the full configuration and connection.
 * The full configuration and connection procedure is run with the TimeClient_ConnConf() function
 * call.
 * @param error_type Error type (see the application profile error definition)
 * @param code Error code
 * @retval None
 */
extern void TimeClient_FullConfError_CB(uint8_t error_type, uint8_t code);

/**
 * @brief This callack returns the status of Characteristic notification/indication procedure 
 * @param status Status of the procedure 
 * @retval None
 */
extern void TimeClient_EnableNotificationIndication_CB(uint8_t status);

/**
 * @brief This callback is called when a Current Time Notification is received 
 * @param handle characteristic handle
 * @param length  length of received data
 * @param data_value array of received data
 * @retval None
 */
extern void TimeClient_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void TimeClient_Pairing_CB(uint16_t conn_handle, uint8_t status);

#endif //_TIME_CLIENT_H_
