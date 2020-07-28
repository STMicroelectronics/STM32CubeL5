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
*   FILENAME        -  health_thermometer.c
*
*   COMPILER        -  EWARM
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
*  source code for the sensor role of health thermometer profile
* 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
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
#include <ble_profile.h>
#include <ble_events.h>
#include <uuid.h>

#include <health_thermometer.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* profile states */
#define HT_UNINITIALIZED				(0x00)
#define HT_WAITING_BLE_INIT                             (0x01)

#define HT_IDLE						(0x02)
#define HT_INITIALIZED					(0x03)
#define HT_UNDIRECTED_ADV_MODE				(0x04)
#define HT_UNDIRECTED_ADV_MODE_STOP		        (0x05)
#define HT_ADV_IN_FAST_DISC_MODE			(0x06)
#define HT_ADV_IN_FAST_DISC_MODE_STOP                   (0x07)
#define HT_ADV_IN_LOW_PWR_MODE				(0x08)
#define HT_CONNECTED					(0x09)

/* connected substates */
#define HT_CONNECTED_DO_TEMPERATURE_MEASUREMENT		(0x0A)
#define HT_CONNECTED_STOP_TEMPERATURE_MEASUREMENT	(0x0B)
#define HT_CONNECTED_DO_INTERMEDIATE_TEMPERATURE	(0x0C)
#define HT_CONNECTED_STOP_INTERMEDIATE_TEMPERATURE      (0x0D)
#define HT_CONNECTED_DO_MEASUREMENT_INTERVAL		(0x0E)
#define HT_CONNECTED_STOP_MEASUREMENT_INTERVAL		(0x0F)


  
/* advertising intervals in terms of 625 micro seconds */
#define HT_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define HT_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define HT_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define HT_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */

#define TEMPERATURE_MEASUREMENT_MAX_LEN  (13)

/* NOTE: out of range error code was defined as 0x80 in HTS 1.0. 
   But in Core spec supplement (CSS v4), common profile and service error code is 
   defined and Out of Range is 0xFF now. (Page 30 of CSS v4):

   0xFD Client Characteristic Configuration Descriptor Improperly Configured
   0xFE Procedure Already in Progress
   0xFF Out of Range
*/

/* PTS 6.0.0 is using again 0x80 */
#define HTS_SPECIFICATION_1_0 1 
#if HTS_SPECIFICATION_1_0
/* health thermometer profile specific error code */
#define INTERVAL_OUT_OF_RANGE 			(0x80)
#else
/* Core spec supplement (CSS v4) erro code or out of range: PTS 5.2.0, TC_SP_BI_01_C 
   is expecting this value */
#define INTERVAL_OUT_OF_RANGE 			(0xFF)
#endif

#define MIN_ENCRYPTION_KEY_SIZE			(10)

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )

#define STORE_LE_32(buf, val)    ( ((buf)[0] =  (uint8_t) (val)     ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8)  ) , \
                                   ((buf)[2] =  (uint8_t) (val>>16) ) , \
                                   ((buf)[3] =  (uint8_t) (val>>24) ) ) 

/******************************************************************************
* type definitions
******************************************************************************/
typedef struct _tThermometerContext
{
  /**
   * state of the health thermometer
   * sensor state machine
   */ 
  tProfileState state;
  
  /**
   * substate of the health thermometer
   * sensor state machine
   */ 
  tProfileState substate;    
  
  /**
   * callback to notify the applciation with
   * the profile specific events
   */ 
  BLE_CALLBACK_FUNCTION_TYPE applcb;
      
   /**
   * flag which indicates the features supported
   * by the blood pressure sensor
   */ 
  uint8_t thermometerFeatures;
      
  /**
   * minimum valid interval for the measurement
   * interval characteristic
   */ 
  uint16_t minValidInterval;
      
  /**
   * maximum valid interval for the measurement
   * interval characteristic
   */ 
  uint16_t maxValidInterval;
      
  /**
   * current measurement interval. This field is
   * updated everytime, the client writes to this
   * characteristic or the user updates this field.
   * The default value of this field is 0
   */ 
  uint16_t measurementInterval;
      
  /**
   * health thermometer service handle
   */ 
  uint16_t htServiceHandle;
      
  /**
   * temperature measurement characteristic
   * handle
   */ 
  uint16_t tempMeasurementCharHandle;
      
  /**
   * temperature type characteristic
   * handle
   */ 
  uint16_t tempTypeCharHandle;
      
  /**
   * intermediate temperature
   * characteristic handle
   */ 
  uint16_t intermediateTempCharHandle;
      
  /**
   * measurement interval characteristic
   * handle
   */ 
  uint16_t miCharHandle;
  
  /**
   * will contain the timer ID of the 
   * timer started by the health thermometer profile
   * the same timerID will be used to handle both
   * advertising as well as idle connection timeout
   * since both timers are not required simultaneously
   */ 
  tTimerID timerID;
      
  /**
   * timer ID of the timer started to keep track
   * of the interval between sending of 2
   * temperature measurements
   */ 
  tTimerID intervalTimerID;
  
  /**
   * connection handle
   */
   uint16_t connHandle;

  /**
   * profile interface context. This is
   * registered with the main profile
   */ 
  tProfileInterfaceContext HTtoBLEInf;
}tThermometerContext;

/******************************************************************************
* Variable Declarations
******************************************************************************/
static tThermometerContext thermometer;

/******************************************************************************
* Imported Variable
******************************************************************************/
extern void *profiledbgfile;
extern uint8_t bnrg_expansion_board;

/******************************************************************************
 * Function Prototypes
******************************************************************************/
static tBleStatus HT_Add_Services_Characteristics(void);
static tBleStatus HT_Sensor_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax);
static void HT_AdvTimeout_Handler(void);
static void HT_Exit(uint8_t errCode);
static void HT_IdleConnection_Handler(void);
static void HT_Interval_Expired(void);

