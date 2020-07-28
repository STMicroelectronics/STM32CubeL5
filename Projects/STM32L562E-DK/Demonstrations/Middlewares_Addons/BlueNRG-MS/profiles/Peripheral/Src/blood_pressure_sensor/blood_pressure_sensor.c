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
*   FILENAME        -  blood_pressure_sensor.c
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      28/07/2014
*   $Revision$:  second version
*   $Author$:    
*   Comments:    Redesigned the blood pressure sensor profile according to 
*                unified BlueNRG DK framework    
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  source code for the sensor role of blood pressure profile
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
#include <blood_pressure.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* profile states */
#define BPS_UNINITIALIZED		        (0x00)
#define BPS_WAITING_BLE_INIT                    (0x01)
#define BPS_IDLE				(0x02)
#define BPS_INITIALIZED			        (0x03)
#define BPS_UNDIRECTED_ADV_MODE			(0x04)
#define BPS_UNDIRECTED_ADV_MODE_STOP		(0x05)
#define BPS_ADV_IN_FAST_DISC_MODE		(0x06)
#define BPS_ADV_IN_FAST_DISC_MODE_STOP          (0x07)
#define BPS_ADV_IN_LOW_PWR_MODE			(0x08)
#define BPS_CONNECTED				(0x09)
/* connected substates */
#define BPS_CONNECTED_DO_BPM		        (0x0A)
#define BPS_CONNECTED_STOP_BPM			(0x0B)
#define BPS_CONNECTED_DO_ICP			(0x0C)
#define BPS_CONNECTED_STOP_ICP		        (0x0D)

#define BPS_ICP_CHAR_SUPPORTED			(0x40)
#define MULTIPLE_BONDS_SUPPORTED 		(0x20)

/* advertising intervals in terms of 625 micro seconds */
#define BPS_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define BPS_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define BPS_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define BPS_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */

#define MIN_ENCRYPTION_KEY_SIZE          (10)

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )
/******************************************************************************
* type definitions
******************************************************************************/
typedef struct _tBloodPressureContext
{
  /**
   * state of the blood pressure
   * sensor state machine
   */ 
  tProfileState bpsState;
  
  /**
   * state of the blood pressure
   * sensor state machine
   */ 
  tProfileState bpsSubState;
      
  /**
   * callback to notify the applciation with
   * the profile specific events
   */ 
  BLE_CALLBACK_FUNCTION_TYPE applcb;
      
   /**
   * flag which indicates the features supported
   * by the blood pressure sensor
   */ 
  uint8_t bpsFeaturesSupported;
      
  /**
   * blood pressure service handle
   */ 
  uint16_t bpServiceHandle;
  
  /**
   * blood pressure measurement characteristic
   * handle
   */ 
  uint16_t bpmCharHandle;
  
  /**
   * blood pressure feature characteristic
   * handle
   */ 
  uint16_t bpfCharHandle;
      
  /**
   * intermediate cuff pressure 
   * characteristic handle
   */ 
  uint16_t icpCharHandle;

  /**
   * will contain the timer ID of the 
   * timer started by the blood pressure profile
   * the same timerID will be used to handle both
   * advertising as well as idle connection timeout
   * since both timers are not required simultaneously
   */ 
  tTimerID timerID;
      
  /**
   * profile interface context. This is
   * registered with the main profile
   */ 
  tProfileInterfaceContext BPStoBLEInf;
}tBloodPressureContext;

/******************************************************************************
* Variable Declarations
******************************************************************************/
static tBloodPressureContext bloodPressureSensor;

/******************************************************************************
* Imported Variable
******************************************************************************/
extern void *profiledbgfile;

/******************************************************************************
 * Function Prototypes
******************************************************************************/
static tBleStatus BPS_Add_Services_Characteristics(void);
static tBleStatus BPS_Sensor_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax);
static void BPS_AdvTimeout_Handler(void);
static void BPS_Exit(uint8_t errCode);
static void BPS_IdleConnection_Handler(void);

