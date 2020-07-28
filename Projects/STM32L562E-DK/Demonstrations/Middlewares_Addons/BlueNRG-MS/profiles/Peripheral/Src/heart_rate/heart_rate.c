/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2014 STMicroelectronics International NV
*
*   FILENAME        -  heart_rate.c
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/07/2014
*   $Revision$:  first version
*   $Author$:    
*   Comments:    Heart Rate Profile Implementation File 
*
*   $Date$:      09/07/2014
*   $Revision$:  Third version 
*   $Author$:    
*   Comments:    Redesigned the Heart Rate Profile according to unified BlueNRG 
*                DK framework          
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  This file will have implementation of Heart Rate Profile. 
* 
******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
#include "hci.h"
#include "bluenrg_aci.h"
#include "bluenrg_gatt_server.h"
#include "hci_const.h"
#include "bluenrg_gap.h"
#include "sm.h"
#include <stdio.h>

#include <host_config.h>
#include <debug.h>
#include <timer.h>
#include <ble_profile.h> 
#include <ble_events.h> 
#include <uuid.h>
#include <hal.h>

#include <heart_rate.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Advertising intervals in terms of 625 micro seconds */
#define HRP_FAST_CONN_ADV_INTERVAL_MIN	               (0x20) /* 20ms */
#define HRP_FAST_CONN_ADV_INTERVAL_MAX	               (0x30) /* 30ms */
#define HRP_LOW_PWR_ADV_INTERVAL_MIN	               (0x640) /* 1 second */
#define HRP_LOW_PWR_ADV_INTERVAL_MAX	               (0xFA0) /* 2.5 seconds */

/* heart rate profile main states */
#define HRPROFILE_STATE_UNINITIALIZED                  (0x00)
#define HRPROFILE_STATE_WAITING_BLE_INIT               (0x10)
#define HRPROFILE_STATE_INITIALIZED                    (0x20)
#define HRPROFILE_STATE_ADVERTIZE                      (0x30)
#define HRPROFILE_STATE_CONNECTED                      (0x40)
#define HRPROFILE_STATE_INVALID                        (0xFF)

/* substates for HRPROFILE_STATE_ADVERTIZE main state */
#define HRPROFILE_STATE_ADVERTISABLE_IDLE                            (0x31) 
#define HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST                   (0x32) 
/* This substate is set on timer timeout */
#define HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP   (0x33)
#define HRPROFILE_STATE_FAST_DISCOVERABLE_DO_START                   (0x34)
#define HRPROFILE_STATE_FAST_DISCOVERABLE                            (0x35) 
/* This substate is set on timer timeout */
#define HRPROFILE_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP            (0x36)
#define HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_DO_START              (0x37)
#define HRPROFILE_STATE_LOW_POWER_DISCOVERABLE                       (0x38) 
/* This substate is set on timer timeout */
#define HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP       (0x39)
#define HRPROFILE_STATE_BLE_ADVERTISING                              (0x3A) 
#define HRPROFILE_STATE_PROCESSING_PAIRING                           (0x3B)

/* substates for HRPROFILE_STATE_CONNECTED main state */
#define HRPROFILE_STATE_CONNECTED_IDLE                      (0x41)
#define HRPROFILE_STATE_DO_DISCONNECT                       (0x42)
#define HRPROFILE_STATE_DISCONNECTING                       (0x43)
#define HRPROFILE_STATE_DISCONNECTED                        (0x44)

/******************************************************************************
* type definitions
******************************************************************************/
typedef struct _tHeartRateContext
{
  /**
  * main state of the heart rate
  * sensor state machine
  */ 
  tProfileState mainState;
  
  /**
  * sub state of the heart rate
  * sensor state machine
  */ 
  tProfileState subState;
  
  /**
  * FOR INTERNAL USE ONLY
  * the sensor location information is not sent by
  * default in the heart rate measurement. If it needs to
  * be sent, the application needs to enable it before 
  * sending out any heart rate measurement updates.
  * this flag keeps track of the communication status
  * of the heart rate measurement
  */
  uint8_t hrmCommunicationStarted;
  
  /**
  * callback to notify the applciation with
  * the profile specific events
  */ 
  BLE_CALLBACK_FUNCTION_TYPE applicationNotifyFunc;
  
  /**
  * connection handle
  */ 
  uint16_t connHandle;
  /**
  * the characteristics supported in
  * the heart rate service
  */ 
  uint16_t serviceSupport;
  
  /**
  * heart rate service handle
  */ 
  uint16_t hrServiceHandle;
  
  /**
  * heart rate measurement characteristic
  * handle
  */ 
  uint16_t hrmCharHandle;
  
  /**
  * heart rate measurement characteristic
  * descriptor handle
  */ 
  uint16_t hrmCharDescHandle;
  
  /**
  * body sensor location 
  * characteristic handle
  */ 
  uint16_t bodySensorLocCharHandle;
  
  /**
  * control point characteristic
  * handle
  */ 
  uint16_t ctrlPointCharHandle;
  
  /**
  * device information characteristic
  * handle
  */ 
  uint16_t devInfoServiceHandle;
  
  /**
  * manufacturer string characteristic
  * handle of the device information service
  */ 
  uint16_t manufacturerStringCharHandle;
  
  /**
  * sensor contact support flag
  * when set, the sensor location
  * information is included in the
  * heart rate measurement characteristic
  */ 
  uint8_t sensorContactSupport;
  
  /**
  * will contain the timer ID of the 
  * timer started by the heart rate profile
  */ 
  tTimerID timerID;
  /**
  * profile interface context. This is
  * registered with the main profile
  */ 
  tProfileInterfaceContext HRStoBLEInf;
  
  /**
  * body sensor location
  */ 
  tBodySensorLocation hrsBodySensorLocation;
}tHeartRateContext;

/******************************************************************************
* Variable Declarations
******************************************************************************/
static tHeartRateContext heartRate;

/******************************************************************************
* Imported Variable
******************************************************************************/
extern void *profiledbgfile;

