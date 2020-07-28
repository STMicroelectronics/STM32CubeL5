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
*   FILENAME        -  glucose_collector.h
*
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/01/2015
*   $Revision$:  first version
*   $Author$:    
*   Comments:    Glucose Collector Profile Central role header file
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the Glucose Collector application profioe central role
* 
******************************************************************************/

#ifndef _GLUCOSE_COLLECTOR_H_
#define _GLUCOSE_COLLECTOR_H_

/******************************************************************************
 * Include Files
******************************************************************************/
#include <host_config.h>
#include <debug.h>
#include <timer.h>
#include <uuid.h>
/******************************************************************************
* Macro Declarations
******************************************************************************/
#define RACP_COMMAND_MAX_LEN                (17) /* (Within Range with Time)*/
#define RACP_BASE_TIME_SIZE                 (7)

#define READ_CHARACTERISTIC_MAX_SIZE 30       //TBR: value to be tuned 
#define GLUCOSE_READ_SIZE  2 
#define GLUCOSE_DIS_MANUFACTURER_NAME_SIZE 22 
#define GLUCOSE_DIS_MODEL_NUMBER_SIZE 22      
#define GLUCOSE_DIS_SYSTEM_ID_SIZE 8          

#define GL_COLLECTOR_PROCEDURE_TIMEOUT 30 

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )

/* glucose collector States */
#define GLUCOSE_COLLECTOR_UNINITIALIZED                                  0x0000
#define GLUCOSE_COLLECTOR_CONNECTED_IDLE                                 0x0001
#define GLUCOSE_COLLECTOR_INITIALIZED                                    0x0002
#define GLUCOSE_COLLECTOR_DEVICE_DISCOVERY                               0x0003
#define GLUCOSE_COLLECTOR_START_CONNECTION                               0x0004
#define GLUCOSE_COLLECTOR_CONNECTED                                      0x0005

#define GLUCOSE_COLLECTOR_SERVICE_DISCOVERY                              0x0006

#define GLUCOSE_COLLECTOR_MEASUREMENT_CHARAC_DISCOVERY                   0x0007 
#define GLUCOSE_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY              0x0008 

#define GLUCOSE_COLLECTOR_ENABLE_MEASUREMENT_NOTIFICATION                0x0009  
#define GLUCOSE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_NOTIFICATION           0x000A 
#define GLUCOSE_COLLECTOR_ENABLE_MEASUREMENT_CONTEXT_NOTIFICATION        0x000B  
#define GLUCOSE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_CONTEXT_NOTIFICATION   0x000C 
#define GLUCOSE_COLLECTOR_ENABLE_RACP_INDICATION                         0x000D 
#define GLUCOSE_COLLECTOR_WAIT_ENABLE_RACP_INDICATION                    0x000E 

#define GLUCOSE_COLLECTOR_SEND_RACP                                      0x000F 
#define GLUCOSE_COLLECTOR_WAIT_SEND_RACP                                 0x0010  
#define GLUCOSE_COLLECTOR_CONN_PARAM_UPDATE_REQ                          0x0011
#define GLUCOSE_COLLECTOR_DISCONNECTION                                  0x0012

#define GLUCOSE_COLLECTOR_READ_FEATURE                                   0x0013 
#define GLUCOSE_COLLECTOR_WAIT_READ_FEATURE                              0x0014 

#define PRIMARY_SERVICES_ARRAY_SIZE 50
#define CHARAC_OF_SERVICE_ARRAY_SIZE 100 

/* Glucose Collector Error Condition */

/**
 * @brief Error during the service discovery procedure
 */
#define GLC_ERROR_IN_SERVICE_DISCOVERY       0x01

/**
 * @brief Error during the disconnection procedure
 */
#define GLC_ERROR_IN_DISCONNECTION           0x02

/**
 * @brief Error during the characteristic discovery procedure
 */
#define GLC_ERROR_IN_CHARAC_DISCOVERY        0x03

/**
 * @brief Error during the characteristic descriptor discovery procedure
 */
#define GLC_ERROR_IN_CHARAC_DESC_DISCOVERY   0x04

/**
 * @brief Error during the Write Procedure
 */
#define GLC_ERROR_IN_WRITE_PROCEDURE         0x05

/**
 * @brief Error during the enable of indication 
 */
#define GLC_ERROR_IN_ENABLE_INDICATION       0x06
 
  
/**
 * @brief Error during the enable of notification
 */
#define GLC_ERROR_IN_ENABLE_NOTIFICATION     0x07
   
/**
 * @brief Error during the connection procedure procedure
 */
#define GLC_ERROR_IN_CONNECTION              0x08

/**
 * @brief Error during the read  procedure
 */
#define GLC_ERROR_IN_READ                    0x09

