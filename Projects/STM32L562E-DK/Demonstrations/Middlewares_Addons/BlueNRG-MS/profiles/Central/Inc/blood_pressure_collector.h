/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
* File Name          : blood_pressure_collector.h
* Author             : AMS - AAS, RF Application Team
* Version            : V1.0.0
* Date               : 11-February-2015
* Description        : Header for blood_pressure_collector.c module
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BLOOD_PRESSURE_COLLECTOR_H
#define __BLOOD_PRESSURE_COLLECTOR_H

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

#include "blood_pressure_service.h"

/******************************************************************************
* Macro Declarations
******************************************************************************/

#define READ_CHARACTERISTIC_MAX_SIZE            30   //TBR: value to be tuned 
#define BP_FEATURE_READ_SIZE			2
#define BP_DIS_MANUFACTURER_NAME_SIZE           22   //TBR: value to be tuned 
#define BP_DIS_MODEL_NUMBER_SIZE                22      //TBR: value to be tuned 
#define BP_DIS_SYSTEM_ID_SIZE                   8  

#define PRIMARY_SERVICES_ARRAY_SIZE 50
#define CHARAC_OF_SERVICE_ARRAY_SIZE 100

/*
#define MAX_BPM_CHAR_VAL_LEN			19
#define MAX_ICP_CHAR_VAL_LEN			19
*/

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )

/* Blood Pressure collector States */
#define BLOOD_PRESSURE_COLLECTOR_UNINITIALIZED                          0x0000
#define BLOOD_PRESSURE_COLLECTOR_CONNECTED_IDLE                         0x0001
#define BLOOD_PRESSURE_COLLECTOR_INITIALIZED                            0x0002
#define BLOOD_PRESSURE_COLLECTOR_DEVICE_DISCOVERY                       0x0003
#define BLOOD_PRESSURE_COLLECTOR_START_CONNECTION                       0x0004
#define BLOOD_PRESSURE_COLLECTOR_CONNECTED                              0x0005

#define BLOOD_PRESSURE_COLLECTOR_SERVICE_DISCOVERY                      0x0006

#define BLOOD_PRESSURE_COLLECTOR_CHARAC_DISCOVERY                       0x0007 
#define BLOOD_PRESSURE_COLLECTOR_WAIT_CHARAC_DISCOVERY                  0x0008 

#define BLOOD_PRESSURE_COLLECTOR_CONN_PARAM_UPDATE_REQ                  0x0009
#define BLOOD_PRESSURE_COLLECTOR_DISCONNECTION                          0x000A

#define BLOOD_PRESSURE_COLLECTOR_READ_BP_FEATURE                        0x000B 
#define BLOOD_PRESSURE_COLLECTOR_WAIT_READ_BP_FEATURE                   0x000C

#define BLOOD_PRESSURE_COLLECTOR_ENABLE_BP_MEASUREMENT_INDICATION       0x000D 
#define BLOOD_PRESSURE_COLLECTOR_WAIT_ENABLE_BP_MEASUREMENT_INDICATION  0x000E

#define BLOOD_PRESSURE_COLLECTOR_ENABLE_ICP_NOTIFICATION                0x000F 
#define BLOOD_PRESSURE_COLLECTOR_WAIT_ENABLE_ICP_NOTIFICATION           0x0010

/* Blood Pressure Collector Error Condition */

/**
 * @brief Error during the service discovery procedure
 */
#define BPC_ERROR_IN_SERVICE_DISCOVERY       0x01

/**
 * @brief Error during the disconnection procedure
 */
#define BPC_ERROR_IN_DISCONNECTION           0x02

/**
 * @brief Error during the characteristic discovery procedure
 */
#define BPC_ERROR_IN_CHARAC_DISCOVERY        0x03

/**
 * @brief Error during the characteristic descriptor discovery procedure
 */
#define BPC_ERROR_IN_CHARAC_DESC_DISCOVERY   0x04

/**
 * @brief Error during the Write Procedure
 */
//#define BPC_ERROR_IN_WRITE_PROCEDURE         0x05

/**
 * @brief Error during the enable of indication 
 */
#define BPC_ERROR_IN_ENABLE_INDICATION       0x06
 
  
/**
 * @brief Error during the enable of notification
 */
#define BPC_ERROR_IN_ENABLE_NOTIFICATION     0x07
   
/**
 * @brief Error during the connection procedure procedure
 */
#define BPC_ERROR_IN_CONNECTION              0x08