static void HTProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState HTProfile_Read_MainStateMachine(void);
static void HTProfile_Write_SubStateMachine(tProfileState localsubState);
static tProfileState HTProfile_Read_SubStateMachine(void);
/*******************************************************************************
* SAP
*******************************************************************************/
tBleStatus HT_Init(uint8_t thermometerFeatures,
                   uint16_t minValidInterval,
                   uint16_t maxValidInterval,
                   BLE_CALLBACK_FUNCTION_TYPE applcb)
{
  tBleStatus retval = BLE_STATUS_FAILED;
      
  HT_DBG_MSG(profiledbgfile, "HT_Init()\n");

  if(Is_Profile_Present(PID_HEALTH_THERMOMETER) == BLE_STATUS_SUCCESS)
  {
    return BLE_STATUS_PROFILE_ALREADY_INITIALIZED;
  }

  if(applcb == NULL)
  {
    return BLE_STATUS_NULL_PARAM;
  }
      
  /* Initialize Data Structures */
  BLUENRG_memset(&thermometer,0,sizeof(thermometer));
  thermometer.applcb = applcb;
  thermometer.timerID = 0xFF;
  thermometer.intervalTimerID = 0xFF;
  thermometer.thermometerFeatures = thermometerFeatures;
  
  if(thermometerFeatures & MEASUREMENT_INTERVAL_CHAR)
  {
    thermometer.minValidInterval = minValidInterval;
    thermometer.maxValidInterval = maxValidInterval;
  }
  else 
  {
    /* thermometer.measurementInterval should be set to 1 on initialization for allowing the temperature measurement indication
       when the temperature measurement characteristic is not supported (otherwise no temperature can be notified) */
    thermometer.measurementInterval = 1; 
  }
      
  HTProfile_Write_MainStateMachine(HT_UNINITIALIZED);
  HTProfile_Write_SubStateMachine(HT_UNINITIALIZED);
      
  /* register the blood pressure sensor with BLE main Profile */
  BLUENRG_memset(&thermometer.HTtoBLEInf,0,sizeof(thermometer.HTtoBLEInf));
  thermometer.HTtoBLEInf.profileID = PID_HEALTH_THERMOMETER;
  thermometer.HTtoBLEInf.callback_func = HT_Event_Handler;
  thermometer.HTtoBLEInf.voteForAdvertisableState = 0;
  /* manufacturer name, model number and system ID characteristics
   * have to be added to the device information service
   */ 
  thermometer.HTtoBLEInf.DISCharRequired = (MANUFACTURER_NAME_STRING_CHAR_MASK|MODEL_NUMBER_STRING_CHAR_MASK|SYSTEM_ID_CHAR_MASK);
  retval = BLE_Profile_Register_Profile(&thermometer.HTtoBLEInf);
  if (retval != BLE_STATUS_SUCCESS)
  {
      /* Profile could not be registered with the BLE main profile */
      return (retval);
  }
      
  HTProfile_Write_MainStateMachine(HT_WAITING_BLE_INIT); 
  HT_DBG_MSG(profiledbgfile,"Health Thermometer Profile is Initialized\n");
      
  return (BLE_STATUS_SUCCESS);
}/* end HT_Init() */

tBleStatus HT_Advertize(uint8_t useWhitelist)
{
  tBleStatus status = BLE_STATUS_FAILED;
  
  HT_DBG_MSG(profiledbgfile,"HT_Advertize() %x\n",HTProfile_Read_MainStateMachine());
  
  if(HTProfile_Read_MainStateMachine() == HT_INITIALIZED)
  {
    if(useWhitelist)
    {
      /* start the undirected connectable mode */
      status = aci_gap_set_undirected_connectable(PUBLIC_ADDR, WHITE_LIST_FOR_ALL);   
      if(status == BLE_STATUS_SUCCESS)
      {
        HTProfile_Write_MainStateMachine(HT_UNDIRECTED_ADV_MODE); 
        
        HT_DBG_MSG(profiledbgfile,"HT_Advertize(), enabled undirected connectable mode \n");
        /* start a timer for 120 seconds */
        Blue_NRG_Timer_Start(120,HT_AdvTimeout_Handler,&thermometer.timerID);
        
        /* request for main profile's state change */
        BLE_Profile_Change_Advertise_Sub_State(&thermometer.HTtoBLEInf, 
                                               BLE_PROFILE_STATE_ADVERTISING);
      }
      else
      {
        /* inform the application that the advertizing was not
         * enabled
         */
        HT_DBG_MSG (profiledbgfile,"HT_Advertize(), FAILED to start undirected connectable mode, Error: %02X !!\n", status);
        HTProfile_Write_MainStateMachine(HT_IDLE);
        thermometer.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
      }
    }
    else
    {
      status = HT_Sensor_Set_Discoverable(HT_FAST_CONN_ADV_INTERVAL_MIN,HT_FAST_CONN_ADV_INTERVAL_MAX);
      if(status == BLE_STATUS_SUCCESS)
      {
        HTProfile_Write_MainStateMachine(HT_ADV_IN_FAST_DISC_MODE);
        HT_DBG_MSG(profiledbgfile,"HT_Advertize(), enabled Limited discoverable mode, fast connection\n");
        
        /* start a timer for 30 seconds */
        Blue_NRG_Timer_Start(30,HT_AdvTimeout_Handler,&thermometer.timerID);
        /* request for main profile's state change */
        BLE_Profile_Change_Advertise_Sub_State(&thermometer.HTtoBLEInf, 
                                               BLE_PROFILE_STATE_ADVERTISING);
      }
      else
      {
        /* inform the application that the advertizing was not
         * enabled
         */
        HT_DBG_MSG(profiledbgfile,"HT_Advertize(),FAILED to start limited discoverable mode %x\n",status); 
        HTProfile_Write_MainStateMachine(HT_IDLE);
        thermometer.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
      }
    }
  }
  return status;
}/* end HT_Advertize() */