/******************************************************************************
* Type definitions
******************************************************************************/

/* Glucose Collector Central Information */
typedef struct glcInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** TX output power */
  uint8_t txPower;
  
  /* It enables single racp post processing for PTS tests */
  uint8_t racp_post_processing; 
  /* Storage area size for received notification for a single RACP procedure */
  uint8_t max_num_of_single_racp_notifications;
} glcInitDevType;

typedef struct glcSecurityS {
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
} glcSecurityType;

typedef struct glcConnDevS {
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
} glcConnDevType;

typedef struct glcDevDiscS {
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
} glcDevDiscType;

typedef struct glcConnUpdateParamS {
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
} glcConnUpdateParamType;

/* Glucose measurement characteristic: Base Time structure */
typedef struct _tBasetime
{
   uint16_t year;
   uint8_t month;
   uint8_t day;
   uint8_t hours;
   uint8_t minutes;
   uint8_t seconds;
} tBasetime;

/* Glucose measurement characteristic structure */ 
typedef struct _tGlucoseMeasurementVal {
   /**
    * bitmask of the presence fields  in the characteristic
    */ 
   uint8_t flags;
   uint16_t sequenceNumber;
   tBasetime baseTime;
   int16_t timeOffset;
   uint16_t glucoseConcentration; /* SFLOAT units of Kgs or Liters */
   uint8_t typeSampleLocation;
   uint16_t sensorStatusAnnunciation;
} tGlucoseMeasurementVal;

/* Glucose measurement context characteristic structure */ 
typedef struct _tGlucoseMeasurementContextVal {
   /**
    * bitmask of the presence fields  in the characteristic
    */ 
   uint8_t flags;
   uint16_t sequenceNumber;
   uint8_t extendedFlags;
   uint8_t carbohydrateId;
   uint16_t carbohydrateUnits; /* SFLOAT units of Kgs */
   uint8_t meal;
   uint8_t testerHealth;
   uint16_t exerciseDuration;
   uint8_t exerciseIntensity;
   uint8_t medicationId;
   uint16_t medicationUnits; /* SFLOAT units of Kgs or Liters */
   uint16_t HbA1c;
} tGlucoseMeasurementContextVal;

/* glucose collector Context variable */
typedef struct glucoseCollectorContextS {
  
   /**
   * will contain the timer ID of the 
   * timer started by the glucose collector profile
   * for handling the RACP procedure timings
   */ 
  tTimerID timerID;
  
  uint32_t state;
  uint8_t public_addr[6];
  uint16_t connHandle;
  uint8_t numpeerServices;
  
  uint8_t glucoseServicePresent; 
  uint8_t DeviceInfoServicePresent; 
  
  uint8_t peerServices[PRIMARY_SERVICES_ARRAY_SIZE];
  /* Glucose measurement value handle */
  uint16_t MeasurementCharValHandle; 
  /* Glucose measurement context value handle */
  uint16_t MeasurementContextCharValHandle; 
  /* Glucose feature value handle */
  uint16_t FeatureCharValHandle;
  /* Record Access Control Point (RACP) characteristic value 
   * handle
   */ 
  uint16_t RACPCharValHandle;
     
  /* Device Information Service Characteristics handles */
  uint16_t ManufacturerNameCharValHandle;
  uint16_t ModelNumberCharValHandle;
  uint16_t SystemIDCharValHandle;
  
  uint8_t numCharac;
  uint8_t charac[CHARAC_OF_SERVICE_ARRAY_SIZE];
  uint16_t uuid_searched;
 
  uint8_t data[READ_CHARACTERISTIC_MAX_SIZE]; 
  uint16_t dataLen;
  
  uint8_t fullConf;
  
  /**
  * flags 
  */
  BOOL Is_Write_Ongoing; 
  BOOL MeasurementContext_Is_Expected; 
  BOOL racp_notifications_postprocessig_is_enabled; 
  BOOL start_racp_notifications_analyis; 
  uint16_t number_of_measurement_notifications; 
  uint16_t number_of_measurement_context_notifications; 
  uint16_t number_of_notifications_for_racp; 
  uint16_t max_num_of_single_racp_notifications;

  uint16_t racp_notifications_index;
  uint16_t glucose_feature;
} glucoseCollectorContext_Type;

/* typedef used for storing the specific filter type parameter to be used */
typedef union _tfilterTypeParameter{
  uint16_t sequence_number; 
  tBasetime user_facing_time;
}tfilterTypeParameter;
/******************************************************************************
* Imported Variable
******************************************************************************/
/* glucose collector context */
extern glucoseCollectorContext_Type glucoseCollectorContext;

/******************************************************************************
* Function Declarations
******************************************************************************/

/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref glcInitDevType)
 * @retval Status of call
 */