/******************************************************************************
* Function Prototypes
******************************************************************************/
static tBleStatus HRProfile_Add_Services_Characteristics(void);
static tBleStatus HR_Sensor_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax,uint8_t AdvFilterPolicy);
static void HRProfile_Advertise_Period_Timeout_Handler (void);

static void HRProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState HRProfile_Read_MainStateMachine(void);
static void HRProfile_Write_SubStateMachine(tProfileState localsubState);
static tProfileState HRProfile_Read_SubStateMachine(void);

/*******************************************************************************
* SAP
*******************************************************************************/
tBleStatus HRProfile_Init(uint8_t featureSupportFlag,
                          BLE_CALLBACK_FUNCTION_TYPE hrProfileCallbackFunc, 
                          tBodySensorLocation sensorLocation) 
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  
  HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Init(), Start \n" );
  
  if (Is_Profile_Present(PID_HEART_RATE_SENSOR) == BLE_STATUS_SUCCESS)
  {
	return (BLE_STATUS_PROFILE_ALREADY_INITIALIZED);
  }  
  if (hrProfileCallbackFunc == NULL) 
  {
    return (BLE_STATUS_NULL_PARAM);
  }
  
  /* Initialize Heart Rate Profile's Data Structures */
  BLUENRG_memset(&heartRate,0,sizeof(tHeartRateContext));
  
  heartRate.applicationNotifyFunc = hrProfileCallbackFunc; 
  heartRate.timerID = 0xFF;
  
  heartRate.serviceSupport = 0x0000;
  heartRate.sensorContactSupport = 0x00;
  
  if (featureSupportFlag & BODY_SENSOR_LOCATION_SUPPORT_MASK)
  {
    heartRate.serviceSupport |= BODY_SENSOR_LOCATION_SUPPORT_MASK;
    if (sensorLocation <= BODY_SENSOR_LOCATION_FOOT)
    {
      heartRate.hrsBodySensorLocation = sensorLocation;
    }
  }
  if (featureSupportFlag & ENERGY_EXTENDED_INFO_SUPPORT_MASK)
  {
    heartRate.serviceSupport |= ENERGY_EXTENDED_INFO_SUPPORT_MASK;
  }
  
  /* Initialize Heart Rate Profile to BLE main Profile */
  BLUENRG_memset ( &heartRate.HRStoBLEInf, 0, sizeof(heartRate.HRStoBLEInf) );
  
  
  heartRate.HRStoBLEInf.profileID = PID_HEART_RATE_SENSOR; 
  
  /* heartRate.HRStoBLEInf.callback_func is the event handler to be called inside HCI_Event_CB() */
  heartRate.HRStoBLEInf.callback_func = HRProfile_Rx_Event_Handler; 
  
  heartRate.HRStoBLEInf.voteForAdvertisableState = 0;
  
  /* manufacturer name characteristic
  *  has to be added to the device information service
  */ 
  heartRate.HRStoBLEInf.DISCharRequired = (MANUFACTURER_NAME_STRING_CHAR_MASK); 
  
  /* register profile */
  retval = BLE_Profile_Register_Profile ( &heartRate.HRStoBLEInf );
  if (retval != BLE_STATUS_SUCCESS)
  {
    /* Profile could not be registered with the BLE main profile */
    return (retval);
  }

  /* Initialize profile's state machine's state variables */
  HRProfile_Write_MainStateMachine(HRPROFILE_STATE_WAITING_BLE_INIT);
  HRProfile_Write_SubStateMachine(HRPROFILE_STATE_WAITING_BLE_INIT);
  
  HRPROFILE_MESG_DBG ( profiledbgfile, "Heart Rate Profile is Initialized \n");
  
  return (retval);
}/* end HRProfile_Init() */


/* HR_Sensor_Make_Discoverable */
tBleStatus HR_Sensor_Make_Discoverable(uint8_t useBoundedDeviceList)
{
  tBleStatus retval = BLE_STATUS_FAILED; /* TBR */
  
  if ((HRProfile_Read_MainStateMachine() == HRPROFILE_STATE_ADVERTIZE)  &&
      (HRProfile_Read_SubStateMachine() == HRPROFILE_STATE_ADVERTISABLE_IDLE))
  {
    HRPROFILE_MESG_DBG(profiledbgfile, "HR_Sensor_Make_Discoverable(), HRP Set Discoverable mode: %d \n", useBoundedDeviceList );
    
    if ((useBoundedDeviceList) && (gBLEProfileContext.bleSecurityParam.bonding_mode)) 
    {
      HRPROFILE_MESG_DBG ( profiledbgfile, "HR_Sensor_Make_Discoverable(), STARTING Advertising for White List devices \n");
      
      retval = HR_Sensor_Set_Discoverable(HRP_FAST_CONN_ADV_INTERVAL_MIN,HRP_FAST_CONN_ADV_INTERVAL_MAX, WHITE_LIST_FOR_ALL);  /* use white list */
      if (retval == BLE_STATUS_SUCCESS)
      {
        /* start a timer of 10 seconds to try to connnect to white list device */
        Blue_NRG_Timer_Start (10, HRProfile_Advertise_Period_Timeout_Handler,&heartRate.timerID); 
        
        HRPROFILE_MESG_DBG ( profiledbgfile, "HR_Sensor_Make_Discoverable(), Device is now in Advertising for White List devices \n");
        /* change profile's sub states */
        HRProfile_Write_SubStateMachine(HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST);
        /* request for main profile's state change */
        BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf,BLE_PROFILE_STATE_ADVERTISING); 
        HRPROFILE_MESG_DBG ( profiledbgfile, "HR_Sensor_Make_Discoverable(), main profile state has been set.  \n");
      } 
      else
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HR_Sensor_Make_Discoverable(), FAILED to start advertising for White List devices, Error: %02X !!\n", retval);
        return retval; 
      }
    }
    else
    {
      HRPROFILE_MESG_DBG ( profiledbgfile, "HR_Sensor_Make_Discoverable(), STARTING FAST General Discoverable Mode \n");
      
      retval = HR_Sensor_Set_Discoverable(HRP_FAST_CONN_ADV_INTERVAL_MIN,HRP_FAST_CONN_ADV_INTERVAL_MAX, NO_WHITE_LIST_USE);  /* no white list */
      if (retval == BLE_STATUS_SUCCESS)
      {
        /* start a timer of 30 seconds for first phase of discoverable mode (fast discoverable mode) */
        Blue_NRG_Timer_Start (30, HRProfile_Advertise_Period_Timeout_Handler,&heartRate.timerID);

        
        HRPROFILE_MESG_DBG ( profiledbgfile, "HR_Sensor_Make_Discoverable(), Device is now in FAST General Discoverable Mode \n");
        /* change profile's sub states */
        HRProfile_Write_SubStateMachine(HRPROFILE_STATE_FAST_DISCOVERABLE);
        /* request for main profile's state change */
        BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf, BLE_PROFILE_STATE_ADVERTISING); 
        HRPROFILE_MESG_DBG ( profiledbgfile, "HR_Sensor_Make_Discoverable(), main profile state has been set.  \n");
      } 
      else
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HR_Sensor_Make_Discoverable(), FAILED to start FAST General Discoverable Mode, Error: %02X !!\n", retval);
        return retval; 
      }
    }
  }
  else
  {
    HRPROFILE_MESG_DBG ( profiledbgfile, "HR_Sensor_Make_Discoverable(), HRP Set Discoverable mode: Invalid Task State \n");
  }
  return (retval);
}/* end HR_Sensor_Make_Discoverable() */


