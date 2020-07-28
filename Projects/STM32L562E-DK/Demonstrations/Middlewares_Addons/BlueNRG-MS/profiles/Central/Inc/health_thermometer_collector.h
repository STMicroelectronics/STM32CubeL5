/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
* File Name          : health_thermometer_collector.h
* Author             : AMS - AAS, RF Application Team
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
#ifndef __HEALTH_THERMOMETER_COLLECTOR_H
#define __HEALTH_THERMOMETER_COLLECTOR_H

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

#include "health_thermometer_service.h"

/******************************************************************************
* Macro Declarations
******************************************************************************/

#define READ_CHARACTERISTIC_MAX_SIZE            30   //TBR: value to be tuned 
#define TEMP_TYPE_READ_SIZE                     1 
#define MEAS_INTER_READ_SIZE                    2 
#define MEAS_INTER_VALID_RANGE_READ_SIZE        4    // should be 8, but in the Profile project is 4 
#define HT_DIS_MANUFACTURER_NAME_SIZE           22   //TBR: value to be tuned 
#define HT_DIS_MODEL_NUMBER_SIZE                22   //TBR: value to be tuned 
#define HT_DIS_SYSTEM_ID_SIZE                   8          

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )

/* health thermometer collector States */
#define HEALTH_THERMOMETER_COLLECTOR_UNINITIALIZED                                     0x0000
#define HEALTH_THERMOMETER_COLLECTOR_CONNECTED_IDLE                                    0x0001
#define HEALTH_THERMOMETER_COLLECTOR_INITIALIZED                                       0x0002
#define HEALTH_THERMOMETER_COLLECTOR_DEVICE_DISCOVERY                                  0x0003
#define HEALTH_THERMOMETER_COLLECTOR_START_CONNECTION                                  0x0004
#define HEALTH_THERMOMETER_COLLECTOR_CONNECTED                                         0x0005

#define HEALTH_THERMOMETER_COLLECTOR_SERVICE_DISCOVERY                                 0x0006

#define HEALTH_THERMOMETER_COLLECTOR_MEASUREMENT_CHARAC_DISCOVERY                      0x0007 
#define HEALTH_THERMOMETER_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY                 0x0008 

#define HEALTH_THERMOMETER_COLLECTOR_ENABLE_TEMPERATURE_MEASUREMENT_INDICATION         0x0009   
#define HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_TEMPERATURE_MEASUREMENT_INDICATION    0x000A   
#define HEALTH_THERMOMETER_COLLECTOR_ENABLE_INTERMEDIATE_TEMPERATURE_NOTIFICATION      0x000B   
#define HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_INTERMEDIATE_TEMPERATURE_NOTIFICATION 0x000C   
#define HEALTH_THERMOMETER_COLLECTOR_ENABLE_MEASUREMENT_INTERVAL_INDICATION            0x000D  
#define HEALTH_THERMOMETER_COLLECTOR_WAIT_ENABLE_MEASUREMENT_INTERVAL_INDICATION       0x000E  

#define HEALTH_THERMOMETER_COLLECTOR_SEND_RACP                                         0x000F  
#define HEALTH_THERMOMETER_COLLECTOR_WAIT_SEND_RACP                                    0x0010 
#define HEALTH_THERMOMETER_COLLECTOR_CONN_PARAM_UPDATE_REQ                             0x0011
#define HEALTH_THERMOMETER_COLLECTOR_DISCONNECTION                                     0x0012

#define HEALTH_THERMOMETER_COLLECTOR_READ_TEMPERATURE_TYPE                             0x0013  
#define HEALTH_THERMOMETER_COLLECTOR_WAIT_READ_TEMPERATURE_TYPE                        0x0014 

#define HEALTH_THERMOMETER_COLLECTOR_READ_MEASUREMENT_INTERVAL                         0x0015  
#define HEALTH_THERMOMETER_COLLECTOR_WAIT_READ_MEASUREMENT_INTERVAL                    0x0016 

#define PRIMARY_SERVICES_ARRAY_SIZE 50
#define CHARAC_OF_SERVICE_ARRAY_SIZE 100 

/* health thermometer collector Error Condition */

/**
 * @brief Error during the service discovery procedure
 */
#define HTC_ERROR_IN_SERVICE_DISCOVERY       0x01

/**
 * @brief Error during the disconnection procedure
 */
#define HTC_ERROR_IN_DISCONNECTION           0x02

/**
 * @brief Error during the characteristic discovery procedure
 */
#define HTC_ERROR_IN_CHARAC_DISCOVERY        0x03

/**
 * @brief Error during the characteristic descriptor discovery procedure
 */
#define HTC_ERROR_IN_CHARAC_DESC_DISCOVERY   0x04