tBleStatus GL_Collector_Init(glcInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref glcDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the GL_Collector_DeviceDiscovery_CB()
 */
tBleStatus GL_Collector_DeviceDiscovery(glcDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref glcSecurityType)
 * @retval Status of call
 */
tBleStatus GL_Collector_SecuritySet(glcSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * GL_Collector_SecuritySet()  
 * @retval Status of call
 */
tBleStatus GL_Collector_StartPairing(void);

/**
  * @brief Clear Glucose Collector Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus GL_Collector_Clear_Security_Database(void);

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback GL_Collector_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus GL_Collector_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref glcConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * GL_Collector_ConnectionStatus_CB() 
 * This function MUST not be called if the application uses the GL_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus GL_Collector_DeviceConnection(glcConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the GL_Collector_ConnectionStatus_CB()
 */
tBleStatus GL_Collector_DeviceDisconnection(void);

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
tBleStatus GL_Collector_ConnectionParameterUpdateRsp(uint8_t accepted, glcConnUpdateParamType *param);

/**
 * @brief Starts the service discovery procedure
 * on the peer device
 * @retval Status of call
 * @note All the information of the service will be returned
 * on the GL_Collector_ServicesDiscovery_CB()
 * This function MUST not be called if the application uses the GL_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus GL_Collector_ServicesDiscovery(void);

/**
 * @brief Starts the characteristic discovery procedure
 * on the peer device
 * @param uuid_service UUID of the service to use to 
 * discover all the related characteristics
 * @retval Status of call
 * @note All the information of the service will be
 * returned on the GL_Collector_CharacOfService_CB()
 * This function MUST not be called if the application uses the GL_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus GL_Collector_DiscCharacServ(uint16_t uuid_service);


/**
 * @brief Run a profile state machine to execute all the central role procedure:
 * connection, service discovery, characteristic dicovery and 
 * peer device configuration procedure.
 * @param connParam Contains all the values used during the connection
 * procedure (see @ref glcConnDevType)
 * @retval Status of call
 * @note In the application main loop is REQUIRED to call the function 
 * GL_Collector_StateMachine() to run the Glucose Collectorinternal state machine.
 * If some procedure fails, the failure code will be signaled on the 
 * GL_Collector_FullConfError_CB().
 */
tBleStatus GL_Collector_ConnConf(glcConnDevType connParam);

/**
 * @brief Run the Glucose Collector internal state machine. This function is
 * required when the application executes the GL_Collector_ConnConf()
 * @retval None
 */
void GL_Collector_StateMachine(void);

/**
 * @brief Find the device services
 * @param uuid_service  service uuid to be searched
 * @param [out] *start_handle pointer to service start handle
 * @param [out] *end_handle pointer to service end handle
 * @retval Status of call
 */
tBleStatus GL_Collector_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle);

/**
  * @brief Discovery Glucose Measurement characteristic descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus GL_Collector_Start_Glucose_Measurement_Characteristic_Descriptor_Discovery(void);

/**
  * @brief Discovery Glucose Measurement Context characteristic descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus GL_Collector_Start_Glucose_Measurement_Context_Characteristic_Descriptor_Discovery(void);

/**
  * @brief Discovery Glucose RACP characteristic descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus GL_Collector_Start_RACP_Characteristic_Descriptor_Discovery(void);

/**
* @brief  Perform the requested RACP procedure 
* @param  racp_opcode:             RACP opcode
* @param  racp_operator:           RACP operator 
* @param  racp_filter_type:        RACP filter type   
* @param  racp_filter_parameter_1: RACP filter parameter 1 (NULL if not needed)
* @param  racp_filter_parameter_1: RACP filter parameter 2 (NULL if not needed)
* @retval status 
*/
tBleStatus GL_Collector_Send_RACP(uint8_t racp_opcode,
                                  uint8_t racp_operator,
                                  uint8_t racp_filter_type, 
                                  tfilterTypeParameter* racp_filter_parameter_1,
                                  tfilterTypeParameter* racp_filter_parameter_2);

/**
  * @brief Read Glucose Sensor Feature Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus GL_Collector_ReadFeatureChar(void);

/**
  * @brief Read Device Info Manufacturer Name Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus GL_Collector_ReadDISManufacturerNameChar(void);

/**
  * @brief Read Device Info Model Number Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus GL_Collector_ReadDISModelNumberChar(void);

/**
  * @brief Read Device Info System ID Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus GL_Collector_ReadDISSystemIDChar(void);

/**
  * @brief Enable Glucose Measurement Characteristic Notification
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus GL_Collector_Enable_Glucose_Measurement_Char_Notification(void);

/**
  * @brief Enable Glucose Measurement Context Characteristic Notification
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus GL_Collector_Enable_Glucose_Measurement_Context_Char_Notification(void);

/**
  * @brief Enable Glucose RACP Characteristic Indication
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus GL_Collector_Enable_Glucose_RACP_Char_Indication(void);

/**
  * @brief Utility for allowing post processing of received notifications to a single RACP procedure
  * @param [in] None
  * @retval BOOL  
  */
BOOL GL_Collector_Util_Perform_RACP_Post_processing(void);

/**
  * @brief State Machine allowing to handle the post processing utility
  * of received notifications to a single RACP procedure 
  * @param [in] None
  * @retval None  
  */
void GL_Collector_PostProcess_RACP_Notification_SM(void);

/**
* @brief  It checks the write response status and error related to the RACP procedure 
*         It should be called within the Profile event handler, on EVT_BLUE_GATT_PROCEDURE_COMPLETE 
* @param  err_code:  RACP write response error code (from Glucose Sensor) 
*/
void GL_Collector_RACP_Check_Write_Response(uint8_t err_code);

/**
* @brief  It handles the RACP Characteristic Indication from a Glucose Sensor
* @param  attr_handle: glucose  characteristic handle
* @param  data_lenght: glucose sensor characteristic value lenght
* @param  value: glucose sensor characteristic value 
*/
void GL_Collector_RACP_Indications(uint16_t attr_handle, uint8_t data_lenght,uint8_t * value);

/**
  * @brief RACP write procedure timed out handler 
  * @param [in] None
  * @retval None  
  */
void GL_Collector_ProcedureTimeoutHandler(void);

/***************  Glucose Collector Public Function callback ******************/

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
extern void GL_Collector_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
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
 * @note This callback MUST not be called if the application uses the GL_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void GL_Collector_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

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
extern void GL_Collector_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the Glucose Collector receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * GL_Collector_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref glcConnUpdateParamType for more details)
 * @retval None
 */