tBleStatus HT_Send_Temperature_Measurement(tTempMeasurementVal tempMeasurementVal)
{
  uint8_t charval[TEMPERATURE_MEASUREMENT_MAX_LEN];
  uint8_t idx = 0;
  tBleStatus retval = BLE_STATUS_FAILED;

  HT_DBG_MSG(profiledbgfile,"HT_Send_Temperature_Measurement(),thermometer.state: %02X,thermometer.intervalTimerID:%02X, thermometer.measurementInterval:%02X \n", HTProfile_Read_MainStateMachine(),thermometer.intervalTimerID,thermometer.measurementInterval);
  if((HTProfile_Read_MainStateMachine() == HT_CONNECTED) &&
      (thermometer.intervalTimerID == 0xFF) &&
          (thermometer.measurementInterval != 0x00))
  {
    BLUENRG_memset(charval,0x00,TEMPERATURE_MEASUREMENT_MAX_LEN);
    
    charval[idx] = tempMeasurementVal.flags;
    if(thermometer.thermometerFeatures & TEMPERATURE_TYPE)
    {
      /* if the temperature type is static, then
       * the temperature type characteristic has to
       * be used and the temperature flag should not be used
       */ 
       charval[idx] &= ~FLAG_TEMPERATURE_TYPE;
    }
    idx++;
    STORE_LE_32(&charval[idx],tempMeasurementVal.temperature);
    idx += 4;
    
    if(tempMeasurementVal.flags & FLAG_TIMESTAMP_PRESENT)
    {
      if((tempMeasurementVal.timeStamp.year == 0) ||(tempMeasurementVal.timeStamp.month == 0))
      {
        HT_DBG_MSG(profiledbgfile,"HT_Send_Temperature_Measurement(), Error: tempMeasurementVal.timeStamp.year: %d, tempMeasurementVal.timeStamp.month: %d\n",tempMeasurementVal.timeStamp.year, tempMeasurementVal.timeStamp.month);
        return BLE_STATUS_INVALID_PARAMS;
      }
      
      HT_DBG_MSG(profiledbgfile,"HT_Send_Temperature_Measurement(), year %d\n",tempMeasurementVal.timeStamp.year);
      
      STORE_LE_16(&charval[idx],tempMeasurementVal.timeStamp.year);
      HT_DBG_MSG(profiledbgfile,"%x %x\n",charval[idx],charval[idx+1]);
      idx += 2;
      charval[idx++] = tempMeasurementVal.timeStamp.month;
      charval[idx++] = tempMeasurementVal.timeStamp.day;
      charval[idx++] = tempMeasurementVal.timeStamp.hours;
      charval[idx++] = tempMeasurementVal.timeStamp.minutes;
      charval[idx++] = tempMeasurementVal.timeStamp.seconds;
    }
    
    /* only if the temperture type is non-static i.e, the
     * TEMPERATURE_TYPE flag is not set this field
     * has to be included
     */ 
    if((!(thermometer.thermometerFeatures & TEMPERATURE_TYPE)) &&
       (tempMeasurementVal.flags & FLAG_TEMPERATURE_TYPE))
    {
      HT_DBG_MSG(profiledbgfile,"adding temperature type\n");
      charval[idx++] = tempMeasurementVal.temperatureType;
    }
    
    retval =  aci_gatt_update_char_value(thermometer.htServiceHandle,
                                         thermometer.tempMeasurementCharHandle,
                                         0, /* charValOffset */
                                         idx, /* charValueLen */
                                         (uint8_t *) charval);  
    
    if(retval == BLE_STATUS_SUCCESS)
    {
      
      /* stop the idle timer */
     Blue_NRG_Timer_Stop(thermometer.timerID);
      
      /* start the interval timer if the indications have
       * to be sent only periodically
       */
      Blue_NRG_Timer_Start(thermometer.measurementInterval,HT_Interval_Expired,&thermometer.intervalTimerID);
      
      HT_DBG_MSG(profiledbgfile,"HT_Send_Temperature_Measurement(), temperature measurement sent successfully\n");
      
      HTProfile_Write_SubStateMachine(HT_CONNECTED_DO_TEMPERATURE_MEASUREMENT);
    }
    else
    {
      HT_DBG_MSG(profiledbgfile,"HT_Send_Temperature_Measurement(), FAILED to send temperature %02X \n", retval);
    }
  }
  
  return retval;
}/* END HT_Send_Temperature_Measurement() */


tBleStatus HT_Send_Intermediate_Temperature(tTempMeasurementVal tempMeasurementVal)
{
  uint8_t charval[TEMPERATURE_MEASUREMENT_MAX_LEN];
  uint8_t idx = 0;
  tBleStatus retval = BLE_STATUS_FAILED;
  
  if((thermometer.thermometerFeatures & INTERMEDIATE_TEMPERATURE_CHAR) &&
     (HTProfile_Read_MainStateMachine() == HT_CONNECTED))
  {
    BLUENRG_memset(charval,0x00,TEMPERATURE_MEASUREMENT_MAX_LEN);
    
    charval[idx] = tempMeasurementVal.flags;
    if(thermometer.thermometerFeatures & TEMPERATURE_TYPE)
    {
      /* if the temperature type is static, then
       * the temperature type characteristic has to
       * be used and the temperature flag should not be used
       */ 
       charval[idx] &= ~FLAG_TEMPERATURE_TYPE;
    }
    idx++;
    STORE_LE_32(&charval[idx],tempMeasurementVal.temperature);
    idx += 4;
    
    if(tempMeasurementVal.flags & FLAG_TIMESTAMP_PRESENT)
    {
      if((tempMeasurementVal.timeStamp.year == 0) ||
         (tempMeasurementVal.timeStamp.month == 0))
      {
              return BLE_STATUS_INVALID_PARAMS;
      }
      
      HT_DBG_MSG(profiledbgfile,"year %x\n",tempMeasurementVal.timeStamp.year);
      STORE_LE_16(&charval[idx],tempMeasurementVal.timeStamp.year);
      HT_DBG_MSG(profiledbgfile,"%x %x\n",charval[idx],charval[idx+1]);
      idx += 2;
      charval[idx++] = tempMeasurementVal.timeStamp.month;
      charval[idx++] = tempMeasurementVal.timeStamp.day;
      charval[idx++] = tempMeasurementVal.timeStamp.hours;
      charval[idx++] = tempMeasurementVal.timeStamp.minutes;
      charval[idx++] = tempMeasurementVal.timeStamp.seconds;
    }
    
    /* only if the temperture type is non-static i.e, the
     * TEMPERATURE_TYPE flag is not set this field
     * has to be included
     */ 
    if((!(thermometer.thermometerFeatures & TEMPERATURE_TYPE)) &&
       (tempMeasurementVal.flags & FLAG_TEMPERATURE_TYPE))
    {
      charval[idx++] = tempMeasurementVal.temperatureType;
    }   
    
    retval =  aci_gatt_update_char_value(thermometer.htServiceHandle,
                                         thermometer.intermediateTempCharHandle,
                                         0, /* charValOffset */
                                         idx, /* charValueLen */
                                         (uint8_t *) charval); 
    if(retval == BLE_STATUS_SUCCESS)
    {
      HT_DBG_MSG(profiledbgfile,"HT_Send_Intermediate_Temperature(), intermediate temperature measurement sent successfully\n");
      HTProfile_Write_SubStateMachine(HT_CONNECTED_DO_INTERMEDIATE_TEMPERATURE);
      Blue_NRG_Timer_Stop(thermometer.timerID);
      thermometer.timerID = 0xFF;
    }
    else
    {
      HT_DBG_MSG(profiledbgfile,"HT_Send_Intermediate_Temperature(), FAILED to send intermediate temperature %02X \n", retval);
    }
  }
  return retval;
}/* end HT_Send_Intermediate_Temperature() */