static void BPProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState BPProfile_Read_MainStateMachine(void);
static void BPProfile_Write_SubStateMachine(tProfileState localsubState);
static tProfileState BPProfile_Read_SubStateMachine(void);
/*******************************************************************************
* SAP
*******************************************************************************/
tBleStatus BPS_Init(BOOL intermediateCuffPressureChar,
		    uint8_t feature,
		    BLE_CALLBACK_FUNCTION_TYPE BPScb) 
{
  tBleStatus retval = BLE_STATUS_FAILED;
      
  BPS_DBG_MSG(profiledbgfile, "BPS_Init\n");

  if(Is_Profile_Present(PID_BLOOD_PRESSURE_SENSOR) == BLE_STATUS_SUCCESS)
  {
    return BLE_STATUS_PROFILE_ALREADY_INITIALIZED;
  }

  if(BPScb == NULL)
  {
    return BLE_STATUS_NULL_PARAM;
  }
      
  /* Initialize Data Structures */
  BLUENRG_memset(&bloodPressureSensor,0,sizeof(bloodPressureSensor));
  bloodPressureSensor.applcb = BPScb;
  bloodPressureSensor.timerID = 0xFF;
  bloodPressureSensor.bpsFeaturesSupported = feature;
  
  if(intermediateCuffPressureChar)
  {
    bloodPressureSensor.bpsFeaturesSupported |= BPS_ICP_CHAR_SUPPORTED;
  }
      
  BPProfile_Write_MainStateMachine(BPS_UNINITIALIZED);
  BPProfile_Write_SubStateMachine(BPS_UNINITIALIZED);
  
  /* register the blood pressure sensor with BLE main Profile */
  BLUENRG_memset(&bloodPressureSensor.BPStoBLEInf,0,sizeof(bloodPressureSensor.BPStoBLEInf));
  bloodPressureSensor.BPStoBLEInf.profileID = PID_BLOOD_PRESSURE_SENSOR;
  bloodPressureSensor.BPStoBLEInf.callback_func = BPS_Event_Handler;
  bloodPressureSensor.BPStoBLEInf.voteForAdvertisableState = 0;
  /* manufacturer name, model number and system ID characteristics
   * have to be added to the device information service
   */ 
  bloodPressureSensor.BPStoBLEInf.DISCharRequired = (MANUFACTURER_NAME_STRING_CHAR_MASK|MODEL_NUMBER_STRING_CHAR_MASK|SYSTEM_ID_CHAR_MASK);
  retval = BLE_Profile_Register_Profile(&bloodPressureSensor.BPStoBLEInf);
  if (retval != BLE_STATUS_SUCCESS)
  {
    /* Profile could not be registered with the BLE main profile */
    GL_DBG_MSG(profiledbgfile,"BPS_Init(), BLE_Profile_Register_Profile() failed\n");
    return (retval);
  }
      
  BPProfile_Write_MainStateMachine(BPS_WAITING_BLE_INIT);
  
  BPS_DBG_MSG(profiledbgfile,"Blood pressure Profile is Initialized\n");
      
  return (BLE_STATUS_SUCCESS);
}/* end BPS_Init() */

tBleStatus BPS_Advertize(uint8_t useWhitelist)
{
  tBleStatus status = BLE_STATUS_FAILED;
  //uint8_t name[] = {0x09,'B','L','U','E','N','R','G'};
  
  BPS_DBG_MSG(profiledbgfile,"BPS_Advertize %x\n",BPProfile_Read_MainStateMachine());
	
  if(BPProfile_Read_MainStateMachine() == BPS_INITIALIZED)
  {
    if(useWhitelist)
    {
      /* start the undirected connectable mode */
      status = aci_gap_set_undirected_connectable(PUBLIC_ADDR, WHITE_LIST_FOR_ALL);  
      
      if(status == BLE_STATUS_SUCCESS)
      {
        BPProfile_Write_MainStateMachine(BPS_UNDIRECTED_ADV_MODE);
        BPS_DBG_MSG(profiledbgfile,"BPS_Advertize(), enabled advertising fast connection\n");
        /* start a timer for 120 seconds */
        Blue_NRG_Timer_Start(120,BPS_AdvTimeout_Handler,&bloodPressureSensor.timerID);
          
        /* request for main profile's state change */
        BLE_Profile_Change_Advertise_Sub_State(&bloodPressureSensor.BPStoBLEInf,BLE_PROFILE_STATE_ADVERTISING);
      }
      else
      {
        /* inform the application that the advertizing was not
         * enabled
         */
        BPS_DBG_MSG (profiledbgfile,"BPS_Advertize(), FAILED to start undirected connectable mode, Error: %02X !!\n", status);
        BPProfile_Write_MainStateMachine(BPS_IDLE);
        bloodPressureSensor.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
      }
    }
    else
    {
      status = BPS_Sensor_Set_Discoverable(BPS_FAST_CONN_ADV_INTERVAL_MIN,BPS_FAST_CONN_ADV_INTERVAL_MAX);
        
      if(status == BLE_STATUS_SUCCESS)
      {
        BPProfile_Write_MainStateMachine(BPS_ADV_IN_FAST_DISC_MODE);
        BPS_DBG_MSG(profiledbgfile,"BPS_Advertize(), enabled advertising fast connection\n");
        
        /* start a timer for 30 seconds */
        Blue_NRG_Timer_Start(30,BPS_AdvTimeout_Handler,&bloodPressureSensor.timerID); 
                      
        /* request for main profile's state change */
        BLE_Profile_Change_Advertise_Sub_State(&bloodPressureSensor.BPStoBLEInf, BLE_PROFILE_STATE_ADVERTISING);
        
      }
      else
      {
        /* inform the application that the advertizing was not enabled */
         BPS_DBG_MSG (profiledbgfile,"BPS_Advertize(), FAILED to start limited discoverable mode, Error: %02X !!\n", status);
        BPProfile_Write_MainStateMachine(BPS_IDLE);
        bloodPressureSensor.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
      }
    }
  }
  return status;
}/* end BPS_Advertize() */

