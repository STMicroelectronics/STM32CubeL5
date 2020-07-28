/******************************************************************************
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
*   FILENAME        -  health_thermometer.h
*
*   COMPILER        -  gnu gcc
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      29/07/2014
*   $Revision$:  second version
*   $Author$:    
*   Comments:    Redesigned the health thermometer sensor profile according to 
*                unified BlueNRG DK framework  
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the health thermometer profile
* 
******************************************************************************/

#ifndef _HEALTH_THERMOMETER_H_
#define _HEALTH_THERMOMETER_H_

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* thermometerFeatures bitmasks */
typedef uint8_t tThermometerFeatures;
#define INTERMEDIATE_TEMPERATURE_CHAR 		 (0x01)
#define MEASUREMENT_INTERVAL_CHAR	         (0x02)
#define TEMPERATURE_TYPE 			 (0x04)

/* bitmask for the flags field */
#define FLAG_TEMPERATURE_UNITS_FARENHEIT 	 (0x01)
#define FLAG_TIMESTAMP_PRESENT 		         (0x02)
#define FLAG_TEMPERATURE_TYPE 		         (0x04)

/* temperature type flags */
typedef uint8_t tTemperatureType;
#define TEMP_MEASURED_AT_ARMPIT 		 (0x01)
#define TEMP_MEASURED_FOR_BODY 			 (0x02)/* this is the generic default value */
#define TEMP_MEASURED_AT_EAR 			 (0x03)
#define TEMP_MEASURED_AT_FINGER 		 (0x04)
#define TEMP_MEASURED_AT_GASTRO_INTESTINAL_TRACT (0x05)
#define TEMP_MEASURED_AT_MOUTH 			 (0x06)
#define TEMP_MEASURED_AT_RECTUM 	         (0x07)
#define TEMP_MEASURED_AT_TOE 			 (0x08)
#define TEMP_MEASURED_AT_TYMPANUM 	         (0x09) /* measured at ear drum */

/******************************************************************************
* Type definitions
******************************************************************************/
typedef struct _tTimestamp
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
}tTimestamp;

typedef struct _tTempMeasurementVal
{
  /**
   * bitmask of the fields supported in the
   * characteristic
   */ 
  uint8_t flags;
    
  /**
   * temperature measurement value
   * if the measurement value is unavailable
   * due to hardware failure or some other reason,
   * then this field has to be set to 0x007FFFFF
   */
   uint32_t temperature;

  /**
   * timestamp of the measurement
   * a value of 0 is not accepted for
   * month and year
   */ 
  tTimestamp timeStamp;
    
  /**
   * temperature type
   */
  tTemperatureType temperatureType; 
}tTempMeasurementVal;

/******************************************************************************
* Function Declarations
******************************************************************************/
/**
 * HT_Init
 * 
 * @param[in] thermometerFeatures: 
 * bitmask for the characteristics to be added to the
 * health thermometer service\n
 * INTERMEDIATE_TEMPERATURE_CHAR - bit 0
 * MEASUREMENT_INTERVAL_CHAR     - bit 1
 * TEMPERATURE_TYPE				 - bit 3
 * if bit3 is set, the temperature type characteristic will
 * be added and its value cannot be updated while in a connection.
 * If the temperature type has to be notified in every measurement
 * value, then this bit should not be set
 * @param[in] minValidInterval: the minimum valid interval
 * value for the measurement interval characteritic. This is
 * valid only if the MEASUREMENT_INTERVAL_CHAR flag is set
 * in the thermometerFeatures
 * @param[in] maxValidInterval: the maximum valid interval
 * value for the measurement interval characteritic. This is
 * valid only if the MEASUREMENT_INTERVAL_CHAR flag is set
 * in the thermometerFeatures
 * @param[in] applcb: callback function to be called by
 * the profile to notify the application of the events
 * 
 * Initializes the health thermometer profile and registers
 * it with the BLE profile
 * 
 * @return BLE_STATUS_SUCCESS if the profile was successfully
 * registered
 */ 
tBleStatus HT_Init(tThermometerFeatures thermometerFeatures,
                   uint16_t minValidInterval,
                   uint16_t maxValidInterval,
                   BLE_CALLBACK_FUNCTION_TYPE applcb); 

/**
 * HT_Advertize
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
tBleStatus HT_Advertize(uint8_t useWhitelist);

/**
 * HT_Send_Temperature_Measurement
 * 
 * @param[in] tempMeasurementVal :  temperature measurement value
 * the flags have to be set according to the flag bitmask defined 
 * in the health_thermometer header
 * 
 * updates the temperature measurement characteristic
 * 
 * @return BLE_STATUS_SUCCESS if the update was started succesfully.
 * Once the update is completed, an event 
 */ 
tBleStatus HT_Send_Temperature_Measurement(tTempMeasurementVal tempMeasurementVal);

/**
 * HT_Send_Intermediate_Temperature
 * 
 * @param[in] tempMeasurementVal :  temperature measurement value
 * the flags have to be set according to the flag bitmask defined 
 * in the health_thermometer header. It is recommended not to use 
 * the timestamp and temperature type fields
 * 
 * updates the temperature measurement characteristic
 * 
 * @return BLE_STATUS_SUCCESS if the update was started succesfully.
 * Once the update is completed, an event 
 */ 
tBleStatus HT_Send_Intermediate_Temperature(tTempMeasurementVal tempMeasurementVal);

/**
 * HT_Update_Measurement_Interval
 * 
 * @param[in] interval : the interval value to be written to
 * the measurement interval characteristic. The temperature
 * measurements will be sent only after a gap of the interval
 * specified. If the application gives a value to be sent before
 * the interval is passed, an error is returned. A value of
 * 0 indicates that there need not be a periodic update
 */ 
tBleStatus HT_Update_Measurement_Interval(uint16_t interval);

/**
 * HT_Update_Temperature_Type
 * 
 * @param[in] type : one of the values defined in
 *            the header
 * 
 * Updates the temperature type. The temperature
 * type indicates the part of the body where the
 * temperature is being measured. This has to be 
 * static during an active connection
 */ 
tBleStatus HT_Update_Temperature_Type(uint8_t type);

/**
 * HT_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 * 
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB().
 */ 
void HT_Event_Handler(void *pckt);

/**
 * HT_StateMachine
 * 
 * @param[in] None
 * 
 * HR profile's state machine: to be called on application main loop. 
 */ 
tBleStatus HT_StateMachine(void);


#endif /* _HEALTH_THERMOMETER_H_ */