tBleStatus HT_Update_Measurement_Interval(uint16_t interval)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  if(thermometer.thermometerFeatures & MEASUREMENT_INTERVAL_CHAR)
  {
    if((interval <= thermometer.maxValidInterval) &&
            (interval >= thermometer.minValidInterval))
    {
      retval =  aci_gatt_update_char_value(thermometer.htServiceHandle,
                                         thermometer.miCharHandle,
                                         0, /* charValOffset */
                                         2, /* charValueLen */
                                         (uint8_t *) &interval); 
           
      if(retval == BLE_STATUS_SUCCESS)
      {
        HT_DBG_MSG(profiledbgfile,"HT_Update_Measurement_Interval(), updated measurement interval%02X \n", interval);
        thermometer.measurementInterval = interval;
        if(HTProfile_Read_MainStateMachine() == HT_CONNECTED)
        {
          /* stop any periodic timer running.
           * The timer will be again started when
           * a measurement is sent
           */
           HTProfile_Write_SubStateMachine(HT_CONNECTED_DO_MEASUREMENT_INTERVAL);           
           Blue_NRG_Timer_Stop(thermometer.intervalTimerID);
           thermometer.intervalTimerID = 0xFF;
           
           /* stop the idle timer
            */           
           Blue_NRG_Timer_Stop(thermometer.timerID);
           thermometer.timerID = 0xFF;
        }
      }
      else
      {
        HT_DBG_MSG(profiledbgfile,"HT_Update_Measurement_Interval(), FAILED to update measurement interval %02X \n", retval);
      }
    }
    else
    {
      retval = BLE_STATUS_INVALID_PARAMS;
    }
  }
  
  return retval;
}/* end HT_Update_Measurement_Interval() */

tBleStatus HT_Update_Temperature_Type(uint8_t type)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  /* if this characteristic is present, then the value has to be static
   * while in connection. So donot allow an update during connection
   */ 
  if((thermometer.thermometerFeatures & TEMPERATURE_TYPE) &&
     (HTProfile_Read_MainStateMachine() != HT_CONNECTED))
  {
    retval =  aci_gatt_update_char_value(thermometer.htServiceHandle,
                                         thermometer.tempTypeCharHandle,
                                         0, /* charValOffset */
                                         1, /* charValueLen */
                                         (uint8_t *) &type);
    if(retval == BLE_STATUS_SUCCESS)
    {
      HT_DBG_MSG(profiledbgfile,"HT_Update_Temperature_Type(), updated temperature type with %2x\n",type);
    }
    else
    {
      HT_DBG_MSG(profiledbgfile,"FAILED to update temperature type %02X \n", retval);
    }
  }
  
  return retval;
}/* end HT_Update_Temperature_Type() */
/*******************************************************************************
* Local Functions
*******************************************************************************/

static void HTProfile_Write_MainStateMachine(tProfileState localmainState)
{
  thermometer.state = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState HTProfile_Read_MainStateMachine(void)
{
  return(thermometer.state);
}

static void HTProfile_Write_SubStateMachine(tProfileState localsubState)
{
  thermometer.substate = localsubState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState HTProfile_Read_SubStateMachine(void)
{
  return(thermometer.substate);
}

/*
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
static void HTProfile_Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  uint8_t attValLen;
  uint16_t attrHandle;
  uint16_t attValue;
  
  attrHandle = handle;
  
  attValLen = data_length; 
  BLUENRG_memcpy((void *) &attValue, (void *) att_data,attValLen); 
  
  HT_DBG_MSG(profiledbgfile,"HT_Event_Handler(), ----------------- HCI_EVT_BLUE_GATT_ATTRIBUTE_MODIFIED, attrHandle: %2x, attValLen: %x,  attValue: %2x\n",attrHandle,attValLen,attValue);
  
  if(attrHandle == (thermometer.miCharHandle + 1))
  {
    /* this event is received only when the client writes to the attribute
    * value. This event is not got when the application updates the
    * measurement interval characteristic
    */ 
    //attValLen = data_length;
    
    //attValue = LE_TO_NRG_16(response->evtPkt.vendorSpecificEvt.vsEvtPkt.gattAttrModified.attData); original 
    
    /* store the received remperature measurement value */ 
    //BLUENRG_memcpy((void *) &attValue, (void *) att_data,attValLen); 
    HT_DBG_MSG(profiledbgfile,"HT_Event_Handler(), ----------- EVT_BLUE_GATT_ATTRIBUTE_MODIFIED, attValue: %x\n",attValue);
    thermometer.measurementInterval = attValue;
    
    /* notify the application */	
    thermometer.applcb(EVT_HT_MEASUREMENT_INTERVAL_RECEIVED,2,(uint8_t*)&attValue);												
  }
  
}

/* HT_Sensor_Set_Discoverable() */
tBleStatus HT_Sensor_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  
  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'H','e','a','l','t','h','T','h','e','r','m','o','m','e','t','e','r'};
  
  retval = aci_gap_set_limited_discoverable(ADV_IND, 
                                            AdvIntervMin, 
                                            AdvIntervMax,
                                            PUBLIC_ADDR, 
                                            NO_WHITE_LIST_USE, 
                                            sizeof(local_name), 
                                            local_name, 
                                            gBLEProfileContext.advtServUUIDlen, 
                                            gBLEProfileContext.advtServUUID,    
                                            0, 
                                            0);

  return retval;
}/* end HT_Sensor_Set_Discoverable() */