tBleStatus BPS_Send_Blood_Pressure_Measurement(tBPMValue bpmval)
{
  uint8_t charval[MAX_BPM_CHAR_VAL_LEN]; //TBR ???
  uint8_t idx = 0;
  tBleStatus retval = BLE_STATUS_FAILED;
  
  if(BPProfile_Read_MainStateMachine() == BPS_CONNECTED)
  {
    BLUENRG_memset(charval,0x00,MAX_BPM_CHAR_VAL_LEN);
    
    charval[idx++] = bpmval.flags;
    STORE_LE_16(&charval[idx],bpmval.systolic);
    idx += 2;
    STORE_LE_16(&charval[idx],bpmval.diastolic);
    idx += 2;
    STORE_LE_16(&charval[idx],bpmval.map);
    idx += 2;
    
    if(bpmval.flags & FLAG_TIME_STAMP_PRESENT)
    {
      BPS_DBG_MSG(profiledbgfile,"year %x\n",bpmval.timeStamp.year);
      STORE_LE_16(&charval[idx],bpmval.timeStamp.year);
      BPS_DBG_MSG(profiledbgfile,"%x %x\n",charval[idx],charval[idx+1]);
      idx += 2;
      charval[idx++] = bpmval.timeStamp.month;
      charval[idx++] = bpmval.timeStamp.day;
      charval[idx++] = bpmval.timeStamp.hours;
      charval[idx++] = bpmval.timeStamp.minutes;
      charval[idx++] = bpmval.timeStamp.seconds;
    }
    
    if(bpmval.flags & FLAG_PULSE_RATE_PRESENT)
    {
      STORE_LE_16(&charval[idx],bpmval.pulseRate);
      idx += 2;
    }
    
    if(bpmval.flags & FLAG_USER_ID_PRESENT)
    {
      charval[idx++] = bpmval.userID;
    }
    
     /**
      * measurement status is a bitmask 
      * bit 0 : BODY_MOVEMENT_DETECTION_FLAG
      *     0 -> no body movement 
      * 	1 -> body movement during measurement
      * bit 1: CUFF_FIT_DETECTION_FLAG
      *     0 -> cuff fits properly
      *     1 -> cuff too lose
      * bit 2 : IRREGULAR_PULSE_DETECTION_FLAG
      *     0 -> no irregular pulse detected
      *     1 -> irregular pulse detected
      * bit 3 and bit 4: PULSE_RATE_RANGE_DETECTION_FLAG
      *     00 -> pulse rate is within the range
      *     01 -> pulse rate exceeds upper limit
      *     02 -> pulse rate is lesser than lower limit
      * bit 5 : MEASUREMENT_POSITION_DETECTION_FLAG
      *     0 -> proper measurement position
      *     1 -> improper measurement position
      */
    if(bpmval.flags & FLAG_MEASUREMENT_STATUS_PRESENT)
    {
      if((bloodPressureSensor.bpsFeaturesSupported & BODY_MOVEMENT_DETECTION_SUPPORT_BIT) &&
         (bpmval.measurementStatus.bodyMovementDetectionFlag))
      {
        charval[idx] =  BODY_MOVEMENT_DETECTION_FLAG;
      }
      
      if((bloodPressureSensor.bpsFeaturesSupported & CUFF_FIT_DETECTION_SUPPORT_BIT) &&
         (bpmval.measurementStatus.cuffFitDetectionFlag))
      {
        charval[idx] |=  CUFF_FIT_DETECTION_FLAG;
      }
      
      if((bloodPressureSensor.bpsFeaturesSupported & IRREGULAR_PULSE_DETECTION_SUPPORT_BIT) &&
        (bpmval.measurementStatus.irregularPulseDetectionFlag))
      {
        charval[idx] |=  IRREGULAR_PULSE_DETECTION_FLAG;
      }
      
      if(bloodPressureSensor.bpsFeaturesSupported & PULSE_RATE_RANGE_DETECTION_SUPPORT_BIT)
      {
        if(bpmval.measurementStatus.pulseRateRangeDetectionFlag == 0x01)
        {
          charval[idx] |=  PULSE_RATE_RANGE_EXCEEDS_UPPER_LIMIT;
        }
        else if(bpmval.measurementStatus.pulseRateRangeDetectionFlag == 0x02)
        {
          charval[idx] |=  PULSE_RATE_RANGE_BELOW_LOWER_LIMIT;
        }
      }
      
      if((bloodPressureSensor.bpsFeaturesSupported & MEASUREMENT_POSITION_DETECTION_SUPPORT_BIT) &&
        (bpmval.measurementStatus.measurementPositionDetectionFlag))
      {
        charval[idx] |=  MEASUREMENT_POSITION_DETECTION_FLAG;
      }
      
      /* the measurement status field is 16 bits.
       * bit 6 to 15 are reserved for future use
       * and have to be set to 0
       */ 
      idx += 2;
    }
    
    retval =  aci_gatt_update_char_value(bloodPressureSensor.bpServiceHandle, 
                                         bloodPressureSensor.bpmCharHandle,
                                         0, /* charValOffset */
                                         idx, /* charValueLen */
                                         (uint8_t *) charval);  
    
    if(retval == BLE_STATUS_SUCCESS)
    {
      BPS_DBG_MSG(profiledbgfile,"BPS_Send_Blood_Pressure_Measurement(), blood pressure measurement sent successfully\n");
      Blue_NRG_Timer_Stop(bloodPressureSensor.timerID);
      bloodPressureSensor.timerID = 0xFF;
      BPProfile_Write_SubStateMachine(BPS_CONNECTED_DO_BPM);
    }
    else
    {
      BPS_DBG_MSG(profiledbgfile,"BPS_Send_Blood_Pressure_Measurement(), FAILED to send BPM value %02X \n", retval);
    }
  }
  return retval;
}/* end BPS_Send_Blood_Pressure_Measurement() */