/**
 * @brief Error during the Write Procedure
 */
#define HTC_ERROR_IN_WRITE_PROCEDURE         0x05

/**
 * @brief Error during the enable of indication 
 */
#define HTC_ERROR_IN_ENABLE_INDICATION       0x06
 
  
/**
 * @brief Error during the enable of notification
 */
#define HTC_ERROR_IN_ENABLE_NOTIFICATION     0x07
   
/**
 * @brief Error during the connection procedure procedure
 */
#define HTC_ERROR_IN_CONNECTION              0x08

/**
 * @brief Error during the read  procedure
 */
#define HTC_ERROR_IN_READ                    0x09

/******************************************************************************
* Type definitions
******************************************************************************/

/* Health Thermometer Collector Central Information */
typedef struct htcInitDevS {
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
} htcInitDevType;

typedef struct htcSecurityS {
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
} htcSecurityType;

typedef struct htcConnDevS {
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
} htcConnDevType;

typedef struct htcDevDiscS {
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
} htcDevDiscType;

typedef struct htcConnUpdateParamS {
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
} htcConnUpdateParamType;

typedef struct _tTimestamp
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
}tTimestamp;

typedef struct _tTemperatureMeasurementVal 
{
   /**
    * bitmask of the presence fields  in the characteristic
    */ 
   uint8_t flags;
   /** 
    * The Temperature Measurement Field is a FLOAT-Type data type.
    * This is defined to represent numeric values that are not integer in type. 
    * The FLOAT-Type is defined as a 32-bit value that contains
    * an 8-bit signed exponent to base 10, followed by a 24-bit signed integer (mantissa).
    *
    * Temperature Measurement Value = Temp_Meas_Mantissa * pow(10, Temp_Meas_Exponent)
    *
    */
   int32_t Temp_Meas_Mantissa;  
   int8_t Temp_Meas_Exponent;
   /** 
    * Assigned number for Celsius or Fahrenheit unit in the 
    * International Bureau of Weights and Measures (BIPM)
    */
   uint16_t Temp_Meas_Unit; 
   tTimestamp Time_Stamp;
   uint8_t Temp_Type; 
} TemperatureMeasurementVal;


typedef struct _tValidRange
{
  /* minimum valid interval for the measurement interval characteristic */ 
  uint16_t minValidInterval;   
  /* maximum valid interval for the measurement interval characteristic */ 
  uint16_t maxValidInterval;
}tValidRange;

/* health Thermometer Collector Context variable */
typedef struct healthThermometerCollectorS {
  
  uint32_t state;
  uint8_t public_addr[6];
  uint16_t connHandle;
  uint8_t numpeerServices;
  
  uint8_t HT_ServicePresent;          //TBR??? glucoseServicePresent --> HT_ServicePresent
  uint8_t DeviceInfoServicePresent; 
  uint8_t peerServices[PRIMARY_SERVICES_ARRAY_SIZE];
  /**
   * Temperature Measurement characteristic value handle
   */ 
  uint16_t TempMeasurementCharValHandle;
  /**
   * Temperature Type characteristic handle
   */ 
  uint16_t TempTypeCharValHandle;
  /**
   * Intermediate Temperature characteristic handle
   */ 
  uint16_t IntermediateTempCharValHandle;
  /**
   * Measurement Interval characteristic handle
   */ 
  uint16_t MeasurementIntervalCharValHandle;
  
  /* Device Information Service Characteristics handles */
  uint16_t ManufacturerNameCharValHandle;
  uint16_t ModelNumberCharValHandle;
  uint16_t SystemIDCharValHandle;
  
  
  /**
   * Temperature Measurement characteristic value
   */
  TemperatureMeasurementVal TempMeasurementValue;
  /**
   * Temperature Type characteristic value
   */ 
  uint8_t TempTypeValue;
  /**
   * Intermediate Temperature characteristic value
   */ 
  TemperatureMeasurementVal IntermediateTempValue;
  /**
   * Measurement Interval characteristic value
   */ 
  uint16_t MeasurementIntervalValue;
  /**
   * Measurement Interval Valid Range Descriptor value
   */ 
  tValidRange MeasurementIntervalValidRangeValue;
     
  uint8_t numCharac;
  uint8_t charac[CHARAC_OF_SERVICE_ARRAY_SIZE];
  uint16_t uuid_searched;
 
  uint8_t data[READ_CHARACTERISTIC_MAX_SIZE]; 
  uint16_t dataLen;
  
  uint8_t fullConf;
  
} healthThermometerCollectorContext_Type;

/******************************************************************************
* Imported Variable
******************************************************************************/

/* Health Thermometer collector context */
extern healthThermometerCollectorContext_Type healthThermometerCollectorContext; 

extern BOOL bReadValidRangeDescr;