/* HRProfile_Set_Body_Sensor_Location() */
tBleStatus HRProfile_Set_Body_Sensor_Location(tBodySensorLocation bdsValue)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  uint8_t tempBuf[5] = {0};
  
  HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Set_Body_Sensor_Location(), Body Sensor Location is to update !! \n");
  
  /* check whether the body sensor location value can be accepted for char update of not */
  if((heartRate.serviceSupport & BODY_SENSOR_LOCATION_SUPPORT_MASK) &&
     ((HRProfile_Read_MainStateMachine() == HRPROFILE_STATE_INITIALIZED) || (HRProfile_Read_MainStateMachine() == HRPROFILE_STATE_ADVERTIZE)) &&
       (bdsValue <= BODY_SENSOR_LOCATION_FOOT))
  {
    heartRate.hrsBodySensorLocation = bdsValue;
    
    retval =  aci_gatt_update_char_value(heartRate.hrServiceHandle, 
                                         heartRate.bodySensorLocCharHandle,
                                         0, /* charValOffset */
                                         1, /* charValueLen */
                                         (const uint8_t *) &(heartRate.hrsBodySensorLocation));    
    if (retval == BLE_STATUS_SUCCESS)
    {
      HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Set_Body_Sensor_Location(), Body sensor location is req of update \n");
      /* TBR: do we need to notify main application */
      HOST_TO_LE_16(tempBuf,heartRate.hrServiceHandle);
      HOST_TO_LE_16(&tempBuf[2],heartRate.bodySensorLocCharHandle);
      tempBuf[4] = retval;
      heartRate.applicationNotifyFunc(EVT_HRS_CHAR_UPDATE_CMPLT,5,tempBuf);
    }
    else
    {
      HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Set_Body_Sensor_Location(), FAILED to update body sensor location %02X \n", retval);
      return retval;
    }
  }
  else
  {
    HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Set_Body_Sensor_Location(), Heart Rate Sensor is not in proper state to update Body Sensor Location \n");
    retval = BLE_STATUS_FAILED;
  }
  
  return (retval);
}/* end HRProfile_Set_Body_Sensor_Location() */

/* HRProfile_Set_Sensor_Contact_Support_Bit() */
tBleStatus HRProfile_Set_Sensor_Contact_Support_Bit (void)
{
  if (heartRate.hrmCommunicationStarted == 0)
  {
    heartRate.sensorContactSupport = 0x01;
    return (BLE_STATUS_SUCCESS);
  }
  else
  {
    return (BLE_STATUS_FAILED);
  }
}/* end HRProfile_Set_Sensor_Contact_Support_Bit() */