tBleStatus BPS_Send_Intermediate_Cuff_Pressure(tICPValue icpval)
{
  uint8_t charval[MAX_BPM_CHAR_VAL_LEN];
  uint8_t idx = 0;
  tBleStatus retval = BLE_STATUS_FAILED;
  
  if((bloodPressureSensor.bpsFeaturesSupported & BPS_ICP_CHAR_SUPPORTED) &&
     (BPProfile_Read_MainStateMachine() == BPS_CONNECTED))
  {
    BLUENRG_memset(charval,0x00,MAX_BPM_CHAR_VAL_LEN);
    
    /* pulse rate and timestamp
     * fields are not recommended to be supported
     * in the intermediate cuff pressure value
     */ 
    charval[idx++] |= (icpval.flags & (FLAG_BLOOD_PRESSURE_UNITS_KPA|FLAG_USER_ID_PRESENT|FLAG_MEASUREMENT_STATUS_PRESENT));
    
    STORE_LE_16(&charval[idx],icpval.icp);
    idx += 2;
    
    /* fill the next four bytes with
     * the short NAN value 0x07FF as
     * these fields are not used in intermediate
     * cuff pressure measurement
     */ 
    charval[idx++] = 0xFF;
    charval[idx++] = 0x07;
    charval[idx++] = 0xFF;
    charval[idx++] = 0x07;
    
    if(icpval.flags & FLAG_USER_ID_PRESENT)
    {
      charval[idx++] = icpval.userID;
    }
    

     /**
      * measurement status is a bitmask 
      * bit 0 : BODY_MOVEMENT_DETECTION_FLAG
      *     0 -> no body movement 
      * 	1 -> body movement during measurement
      * bit 1: CUFF_FIT_DETECTION_FLAG
      *     0 -> cuff fits properly
      *     1 -> cuff too lose
      * bit 2 : IRREGULAR_PULSE_DETECTION_FLAG
      *     0 -> no irregular pulse detected
      *     1 -> irregular pulse detected
      * bit 3 and bit 4: PULSE_RATE_RANGE_DETECTION_FLAG
      *     00 -> pulse rate is within the range
      *     01 -> pulse rate exceeds upper limit
      *     02 -> pulse rate is lesser than lower limit
      * bit 5 : MEASUREMENT_POSITION_DETECTION_FLAG
      *     0 -> proper measurement position
      *     1 -> improper measurement position
      */
    if(icpval.flags & FLAG_MEASUREMENT_STATUS_PRESENT)
    {
      if((bloodPressureSensor.bpsFeaturesSupported & BODY_MOVEMENT_DETECTION_SUPPORT_BIT) &&
         (icpval.measurementStatus.bodyMovementDetectionFlag))
      {
        charval[idx] =  BODY_MOVEMENT_DETECTION_FLAG;
      }
      
      if((bloodPressureSensor.bpsFeaturesSupported & CUFF_FIT_DETECTION_SUPPORT_BIT) &&
         (icpval.measurementStatus.cuffFitDetectionFlag))
      {
        charval[idx] |=  CUFF_FIT_DETECTION_FLAG;
      }
      
      if((bloodPressureSensor.bpsFeaturesSupported & IRREGULAR_PULSE_DETECTION_SUPPORT_BIT) &&
        (icpval.measurementStatus.irregularPulseDetectionFlag))
      {
        charval[idx] |=  IRREGULAR_PULSE_DETECTION_FLAG;
      }
      
      if(bloodPressureSensor.bpsFeaturesSupported & PULSE_RATE_RANGE_DETECTION_SUPPORT_BIT)
      {
              if(icpval.measurementStatus.pulseRateRangeDetectionFlag == 0x01)
              {
                charval[idx] |=  PULSE_RATE_RANGE_EXCEEDS_UPPER_LIMIT;
              }
              else if(icpval.measurementStatus.pulseRateRangeDetectionFlag == 0x02)
              {
                charval[idx] |=  PULSE_RATE_RANGE_BELOW_LOWER_LIMIT;
              }
      }
      
      if((bloodPressureSensor.bpsFeaturesSupported & MEASUREMENT_POSITION_DETECTION_SUPPORT_BIT) &&
        (icpval.measurementStatus.measurementPositionDetectionFlag))
      {
        charval[idx] |=  MEASUREMENT_POSITION_DETECTION_FLAG;
      }
      
      /* the measurement status field is 16 bits.
       * bit 6 to 15 are reserved for future use
       * and have to be set to 0
       */ 
      idx += 2;
    }
          
    retval =  aci_gatt_update_char_value(bloodPressureSensor.bpServiceHandle, 
                                         bloodPressureSensor.icpCharHandle,
                                         0, /* charValOffset */
                                         idx, /* charValueLen */
                                         (uint8_t *) charval);  
    if(retval == BLE_STATUS_SUCCESS)
    {
      BPS_DBG_MSG(profiledbgfile,"intermediate cuff pressure measurement sent successfully\n");
      Blue_NRG_Timer_Stop(bloodPressureSensor.timerID);
      bloodPressureSensor.timerID = 0xFF;
      BPProfile_Write_SubStateMachine(BPS_CONNECTED_DO_ICP);
    }
    else
    {
      BPS_DBG_MSG(profiledbgfile,"FAILED to send ICP value %02X \n", retval);
    }
  }
  
  return retval;
}/* end BPS_Send_Intermediate_Cuff_Pressure() */
/*******************************************************************************
* Local Functions
*******************************************************************************/