/**
 * @brief Error during the read  procedure
 */
#define BPC_ERROR_IN_READ                    0x09



/* Typedef Definition --------------------------------------------------------*/

/* Profile Central Information */
typedef struct bpcInitDevS {
  /** Device Public address */
  uint8_t *public_addr;
  /** Device name length */
  uint8_t device_name_len;
  /** Device name */
  uint8_t *device_name;
  /** TX output power */
  uint8_t txPower;
} bpcInitDevType;

typedef struct bpcSecurityS {
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
} bpcSecurityType;

typedef struct bpcConnDevS {
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
} bpcConnDevType;

typedef struct bpcDevDiscS {
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
} bpcDevDiscType;

typedef struct bpcConnUpdateParamS {
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
} bpcConnUpdateParamType;


typedef struct _tDateTime
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
}tDateTime;


typedef struct _tMeasurementStatus
{
   /**
    * BODY_MOVEMENT_DETECTION_FLAG
    *   0 -> no body movement 
    * 	1 -> body movement during measurement
    */ 
  uint8_t bodyMovementDetectionFlag;

  /**
   * CUFF_FIT_DETECTION_FLAG
   *     0 -> cuff fits properly
   *     1 -> cuff too lose
   */ 
  uint8_t cuffFitDetectionFlag;

   /**
    * IRREGULAR_PULSE_DETECTION_FLAG
    *     0 -> no irregular pulse detected
    *     1 -> irregular pulse detected
    */ 
    
  uint8_t irregularPulseDetectionFlag;

   /**
    * PULSE_RATE_RANGE_DETECTION_FLAG
    *     00 -> pulse rate is within the range
    *     01 -> pulse rate exceeds upper limit
    *     02 -> pulse rate is lesser than lower limit
    */
  uint8_t pulseRateRangeDetectionFlag;

  /**
    * MEASUREMENT_POSITION_DETECTION_FLAG
    *     0 -> proper measurement position
    *     1 -> improper measurement position
    */
  uint8_t measurementPositionDetectionFlag;
}tMeasurementStatus;

typedef struct _tBPMValue
{
  /**
   * the flags is a bitmask which tells the peer
   * of the data to follow:\n
   * bit0 : a value of 1 indicates that the unit is kPa
   *        a value of 0 indicates that the unit is mmHg
   * bit1 : a value of 1 implies there is a timestamp field
   *        present in the data
   * bit2 : a value of 1 implies there is a pulse rate field
   *        present in the data
   * bit3 : a value of 1 implies there is a user ID field
   *        present in the data
   * bit4 : a value of 1 implies there is a measurement status
   * 	   field present in the data
   */ 
  uint8_t flags;

  /**
   * Blood Pressure Measurement Compound Value systolic  
   * pressure is an SFLOAT-Type. It is defined as a 16-bit
   * value with an exponent of a 4-bit signed integer followed 
   * by a mantissa of a12-bit signed integer
   */ 
  int16_t systolic_mantissa;
  int8_t systolic_exponent;

  /**
   * Blood Pressure Measurement Compound Value diastolic  
   * pressure is an SFLOAT-Type. It is defined as a 16-bit 
   * value with an exponent of a 4-bit signed integer followed 
   * by a mantissa of a12-bit signed integer
   */ 
  int16_t diastolic_mantissa;
  int8_t diastolic_exponent;

  /**
   * Blood Pressure Measurement Compound Value Mean Arterial
   * Pressures is an SFLOAT-Type. It is defined as a 16-bit 
   * value with an exponent of a 4-bit signed integer followed 
   * by a mantissa of a12-bit signed integer
   */ 
  int16_t map_mantissa;
  int8_t map_exponent;
  /** 
    * Assigned number for "millimetre_of_mercury (mmHg)"  
    * or Pascal (kPa) unit in the International
    * Bureau of Weights and Measures (BIPM)
    */
   uint16_t bloodPressureUnit; 
  /**
   * if the timestamp flag is set to true,
   * this field chould contain the date and
   * time
   */ 
  tDateTime timeStamp;
  /**
   * if the pulse rate flag is set to true,
   * this field should contain the pulse rate.  
   * It is an SFLOAT-Type. It is defined as a 16-bit 
   * value with an exponent of a 4-bit signed integer followed 
   * by a mantissa of a12-bit signed integer
   */
   int16_t pulseRate_mantissa;   
   int8_t pulseRate_exponent;
   /**
    * if the user id flag is set to true (device support multiple users),
    * this field should contain the unique USER ID
    */ 
   uint8_t userID;
   /**
    * measurement status 
    */   
  tMeasurementStatus measurementStatus;
}tBPMValue;