extern void GL_Collector_ConnectionParameterUpdateReq_CB(glcConnUpdateParamType *param);

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
 * @note This callback MUST not be called if the application uses the GL_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void GL_Collector_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

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
 * @note This callback MUST not be called if the application uses the GL_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void GL_Collector_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This callback returns the read characteristic value of the 
 * connected peer device. Returns, also, the status of the read procedure
 * @param status Status of the read operation
 * @param data_len Length of the data returned
 * @param data Data read
 * @retVal None
 */
extern void GL_Collector_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data);

/**
 * @brief This function is called from the Glucose Collector profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the GL_Collector_SendPinCode() function to set the pin code.
 */
extern void GL_Collector_PinCodeRequired_CB(void);

/**
 * @brief This callack returns the status of the characteristic  enable notification procedure on the
 * connected peer device
 * @param status Status of the procedure 
 * @retval None
 * @note This callback MUST not be called if the application uses the GL_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void GL_Collector_EnableNotification_CB(uint8_t status);
/**
 * @brief This callback is called when an error occurs during the full configuration and connection.
 * The full configuration and connection procedure is run with the GL_Collector_ConnConf() function
 * call.
 * @param error_type Error type (see the application profile error definition)
 * @param code Error code
 * @retval None
 */
extern void GL_Collector_FullConfError_CB(uint8_t error_type, uint8_t code);

/**
 * @brief This callback is called when as response to a RACP write procedure
 * is received just to inform user application (error messages are already
 * raised by glucose collector code)
 * @param error_code Error type 
 * @retval None
 */
extern void GL_Collector_RACP_Write_Response_CB(uint8_t err_code); 

/**
 * @brief This callback is called when a RACP Indication to a RACP write procedure
 * is received just to inform user application (error messages are already
 * raised by glucose collector code)
 * @param racp_response RACP response op code
 * @param value Error code only if racp_response = RACP_RESPONSE_OP_CODE
 * @param num_records number of records only if racp_response = RACP_NUMBER_STORED_RECORDS_RESPONSE_OP_CODE.
 * @retval None
 */
extern void GL_Collector_RACP_Received_Indication_CB(uint8_t racp_response, uint8_t value, uint8_t  num_records); 

/**
 * @brief This callack returns the status of Characteristic notification/indication procedure 
 * @param status Status of the procedure 
 * @retval None
 */
extern void GL_Collector_EnableNotificationIndication_CB(uint8_t status);

/**
 * @brief This callback is called when a Glucose Measurement or Measurement Context Notification is 
 * received  as consequence of a RACP procedure
 * @param handle characteristic handle
 * @param length  length of received data
 * @param data_value array of received data
 * @retval None
 */
extern void GL_Collector_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void GL_Collector_Pairing_CB(uint16_t conn_handle, uint8_t status);

#endif //_GLUCOSE_COLLECTOR_H_