static void BPProfile_Write_MainStateMachine(tProfileState localmainState)
{
  bloodPressureSensor.bpsState = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState BPProfile_Read_MainStateMachine(void)
{
  return(bloodPressureSensor.bpsState);
}

static void BPProfile_Write_SubStateMachine(tProfileState localsubState)
{
  bloodPressureSensor.bpsSubState = localsubState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState BPProfile_Read_SubStateMachine(void)
{
  return(bloodPressureSensor.bpsSubState);
}

/* BPS_Sensor_Set_Discoverable() */
tBleStatus BPS_Sensor_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  
  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','o','o','d','P','r','e','s','s','u','r','e','S','e','n','s','o','r'};
  
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
}/* end BPS_Sensor_Set_Discoverable() */

/**
 * BPSAdd_Services_Characteristics
 * 
* @param[in]: None
 *            
 * It adds Blood pressure service & related characteristics
 */ 
tBleStatus BPS_Add_Services_Characteristics(void)
{
  tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;
  uint8_t numAttribRec = 0;
  uint16_t uuid = 0;
  uint16_t feature = 0;
    
  if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
  {
    BPS_DBG_MSG(profiledbgfile,"Main profile initialized, , Adding Blood Pressure Service\n");
  
    uuid = BLOOD_PRESSURE_SERVICE_UUID;
    numAttribRec = 6;
  
    /* if the intermediate cuff pressure is supported
    * then we have to support the client configuration
    * descriptor for that characteristic as well
    */
    if(bloodPressureSensor.bpsFeaturesSupported & BPS_ICP_CHAR_SUPPORTED)
    {
      numAttribRec += 3;
    }
    
    hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                     (const uint8_t *) &uuid, 
                                      PRIMARY_SERVICE, 
                                      numAttribRec,
                                      &(bloodPressureSensor.bpServiceHandle));
                
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
     BPS_DBG_MSG (profiledbgfile, "BPS_Add_Services_Characteristics(), Blood Pressure Service is added Successfully %04X\n", bloodPressureSensor.bpServiceHandle);
    }
    else 
    {
      BPS_DBG_MSG (profiledbgfile,"BPS_Add_Services_Characteristics(),FAILED to add Blood Pressure Service, Error: %02X !!\n", hciCmdResult);
      BPS_Exit(hciCmdResult); 
      return hciCmdResult;
    }
        
    /* Add Blood Pressure Measurement Characteristic */
    BPS_DBG_MSG (profiledbgfile,"BPS_Add_Services_Characteristics(),Adding Blood Pressure Measurement Characteristic. \n");
                      
    uuid = BLOOD_PRESSURE_MEASUREMENT_CHAR_UUID;
    
    hciCmdResult = aci_gatt_add_char(bloodPressureSensor.bpServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &uuid , 
                                     MAX_BPM_CHAR_VAL_LEN, 
                                     CHAR_PROP_INDICATE, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                     MIN_ENCRYPTION_KEY_SIZE,  /* encryKeySize */
                                     CHAR_VALUE_LEN_VARIABLE, /* isVariable */
                                     &(bloodPressureSensor.bpmCharHandle));
     
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      BPS_DBG_MSG (profiledbgfile, "BPS_Add_Services_Characteristics(),Blood Pressure Measurement Characteristic Added Successfully  %04X \n", bloodPressureSensor.bpmCharHandle);
    }
    else 
    {
      BPS_DBG_MSG (profiledbgfile,"BPS_Add_Services_Characteristics(),FAILED to add Blood Pressure Measurement Characteristic,  Error: %02X !!\n", hciCmdResult);
      BPS_Exit(hciCmdResult); 
      return hciCmdResult;
    }          
     
    /* Add Blood Pressure Feature Characteristic */
    BPS_DBG_MSG (profiledbgfile,"BPS_Add_Services_Characteristics(),Adding Blood Pressure Feature Characteristic. \n");
                         
    uuid = BLOOD_PRESSURE_FEATURE_CHARAC;
    hciCmdResult = aci_gatt_add_char(bloodPressureSensor.bpServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &uuid , 
                                     MAX_BPF_CHAR_VAL_LEN, 
                                     CHAR_PROP_READ, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                     MIN_ENCRYPTION_KEY_SIZE, /* encryKeySize */
                                     CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                     &(bloodPressureSensor.bpfCharHandle));
       
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      BPS_DBG_MSG (profiledbgfile, "BPS_Add_Services_Characteristics(),Blood Pressure Feature Characteristic Added Successfully  %04X \n", bloodPressureSensor.bpfCharHandle);
    }
    else 
    {
      BPS_DBG_MSG (profiledbgfile,"BPS_Add_Services_Characteristics(),FAILED to add Blood Pressure Feature Characteristic,  Error: %02X !!\n", hciCmdResult);
      BPS_Exit(hciCmdResult); 
      return hciCmdResult;
    }          
                                                                 
    /* Check if intermediate cuff pressure characteristic has to be added */
    if(bloodPressureSensor.bpsFeaturesSupported & BPS_ICP_CHAR_SUPPORTED)
    {
      /* add the intermediate cuff pressure
       * characteristic
       */
      uuid = INTERMEDIATE_CUFF_PRESSURE_CHARAC_UUID;
            
      hciCmdResult = aci_gatt_add_char(bloodPressureSensor.bpServiceHandle, 
                                       UUID_TYPE_16, 
                                       (const uint8_t *) &uuid , 
                                       MAX_ICP_CHAR_VAL_LEN, 
                                       CHAR_PROP_NOTIFY, 
                                       ATTR_PERMISSION_NONE, 
                                       GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                       MIN_ENCRYPTION_KEY_SIZE,  /* encryKeySize */
                                       CHAR_VALUE_LEN_VARIABLE, /* isVariable */
                                        &(bloodPressureSensor.icpCharHandle));
       
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        BPS_DBG_MSG (profiledbgfile, "BPS_Add_Services_Characteristics(),Intermediate cuff pressure Characteristic Added Successfully  %04X \n", bloodPressureSensor.icpCharHandle);
        
        BPS_DBG_MSG(profiledbgfile,"icp handle %x\n",bloodPressureSensor.icpCharHandle);
                          
        /* update the blood pressure feature characteristic */
        feature |= (bloodPressureSensor.bpsFeaturesSupported & 
                     (BODY_MOVEMENT_DETECTION_SUPPORT_BIT |
                      CUFF_FIT_DETECTION_SUPPORT_BIT |
                      IRREGULAR_PULSE_DETECTION_SUPPORT_BIT |
                      PULSE_RATE_RANGE_DETECTION_SUPPORT_BIT | 
                      MEASUREMENT_POSITION_DETECTION_SUPPORT_BIT));
        feature |= MULTIPLE_BONDS_SUPPORTED;
        
        BPS_DBG_MSG(profiledbgfile,"Update BPF VALUE with value: %04x\n",feature); 
        hciCmdResult =  aci_gatt_update_char_value(bloodPressureSensor.bpServiceHandle, 
                                             bloodPressureSensor.bpfCharHandle,
                                             0, /* charValOffset */
                                             2, /* charValueLen */
                                             (const uint8_t *) &feature);    
     
        if (hciCmdResult == BLE_STATUS_SUCCESS)
        {  
          BPS_DBG_MSG(profiledbgfile,"Updated blood pressure feature with success\n");
        }
        else
        {
          BPS_DBG_MSG(profiledbgfile,"Failed to Updated blood pressure feature %02X \n", hciCmdResult);
          BPS_Exit(hciCmdResult); 
          return hciCmdResult;
        }                  
      }
      else 
      {
        BPS_DBG_MSG (profiledbgfile,"BPS_Add_Services_Characteristics(),FAILED to add intermediate cuff pressure Characteristic,  Error: %02X !!\n", hciCmdResult);
        BPS_Exit(hciCmdResult); 
        return hciCmdResult;
      }             
    }
  }
  return hciCmdResult;
}/* end BPS_Add_Services_Characteristics() */
       