typedef struct _tICPValue  /* Intermdediate Cuff Pressure characteristic*/
{
  /**
   * the flags is a bitmask which tells the peer
   * of the data to follow:\n
   * bit0 : a value of 1 indicates that the unit is kPa
   *        a value of 0 indicates that the unit is mmHg
   * bit3 : a value of 1 implies there is a user ID field
   *        present in the data
   * bit4 : a value of 1 implies there is a measurement status
   * 		  field present in the data
   */ 
  uint8_t flags;
  
  /**
   * Intermediate Cuff Pressure Compound Value field 
   * Systolic sub-field becomes Current Cuff Pressure sub-field
   * (Diastolic and MAP fields are unused):
   * it is an SFLOAT-Type, defined as a 16-bit 
   * value with an exponent of a 4-bit signed integer followed 
   * by a mantissa of a12-bit signed integer
   */ 
  uint16_t icp_mantissa; 
  int8_t icp_exponent;
  /** 
    * Assigned number for "millimetre_of_mercury (mmHg)"  
    * or Pascal (kPa) unit in the International
    * Bureau of Weights and Measures (BIPM)
    */
   uint16_t bloodPressureUnit; 
  /**
   * if the timestamp flag is set to true,
   * this field chould contain the date and
   * time
   */ 
  tDateTime timeStamp;
  /**
   * if the pulse rate flag is set to true,
   * this field should contain the pulse rate.  
   * It is an SFLOAT-Type. It is defined as a 16-bit 
   * value with an exponent of a 4-bit signed integer followed 
   * by a mantissa of a12-bit signed integer
   */
   int16_t pulseRate_mantissa;   
   int8_t pulseRate_exponent;
   /**
    * if the user id flag is set to true (device support multiple users),
    * this field should contain the unique USER ID
    */ 
   uint8_t userID;
   /**
    * measurement status 
    */ 
    tMeasurementStatus measurementStatus;
}tICPValue;

/* Blood Pressure collector profile context: it contains timer, chracteristics handles and flags 
   used from collector + peer context for Heart Rate Sensor Discovery ...
*/ 
typedef struct bloodPressureCollectorContextS
{	
  uint32_t state;
  uint8_t public_addr[6];
  uint16_t connHandle;
  uint8_t numpeerServices;
  
  uint8_t BloodPressureServicePresent; 
  uint8_t DeviceInfoServicePresent; 
  
  uint8_t peerServices[PRIMARY_SERVICES_ARRAY_SIZE];
  
  /**
   * Blood Pressure Measurement characteristic value handle
   */ 
  uint16_t BPMeasurementCharValHandle;
  /**
   * Intermediate Cuff Pressure characteristic value handle
   */ 
  uint16_t ICPCharValHandle;
  /**
   * Blood Pressure Feature characteristic value handle
   */ 
  uint16_t BPFeatureCharValHandle;
  
  /**
   * Device Information Service Characteristic value handle
   */
  uint16_t ManufacturerNameCharValHandle;
  uint16_t ModelNumberCharValHandle;
  uint16_t SystemIDCharValHandle;
  
  /**
   * Blood Pressure Measurement characteristic value
   */
  tBPMValue BPMeasurementValue;
  /**
   * Intermediate Cuff Pressure characteristic value
   */ 
  tICPValue ICPValue;
  /**
   * Blood Pressure Feature characteristic value
   */ 
  uint16_t BPFeatureValue;
  
  
  uint8_t numCharac;
  uint8_t charac[CHARAC_OF_SERVICE_ARRAY_SIZE];
  uint16_t uuid_searched;
 
  uint8_t data[READ_CHARACTERISTIC_MAX_SIZE]; 
  uint16_t dataLen;
  
  uint8_t fullConf;

} bloodPressureCollectorContext_Type;


/******************************************************************************
* Imported Variable
******************************************************************************/

/* Blood Pressure collector context */
extern bloodPressureCollectorContext_Type bloodPressureCollectorContext; 


/******************************************************************************
* Function Declarations
******************************************************************************/

/**
 * @brief Init the Central device
 * @param param Contains all the values to initialize the device
 * (see @ref bpcInitDevType)
 * @retval Status of call
 */
tBleStatus BPC_Init(bpcInitDevType param);

