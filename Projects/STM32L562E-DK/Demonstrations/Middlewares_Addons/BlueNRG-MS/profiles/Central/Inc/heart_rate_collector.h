/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : heart_rate_collector.h
* Author             : AMS - VMA, RF Application Team
* Version            : V1.0.0
* Date               : 14-January-2015
* Description        : Header for master.c module
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HEART_RATE_COLLECTOR_H
#define __HEART_RATE_COLLECTOR_H

/* Includes ------------------------------------------------------------------*/
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

#include "heart_rate_service.h"

/******************************************************************************
* Macro Declarations
******************************************************************************/

#define READ_CHARACTERISTIC_MAX_SIZE 30    //TBR: value to be tuned 
#define HR_READ_SIZE  1 
#define HR_DIS_MANUFACTURER_NAME_SIZE 22   //TBR: value to be tuned 

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )
/* heart rate collector States */
#define HEART_RATE_COLLECTOR_UNINITIALIZED                                  0x0000
#define HEART_RATE_COLLECTOR_CONNECTED_IDLE                                 0x0001
#define HEART_RATE_COLLECTOR_INITIALIZED                                    0x0002
#define HEART_RATE_COLLECTOR_DEVICE_DISCOVERY                               0x0003
#define HEART_RATE_COLLECTOR_START_CONNECTION                               0x0004
#define HEART_RATE_COLLECTOR_CONNECTED                                      0x0005

#define HEART_RATE_COLLECTOR_SERVICE_DISCOVERY                              0x0006

#define HEART_RATE_COLLECTOR_MEASUREMENT_CHARAC_DISCOVERY                   0x0007 
#define HEART_RATE_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY              0x0008 

#define HEART_RATE_COLLECTOR_SEND_CP                                        0x0009  
#define HEART_RATE_COLLECTOR_WAIT_SEND_CP                                   0x000A  

#define HEART_RATE_COLLECTOR_CONN_PARAM_UPDATE_REQ                          0x000B
#define HEART_RATE_COLLECTOR_DISCONNECTION                                  0x000C

#define HEART_RATE_COLLECTOR_READ_BODY_SENSOR                               0x000D
#define HEART_RATE_COLLECTOR_WAIT_READ_BODY_SENSOR                          0x000E

#define HEART_RATE_COLLECTOR_ENABLE_MEASUREMENT_NOTIFICATION                0x000F  
#define HEART_RATE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_NOTIFICATION           0x0010


#define PRIMARY_SERVICES_ARRAY_SIZE 50
#define CHARAC_OF_SERVICE_ARRAY_SIZE 100

/* Heart Rate Collector Error Condition */

/**
 * @brief Error during the service discovery procedure
 */
#define HRC_ERROR_IN_SERVICE_DISCOVERY       0x01

/**
 * @brief Error during the disconnection procedure
 */
#define HRC_ERROR_IN_DISCONNECTION           0x02

/**
 * @brief Error during the characteristic discovery procedure
 */
#define HRC_ERROR_IN_CHARAC_DISCOVERY        0x03

/**
 * @brief Error during the characteristic descriptor discovery procedure
 */
#define HRC_ERROR_IN_CHARAC_DESC_DISCOVERY   0x04

/**
 * @brief Error during the Write Procedure
 */
#define HRC_ERROR_IN_WRITE_PROCEDURE         0x05

/**
 * @brief Error during the enable of indication 
 */
#define HRC_ERROR_IN_ENABLE_INDICATION       0x06
 
  
/**
 * @brief Error during the enable of notification
 */
#define HRC_ERROR_IN_ENABLE_NOTIFICATION     0x07
   
/**
 * @brief Error during the connection procedure procedure
 */
#define HRC_ERROR_IN_CONNECTION              0x08

/**
 * @brief Error during the read  procedure
 */
#define HRC_ERROR_IN_READ                    0x09

/* Typedef Definition --------------------------------------------------------*/

/* Profile Central Information */
typedef struct hrcInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** TX output power */
  uint8_t txPower;
} hrcInitDevType;

typedef struct hrcSecurityS {
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
} hrcSecurityType;

typedef struct hrcConnDevS {
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
} hrcConnDevType;

typedef struct hrcDevDiscS {
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
} hrcDevDiscType;

typedef struct hrcConnUpdateParamS {
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
} hrcConnUpdateParamType;

typedef struct _tHeartRateMeasurementVal {
   /**
    * bitmask of the presence fields  in the characteristic
    */ 
   uint8_t flags;
   uint8_t HRM_value8;
   uint16_t HRM_value16;
   uint16_t energyExpended;
   uint16_t *RR_interval;
   uint8_t maxNumRR_interval;
} tHeartRateMeasurementVal;