/**
 * HT_Add_Services_Characteristics
 * 
 * @param[in]: None
 *            
 * It adds Health Thermometer service & related characteristics
 */ 
tBleStatus HT_Add_Services_Characteristics(void)
{
  tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;
  uint8_t numAttribRec = 0;
  uint16_t uuid = 0;
  uint16_t descHandle;
  //uint8_t buf[] = {0x09,'B','L','U','E','N','R','G'}; //TBR 
  uint8_t buf[4] = {0,0,0,0}; //TBR 
  
  HT_DBG_MSG(profiledbgfile,"HT_Add_Services_Characteristics(): Main profile initialized, Adding Health Thermometer service\n");
                              
  uuid = HEALTH_THERMOMETER_SERVICE_UUID;
  numAttribRec = 4;
  
  /* if the intermediate temperature characteristic is supported
   * then we have to support the client configuration
   * descriptor for that characteristic as well
   */
  if(thermometer.thermometerFeatures & INTERMEDIATE_TEMPERATURE_CHAR)
  {
          numAttribRec += 3;
  }
  
  /* if the measurement interval characteristic is supported
   * then we have to support the client configuration
   * descriptor and valid range descriptor also
   */
  if(thermometer.thermometerFeatures & MEASUREMENT_INTERVAL_CHAR)
  {
          numAttribRec += 4;
  }
  
  /* temperature type characteristic has no descriptors */
  if(thermometer.thermometerFeatures & TEMPERATURE_TYPE)
  {
          numAttribRec += 2;
  }
  /* Add Health Thermometer Service */
  hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                  (const uint8_t *) &uuid, 
                                   PRIMARY_SERVICE, 
                                   numAttribRec,
                                   &(thermometer.htServiceHandle));
              
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
     HT_DBG_MSG (profiledbgfile, "HT_Add_Services_Characteristics(), Health Thermometer Service is added Successfully %04X\n", thermometer.htServiceHandle);
  }
  else 
  {
      HT_DBG_MSG (profiledbgfile,"HT_Add_Services_Characteristics(),FAILED to add Health Thermometer, Error: %02X !!\n", hciCmdResult);
      HT_Exit(hciCmdResult); 
      return hciCmdResult;
  }
  
  /* Add Health Thermometer Measurement Characteristic */
  HT_DBG_MSG (profiledbgfile,"HT_Add_Services_Characteristics(),Adding Health Thermometer Measurement Characteristic. \n");
                      
  uuid = TEMPERATURE_MEASUREMENT_CHAR_UUID;
  /* Add  Health Thermometer Measurement Characteristic */
  hciCmdResult = aci_gatt_add_char(thermometer.htServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &uuid , 
                                   TEMPERATURE_MEASUREMENT_MAX_LEN, 
                                   CHAR_PROP_INDICATE, 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                   MIN_ENCRYPTION_KEY_SIZE,  /* encryKeySize */
                                   CHAR_VALUE_LEN_VARIABLE, /* isVariable */
                                   &(thermometer.tempMeasurementCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    HT_DBG_MSG (profiledbgfile, "HT_Add_Services_Characteristics(),Health Thermometer Measurement Characteristic Added Successfully  %04X \n", thermometer.tempMeasurementCharHandle);
  }
  else 
  {
    HT_DBG_MSG (profiledbgfile,"HT_Add_Services_Characteristics(),FAILED to add Health Thermometer Measurement Characteristic,  Error: %02X !!\n", hciCmdResult);
    HT_Exit(hciCmdResult); 
    return hciCmdResult;
  }          
   
  if(thermometer.thermometerFeatures & TEMPERATURE_TYPE)
  {
    /* Add Health Thermometer temperature type Characteristic */
    HT_DBG_MSG (profiledbgfile,"HT_Add_Services_Characteristics(),Adding Health Thermometer temperature type Characteristic. \n");
    uuid = TEMPERATURE_TYPE_CHAR_UUID;
    
    hciCmdResult = aci_gatt_add_char(thermometer.htServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &uuid , 
                                     1, 
                                     CHAR_PROP_READ, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                     MIN_ENCRYPTION_KEY_SIZE, /* encryKeySize */
                                     CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                     &(thermometer.tempTypeCharHandle));
    
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HT_DBG_MSG (profiledbgfile, "HT_Add_Services_Characteristics(),Health Thermometer temperature type Characteristic Added Successfully  %04X \n", thermometer.tempTypeCharHandle);
    }
    else 
    {
      HT_DBG_MSG (profiledbgfile,"HT_Add_Services_Characteristics(),FAILED to add Health Thermometer temperature type Characteristic,  Error: %02X !!\n", hciCmdResult);
      HT_Exit(hciCmdResult); 
      return hciCmdResult;
    }  
  }
  if(thermometer.thermometerFeatures & INTERMEDIATE_TEMPERATURE_CHAR)
  {
    /* Add Health Thermometer intermediate temperature Characteristic */
      HT_DBG_MSG (profiledbgfile,"HT_Add_Services_Characteristics(),Adding Health Thermometer intermediate temperature Characteristic. \n");
      
    /* add the intermediate temperature
     * characteristic
     */
    uuid = INTERMEDIATE_TEMPERATURE_CHAR_UUID;
    hciCmdResult = aci_gatt_add_char(thermometer.htServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &uuid , 
                                     TEMPERATURE_MEASUREMENT_MAX_LEN, 
                                     CHAR_PROP_NOTIFY, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                     MIN_ENCRYPTION_KEY_SIZE,  /* encryKeySize */
                                     CHAR_VALUE_LEN_VARIABLE, /* isVariable */
                                     &(thermometer.intermediateTempCharHandle));
     
     if (hciCmdResult == BLE_STATUS_SUCCESS)
     {
       HT_DBG_MSG (profiledbgfile, "HT_Add_Services_Characteristics(),Health Thermometer intermediate temperature Characteristic Added Successfully  %04X \n", thermometer.intermediateTempCharHandle);       
     }
     else 
     {
         HT_DBG_MSG (profiledbgfile,"HT_Add_Services_Characteristics(),FAILED to add Health Thermometer intermediate temperature Characteristic,  Error: %02X !!\n", hciCmdResult);
         HT_Exit(hciCmdResult); 
         return hciCmdResult;
     }          
  }
  
  if(thermometer.thermometerFeatures & MEASUREMENT_INTERVAL_CHAR)
  {
    /* add the measurement interval
     * characteristic
     */
    
    uuid = MEASUREMENT_INTERVAL_CHAR_UUID;  
    hciCmdResult = aci_gatt_add_char(thermometer.htServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &uuid , 
                                     2, 
                                     CHAR_PROP_READ|CHAR_PROP_WRITE|CHAR_PROP_INDICATE, 
                                     ATTR_PERMISSION_AUTHEN_WRITE, 
                                     GATT_NOTIFY_ATTRIBUTE_WRITE|GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP, /* gattEvtMask */
                                     MIN_ENCRYPTION_KEY_SIZE,  /* encryKeySize */
                                     CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                     &(thermometer.miCharHandle));
   
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HT_DBG_MSG (profiledbgfile, "HT_Add_Services_Characteristics(),Health Thermometer measurement interval Characteristic Added Successfully: %04X,buf: %04X\n", thermometer.miCharHandle, buf);
      
      /* Set security permission of measurement interval characteristic descriptor to None (0x00): as defined by profile specification. 
          By default, on BlueNRG-MS the characteristic descriptor has the same security permission of the characteristic. 
          By default, on BlueNRG the characteristic descriptor has security permission equal to None. 
       */
      hciCmdResult =  aci_gatt_set_security_permission(thermometer.htServiceHandle, 
                                                       thermometer.miCharHandle + 2,
                                                       0x00);
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HT_DBG_MSG (profiledbgfile, "HT_Add_Services_Characteristics(),aci_gatt_set_security_permission() OK\n");
      }
      else 
      {
        HT_DBG_MSG (profiledbgfile,"HT_Add_Services_Characteristics(),FAILED on aci_gatt_set_security_permission(): %02X !!\n", hciCmdResult);
        HT_Exit(hciCmdResult); 
        return hciCmdResult;
      }          
      
      HT_DBG_MSG(profiledbgfile,"HT_Add_Services_Characteristics(), Adding Health Thermometer valid range characteristic descriptor \n");
                                      
      uuid = VALID_RANGE_DESCRIPTOR_UUID;
      STORE_LE_16(buf,thermometer.minValidInterval);
      STORE_LE_16(&buf[2],thermometer.maxValidInterval);
      
      /* add the valid descriptor */
      hciCmdResult = aci_gatt_add_char_desc(thermometer.htServiceHandle, 
                                            thermometer.miCharHandle, 
                                            UUID_TYPE_16,
                                            (uint8_t *)&uuid, 
                                            4,
                                            4,
                                            (void *)&buf, //TBR 
                                            ATTR_PERMISSION_NONE,
                                            ATTR_ACCESS_READ_ONLY,
                                            GATT_DONT_NOTIFY_EVENTS,
                                            MIN_ENCRYPTION_KEY_SIZE, 
                                            0x00,
                                            &descHandle);
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
         HT_DBG_MSG (profiledbgfile, "HT_Add_Services_Characteristics(),Health Thermometer valid range characteristic descriptor Added Successfully  %04X \n", descHandle);
      }
      else 
      {
         HT_DBG_MSG (profiledbgfile,"HT_Add_Services_Characteristics(),FAILED to add Health Thermometer valid range characteristic descriptor,  Error: %02X !!\n", hciCmdResult);
         HT_Exit(hciCmdResult); 
         return hciCmdResult;
      }  
    }
    else 
    {
      HT_DBG_MSG (profiledbgfile,"HT_Add_Services_Characteristics(),FAILED to add Health Thermometer measurement interval Characteristic,  Error: %02X !!\n", hciCmdResult);
      HT_Exit(hciCmdResult); 
      return hciCmdResult;
    }          
  }
  return hciCmdResult;
}/* end HT_Add_Services_Characteristics() */