/* HRProfile_Send_HRM_Value() */
tBleStatus HRProfile_Send_HRM_Value ( tHeartRateMeasure heartRateVal )
{
  uint8_t hrmCharLength = 0;
  uint8_t hrmCharValue[30] = {0};
  uint16_t rrValTemp = 0;
  uint8_t maxRRIntervalValues = 0;
  uint8_t indx = 0;
  uint8_t tempBuf[5] = {0}; //TBR 
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  
  HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Send_HRM_Value(), Received a Heart Rate Measurement value to send \n");
  
  if ((HRProfile_Read_MainStateMachine() == HRPROFILE_STATE_CONNECTED) &&
      (HRProfile_Read_SubStateMachine() == HRPROFILE_STATE_CONNECTED_IDLE))
  {
    {
      /* set the status that heart rate measurment value
      * communication has already started */
      heartRate.hrmCommunicationStarted = 1;
      
      /* reset the characteristic buffer and its length value. */
      BLUENRG_memset ( (void*)hrmCharValue, 0, 30);
      hrmCharLength = 1;
      
      /* Heart Rate Measurment Value */
      if (heartRateVal.valueformat == HRM_VALUE_FORMAT_UINT8)
      {
        hrmCharValue[hrmCharLength] = (uint8_t)heartRateVal.heartRateValue;
        hrmCharLength++;
      }
      else if (heartRateVal.valueformat == HRM_VALUE_FORMAT_UINT16)
      {
        hrmCharValue[0] |= HRM_VALUE_FORMAT_UINT16;
        hrmCharValue[hrmCharLength] = (uint8_t)(heartRateVal.heartRateValue & 0xFF);
        hrmCharLength++;
        hrmCharValue[hrmCharLength] = (uint8_t)(heartRateVal.heartRateValue >> 8);
        hrmCharLength++;
      }
      else
      {
        HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Send_HRM_Value(),Invalid HRM value format \n");
        return (retval);
      }
      
      /* Sensor Contact */
      if (heartRate.sensorContactSupport)
      {
        hrmCharValue[0] |= HRM_SENSOR_CONTACT_SUPPORTED;
        if (heartRateVal.sensorContact == HRM_SENSOR_CONTACT_NOT_PRESENT)
        {
          hrmCharValue[0] |= HRM_SENSOR_CONTACT_NOT_PRESENT;
        }
        else if (heartRateVal.sensorContact == HRM_SENSOR_CONTACT_PRESENT)
        {
          hrmCharValue[0] |= HRM_SENSOR_CONTACT_PRESENT;
        }
        else
        {
          HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Send_HRM_Value(),Invalid value for sensor contact \n");
          return (retval);
        }
      }
      
      /* Energy Extended */
      if (heartRate.serviceSupport & ENERGY_EXTENDED_INFO_SUPPORT_MASK)
      {
        if (heartRateVal.energyExpendedStatus == HRM_ENERGY_EXPENDED_NOT_PRESENT)
        {
          hrmCharValue[0] |= HRM_ENERGY_EXPENDED_NOT_PRESENT;
        }
        else if (heartRateVal.energyExpendedStatus == HRM_ENERGY_EXPENDED_PRESENT)
        {
          HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Send_HRM_Value(),HRM energy expended is present \n");
          hrmCharValue[0] |= HRM_ENERGY_EXPENDED_PRESENT;
          hrmCharValue[hrmCharLength] = (uint8_t)(heartRateVal.enrgyExpended & 0xFF);
          hrmCharLength++;
          hrmCharValue[hrmCharLength] = (uint8_t)(heartRateVal.enrgyExpended >> 8);
          hrmCharLength++;
        }
        else
        {
          HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Send_HRM_Value(),Invalid Value for energy expended parameter \n");
          return (retval);
        }
      }
      
      /* RR Interval Values */
      if (heartRateVal.rrIntervalStatus == HRM_RR_INTERVAL_NOT_PRESENT)
      {
        hrmCharValue[0] |= HRM_RR_INTERVAL_NOT_PRESENT;
      }
      else if (heartRateVal.rrIntervalStatus == HRM_RR_INTERVAL_PRESENT)
      {
        hrmCharValue[0] |= HRM_RR_INTERVAL_PRESENT;
        maxRRIntervalValues = 9;
        if (heartRateVal.valueformat == HRM_VALUE_FORMAT_UINT16)
        {
          maxRRIntervalValues--;
        }
        
        if (hrmCharValue[0] & HRM_ENERGY_EXPENDED_PRESENT)
        {
          maxRRIntervalValues--;
        }
        
        if (maxRRIntervalValues > heartRateVal.numOfRRIntervalvalues)
        {
          /* use only max applowed values and most latest values */
          maxRRIntervalValues = heartRateVal.numOfRRIntervalvalues;
        }
        
        for ( indx = 0 ; indx < maxRRIntervalValues ; indx++ )
        {
          rrValTemp = heartRateVal.rrIntervalValues[indx];
          hrmCharValue[hrmCharLength] = (uint8_t)(rrValTemp & 0xFF);
          hrmCharLength++;
          hrmCharValue[hrmCharLength] = (uint8_t)(rrValTemp >> 8);
          hrmCharLength++;
        }
      }
      else
      {
        HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Send_HRM_Value(),Invalid value for RR Interval Present parameter \n");
        return (retval);
      }
    }
    
    retval =  aci_gatt_update_char_value(heartRate.hrServiceHandle, 
                                         heartRate.hrmCharHandle,
                                         0, /* charValOffset */
                                         hrmCharLength, /* charValueLen */
                                         (const uint8_t *) &hrmCharValue[0]);    
      if (retval == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Send_HRM_Value(), HRM value sent successfully \n");
        
        /* start a timer to count starvation of heart rate measurment value timeout. */
        /*Timer_Start_Timeout (20, HRProfile_Device_Disconnect);*/
        
        /* TBR: notify characteristic update to main profile */
        HOST_TO_LE_16(tempBuf,heartRate.hrServiceHandle);
        HOST_TO_LE_16(&tempBuf[2],heartRate.hrmCharHandle);
        tempBuf[4] = retval;
        heartRate.applicationNotifyFunc(EVT_HRS_CHAR_UPDATE_CMPLT,5,tempBuf); 
      }
      else
      {
        HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Send_HRM_Value(),FAILED to send HRM value %02X \n", retval);
      }
  }
  else
  {
    HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Send_HRM_Value(),Heart Rate Profile is not in proper state to send HRM value \n");
    retval = BLE_STATUS_FAILED;
  }
  
  return (retval);
}/* end HRProfile_Send_HRM_Value() */

/******************************************************************************
* Local Functions
******************************************************************************/

static void HRProfile_Write_MainStateMachine(tProfileState localmainState)
{
  heartRate.mainState = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState HRProfile_Read_MainStateMachine(void)
{
  return(heartRate.mainState);
}

static void HRProfile_Write_SubStateMachine(tProfileState localsubState)
{
  heartRate.subState = localsubState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState HRProfile_Read_SubStateMachine(void)
{
  return(heartRate.subState);
}

/* HR_Sensor_Set_Discoverable() */
tBleStatus HR_Sensor_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax,uint8_t AdvFilterPolicy)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'H','e','a','r','t','R','a','t','e','S','e','n','s','o','r'};
  
  retval = aci_gap_set_discoverable(ADV_IND,
                                    AdvIntervMin, 
                                    AdvIntervMax,
                                    PUBLIC_ADDR, 
                                    AdvFilterPolicy, /* use white list */
                                    sizeof(local_name), 
                                    local_name, 
                                    gBLEProfileContext.advtServUUIDlen, 
                                    gBLEProfileContext.advtServUUID,    
                                    0, 
                                    0);
  
  return retval;
}/* end HR_Sensor_Set_Discoverable() */