/**
 * BPS_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 * 
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB() 
 */ 
void BPS_Event_Handler(void *pckt)
{ 
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  BPS_DBG_MSG(profiledbgfile, "BPS_Event_Handler(), Blood pressureState: %02X \n", BPProfile_Read_MainStateMachine() );
  
  if(hci_pckt->type != HCI_EVENT_PKT)
  {
    BPS_DBG_MSG (profiledbgfile,"BPS_Event_Handler(),Recevied packed is not an HCI Event: %02X !!\n", hci_pckt->type);
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
          
          BPS_DBG_MSG( profiledbgfile, "BPS_Event_Handler(), Received event EVT_LE_CONN_COMPLETE %2x\n", cc->status);
          if (cc->status == BLE_STATUS_SUCCESS)
          {
            Blue_NRG_Timer_Stop(bloodPressureSensor.timerID);
            bloodPressureSensor.timerID = 0xFF;
            BPProfile_Write_MainStateMachine(BPS_CONNECTED); 
          }
        }
        break;
      }
    }
    break;
    case EVT_DISCONN_COMPLETE:
    {
      evt_disconn_complete *cc = (void *)event_pckt->data;
      
      BPS_DBG_MSG(profiledbgfile,"BPS_Event_Handler(), Received event EVT_DISCONN_COMPLETE %2x\n", cc->status);
      if (cc->status == BLE_STATUS_SUCCESS)
      {
        /* stop the idle connection timer if it is running */
        Blue_NRG_Timer_Stop(bloodPressureSensor.timerID);
        bloodPressureSensor.timerID = 0xFF;
        BPProfile_Write_MainStateMachine(BPS_INITIALIZED); 
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
          BPS_DBG_MSG(profiledbgfile,"BPS_Event_Handler(), Received event EVT_BLUE_GAP_LIMITED_DISCOVERABLE \n");
          if((BPProfile_Read_MainStateMachine() == BPS_ADV_IN_FAST_DISC_MODE) ||
              (BPProfile_Read_MainStateMachine() == BPS_ADV_IN_LOW_PWR_MODE))
          {
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State(&bloodPressureSensor.BPStoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE); 
            
            if(BPProfile_Read_MainStateMachine() == BPS_ADV_IN_LOW_PWR_MODE)
            {
              BPProfile_Write_MainStateMachine(BPS_INITIALIZED);
              /* notify the application */
              bloodPressureSensor.applcb(EVT_MP_ADVERTISING_TIMEOUT,0,NULL);
            }
          }
        }
        break;
      }
    }
    break;
  }
}/* end BPS_Event_Handler() */