/******************************************************************************
* Function Declarations
******************************************************************************/

/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref htcInitDevType)
 * @retval Status of call
 */
tBleStatus HT_Collector_Init(htcInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref htcDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the HT_Collector_DeviceDiscovery_CB()
 */
tBleStatus HT_Collector_DeviceDiscovery(htcDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref htcSecurityType)
 * @retval Status of call
 */
tBleStatus HT_Collector_SecuritySet(htcSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * HT_Collector_SecuritySet()  
 * @retval Status of call
 */
tBleStatus HT_Collector_StartPairing(void);

/**
  * @brief Clear Glucose Collector Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus HT_Collector_Clear_Security_Database(void);

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback HT_Collector_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus HT_Collector_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref htcConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * HT_Collector_ConnectionStatus_CB() 
 * This function MUST not be called if the application uses the HT_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus HT_Collector_DeviceConnection(htcConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the HT_Collector_ConnectionStatus_CB()
 */
tBleStatus HT_Collector_DeviceDisconnection(void);

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
tBleStatus HT_Collector_ConnectionParameterUpdateRsp(uint8_t accepted, htcConnUpdateParamType *param);

/**
 * @brief Starts the service discovery procedure
 * on the peer device
 * @retval Status of call
 * @note All the information of the service will be returned
 * on the HT_Collector_ServicesDiscovery_CB()
 * This function MUST not be called if the application uses the HT_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus HT_Collector_ServicesDiscovery(void);

/**
 * @brief Starts the characteristic discovery procedure
 * on the peer device
 * @param uuid_service UUID of the service to use to 
 * discover all the related characteristics
 * @retval Status of call
 * @note All the information of the service will be
 * returned on the HT_Collector_CharacOfService_CB()
 * This function MUST not be called if the application uses the HT_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus HT_Collector_DiscCharacServ(uint16_t uuid_service);


/**
 * @brief Run a profile state machine to execute all the central role procedure:
 * connection, service discovery, characteristic dicovery and 
 * peer device configuration procedure.
 * @param connParam Contains all the values used during the connection
 * procedure (see @ref htcConnDevType)
 * @retval Status of call
 * @note In the application main loop is REQUIRED to call the function 
 * HT_Collector_StateMachine() to run the Glucose Collectorinternal state machine.
 * If some procedure fails, the failure code will be signaled on the 
 * HT_Collector_FullConfError_CB().
 */
tBleStatus HT_Collector_ConnConf(htcConnDevType connParam);

/**
 * @brief Run the Glucose Collector internal state machine. This function is
 * required when the application executes the HT_Collector_ConnConf()
 * @retval None
 */
void HT_Collector_StateMachine(void);

/**
 * @brief Find the device services
 * @param uuid_service  service uuid to be searched
 * @param [out] *start_handle pointer to service start handle
 * @param [out] *end_handle pointer to service end handle
 * @retval Status of call
 */
tBleStatus HT_Collector_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle);

/**
  * @brief Discovery Temperature Measurement Client Characteristic Descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Start_Temperature_Measurement_Client_Char_Descriptor_Discovery(void);


/**
  * @brief Discovery Intermediate Temperature Client Characteristic Descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Start_Intermediate_Temperature_Client_Char_Descriptor_Discovery(void);

/**
  * @brief Discovery Measurement Interval Client Characteristic Descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Start_Measurement_Interval_Client_Char_Descriptor_Discovery(void);

/**
  * @brief Read Device Information Service Characteristics
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_ReadDISManufacturerNameChar(void);

/**
  * @brief Read Device Info Model Number Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_ReadDISModelNumberChar(void);

/**
  * @brief Read Device Info System ID Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_ReadDISSystemIDChar(void);

/**
  * @brief Enable Temperature Measurement Characteristic for Indication
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Enable_Temperature_Measurement_Char_Indication(void);

/**
  * @brief Enable Intermediate Temperature Characteristic for Notification
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Enable_Intermediate_Temperature_Char_Notification(void);

/**
  * @brief Enable Measurement Interval Characteristic for Indication
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Enable_Measurement_Interval_Char_Indication(void);

/**
  * @brief Read Measurement Interval characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Read_Measurement_Interval(void);

/**
  * @brief Write Measurement Interval characteristic 
    * @param [in] writeValue: value to write 
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Write_Measurement_Interval(uint16_t writeValue);

/**
  * @brief Read Measurement Interval Valid Range descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Read_Measurement_Interval_Valid_Range_Descr(void);

/**
  * @brief Read Temperature Type characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Read_Temperature_Type(void);

/**
  * @brief  Read the Characteristic Value for Measurement Interval Char
  * @param [in] None
  * @retval Error Code
  */
void HT_Collector_Read_Measurement_Interval_CB(void); 

/**
 * @brief  It checks the write response status and error related to the 
 *         Measurement_Interval characteristic write request     
 *         It should be called within the Profile event handler, on EVT_BLUE_GATT_ERROR_RESP, 
 *         EVT_BLUE_GATT_PROCEDURE_COMPLETE (TBR?)  
 * @param  err_code: write response error code (from Health Thermometer Sensor) 
 */
void HT_Collector_Measurement_Interval_Check_Write_Response_CB(uint8_t err_code);

/**
  * @brief  Read the Value for Measurement Interval Valid Range Descriptor
  * @param [in] None
  * @retval Error Code
  */
void HT_Collector_Read_Measurement_Interval_Valid_Range_CB(void);

/**
  * @brief  Read the Characteristic Value for Temperature Type Char
  * @param [in] None
  * @retval Error Code
  */
void HT_Collector_Read_Temperature_Type_CB(void); 

/**
* @brief  It stores the Temperature Measurement Characteristics Indication
* @param  attr_handle: Temperature Measurement characteristic handle
* @param  data_lenght: Temperature Measurement characteristic value lenght
* @param  value: Temperature Measurement characteristic value 
*/
void HT_Collector_Temperature_Measurement_Indication_CB(uint16_t attr_handle, uint8_t data_length,uint8_t * value);

/**
* @brief  It stores the Intermediate Temperature Characteristics Notification
* @param  attr_handle: Intermediate Temperature characteristic handle
* @param  data_lenght: Intermediate Temperature characteristic value lenght
* @param  value: Intermediate Temperature characteristic value 
*/
void HT_Collector_Intermediate_Temperature_Notification_CB(uint16_t attr_handle, uint8_t data_length,uint8_t * value);

/**
* @brief  It stores the Measurement Interval Characteristics Indication
* @param  attr_handle: Measurement Interval characteristic handle
* @param  data_lenght: Measurement Interval characteristic value lenght
* @param  value: Measurement Interval characteristic value 
*/
void HT_Collector_Measurement_Interval_Indication_CB(uint16_t attr_handle, uint8_t data_length,uint8_t * value);


/**
  * @brief Read Measurement Interval Valid Range Descriptor characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HT_Collector_Read_Measurement_Interval_Valid_Range_Descriptor(void);

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
extern void HT_Collector_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
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
 * @note This callback MUST not be called if the application uses the HT_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HT_Collector_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

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
extern void HT_Collector_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the Glucose Collector receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * HT_Collector_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref htcConnUpdateParamType for more details)
 * @retval None
 */
extern void HT_Collector_ConnectionParameterUpdateReq_CB(htcConnUpdateParamType *param);

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
 * @note This callback MUST not be called if the application uses the HT_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HT_Collector_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

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
 * @note This callback MUST not be called if the application uses the HT_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HT_Collector_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This callback returns the read characteristic value of the 
 * connected peer device. Returns, also, the status of the read procedure
 * @param status Status of the read operation
 * @param data_len Length of the data returned
 * @param data Data read
 * @retVal None
 */
extern void HT_Collector_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data);