/**
* HRProfile_Add_Services_Characteristics
* 
* @param[in]: None
*            
* It adds Hearth Rate service & related characteristics
*/ 
tBleStatus HRProfile_Add_Services_Characteristics(void)
{
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;
  uint16_t UUID = 0;
  uint8_t serviceMaxAttributeRecord = 0;
  
  /* Add Heart Rate Service */
  HRPROFILE_MESG_DBG( profiledbgfile, "Adding Heart Rate Service \n");
  /* Max_Attribute_Records = 2*no_of_char + 1 */
  /* serviceMaxAttributeRecord = 1 for heart rate service +
  *                             2 for hear rate measurment characteristic +
  *                             1 for client char configuration descriptor +
  *                             2 for body sensor location characteristic + 
  *                             2 for control point characteristic
  */
  serviceMaxAttributeRecord = 4;
  
  if (heartRate.serviceSupport & BODY_SENSOR_LOCATION_SUPPORT_MASK)
  {   
    /* for optional body sensor location characteristic */
    serviceMaxAttributeRecord += 2;
  }
  if (heartRate.serviceSupport & ENERGY_EXTENDED_INFO_SUPPORT_MASK)
  {
    /* for optional heart rate control point characteristic */
    serviceMaxAttributeRecord += 2;
  }
  
  /* store the heart rate service UUID in LE format */ 
  UUID = HEART_RATE_SERVICE_UUID;
  hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                   (const uint8_t *) &UUID, 
                                   PRIMARY_SERVICE, 
                                   serviceMaxAttributeRecord,
                                   &(heartRate.hrServiceHandle));
  
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    HRPROFILE_MESG_DBG (profiledbgfile, "HRProfile_Add_Services_Characteristics(), Heart Rate Service is added Successfully %04X\n", heartRate.hrServiceHandle);
  }
  else 
  {
    HRPROFILE_MESG_ERR (profiledbgfile,"HRProfile_Add_Services_Characteristics(),FAILED to add Heart Rate Service, Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }
  
  /* Add Heart Rate Measurement Characteristic */
  HRPROFILE_MESG_DBG (profiledbgfile,"HRProfile_Add_Services_Characteristics(),Adding Heart Rate Measurement Characteristic. \n");
  
  UUID = HEART_RATE_MEASURMENT_UUID;
  
  hciCmdResult = aci_gatt_add_char(heartRate.hrServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &UUID , 
                                   30, /* size of the characteristic flags(1b)+measure(2b)+EE(2b)+RR(7b)*/ 
                                   CHAR_PROP_NOTIFY, 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                   10, /* encryKeySize */
                                   1, /* isVariable */
                                   &(heartRate.hrmCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    HRPROFILE_MESG_DBG (profiledbgfile, "HRProfile_Add_Services_Characteristics(),Heart Rate Measurement Characteristic Added Successfully  %04X \n", heartRate.hrmCharHandle);
  }
  else 
  {
    HRPROFILE_MESG_ERR (profiledbgfile,"HRProfile_Add_Services_Characteristics(),FAILED to add Heart Rate Measurement Characteristic,  Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }          
  
  /* Add Sensor Location Characteristic */
  HRPROFILE_MESG_DBG (profiledbgfile,"HRProfile_Add_Services_Characteristics(),Adding Sensor Location Characteristic. \n");
  
  UUID = SENSOR_LOCATION_UUID;
  hciCmdResult = aci_gatt_add_char(heartRate.hrServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &UUID , 
                                   1, /* bytes */
                                   CHAR_PROP_READ, 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                   10, /* encryKeySize */
                                   0, /* isVariable: 0 */
                                   &(heartRate.bodySensorLocCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    HRPROFILE_MESG_DBG (profiledbgfile, "HRProfile_Add_Services_Characteristics(),Sensor Location Characteristic Added Successfully  %04X \n", heartRate.bodySensorLocCharHandle);
  }
  else 
  {
    HRPROFILE_MESG_ERR (profiledbgfile,"HRProfile_Add_Services_Characteristics(),FAILED to add Sensor Location Characteristic,  Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }       
  
  
  HRPROFILE_MESG_DBG (profiledbgfile,"HRProfile_Add_Services_Characteristics(),Setting default Body Sensor Location. \n");
  HRPROFILE_MESG_DBG (profiledbgfile,"HRProfile_Add_Services_Characteristics(),Body sensor value is %d \n",heartRate.hrsBodySensorLocation); 
  
  hciCmdResult =  aci_gatt_update_char_value(heartRate.hrServiceHandle, 
                                             heartRate.bodySensorLocCharHandle,
                                             0, /* charValOffset */
                                             1, /* charValueLen */
                                             (const uint8_t *) &(heartRate.hrsBodySensorLocation));    
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    HRPROFILE_MESG_DBG (profiledbgfile, "HRProfile_Add_Services_Characteristics(),Body Sensor Location has been set Successfully \n");    
    
#if 0 /* Do we need to  notify main profile TBR */
    uint8_t tempBuf[5] = {0}; 
    HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Add_Services_Characteristics(), body sensor location updated\n");
    STORE_LE_16(tempBuf,heartRate.hrServiceHandle);
    STORE_LE_16(&tempBuf[2],heartRate.bodySensorLocCharHandle);
    tempBuf[4] = hciCmdResult;
    heartRate.applicationNotifyFunc(EVT_HRS_CHAR_UPDATE_CMPLT,5,tempBuf);     
#endif 
  }
  else
  {
    HRPROFILE_MESG_ERR (profiledbgfile,"HRProfile_Add_Services_Characteristics(),FAILED to set default Body Sensor Location: Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }
  
  /* Hearth Rate Control point characteristic is mandatory only if Energy Expended feature is supported */       
  if (heartRate.serviceSupport & ENERGY_EXTENDED_INFO_SUPPORT_MASK)
  {
    HRPROFILE_MESG_DBG (profiledbgfile,"HRProfile_Add_Services_Characteristics(),Adding Control Point Characteristic. \n");
    
    UUID = CONTROL_POINT_UUID;
    
    hciCmdResult = aci_gatt_add_char(heartRate.hrServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &UUID , 
                                     1, /* bytes */
                                     CHAR_PROP_WRITE, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP, /* gattEvtMask */
                                     10, /* encryKeySize */
                                     0, /* isVariable: 0*/
                                     &(heartRate.ctrlPointCharHandle));
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HRPROFILE_MESG_DBG (profiledbgfile, "HRProfile_Add_Services_Characteristics(),Control Point Characteristic Added Successfully  %04X \n", heartRate.ctrlPointCharHandle);
    }
    else 
    {
      HRPROFILE_MESG_ERR (profiledbgfile,"HRProfile_Add_Services_Characteristics(),FAILED to add Control Point Characteristic,  Error: %02X !!\n", hciCmdResult);
      return hciCmdResult;
    }          
  }
  
  return (hciCmdResult);
}  /* HRProfile_Add_Services_Characteristics() */


/**
* HRProfile_Rx_Event_Handler
* 
* @param[in] pckt : Pointer to the ACI packet
* 
* It parses the events received from ACI according to the profile's state machine.
* Inside this function each event must be identified and correctly parsed.
* NOTE: It is the event handler to be called inside HCI_Event_CB() 
*/ 
void HRProfile_Rx_Event_Handler(void *pckt)
{
  uint16_t attrHandle = 0;
  uint8_t attValLen = 0;
  uint8_t attValue = 0;
  tBleStatus retval = BLE_STATUS_SUCCESS;
  
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  HRPROFILE_MESG_DBG(profiledbgfile, "HRProfile_Rx_Event_Handler(), Heart Rate Sensor States: %02X %02X \n", 
                     heartRate.mainState, heartRate.subState );
  
  if(hci_pckt->type != HCI_EVENT_PKT)
  {
    HRPROFILE_MESG_ERR (profiledbgfile,"HRProfile_Rx_Event_Handler(),Recevied packed is not an HCI Event: %02X !!\n", hci_pckt->type);
    return;
  }
  
  switch(event_pckt->evt)
  {
  case EVT_DISCONN_COMPLETE:
    {
      HRPROFILE_MESG_DBG(profiledbgfile, "HRProfile_Rx_Event_Handler(), Received event HCI_EVT_DISCONN_COMPLETE \n");
      if ( ( (HRProfile_Read_MainStateMachine() == HRPROFILE_STATE_ADVERTIZE) &&
            (HRProfile_Read_SubStateMachine() == HRPROFILE_STATE_PROCESSING_PAIRING) ) ||
          ( HRProfile_Read_MainStateMachine() == HRPROFILE_STATE_CONNECTED ) )
      {
        evt_disconn_complete *cc = (void *)event_pckt->data;
        if (cc->status == BLE_STATUS_SUCCESS)
        {
          HRPROFILE_MESG_DBG(profiledbgfile, "HRProfile_Rx_Event_Handler(), Move to  HRPROFILE_STATE_ADVERTIZE, HRPROFILE_STATE_ADVERTISABLE_IDLE  state,substate \n");
          HRProfile_Write_MainStateMachine(HRPROFILE_STATE_ADVERTIZE);
          HRProfile_Write_SubStateMachine(HRPROFILE_STATE_ADVERTISABLE_IDLE);
          
          heartRate.sensorContactSupport = 0;
          heartRate.hrmCommunicationStarted = 0;
          
          /* stop timer */
          Blue_NRG_Timer_Stop(heartRate.timerID); //TBR
          heartRate.timerID = 0xFF;
        }
      }
    }
    break;
    
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      switch(evt->subevent)
      {
      case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
          
          HRPROFILE_MESG_DBG( profiledbgfile, "HRProfile_Rx_Event_Handler(), Received event EVT_LE_CONN_COMPLETE %2x\n", cc->status);
          if (cc->status == BLE_STATUS_SUCCESS)
          {
            /* store connection handle */
            heartRate.connHandle = cc->handle;
                        
            /* if the main profile is in the connected state that means
            * we need not wait for pairing
            */ 
            
            /* stop timer */
            Blue_NRG_Timer_Stop(heartRate.timerID); //TBR
            heartRate.timerID = 0xFF;
            
            if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTED)
            {
            	HRProfile_Write_MainStateMachine(HRPROFILE_STATE_CONNECTED);
            	HRProfile_Write_SubStateMachine(HRPROFILE_STATE_CONNECTED_IDLE);
            }
            else
            {
            	HRProfile_Write_SubStateMachine(HRPROFILE_STATE_PROCESSING_PAIRING);
            }
          } 
          else
          {
            /* connection was not successful, now we need to wait for
             * the application to put the device in discoverable mode
             */
        	  HRProfile_Write_SubStateMachine(HRPROFILE_STATE_ADVERTISABLE_IDLE);
          }
        }
        break; /* HCI_EVT_LE_CONN_COMPLETE */
      } /* switch(response->leEvent.subEventCode) */
    }
    break; /* HCI_EVT_LE_META_EVENT */
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      
      switch(blue_evt->ecode)
      {
      case EVT_BLUE_GAP_PAIRING_CMPLT:
        {
          HRPROFILE_MESG_DBG( profiledbgfile, "HRProfile_Rx_Event_Handler(), Received event EVT_BLUE_GAP_PAIRING_CMPLT \n");
          if ((HRProfile_Read_MainStateMachine() == HRPROFILE_STATE_ADVERTIZE) &&
              (HRProfile_Read_SubStateMachine() == HRPROFILE_STATE_PROCESSING_PAIRING))
          {
            HRPROFILE_MESG_DBG( profiledbgfile, "EVT_BLUE_GAP_PAIRING_CMPLT success\n");
            HRProfile_Write_MainStateMachine(HRPROFILE_STATE_CONNECTED);
            HRProfile_Write_SubStateMachine(HRPROFILE_STATE_CONNECTED_IDLE);
          }
        }
        break; /* EVT_BLUE_GAP_PAIRING_CMPLT */
        
      case EVT_BLUE_GATT_WRITE_PERMIT_REQ:
        {
          HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Rx_Event_Handler(), Received event EVT_BLUE_GATT_WRITE_PERMIT_REQ \n");
          
          evt_gatt_write_permit_req * write_perm_req = (void*)blue_evt->data;
          attrHandle = write_perm_req->attr_handle;
          attValLen = write_perm_req->data_length;
          attValue =  write_perm_req->data[0]; 
          
          if ( (attrHandle == (heartRate.ctrlPointCharHandle + 1)) && 
              (HRProfile_Read_MainStateMachine() == HRPROFILE_STATE_CONNECTED) &&
                (HRProfile_Read_SubStateMachine() == HRPROFILE_STATE_CONNECTED_IDLE))
          {   
            HRPROFILE_MESG_DBG ( profiledbgfile, "Received write req to HRM Control Point Char. \n");
            if (attValue == HR_CNTL_POINT_RESET_ENERGY_EXPENDED)
            {
              /* received a correct value for HRM control point char */
              HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Rx_Event_Handler(), Received Reset Energy Expected Value Command \n");
              
              retval = aci_gatt_write_response(heartRate.connHandle,
                                               attrHandle,
                                               0x00, /* write_status = 0 (no error))*/
                                               //(uint8_t)HRM_CNTL_POINT_VALUE_IS_SUPPORTED, /* err_code */
                                               HRM_CNTL_POINT_VALUE_IS_SUPPORTED, /* err_code */
                                               attValLen,
                                               (uint8_t *)&attValue);
              
              /* Check retval value */
              if (retval != BLE_STATUS_SUCCESS)
              {
                HRPROFILE_MESG_ERR (profiledbgfile,"HRProfile_Rx_Event_Handler(),Failed aci_gatt_write_response: %02X !!\n", retval);
                return;
              }
              /* Notify to application to Reset The Energy Expended Value */
              heartRate.applicationNotifyFunc(EVT_HRS_RESET_ENERGY_EXPENDED,0,NULL); 
            }
            else
            {
              /* received value of HRM control point char is incorrect */
              HRPROFILE_MESG_ERR (profiledbgfile,"HRProfile_Rx_Event_Handler(), FAILED: Received not expected Reset Energy Expended Value Command,  Error: %02X !!\n", attValue);
              
              retval = aci_gatt_write_response(heartRate.connHandle,
                                               attrHandle,
                                               0x1, /* write_status = 1 (error))*/
                                               (uint8_t)HRM_CNTL_POINT_VALUE_NOT_SUPPORTED, /* err_code */
                                               attValLen,
                                               (uint8_t *)&attValue);
              /* Check retval value */
              if (retval != BLE_STATUS_SUCCESS)
              {
                HRPROFILE_MESG_ERR (profiledbgfile,"HRProfile_Rx_Event_Handler(),Failed aci_gatt_write_response: %02X !!\n", retval);
                return;
              } 
            }
          }
        }
        break;
      }
    }
    break; /* HCI_EVT_VENDOR_SPECIFIC */
  }
}/* end HRProfile_Rx_Event_Handler */

/**
* HRProfile_StateMachine
* 
* @param[in] None
* 
* HR profile's state machine: to be called on application main loop. 
*/ 
tBleStatus HRProfile_StateMachine(void)
{
  uint8_t retval = BLE_STATUS_SUCCESS;
  
  /* heartRate.mainState, heartRate.subState */
  switch (HRProfile_Read_MainStateMachine())
  {
  case HRPROFILE_STATE_UNINITIALIZED:
    break;
  case HRPROFILE_STATE_WAITING_BLE_INIT:
    
    if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE){
      
      /* Add HR service & characteristics */ 
      retval = HRProfile_Add_Services_Characteristics();
      if (retval == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Init(), Heart Rate Profile is Initialized with Service & Characteristics \n");
        
        /* vote to the main profile to go to advertise state */
        (void) BLE_Profile_Vote_For_Advertisable_State (&heartRate.HRStoBLEInf);
        BLE_Profile_Add_Advertisment_Service_UUID (HEART_RATE_SERVICE_UUID); 
        
        /* Initialize profile's state machine's state variables */
        HRProfile_Write_MainStateMachine(HRPROFILE_STATE_INITIALIZED);
        HRProfile_Write_SubStateMachine(HRPROFILE_STATE_INVALID);
      }
      else
      {
        HRPROFILE_MESG_ERR(profiledbgfile,"HRProfile_Init(), HRProfile_Add_Services_Characteristics FAILED to add Heart Rate Service, Error: %02X !!\n", retval);
        
        /* Notify to application that Heart Rate profile initialization has failed */
        heartRate.applicationNotifyFunc(EVT_HRS_INITIALIZED,1,&retval); 
        
        /* Change profile state to represent uninitialized profile */
        HRProfile_Write_MainStateMachine(HRPROFILE_STATE_UNINITIALIZED);
        HRProfile_Write_SubStateMachine(HRPROFILE_STATE_INVALID);
       
        /* unregister the profile from BLE registered profile list */
        BLE_Profile_Unregister_Profile ( &heartRate.HRStoBLEInf );
        /* Need to remove all added services and characteristics ??? */
      }
    }
    
    break;
    
  case HRPROFILE_STATE_INITIALIZED:
    {
      /* Heart Rate profile has completed its initialization process and voted the 
      * main profile for Advertisable state. Now its waiting for the main profile
      * to set to Advertisable state. */
      
      /* check main profile's state, if its in advertisable state, set the same for 
      * current profile as well */
      if ((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) && 
          (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_CONNECTABLE_IDLE))
      {
    	  HRProfile_Write_MainStateMachine(HRPROFILE_STATE_ADVERTIZE);
    	  HRProfile_Write_SubStateMachine(HRPROFILE_STATE_ADVERTISABLE_IDLE);
        
	heartRate.applicationNotifyFunc(EVT_HRS_INITIALIZED,1,&retval); 
      }
    }
    break; /* HRPROFILE_STATE_INITIALIZED */
    
  case HRPROFILE_STATE_ADVERTIZE:
    {
      switch (HRProfile_Read_SubStateMachine())
      {
      case HRPROFILE_STATE_ADVERTISABLE_IDLE:
        break;
        
      case HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST:
      break;
        
        /* This substate is set on timer timeout */
      case HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP:
        {
          HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_StateMachine(), STOPPING Advertising to White List devices \n");
          
          retval = aci_gap_set_non_discoverable(); 
          if ( retval == BLE_STATUS_SUCCESS)
          {
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
            
            /* change profile's sub states */
            HRProfile_Write_SubStateMachine(HRPROFILE_STATE_FAST_DISCOVERABLE_DO_START);
          }
        }
        break;
        
      case HRPROFILE_STATE_FAST_DISCOVERABLE_DO_START:
        {
          HRPROFILE_MESG_DBG ( profiledbgfile, "STARTING FAST General Discoverable Mode \n");
          
          retval = HR_Sensor_Set_Discoverable(HRP_FAST_CONN_ADV_INTERVAL_MIN,HRP_FAST_CONN_ADV_INTERVAL_MAX, NO_WHITE_LIST_USE);  /* no white list */
          if ( retval == BLE_STATUS_SUCCESS)
          {
            
            HRPROFILE_MESG_DBG ( profiledbgfile, "Device is now in FAST General Discoverable Mode \n");
            
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf, BLE_PROFILE_STATE_ADVERTISING);
            /* change profile's sub states */
            HRProfile_Write_SubStateMachine(HRPROFILE_STATE_FAST_DISCOVERABLE);
            HRPROFILE_MESG_DBG ( profiledbgfile, "main profile state has been set.  \n");
            
            /* start a timer of 30 seconds for first phase of discoverable mode (fast discoverable mode) */
            Blue_NRG_Timer_Start (30, HRProfile_Advertise_Period_Timeout_Handler,&heartRate.timerID);
          }
        }
        break;
        
      case HRPROFILE_STATE_FAST_DISCOVERABLE:
        break;
        
        /* This substate is set on timer timeout */
      case HRPROFILE_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP:
        {
          HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_StateMachine(), STOPPING FAST General Discoverable Mode \n");
          retval = aci_gap_set_non_discoverable(); 
          if (retval == BLE_STATUS_SUCCESS)
          {
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
            /* change profile's sub states */
            HRProfile_Write_SubStateMachine(HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_DO_START);
          }
        }
        break;
        
      case HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_DO_START:
        {
          HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_StateMachine(), STARTING Low Power General Discoverable Mode \n");
          
          retval = HR_Sensor_Set_Discoverable(HRP_LOW_PWR_ADV_INTERVAL_MIN,HRP_LOW_PWR_ADV_INTERVAL_MAX, NO_WHITE_LIST_USE);  /* no white list */
          if ( retval == BLE_STATUS_SUCCESS)
          {
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf,BLE_PROFILE_STATE_ADVERTISING);
            /* change profile's sub states */
            HRProfile_Write_SubStateMachine(HRPROFILE_STATE_LOW_POWER_DISCOVERABLE);
            /* start a timer of 60 seconds for second phase of discoverable mode (low power discoverable mode) */
            Blue_NRG_Timer_Start (60, HRProfile_Advertise_Period_Timeout_Handler,&heartRate.timerID);
          }
        }
        break;
        
      case HRPROFILE_STATE_LOW_POWER_DISCOVERABLE:
        break;
        
        /* This substate is set on timer timeout */
      case HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP:
        {
          HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_StateMachine(), STOPPING Low Power General Discoverable Mode \n");
          retval = aci_gap_set_non_discoverable(); 
          if ( retval == BLE_STATUS_SUCCESS)
          {
            /* change profile's sub states */
        	  HRProfile_Write_SubStateMachine(HRPROFILE_STATE_ADVERTISABLE_IDLE);
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf, BLE_PROFILE_STATE_CONNECTABLE_IDLE); 
            /* notify the application that the advertising has stopped */
            heartRate.applicationNotifyFunc(EVT_MP_ADVERTISING_TIMEOUT,0x00,NULL); 
          }
        }
        break;
        
      case HRPROFILE_STATE_BLE_ADVERTISING:
        break;
        
      case HRPROFILE_STATE_PROCESSING_PAIRING: 
        {
          /* check if any other profile has initiated the disconnect procedure
          * at BLE main profile. */
          if (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_DISCONNECTING) /* TBR: changed from BLE_PROFILE_STATE_CMD_DISCONNECT */
          {
        	  HRProfile_Write_MainStateMachine(HRPROFILE_STATE_CONNECTED);
        	  HRProfile_Write_SubStateMachine(HRPROFILE_STATE_DO_DISCONNECT);
          }
        }
        break;
      }/* switch (heartRate.subState) */
    }
    break; /* HRPROFILE_STATE_ADVERTIZE */
    
  case HRPROFILE_STATE_CONNECTED:
    {
      switch (heartRate.subState)
      {
      case HRPROFILE_STATE_CONNECTED_IDLE:
        break;/* HRPROFILE_STATE_CONNECTED_IDLE */
      case HRPROFILE_STATE_DO_DISCONNECT:
        break;
      case HRPROFILE_STATE_DISCONNECTING:
        break;
      case HRPROFILE_STATE_DISCONNECTED:
        break;
      }
    }
    break; /* HRPROFILE_STATE_CONNECTED */
  }  /* switch (heartRate.mainState) */
  
  return (retval);
}/* end HRProfile_StateMachine*/