/**
* BPS_StateMachine
* 
* @param[in] None
* 
* BPS profile's state machine: to be called on application main loop. 
*/ 
tBleStatus BPS_StateMachine(void)
{
  tBleStatus status = BLE_STATUS_SUCCESS;
  
  switch(BPProfile_Read_MainStateMachine())
  {
    case BPS_UNINITIALIZED:
    break;
    case BPS_WAITING_BLE_INIT:
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
      { 
        /* Add BPS service & characteristics */ 
        status = BPS_Add_Services_Characteristics();
        if (status == BLE_STATUS_SUCCESS)
        {
          BPS_DBG_MSG ( profiledbgfile, "BPS_StateMachine(), Blood Pressure Sensor Profile is Initialized with Service & Characteristics \n");
          
          /* Initialization complete */
          BLE_Profile_Add_Advertisment_Service_UUID(BLOOD_PRESSURE_SERVICE_UUID);
          /* vote to the main profile to go to advertise state */
          BLE_Profile_Vote_For_Advertisable_State(&bloodPressureSensor.BPStoBLEInf);
          
          BPProfile_Write_MainStateMachine(BPS_IDLE);
        }
        else
        {
          GL_DBG_MSG(profiledbgfile,"BPS_StateMachine(), BPS_Add_Services_Characteristics FAILED to add Glucose Service, Characteristics; Error: %02X !!\n", status);
         
          BPS_Exit(status);
        }
      }
    break; 
    case BPS_IDLE:
    {
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE)
      {
        BPS_DBG_MSG(profiledbgfile,"BPS_StateMachine(), Notify the application that initialization is complete\n");
        status = BLE_STATUS_SUCCESS;
        BPProfile_Write_MainStateMachine(BPS_INITIALIZED);
        bloodPressureSensor.applcb(EVT_BPS_INITIALIZED,1,&status);
      }
    }
    break;
    
    case BPS_ADV_IN_FAST_DISC_MODE_STOP:
    {
      status = BPS_Sensor_Set_Discoverable(BPS_LOW_PWR_ADV_INTERVAL_MIN,BPS_LOW_PWR_ADV_INTERVAL_MAX);
      
      if(status == BLE_STATUS_SUCCESS)
      {											 
        BPProfile_Write_MainStateMachine(BPS_ADV_IN_LOW_PWR_MODE);
        BPS_DBG_MSG(profiledbgfile," BPS_StateMachine(),enabled advertising low power connection\n");
      }
      else
      {
        BPProfile_Write_MainStateMachine(BPS_INITIALIZED);
        bloodPressureSensor.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
        BPS_DBG_MSG(profiledbgfile,"BPS_StateMachine(), ERROR: enabled advertising low power  connection\n");
      }
    }
    break;
    
    case BPS_UNDIRECTED_ADV_MODE_STOP:
    {
      /* request for main profile's state change */
      BLE_Profile_Change_Advertise_Sub_State(&bloodPressureSensor.BPStoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
      
      /* the advertising timeout has happened
       * inform the application
       */ 
      BPProfile_Write_MainStateMachine(BPS_IDLE);
      bloodPressureSensor.applcb(EVT_MP_ADVERTISING_TIMEOUT,0,NULL);
      
    }
    break;
    
    /* TO BE REVIEWED */
    case BPS_CONNECTED:
    {
      status = BLE_STATUS_SUCCESS;
      switch(BPProfile_Read_SubStateMachine())
      {
        case (BPS_CONNECTED_DO_BPM):
        {
          BPProfile_Write_SubStateMachine(BPS_CONNECTED_STOP_BPM);
          /* an update was done successfully, now we can start the idle timer */
          Blue_NRG_Timer_Start(5,BPS_IdleConnection_Handler,&bloodPressureSensor.timerID);
          /* notify the application of the successful update */
          bloodPressureSensor.applcb(EVT_BPS_BPM_CHAR_UPDATE_CMPLT,1,&status);
        }
        break;
        case (BPS_CONNECTED_DO_ICP):
        {
          BPProfile_Write_SubStateMachine(BPS_CONNECTED_STOP_ICP);
          /* an update was done successfully, now we can start the idle timer */
          Blue_NRG_Timer_Start(5,BPS_IdleConnection_Handler,&bloodPressureSensor.timerID);
          /* notify the application of the successful update */
          bloodPressureSensor.applcb(EVT_BPS_ICP_CHAR_UPDATE_CMPLT,1,&status);
        }
        break;
      }
    break;
    }
  }
  return (status);
}/* end BPS_StateMachine() */