/**
 * @brief Starts the device discovery procedure
 * @param param Contains all the values to setup the procedure
 * (see @ref bpcDevDiscType)
 * @retval Status of call
 * @note All the information of the device discovered will be returned
 * on the BPC_DeviceDiscovery_CB()
 */
tBleStatus BPC_DeviceDiscovery(bpcDevDiscType param);

/**
 * @brief Setup the device security parameters
 * @param param Contains all the values to setup the device security
 * (see @ref bpcSecurityType)
 * @retval Status of call
 */
tBleStatus BPC_SecuritySet(bpcSecurityType param);

/**
 * @brief Starts Pairing procedure with the peer device
 * according the device security parameters set with the
 * BPC_SecuritySet()  
 * @retval Status of call
 */
tBleStatus BPC_StartPairing(void);

/**
  * @brief Clear Profile  Security database
  * @param [in] None
  * @retval tBleStatus: BLE status
  */
tBleStatus BPC_Clear_Security_Database(void);

/**
 * @brief Sends the pin code during the pairing procedure
 * when required from the peer device.
 * @param pinCode Pin Code required from the peer device
 * @retval Status of call
 * @note This function is called if the callback BPC_PinCodeRequired_CB()
 * is raised from the proxmity monitor profile code.
 */
tBleStatus BPC_SendPinCode(uint32_t pinCode);

/**
 * @brief Starts the Connection procedure with the peer device
 * @param param Contains all the values used during the connection
 * procedure (see @ref bpcConnDevType)
 * @retval Status of call
 * @note The status of the connection procedure will be returned in the 
 * BPC_ConnectionStatus_CB() 
 * This function MUST not be called if the application uses the BPC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus BPC_DeviceConnection(bpcConnDevType param); 

/**
 * @brief Starts the device disconnection procedure
 * @retval Status of call
 * @note The status of the disconnection will be returned on 
 * the BPC_ConnectionStatus_CB()
 */
tBleStatus BPC_DeviceDisconnection(void);

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
tBleStatus BPC_ConnectionParameterUpdateRsp(uint8_t accepted, bpcConnUpdateParamType *param);

/**
 * @brief Starts the service discovery procedure
 * on the peer device
 * @retval Status of call
 * @note All the information of the service will be returned
 * on the BPC_ServicesDiscovery_CB()
 * This function MUST not be called if the application uses the BPC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus BPC_ServicesDiscovery(void);

/**
 * @brief Starts the characteristic discovery procedure
 * on the peer device
 * @param uuid_service UUID of the service to use to 
 * discover all the related characteristics
 * @retval Status of call
 * @note All the information of the service will be
 * returned on the BPC_CharacOfService_CB()
 * This function MUST not be called if the application uses the BPC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
tBleStatus BPC_DiscCharacServ(uint16_t uuid_service);


/**
 * @brief Run a profile state machine to execute all the central role procedure:
 * connection, service discovery, characteristic dicovery and 
 * peer device configuration procedure.
 * @param connParam Contains all the values used during the connection
 * procedure (see @ref bpcConnDevType)
 * @retval Status of call
 * @note In the application main loop is REQUIRED to call the function 
 * BPC_StateMachine() to run the device internal state machine.
 * If some procedure fails, the failure code will be signaled on the 
 * BPC_FullConfError_CB().
 */
tBleStatus BPC_ConnConf(bpcConnDevType connParam);

/**
 * @brief Run the profile internal state machine. This function is
 * required when the application executes the BPC_ConnConf()
 * @retval None
 */
void BPC_StateMachine(void);

/**
 * @brief Find the device services
 * @param uuid_service  service uuid to be searched
 * @param [out] *start_handle pointer to service start handle
 * @param [out] *end_handle pointer to service end handle
 * @retval Status of call
 */
tBleStatus BPC_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle);

/**
  * @brief Discovery Blood Pressure Measurement Client Characteristic Descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_Start_Blood_Pressure_Measurement_Client_Char_Descriptor_Discovery(void);

/**
  * @brief Discovery Intermediate Cuff Pressure Client Characteristic Descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_Start_ICP_Client_Char_Descriptor_Discovery(void);

/**
  * @brief Read Device Info Manufacturer Name Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_ReadDISManufacturerNameChar(void);

/**
  * @brief Read Device Info Model Number Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_ReadDISModelNumberChar(void);

/**
  * @brief Read Device Info System ID Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_ReadDISSystemIDChar(void);

/**
  * @brief Enable Blood Pressure Measurement Characteristic for Indication
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_Enable_BP_Measurement_Char_Indication(void);

/**
  * @brief Enable Intermediate Cuff Pressure Characteristic for Notification
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_Enable_ICP_Char_Notification(void);

/**
  * @brief Read Blood Pressure Feature characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_Read_BP_Feature(void);

/**
  * @brief  Store & analyse the Characteristic Value for Blood Pressure feature Char
  * @param [in] None
  * @retval Error Code
  */