/**
* HRProfile_Advertise_Period_Timeout_Handler
* 
* this function is called when the timer started
* by the heart rate profile for advertising times out
*/ 
void HRProfile_Advertise_Period_Timeout_Handler (void)
{
  tProfileState nextState = HRProfile_Read_SubStateMachine();
  HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Advertise_Period_Timeout_Handler(), Advertising Period Timedout \n");
  if (HRProfile_Read_MainStateMachine() == HRPROFILE_STATE_ADVERTIZE)
  {
    switch (HRProfile_Read_SubStateMachine())
    {
    case HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST:
      nextState = HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP;
      HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Advertise_Period_Timeout_Handler(), White list Advertising Timedout \n");
      break;
    case HRPROFILE_STATE_FAST_DISCOVERABLE:
      nextState = HRPROFILE_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP;
      HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Advertise_Period_Timeout_Handler(), Fast Advertising Timedout \n");
      break;
    case HRPROFILE_STATE_LOW_POWER_DISCOVERABLE:
      nextState = HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP;
      HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Advertise_Period_Timeout_Handler(),Low Power Advertising Timedout \n");
      break;
    }
    
    if (nextState != HRProfile_Read_SubStateMachine())
    {
    	HRProfile_Write_SubStateMachine(nextState);
    }
  }
}/* end HRProfile_Advertise_Period_Timeout_Handler() */