/**
 * HT_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 * 
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB().
 */ 
void HT_Event_Handler(void *pckt)
{
  uint8_t attValLen;
  tBleStatus retval = BLE_STATUS_SUCCESS;
  uint16_t attrHandle;
  uint16_t attValue; 
  
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  HT_DBG_MSG(profiledbgfile, "HT_Event_Handler(), Health Thermometer State: %02X \n", HTProfile_Read_MainStateMachine());
  
  if(hci_pckt->type != HCI_EVENT_PKT)
  {
    HT_DBG_MSG (profiledbgfile,"HT_Event_Handler(),Recevied packed is not an HCI Event: %02X !!\n", hci_pckt->type);
    return;
  }
  
  switch(event_pckt->evt)
  {    
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      switch(evt->subevent)
      {
      case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
          
          if (cc->status == BLE_STATUS_SUCCESS)
          {
            thermometer.connHandle = cc->handle;
            HT_DBG_MSG(profiledbgfile,"HT_Event_Handler(), connected %x\n",thermometer.connHandle);
            
            Blue_NRG_Timer_Stop(thermometer.timerID);
            thermometer.timerID = 0xFF;
            HTProfile_Write_MainStateMachine(HT_CONNECTED);
          }
        }
        break;
      }
    }
    break;
  case EVT_DISCONN_COMPLETE:
    {
      evt_disconn_complete *cc = (void *)event_pckt->data;
      
      HT_DBG_MSG(profiledbgfile,"HT_Event_Handler(), disconnected %2x\n", cc->status);
      if (cc->status == BLE_STATUS_SUCCESS)
      {
        /* stop the idle connection timer if it is running */
        Blue_NRG_Timer_Stop(thermometer.timerID);
        thermometer.timerID = 0xFF;
        HTProfile_Write_MainStateMachine(HT_INITIALIZED);
      }
    }
    break;
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      
      switch(blue_evt->ecode)
      {
      case EVT_BLUE_GAP_LIMITED_DISCOVERABLE: 
        {
          HT_DBG_MSG(profiledbgfile,"HT_Event_Handler(), Recevied event EVT_BLUE_GAP_LIMITED_DISCOVERABLE \n");
          if((HTProfile_Read_MainStateMachine() == HT_ADV_IN_FAST_DISC_MODE) ||
             (HTProfile_Read_MainStateMachine() == HT_ADV_IN_LOW_PWR_MODE))
          {
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State(&thermometer.HTtoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
            
            if(HTProfile_Read_MainStateMachine() == HT_ADV_IN_LOW_PWR_MODE)
            {
              HTProfile_Write_MainStateMachine(HT_INITIALIZED);
              /* notify the application */
              thermometer.applcb(EVT_MP_ADVERTISING_TIMEOUT,0,NULL);
            }
          }
        }
        break;
        
      case EVT_BLUE_GATT_WRITE_PERMIT_REQ:
        { 
          //HT_DBG_MSG(profiledbgfile,"HT_Event_Handler(), EVT_BLUE_GATT_WRITE_PERMIT_REQ \n");
          evt_gatt_write_permit_req * write_perm_req = (void*)blue_evt->data;
          
          attrHandle = write_perm_req->attr_handle;
          attValLen = write_perm_req->data_length;
          
          /* copy the received value (TBR) */
          BLUENRG_memcpy((void *) &attValue, (void *) write_perm_req->data, attValLen); 
          
          HT_DBG_MSG (profiledbgfile,"HT_Event_Handler(), EVT_BLUE_GATT_WRITE_PERMIT_REQ, Handle: 0x%02X, Len: 0x%02X, value: 0x%04X !!\n", attrHandle, attValLen, attValue); 
          
          if(attrHandle == (thermometer.miCharHandle + 1)) 
          {
            if((attValue != 0) &&
               ((attValue < thermometer.minValidInterval) ||
                (attValue > thermometer.maxValidInterval)))
            {
              /* invalid interval value */
              HT_DBG_MSG(profiledbgfile,"HT_Event_Handler(),EVT_BLUE_GATT_WRITE_PERMIT_REQ, invalid interval value\n");
              
              retval = aci_gatt_write_response(thermometer.connHandle,
                                               attrHandle, 
                                               0x01, /* write_status = 1 (error))*/ // BLE_STATUS_FAILED
                                               INTERVAL_OUT_OF_RANGE,
                                               attValLen, 
                                               (uint8_t *)&attValue);
            }
            else
            {
              /* valid interval */
              HT_DBG_MSG(profiledbgfile,"HT_Event_Handler(),EVT_BLUE_GATT_WRITE_PERMIT_REQ, valid interval value\n");
              retval = aci_gatt_write_response(thermometer.connHandle,
                                               attrHandle, 
                                               BLE_STATUS_SUCCESS, 
                                               0x00, 
                                               attValLen, 
                                               (uint8_t *)&attValue);
            }
            
            if(retval == BLE_STATUS_SUCCESS)
            {
              GL_DBG_MSG(profiledbgfile,"HT_Event_Handler(),HCI_GATT_Write_Response_Cmd OK\n");
            }
            else
            {
              GL_DBG_MSG(profiledbgfile,"HT_Event_Handler(),FAILED HCI_GATT_Write_Response_Cmd %02X \n", retval);
            }       
          }
        }
        break;
      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
        {
          if (bnrg_expansion_board == IDB05A1) {
            evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
            HTProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
          else {
            evt_gatt_attr_modified_IDB04A1 *evt = (evt_gatt_attr_modified_IDB04A1*)blue_evt->data;
            HTProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
          
          
        }
        break;
      }
    }
    break;
  }
}/* end HT_Event_Handler()*/