void BPC_Read_BP_Feature_CB(void);

/**
* @brief  It stores the Blood Pressure Measurement Characteristics Indication
* @param  attr_handle: Blood Pressure Measurement characteristic handle
* @param  data_lenght: Blood Pressure Measurement characteristic value lenght
* @param  value: Blood Pressure Measurement characteristic value 
*/
void BPC_BP_Measurement_Indication_CB(uint16_t attr_handle, uint8_t data_length,uint8_t * value);

/**
* @brief  It stores the Intermediate Cuff Pressure Characteristics Notification
* @param  attr_handle: Intermediate Cuff Pressure characteristic handle
* @param  data_lenght: Intermediate Cuff Pressure characteristic value lenght
* @param  value: Intermediate Cuff Pressure characteristic value 
*/
void BPC_ICP_Notification_CB(uint16_t attr_handle, uint8_t data_length,uint8_t * value);


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
extern void BPC_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
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
 * @note This callback MUST not be called if the application uses the BPC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void BPC_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services);

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
extern void BPC_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status);

/**
 * @brief This callback is called when the device receives a connection
 * parameter update request from the  peripheral device connected.
 * Theresponse from the central device may be ignored or sent by 
 * BPC_ConnectionParameterUpdateRsp()
 * @param param Connection update parameter sent by the connected peripheral device.
 * (see @ref bpcConnUpdateParamType for more details)
 * @retval None
 */
extern void BPC_ConnectionParameterUpdateReq_CB(bpcConnUpdateParamType *param);

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
 * @note This callback MUST not be called if the application uses the BPC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void BPC_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

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
 * @note This callback MUST not be called if the application uses the BPC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void BPC_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac);

/**
 * @brief This callback returns the read characteristic value of the 
 * connected peer device. Returns, also, the status of the read procedure
 * @param status Status of the read operation
 * @param data_len Length of the data returned
 * @param data Data read
 * @retVal None
 */
extern void BPC_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data);

/**
 * @brief This function is called from the profile when the
 * MITM pin code is required from the peer device. Inside this function the application
 * needs to call the BPC_SendPinCode() function to set the pin code.
 */
extern void BPC_PinCodeRequired_CB(void);

/**
 * @brief This callack returns the status of the characteristic enable indication procedure on the
 * connected peer device
 * @param status Status of the procedure 
 * @retval None
 * @note This callback MUST not be called if the application uses the BPC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void BPC_EnableIndication_CB(uint8_t status);

/**
 * @brief This callack returns the status of the characteristic enable notification procedure on the
 * connected peer device
 * @param status Status of the procedure 
 * @retval None
 * @note This callback MUST not be called if the application uses the BPC_ConnConf()
 * function, because this procedure is included in the state machine 
 */
extern void BPC_EnableNotification_CB(uint8_t status);

/**
 * @brief This callback is called when an error occurs during the full configuration and connection.
 * The full configuration and connection procedure is run with the BPC_ConnConf() function
 * call.
 * @param error_type Error type (see the application profile error definition)
 * @param code Error code
 * @retval None
 */
extern void BPC_FullConfError_CB(uint8_t error_type, uint8_t code);

/**
 * @brief This callback is called when a Blood Pressure Measurement indication is received 
 * @param handle characteristic handle
 * @param length  length of received data
 * @param data_value array of received data
 * @retval None
 */
extern void BPC_IndicationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value);

/**
 * @brief This callback is called when an Intermediate Cuff Pressure notification is received
 * @param handle characteristic handle
 * @param length  length of received data
 * @param data_value array of received data
 * @retval None
 */
extern void BPC_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value);

/**
 * @brief This callback is called for providing the pairing procedure status
 * @param conn_handle connection handle
 * @param status  pairing status
 * @retval None
 */
extern void BPC_Pairing_CB(uint16_t conn_handle, uint8_t status);


#endif /* __BLOOD_PRESSURE_COLLECTOR_H */