/* Heart Rate collector profile context: it contains timer, chracteristics handles and flags 
   used from collector + peer context for Heart Rate Sensor Discovery ...
*/ 
typedef struct heartRateCollectorContextS
{	
  uint32_t state;
  uint8_t public_addr[6];
  uint16_t connHandle;
  uint8_t numpeerServices;
  
  uint8_t HeartRateServicePresent;  // glucoseServicePresent --> HeartRateServicePresent
  uint8_t DeviceInfoServicePresent; 
  
  uint8_t peerServices[PRIMARY_SERVICES_ARRAY_SIZE];
  
  /**
   * Heart Rate measurement characteristic value handle
   */ 
  uint16_t HRMeasurementCharValHandle;
  /**
   * Body Sensor Location characteristic value handle
   */ 
  uint16_t BodySensorLocValHandle;
  /**
   * Heart Rate Control Point characteristic value handle
   */ 
  uint16_t HRControlPointValHandle;
  
  /**
   * Device Information Service Characteristic value handle
   */
  uint16_t ManufacturerNameCharValHandle;  
  
  uint8_t numCharac;
  uint8_t charac[CHARAC_OF_SERVICE_ARRAY_SIZE];
  uint16_t uuid_searched;
 
  uint8_t data[READ_CHARACTERISTIC_MAX_SIZE]; 
  uint16_t dataLen;
  
  uint8_t fullConf;
  
  /**
   * Heart Rate Measurement characteristic value
   */
  tHeartRateMeasurementVal HRMeasurementValue;
  /**
   * Body Sensor Location characteristic value
   */
  uint8_t BodySensorLocValue;
  
  /**
   * Heart Rate Control Point  characteristic value
   */
  uint8_t HRControlPointValue;  
  
  /**
  * flags 
  */  
  BOOL bEnergyExpended;
} heartRateCollectorContext_Type;

/******************************************************************************
* Imported Variable
******************************************************************************/
/* heart rate collector context */
extern heartRateCollectorContext_Type heartRateCollectorContext; 


/******************************************************************************
* Function Declarations
******************************************************************************/

/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref hrcInitDevType)
 * @retval Status of call
 */