/**
 * HT_StateMachine
 * 
 * @param[in] None
 * 
 * HR profile's state machine: to be called on application main loop. 
 */ 
tBleStatus HT_StateMachine(void)
{
  tBleStatus status = BLE_STATUS_SUCCESS;
 
  switch(HTProfile_Read_MainStateMachine())
  {
        
    case HT_UNINITIALIZED:
    {
    }
    break;
    case HT_WAITING_BLE_INIT:
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
      {
        /* Add HT service & characteristics */ 
        status = HT_Add_Services_Characteristics();
        if (status == BLE_STATUS_SUCCESS)
        {
          HT_DBG_MSG ( profiledbgfile, "HT_StateMachine(),HT_Add_Services_Characteristics(), Health Thermometer Profile is Initialized with Service & Characteristics \n");
          
          /* vote to the main profile to go to advertise state */
          BLE_Profile_Vote_For_Advertisable_State(&thermometer.HTtoBLEInf);
          BLE_Profile_Add_Advertisment_Service_UUID(HEALTH_THERMOMETER_SERVICE_UUID);
          HTProfile_Write_MainStateMachine(HT_IDLE);
        }
        else
        {
          HT_DBG_MSG(profiledbgfile,"HT_StateMachine(), HT_Add_Services_Characteristics() FAILED to add Health Thermometer Service, Characteristics; Error: %02X !!\n", status);
         
          /* Notify to application that Heart Rate profile initialization has failed */
          HT_Exit(status);
        }
      }
    break; 
    
    case HT_IDLE:
    {
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE)
      {
        HT_DBG_MSG(profiledbgfile,"HT_StateMachine(),notify the application that initialization is complete\n");
        status = BLE_STATUS_SUCCESS;
        HTProfile_Write_MainStateMachine(HT_INITIALIZED);
        thermometer.applcb(EVT_HT_INITIALIZED,1,&status);
      }
    } 
    break;
    case HT_ADV_IN_FAST_DISC_MODE_STOP:
    {
       status = HT_Sensor_Set_Discoverable(HT_LOW_PWR_ADV_INTERVAL_MIN,HT_LOW_PWR_ADV_INTERVAL_MAX);
       if(status == BLE_STATUS_SUCCESS)
       {											 
        HTProfile_Write_MainStateMachine(HT_ADV_IN_LOW_PWR_MODE);
        HT_DBG_MSG(profiledbgfile," HT_StateMachine(),enabled advertising low power connection\n");
       }
       else
       {
         HTProfile_Write_MainStateMachine(HT_INITIALIZED);
         thermometer.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
         HT_DBG_MSG(profiledbgfile,"HT_StateMachine(), ERROR: enabled advertising low power  connection: %02X !!\n", status);
       }
    }
    break;
    case HT_UNDIRECTED_ADV_MODE_STOP:
    {
      /* request for main profile's state change */
      BLE_Profile_Change_Advertise_Sub_State(&thermometer.HTtoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
      
      /* the advertising timeout has happened
       * inform the application
       */ 
       HTProfile_Write_MainStateMachine(HT_IDLE);
       thermometer.applcb(EVT_MP_ADVERTISING_TIMEOUT,0,NULL);
    }
    
    break;
    case HT_CONNECTED:
    {
      status = BLE_STATUS_SUCCESS;      
      switch(HTProfile_Read_SubStateMachine())
      {
        case (HT_CONNECTED_DO_TEMPERATURE_MEASUREMENT):
        {
          HTProfile_Write_SubStateMachine(HT_CONNECTED_STOP_TEMPERATURE_MEASUREMENT);
          /* an update was done successfully, now we can start the idle timer */
          Blue_NRG_Timer_Start(5,HT_IdleConnection_Handler,&thermometer.timerID);
          /* notify the application of the successful update */
          /* notify the application of the successful update */
          thermometer.applcb(EVT_HT_TEMPERATURE_CHAR_UPDATE_CMPLT,1,&status);
        }
        break;
        case (HT_CONNECTED_DO_INTERMEDIATE_TEMPERATURE):
        {
          HTProfile_Write_SubStateMachine(HT_CONNECTED_STOP_INTERMEDIATE_TEMPERATURE);
         
          
          /* Start idle Connection timer only if there is no periodic measurement update */ //TBR 
          //if (thermometer.measurementInterval == 0)
          /* an update was done successfully, now we can start the idle timer */
          Blue_NRG_Timer_Start(5,HT_IdleConnection_Handler,&thermometer.timerID);
          
          /* notify the application of the successful update */
          thermometer.applcb(EVT_HT_INTERMEDIATE_TEMP_CHAR_UPDATE_CMPLT,1,&status);
        }
        break;
        case (HT_CONNECTED_DO_MEASUREMENT_INTERVAL):
        {
          HTProfile_Write_SubStateMachine(HT_CONNECTED_STOP_MEASUREMENT_INTERVAL);
          
          HT_DBG_MSG(profiledbgfile,"------- HT_StateMachine(),Blue_NRG_Timer_Start: %02X !!\n", thermometer.measurementInterval);
          /* an update was done successfully, now we can start the idle timer */
          if(thermometer.measurementInterval)
          {
            Blue_NRG_Timer_Start(2*thermometer.measurementInterval,HT_IdleConnection_Handler,&thermometer.timerID); //TBR
          }
          else
          {
            Blue_NRG_Timer_Start(5,HT_IdleConnection_Handler,&thermometer.timerID);
          }
          
          /* notify the application of the successful update. This event is given only when the application
           * had started an update of the measurement interval and it was successful
           */
          thermometer.applcb(EVT_HT_MEASUREMENT_INTERVAL_UPDATE_CMPLT,1,&status);
        }
        break;
      }
    }
    break;
  }
  return status;
}/* end HT_StateMachine() */