/**
 * BPS_AdvTimeout_Handler
 * 
 * advertising timeout handler
 */ 
void BPS_AdvTimeout_Handler(void)
{
  Blue_NRG_Timer_Stop(bloodPressureSensor.timerID);
  bloodPressureSensor.timerID = 0xFF;
  switch(BPProfile_Read_MainStateMachine())
  {
    case BPS_UNDIRECTED_ADV_MODE:
    {
      aci_gap_set_non_discoverable();
      
      BPProfile_Write_MainStateMachine(BPS_UNDIRECTED_ADV_MODE_STOP);
    }  
    case BPS_ADV_IN_FAST_DISC_MODE:
    {
      aci_gap_set_non_discoverable();
      
      BPProfile_Write_MainStateMachine(BPS_ADV_IN_FAST_DISC_MODE_STOP);
    }
    break;
  }
}

/**
 * BPS_IdleConnection_Handler
 * 
 * informs the application that the connection has been
 * idle for 5 seconds. The application may disconnect the
 * link if desired
 */ 
void BPS_IdleConnection_Handler(void)
{
  if(BPProfile_Read_MainStateMachine() == BPS_CONNECTED)
  {
    /* for 5 seconds the connection has been idle
     * so disconnect
     * TODO : or just inform the application and the
     * applciation can decide whether to disconnect
     */
     Blue_NRG_Timer_Stop(bloodPressureSensor.timerID);
     bloodPressureSensor.timerID = 0xFF;
     bloodPressureSensor.applcb(EVT_BPS_IDLE_CONNECTION_TIMEOUT,0x00,NULL);
  }
}

/**
 * BPS_Exit
 * 
 * @param[in] errCode: reason for exit
 * 
 * informs the application for the reason
 * the initialization failed and de-registers 
 * the profile
 */ 
void BPS_Exit(uint8_t errCode)
{
  BPProfile_Write_MainStateMachine(BPS_UNINITIALIZED);
  bloodPressureSensor.applcb(EVT_BPS_INITIALIZED,1,&errCode);
  BLE_Profile_Unregister_Profile(&bloodPressureSensor.BPStoBLEInf);
}