tBleStatus HRC_Init(hrcInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref hrcDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the HRC_DeviceDiscovery_CB()
 */
tBleStatus HRC_DeviceDiscovery(hrcDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref hrcSecurityType)
 * @retval Status of call
 */
tBleStatus HRC_SecuritySet(hrcSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * HRC_SecuritySet()  
 * @retval Status of call
 */
tBleStatus HRC_StartPairing(void);

/**
  * @brief Clear Profile  Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus HRC_Clear_Security_Database(void);

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback HRC_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus HRC_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref hrcConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * HRC_ConnectionStatus_CB() 
 * This function MUST not be called if the application uses the HRC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus HRC_DeviceConnection(hrcConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the HRC_ConnectionStatus_CB()
 */
tBleStatus HRC_DeviceDisconnection(void);

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
tBleStatus HRC_ConnectionParameterUpdateRsp(uint8_t accepted, hrcConnUpdateParamType *param);

/**
 * @brief Starts the service discovery procedure
 * on the peer device
 * @retval Status of call
 * @note All the information of the service will be returned
 * on the HRC_ServicesDiscovery_CB()
 * This function MUST not be called if the application uses the HRC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus HRC_ServicesDiscovery(void);

/**
 * @brief Starts the characteristic discovery procedure
 * on the peer device
 * @param uuid_service UUID of the service to use to 
 * discover all the related characteristics
 * @retval Status of call
 * @note All the information of the service will be
 * returned on the HRC_CharacOfService_CB()
 * This function MUST not be called if the application uses the HRC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus HRC_DiscCharacServ(uint16_t uuid_service);


/**
 * @brief Run a profile state machine to execute all the central role procedure:
 * connection, service discovery, characteristic dicovery and 
 * peer device configuration procedure.
 * @param connParam Contains all the values used during the connection
 * procedure (see @ref hrcConnDevType)
 * @retval Status of call
 * @note In the application main loop is REQUIRED to call the function 
 * HRC_StateMachine() to run the device internal state machine.
 * If some procedure fails, the failure code will be signaled on the 
 * HRC_FullConfError_CB().
 */
tBleStatus HRC_ConnConf(hrcConnDevType connParam);

/**
 * @brief Run the profile internal state machine. This function is
 * required when the application executes the HRC_ConnConf()
 * @retval None
 */
void HRC_StateMachine(void);

/**
 * @brief Find the device services
 * @param uuid_service  service uuid to be searched
 * @param [out] *start_handle pointer to service start handle
 * @param [out] *end_handle pointer to service end handle
 * @retval Status of call
 */
tBleStatus HRC_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle);

/**
  * @brief Discovery Heart Rate Measurement characteristic descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HRC_Start_Heart_Rate_Measurement_Characteristic_Descriptor_Discovery(void);

/**
  * @brief Read Device Info Manufacturer Name Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HRC_ReadDISManufacturerNameChar(void);

/**
  * @brief Enable Heart Rate Measurement Characteristic Notification
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HRC_Enable_HR_Measurement_Char_Notification(void);

/**
  * @brief Body Sensor Location characteristic read 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HRC_Read_Body_Sensor_Location(void);

/**
  * @brief Heart Rate Control Point characteristic write
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HRC_Write_HR_Control_Point(void);

/**
  * @brief  Store & analyse the Characteristic Value for Body Sensor Location Char
  * @param [in] None
  * @retval Error Code
  */
void HRC_Read_Body_Sensor_Location_Handler(void); 

/**
 * @brief  It checks the write response status and error related to the 
 *         Heart Rate Control Point (HRCP) write request to reset energy expended      
 *         It should be called within the Profile event handler, on EVT_BLUE_GATT_ERROR_RESP, 
 *         EVT_BLUE_GATT_PROCEDURE_COMPLETE (TBR?)  
 * @param  err_code:  HRCP write response error code (from Heart Rate Sensor) 
 */
void HRC_CP_Check_Write_Response_Handler(uint8_t err_code);


/**
* @brief  It stores and analyse the Heart Rate Measurement Characteristics Notifications
* @param  attr_handle: heart rate measurement characteristic handle
* @param  data_lenght: heart rate measurement characteristic value lenght
* @param  value: heart rate measurement characteristic value 
*/
void HRC_Notification_Handler(uint16_t attr_handle, uint8_t data_length,uint8_t * value);


/*************** Public Function callback ******************/

/**
 * @brief This callback contains all the information of the device discovered
 * during the central procedure
 * @param status Status of the device discovery procedure
 * @param addr_type Address type of the device discovered
 * @param addr Address of the device discovered
 * @param data_length Data length of the information from the discovered device
 * @param data Data of the discvered device
 * @param RSSI RSSI received during the device discovery procedure
 * @retval None
 */
extern void HRC_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
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
 * @note This callback MUST not be called if the application uses the HRC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HRC_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

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
extern void HRC_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the device receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * HRC_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref hrcConnUpdateParamType for more details)
 * @retval None
 */
extern void HRC_ConnectionParameterUpdateReq_CB(hrcConnUpdateParamType *param);

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
 * @note This callback MUST not be called if the application uses the HRC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HRC_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

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
 * @note This callback MUST not be called if the application uses the HRC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HRC_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This callback returns the read characteristic value of the 
 * connected peer device. Returns, also, the status of the read procedure
 * @param status Status of the read operation
 * @param data_len Length of the data returned
 * @param data Data read
 * @retVal None
 */
extern void HRC_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data);

/**
 * @brief This function is called from the profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the HRC_SendPinCode() function to set the pin code.
 */
extern void HRC_PinCodeRequired_CB(void);

/**
 * @brief This callack returns the status of the characteristic  enable notification procedure on the
 * connected peer device
 * @param status Status of the procedure 
 * @retval None
 * @note This callback MUST not be called if the application uses the HRC_ConnConf()
 * function, because this procedure is included in the state machine 
 */

extern void HRC_EnableNotification_CB(uint8_t status);
/**
 * @brief This callback is called when an error occurs during the full configuration and connection.
 * The full configuration and connection procedure is run with the HRC_ConnConf() function
 * call.
 * @param error_type Error type (see the application profile error definition)
 * @param code Error code
 * @retval None
 */
extern void HRC_FullConfError_CB(uint8_t error_type, uint8_t code);

/**
 * @brief This callback is called when as response to a RACP write procedure
 * is received just to inform user application (error messages are already
 * raised by device code)
 * @param error_code Error type 
 * @retval None
 */
extern void HRC_CP_Write_Response_CB(uint8_t err_code); 

/**
 * @brief This callback is called when a body sensor is  //TBR???
 * received  as consequence of a RACP procedure
 * @param handle characteristic handle
 * @param length  length of received data
 * @param data_value array of received data
 * @retval None
 */
extern void HRC_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void HRC_Pairing_CB(uint16_t conn_handle, uint8_t status);

#endif /* __HEART_RATE_COLLECTOR_H */