/**
 * @brief This function is called from the Glucose Collector profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the HT_Collector_SendPinCode() function to set the pin code.
 */
extern void HT_Collector_PinCodeRequired_CB(void);

/**
 * @brief This callack returns the status of the characteristic  enable notification procedure on the
 * connected peer device
 * @param status Status of the procedure 
 * @retval None
 * @note This callback MUST not be called if the application uses the HT_Collector_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void HT_Collector_EnableNotification_CB(uint8_t status);
/**
 * @brief This callback is called when an error occurs during the full configuration and connection.
 * The full configuration and connection procedure is run with the HT_Collector_ConnConf() function
 * call.
 * @param error_type Error type (see the application profile error definition)
 * @param code Error code
 * @retval None
 */
extern void HT_Collector_FullConfError_CB(uint8_t error_type, uint8_t code);

/**
 * @brief This callack returns the status of Characteristic notification/indication procedure 
 * @param status Status of the procedure 
 * @retval None
 */
extern void HT_Collector_EnableNotificationIndication_CB(uint8_t status);

/**
 * @brief This callback is called when a  Notification is received 
 * @param handle characteristic handle
 * @param length  length of received data
 * @param data_value array of received data
 * @retval None
 */
extern void HT_Collector_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value); 

/**
 * @brief This callback is called when an Indication is received 
 * @param handle characteristic handle
 * @param length  length of received data
 * @param data_value array of received data
 * @retval None
 */
extern void HT_Collector_IndicationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void HT_Collector_Pairing_CB(uint16_t conn_handle, uint8_t status);


#endif /* __HEALTH_THERMOMETER_COLLECTOR_H */