/**
 * HT_Get_IntervalTimerID
 * 
 * utility function to get the intervalTimer value
 */ 

uint8_t HT_Get_IntervalTimerID()
{
  return thermometer.intervalTimerID;
}/* end HT_Get_IntervalTimerID() */

/**
 * HT_AdvTimeout_Handler
 * 
 * advertising timeout handler
 */ 
void HT_AdvTimeout_Handler(void)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;

  Blue_NRG_Timer_Stop(thermometer.timerID);
  thermometer.timerID = 0xFF;

  switch(HTProfile_Read_MainStateMachine())
  {
    case HT_UNDIRECTED_ADV_MODE:
    {
      retval = aci_gap_set_non_discoverable(); 
      if (retval == BLE_STATUS_SUCCESS)
      {
        HTProfile_Write_MainStateMachine(HT_UNDIRECTED_ADV_MODE_STOP);
      }
      else
        HT_DBG_MSG(profiledbgfile,"HT_AdvTimeout_Handler(), ERROR:aci_gap_set_non_discoverable : %02X !!\n", retval);
    }
    case HT_ADV_IN_FAST_DISC_MODE:
    {
      retval = aci_gap_set_non_discoverable(); 
      if (retval == BLE_STATUS_SUCCESS)
      {
        HTProfile_Write_MainStateMachine(HT_ADV_IN_FAST_DISC_MODE_STOP);
      }
      else
        HT_DBG_MSG(profiledbgfile,"HT_AdvTimeout_Handler(), ERROR:aci_gap_set_non_discoverable : %02X !!\n", retval);
    }
    break;
  }
}/* end HT_AdvTimeout_Handler() */

/**
 * HT_Interval_Expired
 * 
 * This is the callback called when
 * the timer started for the periodic
 * interval expires
 */ 
void HT_Interval_Expired(void)
{
  /* just reset the timer ID
   * this indicates that the next measurement
   * when received can be sent
   */ 
  //HT_DBG_MSG(profiledbgfile,"--- Int. Timer End \n");
  Blue_NRG_Timer_Stop(thermometer.intervalTimerID);
  thermometer.intervalTimerID = 0xFF;
}/* end HT_Interval_Expired() */

/**
 * HT_IdleConnection_Handler
 * 
 * informs the application that the connection has been
 * idle for 5 seconds. The application may disconnect the
 * link if desired
 */ 
void HT_IdleConnection_Handler(void)
{
  if(HTProfile_Read_MainStateMachine() == HT_CONNECTED)
  {
    /* for 5 seconds the connection has been idle
     * so disconnect
     * TODO : or just inform the application and the
     * applciation can decide whether to disconnect
     */
     Blue_NRG_Timer_Stop(thermometer.timerID);
     thermometer.timerID = 0xFF;
     thermometer.applcb(EVT_HT_IDLE_CONNECTION_TIMEOUT,0x00,NULL);
  }
}/* end HT_IdleConnection_Handler() */

/**
 * HT_Exit
 * 
 * @param[in] errCode: reason for exit
 * 
 * informs the application for the reason
 * the initialization failed and de-registers 
 * the profile
 */ 
void HT_Exit(uint8_t errCode)
{
  HTProfile_Write_MainStateMachine(HT_UNINITIALIZED);
  thermometer.applcb(EVT_HT_INITIALIZED,1,&errCode);
  BLE_Profile_Unregister_Profile(&thermometer.HTtoBLEInf);
}/* HT_Exit() */
