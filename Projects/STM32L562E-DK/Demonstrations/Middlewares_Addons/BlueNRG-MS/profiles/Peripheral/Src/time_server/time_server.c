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
*   FILENAME        -  time_server.c
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      31/07/2014
*   $Revision$:  second version
*   $Author$:    
*   Comments:    Redesigned the Time Profile's implementation source file for server 
*                role according to unified BlueNRG DK framework.
*                
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* This file have Time Profile's Server role's implementation. 
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

#include <time_server.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* in terms of 625 micro seconds */
#define TIP_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define TIP_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define TIP_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define TIP_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */

/* Time server profile main states */
#define TIME_SERVER_STATE_UNINITIALIZED                 (0x00)
#define TIME_SERVER_STATE_WAITING_BLE_INIT              (0x10)
#define TIME_SERVER_STATE_INITIALIZED                   (0x20)
#define TIME_SERVER_STATE_ADVERTIZE                     (0x30)
#define TIME_SERVER_STATE_CONNECTED                     (0x40)
#define TIME_SERVER_STATE_INVALID                       (0xFF)

/* substates for TIME_SERVER_STATE_ADVERTIZE main state*/
#define TIME_SERVER_STATE_ADVERTISABLE_IDLE                            (0x31)
#define TIME_SERVER_STATE_ADVERTISING_FOR_WHITE_LIST                   (0x32)
#define TIME_SERVER_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP   (0x33)
#define TIME_SERVER_STATE_FAST_DISCOVERABLE_DO_START                   (0x34)
#define TIME_SERVER_STATE_FAST_DISCOVERABLE                            (0x35)
#define TIME_SERVER_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP            (0x36)
#define TIME_SERVER_STATE_LOW_POWER_DISCOVERABLE                       (0x37)
#define TIME_SERVER_STATE_BLE_ADVERTISING                              (0x38)
#define TIME_SERVER_STATE_PROCESSING_PAIRING                           (0x39)

/* substates under TIME_SERVER_STATE_CONNECTED main state */
#define TIME_SERVER_STATE_CONNECTED_IDLE                               (0x41)
#define TIME_SERVER_STATE_CONNECTED_BUSY                               (0x42)
#define TIME_SERVER_STATE_UPDATING_NEW_TIME_VALUE                      (0x43)
#define TIME_SERVER_STATE_DO_DISCONNECT                                (0x44)
#define TIME_SERVER_STATE_DISCONNECTING                                (0x45)
#define TIME_SERVER_STATE_DISCONNECTED                                 (0x46)

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )

/******************************************************************************
* Type Definitions
******************************************************************************/
typedef struct _tTimeServerContext
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
       * callback to notify the applciation with
       * the profile specific events
       */ 
  BLE_CALLBACK_FUNCTION_TYPE applicationNotifyFunc;

  /**
       * flag which is set at the time of initialization
       * by the application. It indicates whether the optional
       * services of Next DST Change and Reference Update Time 
       * are supported
       */ 
  uint8_t servicesSupported;
  
      /**
       * connection handle
       */
   uint16_t connHandle;

      /**
       * handle of current time service
       */ 
  uint16_t currentTimeServiceHandle;
      
      /**
       * handle of current time characteristic
       */ 
  uint16_t currentTimeCharHandle;
      
      /**
       * handle of local time characteristic
       */ 
  uint16_t localTimeInfoCharHandle;
      
      /**
       * handle of reference time characteristic
       */ 
  uint16_t referenceTimeInfoCharHandle;
      
      /**
       * handle of next DST change service
       */ 
  uint16_t nextDSTChangeServiceHandle;
      
      /**
       * handle of DST characteristic
       */ 
  uint16_t timeWithDSTCharHandle;
      
      /**
       * handle of Reference Time Update service
       */ 
  uint16_t referenceUpdateTimeServiceHandle;
      
      /**
       * handle of time update control point characteristic
       */ 
  uint16_t timeUpdateControlPointCharHandle;
      
      /**
       * handle of time update state characteristic
       */ 
  uint16_t timeUpdateStateCharHandle;
      
      /**
       * time update state
       */
  uint8_t timeUpdateState;

      /**
       * ID of the timer started for
       * advertising timeout or delay
       * handling
       */ 
  tTimerID timerID;
      
      /**
       * ID of the timer started for
       * the delay between current time updates
       */
   tTimerID delayTimerID; 
      /**
       * profile interface context to be registered
       * with the main profile
       */ 
  tProfileInterfaceContext TStoBLEInf;
      
      /**
       * this falg is used to send the allow
       * read command
       */ 
      uint8_t timeServerAllowRead;
	
}tTimeServerContext;

/******************************************************************************
 * Local Variable Declarations
******************************************************************************/
static tTimeServerContext timeServer;

static uint8_t ctCharNotifyDelayTimeout = 0;

static uint8_t ctCharNotificationConfig = 0;

tCurrentTime tsCurrentTimeValue = { 
  YEAR_IS_NOT_KNOWN, 
  MONTH_IS_NOT_KNOWN, 
  DATE_IS_NOT_KNOWN, 
  HOURS_MIN, 
  MINUTES_MIN, 
  SECONDS_MIN, 
  DAY_IS_NOT_KNOWN, 
  FRACTION256_MIN, 
  ADJUST_REASON_NO_REASON };

tLocalTimeInformation tsLocalTimeInfoValue = { TIME_ZONE_OFFSET_NOT_KNOWN, 
                                               DST_OFFSET_NOT_KNOWN };

tReferenceTimeInformation tsRefTimeInfoValue = { TIME_REFERENCE_UNKNOWN, 
                                                 TIME_ACCURACY_UNKNOWN, 
                                                 DAYS_SINCE_UPDATE_IS_255_OR_MORE, 
                                                 HOURS_SINCE_UPDATE_IS_255_OR_MORE };

tTimeWithDST tsNextDSTChangeTime = { YEAR_IS_NOT_KNOWN, 
                                     MONTH_IS_NOT_KNOWN, 
                                     DATE_IS_NOT_KNOWN, 
                                     HOURS_MIN, 
                                     MINUTES_MIN, 
                                     SECONDS_MIN, 
                                     DST_OFFSET_NOT_KNOWN };

tTimeUpdateStatus refTimeUpdateStatus = {TIME_UPDATE_STATUS_IDLE, 
					 TIME_UPDATE_RESULT_NOT_ATTEMPTED_AFTER_RESET};


/******************************************************************************
 * Global Variable Declarations
******************************************************************************/

/******************************************************************************
 * Imported Variable
******************************************************************************/
extern void *profiledbgfile;
extern uint8_t bnrg_expansion_board;

/******************************************************************************
 * Function Declarations
******************************************************************************/

static tBleStatus TimeServer_Add_Services_Characteristics(void);
static tBleStatus TimeServer_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax);
static void TimeServer_Exit(uint8_t errCode);
static void TimeServer_Notify_Update(uint16_t servHandle,uint16_t charHandle,tBleStatus retval);
static void TimeServer_Advertise_Period_Timeout_Handler(void);
static void TimeServer_CT_Char_Notify_Delay_Timeout_Handler(void);

static void TSProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState TSProfile_Read_MainStateMachine(void);
static void TSProfile_Write_SubStateMachine(tProfileState localsubState);
static tProfileState TSProfile_Read_SubStateMachine(void);

/******************************************************************************
* SAP
******************************************************************************/
tBleStatus TimeServer_Init(uint8_t servicesToBeSupported, 
                           BLE_CALLBACK_FUNCTION_TYPE timeServerCallbackFunc)
{
  tBleStatus retval = BLE_STATUS_FAILED;

  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Init()\n" );

  if (Is_Profile_Present(PID_TIME_SERVER) == BLE_STATUS_SUCCESS)
  {
    return (BLE_STATUS_PROFILE_ALREADY_INITIALIZED);
  }
      
  if (timeServerCallbackFunc == NULL)
  {
    return (BLE_STATUS_NULL_PARAM);
  }

  /* Initialize Time Server Profile's Data Structures */
  BLUENRG_memset(&timeServer,0,sizeof(tTimeServerContext));
  timeServer.servicesSupported = servicesToBeSupported;
  timeServer.applicationNotifyFunc = timeServerCallbackFunc;

  /* Initialize Time Server Profile to BLE main Profile Interface data structure */
  BLUENRG_memset ( &timeServer.TStoBLEInf, 0, sizeof(tProfileInterfaceContext) );

  timeServer.TStoBLEInf.profileID = PID_TIME_SERVER;
  timeServer.TStoBLEInf.callback_func = TimeServer_Event_Handler;
  timeServer.TStoBLEInf.voteForAdvertisableState = 0;

  retval = BLE_Profile_Register_Profile ( &timeServer.TStoBLEInf );
  if (retval != BLE_STATUS_SUCCESS)
  {
    /* Profile could not be registered with the BLE main profile */
    return (retval);
  }

  /* Initialize profile's state machine's state variables */
  TSProfile_Write_MainStateMachine(TIME_SERVER_STATE_WAITING_BLE_INIT);
  TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_WAITING_BLE_INIT);

  /* Initialize the timer ID   */
  timeServer.delayTimerID = 0xFF;

  TIME_SERVER_MESG_DBG( profiledbgfile, "TimeServer_Init(), Time Server Profile is Initialized \n");
  
  return (BLE_STATUS_SUCCESS);
}/* end TimeServer_Init() */

tBleStatus TimeServer_Update_Current_Time_Value(tCurrentTime timeValue)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  uint8_t canDoUpdateNow = 0;
  
  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(), Received a new Time Value %x %x (minutes) %d\n",TSProfile_Read_MainStateMachine(),TSProfile_Read_SubStateMachine(),timeValue.minutes);

  if ( (TSProfile_Read_MainStateMachine() == TIME_SERVER_STATE_CONNECTED)  &&  
       ( (TSProfile_Read_SubStateMachine() == TIME_SERVER_STATE_CONNECTED_IDLE) ||  //TBR
         (TSProfile_Read_SubStateMachine() == TIME_SERVER_STATE_CONNECTED_BUSY) ) )
  {
    if ( ( ( (timeValue.year < YEAR_MIN) || (timeValue.year > YEAR_MAX) ) && (timeValue.year != YEAR_IS_NOT_KNOWN) ) || 
         ( timeValue.month > DECEMBER ) ||
         ( timeValue.date > DATE_MAX ) ||
         ( timeValue.hours > HOURS_MAX ) ||
         ( timeValue.minutes > MINUTES_MAX ) ||
         ( timeValue.seconds > SECONDS_MAX ) ||
         ( timeValue.day_of_week > SUNDAY ) ||
         ( !( (timeValue.adjustReason == ADJUST_REASON_NO_REASON) || 
              (timeValue.adjustReason == ADJUST_REASON_MANUAL_TIME_UPDATE) || 
              (timeValue.adjustReason == ADJUST_REASON_EXTERNAL_REFERENCE_TIME_UPDATE) || 
              (timeValue.adjustReason == ADJUST_REASON_CHANGE_OF_TIME_ZONE) || 
              (timeValue.adjustReason == ADJUST_REASON_CHANGE_OF_DST) ) ) )
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(), Received Time Value not valid!\n");
      return (retval);
    }

    canDoUpdateNow = 1;
    /* check whether the current time char can be updated immediately or not */
    if (timeValue.adjustReason == ADJUST_REASON_EXTERNAL_REFERENCE_TIME_UPDATE)
    {
      /* check whether char notification is enabled or not */
      if (ctCharNotificationConfig == 0x01)
      {
          /*if (delta_between_last_update_and_new_value < one_minutes)
                           * TODO : this checking is incorrect. It does not handle cases
                           * of rollover, Eg: the old time is dec 31 11:59:56 and the
                           * new time is jan 1 00:00:01. This update should not be
                           * allowed but with the current checking it will be allowed
                           */
          if ( (tsCurrentTimeValue.day_of_week == timeValue.day_of_week) && 
               (tsCurrentTimeValue.year == timeValue.year) && 
               (tsCurrentTimeValue.month == timeValue.month) && 
               (tsCurrentTimeValue.date == timeValue.date) && 
               (tsCurrentTimeValue.hours == timeValue.hours) && 
               (tsCurrentTimeValue.minutes == timeValue.minutes) )
          {
            if (ctCharNotifyDelayTimeout == 0x00)
            {
              /* time client should not be notified for this new time value, 
               * hence keep this new value with profile's local buffer */
              /* save the new time value into local buffer */
              canDoUpdateNow = 0;
              retval = BLE_STATUS_SUCCESS;
              TIME_SERVER_MESG_DBG ( profiledbgfile, "-- time_delay_of_last_notification < fifteen_minutes: Time client should not be notified for new value (< 1 minute)\n");
            }
            else
            {
              /* update CT char and notify to client immediately if last notification for 
               * current time char update was sent more than 15 minutes back */
              canDoUpdateNow = 1;
              TIME_SERVER_MESG_DBG ( profiledbgfile, "--- time_delay_of_last_notification > fifteen_minutes: Time client can notify new value ( < 1 minute)\n");
            }
          }
          else 
          {
            /* update CT char and notify to client immediately if the new time value differ from 
             * the last updated value by more then 1 minutes */
            TIME_SERVER_MESG_DBG ( profiledbgfile, "---- time client can notify new value ( > 1 minute)\n");
            canDoUpdateNow = 1;
          }
      }
      else
      {
        TIME_SERVER_MESG_DBG ( profiledbgfile, "----- notification is not enabled by the client: do update \n");
        /* Update CT char immediately if notification is not enabled by the client */
        canDoUpdateNow = 1;
      }
    }
    else
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "------ timeValue.adjustReason != ADJUST_REASON_EXTERNAL_REFERENCE_TIME_UPDATE: do update \n");
      /* Update CT char and notify to client immediately if the adjust reason is any 
       * other than external reference time update */
      canDoUpdateNow = 1;
    }

    /* current time char can not be updated immediately, hence return */
    if (canDoUpdateNow == 0)
    {
      return (retval);
    }

    TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(),New Current Time value is ready to send %d\n",timeValue.minutes);
    
    BLUENRG_memcpy(&tsCurrentTimeValue,&timeValue,sizeof(tCurrentTime));
    retval =  aci_gatt_update_char_value(timeServer.currentTimeServiceHandle, 
                                         timeServer.currentTimeCharHandle,
                                         0, /* charValOffset */
                                         10, /* charValueLen */
                                         (uint8_t *) &tsCurrentTimeValue);  
    if (retval == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(),Current Time value has been sent for update (minutes: %d)\n", timeValue.minutes);
     
      /* use a timer to track delay between successive current time updates with notification */
      if (timeServer.delayTimerID == 0xFF)
      {
        Blue_NRG_Timer_Start (60*15, TimeServer_CT_Char_Notify_Delay_Timeout_Handler,&timeServer.delayTimerID);
        TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(),Start 15 minutes timer (timerID: %4X)\n", timeServer.delayTimerID); //TBR
      }
      else
      {
        /* restart the timer */
        Blue_NRG_Timer_Stop(timeServer.delayTimerID);
        timeServer.delayTimerID = 0xFF;
        
        Blue_NRG_Timer_Start (60*15, TimeServer_CT_Char_Notify_Delay_Timeout_Handler,&timeServer.delayTimerID);
        TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(),Stop delayTimerID, ReStart 15 minutes timer (timerID: %4X)\n", timeServer.delayTimerID); //TBR
      }
      ctCharNotifyDelayTimeout = 0;
    }
    else
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(),FAILED to send Current Time value %02X \n", retval);
    }
    
    /* Notify main profile of service, characteristic update & result */
    TimeServer_Notify_Update(timeServer.currentTimeServiceHandle,timeServer.currentTimeCharHandle, retval);
  }
  else
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(), Time Server Profile is not in proper state to send New Time value \n");
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
}/* end TimeServer_Update_Current_Time_Value() */


tBleStatus TimeServer_Update_Local_Time_Information ( tLocalTimeInformation localTimeInfo )
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Local_Time_Information(), Received Time Server's Local Time Information  \n");

  if(TSProfile_Read_MainStateMachine() == TIME_SERVER_STATE_CONNECTED)  
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "Local Time Information is ready to update \n");
 
    retval =  aci_gatt_update_char_value(timeServer.currentTimeServiceHandle, 
                                         timeServer.localTimeInfoCharHandle,
                                         0, /* charValOffset */
                                         2, /* charValueLen */
                                         (uint8_t *) &localTimeInfo);                                                                                 
    if (retval == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Local_Time_Information(),Local Time Information has sent successfully \n");
    }
    else
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Local_Time_Information(),FAILED to send Local Time Information %02X \n", retval);
    }
    /* Notify main profile of service, characteristic update & result */
    TimeServer_Notify_Update(timeServer.currentTimeServiceHandle,timeServer.localTimeInfoCharHandle, retval);
  }
  else
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Local_Time_Information(),Time Server Profile is not in proper state to update local time information \n");
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
}/* end TimeServer_Update_Local_Time_Information() */


tBleStatus TimeServer_Update_Reference_Time_Information(tReferenceTimeInformation refTimeInfo)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  
  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Reference_Time_Information(), Received Time Server's Reference Time Information  \n");

  /* validate the parameters */
  if(TSProfile_Read_MainStateMachine() == TIME_SERVER_STATE_CONNECTED) 
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "Reference Time Information is ready to update \n");

    retval =  aci_gatt_update_char_value(timeServer.currentTimeServiceHandle, 
                                         timeServer.referenceTimeInfoCharHandle,
                                         0, /* charValOffset */
                                         4, /* charValueLen */
                                         (uint8_t *) &refTimeInfo);   
    
    if (retval == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Reference_Time_Information(), Reference Time Information has sent successfully \n");
    }
    else
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Reference_Time_Information(), FAILED to update Reference Time Information %02X \n", retval);
    }
    
    /* Notify main profile of service, characteristic update & result */
    TimeServer_Notify_Update(timeServer.currentTimeServiceHandle,timeServer.referenceTimeInfoCharHandle, retval);
  }
  else
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Reference_Time_Information(), Time Server is not in proper state to update reference time information \n");
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
    
}/* end TimeServer_Update_Reference_Time_Information() */


tBleStatus TimeServer_Update_Next_DST_Change(tTimeWithDST timeDST)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  
  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Next_DST_Change(), Received Next DST Change Time \n");

  if(TSProfile_Read_MainStateMachine() == TIME_SERVER_STATE_CONNECTED)  
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Next_DST_Change(), Next DST Change Time is ready to update \n");

    retval =  aci_gatt_update_char_value(timeServer.nextDSTChangeServiceHandle, 
                                         timeServer.timeWithDSTCharHandle,
                                         0, /* charValOffset */
                                         8, /* charValueLen */
                                         (uint8_t *) &timeDST);   
    
    if (retval == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Next_DST_Change(), Next DST Change Time has sent successfully \n");
    }
    else
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Next_DST_Change(), FAILED to update Next DST Change Time %02X \n", retval);
    }
    /* Notify main profile of service, characteristic update & result */
    TimeServer_Notify_Update(timeServer.nextDSTChangeServiceHandle,timeServer.timeWithDSTCharHandle, retval);
  }
  else
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Next_DST_Change(), Time Server is not in proper state to update Next DST Change Time \n");
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
    
}/* end TimeServer_Update_Next_DST_Change() */


/**
 * RefTime_Update_Timeout_Handler
 * 
 * timeout handler for handling the
 * reference time update timeout
 */ 
void RefTime_Update_Timeout_Handler(void)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  
  TIME_SERVER_MESG_DBG(profiledbgfile, "RefTime_Update_Timeout_Handler() \n");
  
  /* a timeout has happened
   * so set the time update state to idle
   * and result to timeout
   */ 
  if(TSProfile_Read_SubStateMachine() == TIME_SERVER_STATE_UPDATING_NEW_TIME_VALUE)
  {
    refTimeUpdateStatus.currentStatus = TIME_UPDATE_STATUS_IDLE;
    refTimeUpdateStatus.result = TIME_UPDATE_RESULT_TIMEOUT;
    
    retval =  aci_gatt_update_char_value(timeServer.referenceUpdateTimeServiceHandle, 
                                         timeServer.timeUpdateStateCharHandle,
                                         0, /* charValOffset */
                                         2, /* charValueLen */
                                         (uint8_t *) &refTimeUpdateStatus);   
    if (retval == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "RefTime_Update_Timeout_Handler(), Time Update Status has sent successfully \n");
    }
    else
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "RefTime_Update_Timeout_Handler(), FAILED to update NTime Update Status %02X \n", retval);
    }
    /* notify the application to stop the update */
    timeServer.applicationNotifyFunc(EVT_TS_STOP_REFTIME_UPDATE,1,&refTimeUpdateStatus.result);
    
    /* reset the time server substate */
    TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_CONNECTED_IDLE);
  }
}/* end RefTime_Update_Timeout_Handler() */


void Update_Reference_Time(uint8_t errCode,tCurrentTime* newTime) /* TBR : It seems not used */
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  
  TIME_SERVER_MESG_DBG ( profiledbgfile, "Update_Reference_Time() \n");
  
  if(TSProfile_Read_SubStateMachine() == TIME_SERVER_STATE_UPDATING_NEW_TIME_VALUE)
  {
    if((errCode != TIME_UPDATE_RESULT_SUCCESSFUL) &&
       (errCode != TIME_UPDATE_RESULT_NO_CONNECTION_TO_REFERENCE) &&
       (errCode != TIME_UPDATE_RESULT_REFERENCE_RESPONDED_WITH_ERROR))
    {
      /* invalid error code */
      return;
    }
    
    /* update the time update state characteristic */
    refTimeUpdateStatus.currentStatus = TIME_UPDATE_STATUS_IDLE;
    refTimeUpdateStatus.result = errCode;
    
    retval =  aci_gatt_update_char_value(timeServer.referenceUpdateTimeServiceHandle, 
                                         timeServer.timeUpdateStateCharHandle,
                                         0, /* charValOffset */
                                         2, /* charValueLen */
                                         (uint8_t *) &refTimeUpdateStatus);   
    if (retval == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "Update_Reference_Time(), Reference Time Update Status has sent successfully \n");
    }
    else
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "Update_Reference_Time(), FAILED to update Reference Time Status %02X \n", retval);
    }
    
    if(errCode == TIME_UPDATE_RESULT_SUCCESSFUL)
    {
      /* TODO if the reference time update was successful, update
       * the current time characteristic
       */ 
    }
  }
}/* end Update_Reference_Time() */

/******************************************************************************
* Local Functions
******************************************************************************/
static void TSProfile_Write_MainStateMachine(tProfileState localmainState)
{
  timeServer.mainState = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState TSProfile_Read_MainStateMachine(void)
{
  return(timeServer.mainState);
}

static void TSProfile_Write_SubStateMachine(tProfileState localsubState)
{
  timeServer.subState = localsubState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState TSProfile_Read_SubStateMachine(void)
{
  return(timeServer.subState);
}

/*
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
static void TSProfile_Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  tTimeUpdateControlPoint attValue;
  //uint8_t attValLen;
  uint16_t attrHandle = 0;
  tBleStatus retval = BLE_STATUS_SUCCESS;
  
  attrHandle = handle;
  //attValue = *(tTimeUpdateControlPoint*) att_data; 
  
  TIME_SERVER_MESG_DBG(profiledbgfile, "---- TimeServer_Event_Handler(),EVT_BLUE_GATT_ATTRIBUTE_MODIFIED, attrHandle =  %02X \n", attrHandle); 
  
  /* check for valid attribute handle, attribute values, profile's states, etc */
  if (attrHandle == (timeServer.timeUpdateControlPointCharHandle + 1))
  {
    attValue = *(tTimeUpdateControlPoint*) att_data; 
    
    if ( (TSProfile_Read_MainStateMachine() == TIME_SERVER_STATE_CONNECTED) && 
        (TSProfile_Read_SubStateMachine() != TIME_SERVER_STATE_DO_DISCONNECT) && 
          (TSProfile_Read_SubStateMachine() != TIME_SERVER_STATE_DISCONNECTING) )
    {
      if (attValue == GET_REFERENCE_UPDATE)
      {
        retval = BLE_STATUS_SUCCESS;
        /* start the referrence time update state machine */
        timeServer.applicationNotifyFunc(EVT_TS_START_REFTIME_UPDATE,1,&retval);
        /* update the time update state in the server */
        refTimeUpdateStatus.currentStatus = TIME_UPDATE_STATUS_UPDATE_PENDING;
        
        retval =  aci_gatt_update_char_value(timeServer.referenceUpdateTimeServiceHandle, 
                                             timeServer.timeUpdateStateCharHandle,
                                             0, /* charValOffset */
                                             2, /* charValueLen */
                                             (uint8_t *) &refTimeUpdateStatus);  
        
        if (retval == BLE_STATUS_SUCCESS)
        {
          TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Event_Handler(),Time Update Status has sent successfully \n");
        }
        else
        {
          TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Event_Handler(),FAILED to update NTime Update Status %02X \n", retval);
        }
        /* start the timeout timer */
        Blue_NRG_Timer_Start(REFERENCE_UPDATE_TIMEOUT,RefTime_Update_Timeout_Handler,&timeServer.timerID);
        TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_UPDATING_NEW_TIME_VALUE);
      }
      else if (attValue == CANCEL_REFERENCE_UPDATE)
      {
        /* stop the reference time update state machine */
        retval = BLE_STATUS_SUCCESS;
        timeServer.applicationNotifyFunc(EVT_TS_STOP_REFTIME_UPDATE,1,&retval);
        
        /* update the time update state in the server */
        refTimeUpdateStatus.currentStatus = TIME_UPDATE_STATUS_IDLE;
        refTimeUpdateStatus.result = TIME_UPDATE_RESULT_CANCELED;
        
        retval =  aci_gatt_update_char_value(timeServer.referenceUpdateTimeServiceHandle, 
                                             timeServer.timeUpdateStateCharHandle,
                                             0, /* charValOffset */
                                             2, /* charValueLen */
                                             (uint8_t *) &refTimeUpdateStatus);  
        if (retval == BLE_STATUS_SUCCESS)
        {
          TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Event_Handler(),Time Update Status has sent successfully \n");
        }
        else
        {
          TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Event_Handler(),FAILED to update NTime Update Status %02X \n", retval);
        }
        if(TSProfile_Read_SubStateMachine() == TIME_SERVER_STATE_UPDATING_NEW_TIME_VALUE)
        {
          /* stop the running timer */
          Blue_NRG_Timer_Stop(timeServer.timerID);
          timeServer.timerID = 0xFF;
          TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_CONNECTED_IDLE);
        }
      }
    }
  }
  /* TBR: new code (ctCharNotificationConfig should be set to 1 if the Current Time Configuration Descriptor has been configured for notification) */
  else if ((attrHandle == (timeServer.currentTimeCharHandle + 2)) && (att_data[0] == 0x01)) /* : Notify value:0x01 */
  {
    /* Current Time Configuration Descriptor has been configured for notification */
    ctCharNotificationConfig = 0x01;
    TIME_SERVER_MESG_DBG ( profiledbgfile, "attrHandle %02X, attrValue %02X\n", attrHandle, att_data[0]);
  }
  else if ((attrHandle == (timeServer.currentTimeCharHandle + 2)) && (att_data[0] == 0x00))  
  {
    /* Current Time Configuration Descriptor has been set to 0 (no notification) */
    TIME_SERVER_MESG_DBG ( profiledbgfile, "attrHandle %02X, attrValue %02X\n", attrHandle, att_data[0]);
    ctCharNotificationConfig = 0x00;
  }
  /* TBR: end new code */
}

/* TimeServer_Set_Discoverable() */
tBleStatus TimeServer_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  const char localName[] = {0x08,'T','S','e','r','v','e','r'};
  
  retval = aci_gap_set_limited_discoverable(ADV_IND,
                                            AdvIntervMin, 
                                            AdvIntervMax,
                                            PUBLIC_ADDR, 
                                            NO_WHITE_LIST_USE, 
                                            8, 
                                            localName, 
                                            gBLEProfileContext.advtServUUIDlen, 
                                            gBLEProfileContext.advtServUUID,    
                                            0, 
                                            0);

  return retval;
}/* end TimeServer_Set_Discoverable() */


/**
 * TimeServer_Add_Services_Characteristics
 * 
* @param[in]: None
 *            
 * It adds Health Thermometer service & related characteristics
 */ 
tBleStatus TimeServer_Add_Services_Characteristics(void)
{ 
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;
  uint16_t uuid = 0;
  uint8_t serviceMaxAttributeRecord = 0;
  
  /* ***************  Add Current Time Service *************************/ 
  TIME_SERVER_MESG_DBG( profiledbgfile, "TimeServer_Add_Services_Characteristics(), Adding Current Time Service \n");

  /* serviceMaxAttributeRecord = 1 for current time service itself +
   *                             2 for current time characteristic +
   *                             1 for current time client char configuraton descriptor +
   *                             2 for local time information characteristic +
   *                             2 for reference time information characteristic
   */
  serviceMaxAttributeRecord = 8;

  /* store the Current Time Service UUID */
  uuid = CURRENT_TIME_SERVICE_UUID;

  /* Add Health Thermometer Service */
  hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                  (const uint8_t *) &uuid, 
                                   PRIMARY_SERVICE, 
                                   serviceMaxAttributeRecord,
                                   &(timeServer.currentTimeServiceHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Current Time Service is added Successfully %04X\n", timeServer.currentTimeServiceHandle);
    
  }
  else
  {
    TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to Current Time Service: Error: %02X !!\n", hciCmdResult);
    TimeServer_Exit(hciCmdResult);
    return hciCmdResult;
  }  
  /* Add Current Time Characteristic */
  TIME_SERVER_MESG_DBG (profiledbgfile,"TimeServer_Add_Services_Characteristics(), Adding Current Time Characteristic. \n");
  
  uuid = CURRENT_TIME_CHAR_UUID; 
  hciCmdResult = aci_gatt_add_char(timeServer.currentTimeServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &uuid , 
                                   10, 
                                   (CHAR_PROP_READ |CHAR_PROP_NOTIFY), 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, /* this is required for controled notification */
                                   10,  /* encryKeySize */
                                   CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                   &(timeServer.currentTimeCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(),Current Time Characteristic Added Successfully %04X\n", timeServer.currentTimeCharHandle);

  }
  else
  {
    TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Current Time Characteristic: Error: %02X !!\n", hciCmdResult);
    TimeServer_Exit(hciCmdResult);
    return hciCmdResult;
  }  

  /* Add Local Time Information Characteristic */
  TIME_SERVER_MESG_DBG (profiledbgfile,"TimeServer_Add_Services_Characteristics(), Adding Local Time Information Characteristic. \n");

  uuid = LOCAL_TIME_INFORMATION_CHAR_UUID; 
  hciCmdResult = aci_gatt_add_char(timeServer.currentTimeServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &uuid , 
                                   2, 
                                   CHAR_PROP_READ, 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                   10, /* encryKeySize */
                                   CHAR_VALUE_LEN_CONSTANT, /* isConstant */
                                   &(timeServer.localTimeInfoCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Local Time Information Characteristic Added Successfully %04X\n", timeServer.localTimeInfoCharHandle);
  }
  else
  {
    TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Local Time Information Characteristic: Error: %02X !!\n", hciCmdResult);
    TimeServer_Exit(hciCmdResult);
    return hciCmdResult;
  }
 
  /* Add Reference Time Information Characteristic */
  TIME_SERVER_MESG_DBG (profiledbgfile,"TimeServer_Add_Services_Characteristics(), Adding Reference Time Information Characteristic. \n");

  uuid = REFERENCE_TIME_INFORMATION_CHAR_UUID; 
  hciCmdResult = aci_gatt_add_char(timeServer.currentTimeServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &uuid , 
                                   4, /* bytes */ 
                                   CHAR_PROP_READ, 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                   10, /* encryKeySize */
                                   CHAR_VALUE_LEN_CONSTANT, /* isConstant */
                                   &(timeServer.referenceTimeInfoCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    TIME_SERVER_MESG_DBG (profiledbgfile, "Reference Time Information Characteristic Added Successfully %04X\n", timeServer.referenceTimeInfoCharHandle);
  }
  else
  {
    TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Reference Time Information Characteristic: Error: %02X !!\n", hciCmdResult);
    TimeServer_Exit(hciCmdResult);
    return hciCmdResult;
  }                     
       
  /* ***************  Add Next DST Change Service *************************/                           
  if (timeServer.servicesSupported & NEXT_DST_CHANGE_SERVICE_BITMASK)
  {
    TIME_SERVER_MESG_DBG (profiledbgfile,"Adding Next DST Change Service. \n");

    /* serviceMaxAttributeRecord = 1 for Next DST Change service itself +
     *                             2 for time with DST characteristic
     */
    serviceMaxAttributeRecord = 3;

    uuid = NEXT_DST_CHANGE_SERVICE_UUID;
    hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                     (const uint8_t *) &uuid, 
                                     PRIMARY_SERVICE, 
                                     serviceMaxAttributeRecord,
                                     &(timeServer.nextDSTChangeServiceHandle));
    
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Next DST Change Service is added Successfully %04X\n", timeServer.nextDSTChangeServiceHandle);
    }
    else
    {
      TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Next DST Change Service: Error: %02X !!\n", hciCmdResult);
      TimeServer_Exit(hciCmdResult);
      return hciCmdResult;
    }
   
    /* Add Time with DST Characteristic */
    TIME_SERVER_MESG_DBG (profiledbgfile,"TimeServer_Add_Services_Characteristics(), Adding Time with DST Characteristic. \n");
    
    uuid = TIME_WITH_DST_CHAR_UUID;
    hciCmdResult = aci_gatt_add_char(timeServer.nextDSTChangeServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &uuid , 
                                     8, /* bytes */
                                     CHAR_PROP_READ, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                     10, /* encryKeySize */
                                     CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                     &(timeServer.timeWithDSTCharHandle));
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
        TIME_SERVER_MESG_DBG (profiledbgfile, "Time with DST Characteristic Added Successfully %04X\n", timeServer.timeWithDSTCharHandle);
    }
    else
    {
      TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Time with DST Characteristic: Error: %02X !!\n",hciCmdResult);
      TimeServer_Exit(hciCmdResult);
      return hciCmdResult;
    }  
  }/* if timeServer.servicesSupported & NEXT_DST_CHANGE_SERVICE_BITMASK */
      
  /* ***************  Add Reference Update TimeChange Service *************************/         
               
  if (timeServer.servicesSupported & REFERENCE_TIME_UPDATE_SERVICE_BITMASK) 
  {
    TIME_SERVER_MESG_DBG (profiledbgfile,"Adding Reference Update Time Service. \n");

    /* serviceMaxAttributeRecord = 1 for Reference update time service itself +
     *                             2 for time update status characteristic + 
     *                             2 for time update control point characteristic
     */
    serviceMaxAttributeRecord = 5;

    uuid = REFERENCE_UPDATE_TIME_SERVICE_UUID;
    hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                  (const uint8_t *) &uuid, 
                                   PRIMARY_SERVICE, 
                                   serviceMaxAttributeRecord,
                                   &(timeServer.referenceUpdateTimeServiceHandle));
    
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Reference Update Time Service is added Successfully %04X\n", timeServer.referenceUpdateTimeServiceHandle);
    }
    else
    {
      TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Reference Update Time Service: Error: %02X !!\n", hciCmdResult);
      TimeServer_Exit(hciCmdResult);
      return hciCmdResult;
    }
               
    /* Add Time Update Controle Point Characteristic */
    TIME_SERVER_MESG_DBG (profiledbgfile,"TimeServer_Add_Services_Characteristics(), Adding Time Update Controle Point Characteristic. \n");
    
    uuid = TIME_UPDATE_CONTROL_POINT_CHAR_UUID;    
    hciCmdResult = aci_gatt_add_char(timeServer.referenceUpdateTimeServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &uuid , 
                                     1, /* byte */
                                     CHAR_PROP_WRITE_WITHOUT_RESP, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                     10,  /* encryKeySize */
                                     CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                     &(timeServer.timeUpdateControlPointCharHandle));
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Time Update Controle Point Characteristic Added Successfully %04X\n", timeServer.timeUpdateControlPointCharHandle);
    }
    else
    {
      TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Time Update Controle Point Characteristic: Error: %02X !!\n", hciCmdResult);
      TimeServer_Exit(hciCmdResult);
      return hciCmdResult;
    }   
                
    /* Add Time Update State Characteristic */
    TIME_SERVER_MESG_DBG (profiledbgfile,"Adding Time Update State Characteristic. \n");
    
    uuid = TIME_UPDATE_STATE_CHAR_UUID;
    hciCmdResult = aci_gatt_add_char(timeServer.referenceUpdateTimeServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &uuid , 
                                     2, /* bytes */
                                     CHAR_PROP_READ, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                     10, /* encryKeySize */
                                     CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                     &(timeServer.timeUpdateStateCharHandle));
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Time Update State Characteristic Added Successfully %04X\n", timeServer.timeUpdateStateCharHandle);
    }
    else
    {
      TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Time Update State Characteristic: Error: %02X !!\n", hciCmdResult);
      TimeServer_Exit(hciCmdResult);
      return hciCmdResult;
    } 
  }               
  return hciCmdResult;
}/* end TimeServer_Add_Services_Characteristics() */


/**
 * TimeServer_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 * 
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB() 
 */ 
void TimeServer_Event_Handler(void *pckt)
{
  //tTimeUpdateControlPoint attValue;
  //uint8_t attValLen;
  //tBleStatus retval = BLE_STATUS_SUCCESS;
  uint16_t attrHandle = 0;

  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Event_Handler: Main State %02X, SubState %02X \n", 
                           TSProfile_Read_MainStateMachine(), TSProfile_Read_SubStateMachine() );
  
  if(hci_pckt->type != HCI_EVENT_PKT)
  {
    TIME_SERVER_MESG_DBG (profiledbgfile,"TimeServer_Event_Handler(),Received packed is not an HCI Event: %02X !!\n", hci_pckt->type);
    return;
  }
  
  switch(event_pckt->evt)
  {
    case EVT_DISCONN_COMPLETE:
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Event_Handler(), Received event HCI_EVT_DISCONN_COMPLETE \n");
      if (TSProfile_Read_MainStateMachine() == TIME_SERVER_STATE_CONNECTED)
      {
       evt_disconn_complete *cc = (void *)event_pckt->data;
       
       TIME_SERVER_MESG_DBG( profiledbgfile,"TimeServer_Event_Handler(), Received an EVT_DISCONN_COMPLETE %02X \n",cc->status);
       if (cc->status == BLE_STATUS_SUCCESS)
       {
        /* Change Profile's State */
        TSProfile_Write_MainStateMachine(TIME_SERVER_STATE_ADVERTIZE);
        TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_ADVERTISABLE_IDLE);
       }
      }
    }
    break;
      
    case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      TIME_SERVER_MESG_DBG( profiledbgfile,"TimeServer_Event_Handler(), Received an EVT_LE_META_EVENT %04X \n",evt->subevent);
      
      switch(evt->subevent)
      {
        case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
          TIME_SERVER_MESG_DBG( profiledbgfile, "TimeServer_Event_Handler(), Received event EVT_LE_CONN_COMPLETE %2x\n", cc->status);
          
          if (cc->status == BLE_STATUS_SUCCESS)
          {
            /* store the connection handle */
            timeServer.connHandle = cc->handle;
            if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTED)
            {
              TSProfile_Write_MainStateMachine(TIME_SERVER_STATE_CONNECTED);
              TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_CONNECTED_IDLE);
            }
            else
            {
              /* connection req is accepted and pairing process started */
              TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_PROCESSING_PAIRING);
            }
          }
          else
          {
            /* connection was not successful, now we need to wait for
             * the application to put the device in discoverable mode
             */
            TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_ADVERTISABLE_IDLE);
          }
          /* Here there is no need to do anything extra if this profile is not
           * controlling the advertising process. Above if-else statement does
           * the needful. */
          /* Stop if any timer is running */
          Blue_NRG_Timer_Stop (timeServer.timerID);
          timeServer.timerID = 0xFF;
                
        }
        break; /* HCI_EVT_LE_CONN_COMPLETE */
      } /* switch(response->leEvent.subEventCode) */
    }
    break; /* EVT_LE_META_EVENT */
    
    case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      
      TIME_SERVER_MESG_DBG(profiledbgfile, "TimeServer_Event_Handler(), Received an VENDOR_SPECIFIC EVENT %04X \n", blue_evt->ecode);
      
      switch(blue_evt->ecode)
      {
        case EVT_BLUE_GAP_LIMITED_DISCOVERABLE:
        {
            TIME_SERVER_MESG_DBG(profiledbgfile,"EVT_BLUE_GAP_LIMITED_DISCOVERABLE \n");

            if ((TSProfile_Read_MainStateMachine() == TIME_SERVER_STATE_ADVERTIZE) && 
                ((TSProfile_Read_SubStateMachine() == TIME_SERVER_STATE_FAST_DISCOVERABLE) ||
                     (TSProfile_Read_SubStateMachine() == TIME_SERVER_STATE_LOW_POWER_DISCOVERABLE)))
            {
              /* Put the main profile in connectable idle state */
              BLE_Profile_Change_Advertise_Sub_State(&timeServer.TStoBLEInf, 
                                                     BLE_PROFILE_STATE_CONNECTABLE_IDLE);
              
              if(TSProfile_Read_SubStateMachine() == TIME_SERVER_STATE_LOW_POWER_DISCOVERABLE)
              {
                /* Limited Discoverable mode has timed out */
                TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_ADVERTISABLE_IDLE);
        
                /* notify the application */
                timeServer.applicationNotifyFunc(EVT_MP_ADVERTISING_TIMEOUT,0x00,NULL);
              }
            }
        }
        break;
        
        case EVT_BLUE_GAP_PAIRING_CMPLT: 
        {
          TIME_SERVER_MESG_DBG(profiledbgfile, "TimeServer_Event_Handler(),EVT_BLUE_GAP_PAIRING_CMPLT \n");
          if ((TSProfile_Read_MainStateMachine() == TIME_SERVER_STATE_ADVERTIZE) && 
              (TSProfile_Read_SubStateMachine() == TIME_SERVER_STATE_PROCESSING_PAIRING))
          {
              
            TSProfile_Write_MainStateMachine(TIME_SERVER_STATE_CONNECTED);
            TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_CONNECTED_IDLE);
             
          }
        }
        break; /* EVT_BLUE_GAP_PAIRING_CMPLT */
        
        case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
        {
          if (bnrg_expansion_board == IDB05A1) {
            evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
            TSProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
          else {
            evt_gatt_attr_modified_IDB04A1 *evt = (evt_gatt_attr_modified_IDB04A1*)blue_evt->data;
            TSProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
        }
        break; /* EVT_BLUE_GATT_ATTRIBUTE_MODIFIED */
    
        case EVT_BLUE_GATT_READ_PERMIT_REQ:
        {
          evt_gatt_read_permit_req * read_permit_req = (void*)blue_evt->data;
          attrHandle = read_permit_req->attr_handle;
          
          TIME_SERVER_MESG_DBG(profiledbgfile, "TimeServer_Event_Handler(),EVT_BLUE_GATT_READ_PERMIT_REQ, attrHandle =  %04X \n", attrHandle); 
          
          if(attrHandle == (timeServer.currentTimeCharHandle + 1))
          {
            /* send the allow read command */			
            timeServer.timeServerAllowRead = 1;
            /* let the application know that the current time is
             * being requested for read. there is no event data
             * for this event
             */
            timeServer.applicationNotifyFunc(EVT_TS_CURTIME_READ_REQ,0,NULL);
          }
        }
        break;
      }
    }
    break; /* HCI_EVT_VENDOR_SPECIFIC */
  }
}/* end TimeServer_Event_Handler() */

/**
 * TimeServer_StateMachine
 * 
 * @param[in] None
 * 
 * TimeServer profile's state machine: to be called on application main loop. 
 */ 
tBleStatus TimeServer_StateMachine(void)
{
  tBleStatus status = BLE_STATUS_SUCCESS;

  uint8_t cmdCmpltStatus = BLE_STATUS_SUCCESS;
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;
	
  /* timeServer.mainState, timeServer.subState */
  switch (TSProfile_Read_MainStateMachine())
  {
      case TIME_SERVER_STATE_UNINITIALIZED:
      break;
      case TIME_SERVER_STATE_WAITING_BLE_INIT:
      {
        /* check whether the main profile has done with ble init or not */    
        if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
        {
          /* Add TimeServer service & characteristics */ 
          status = TimeServer_Add_Services_Characteristics();
          if (status == BLE_STATUS_SUCCESS)
          {
            TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_StateMachine(),TimeServer_StateMachine(), Timer Server Profile is Initialized with Service & Characteristics \n");
            
            /* vote to the main profile to go to advertise state */
            /* change current profiles Main and Sub both states */
            /* change profiles main and sub states */
            TSProfile_Write_MainStateMachine(TIME_SERVER_STATE_INITIALIZED);
            TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_INVALID);

            /* vote to the main profile to go to advertise state */
            (void) BLE_Profile_Vote_For_Advertisable_State (&timeServer.TStoBLEInf);

            /* add the profile specific service UUIDs into global list of supported UUIDs*/   
            BLE_Profile_Add_Advertisment_Service_UUID (CURRENT_TIME_SERVICE_UUID);
            if (timeServer.servicesSupported & NEXT_DST_CHANGE_SERVICE_BITMASK)
            {
                BLE_Profile_Add_Advertisment_Service_UUID (NEXT_DST_CHANGE_SERVICE_UUID);
            }
            if(timeServer.servicesSupported & REFERENCE_TIME_UPDATE_SERVICE_BITMASK)
            {
                BLE_Profile_Add_Advertisment_Service_UUID (REFERENCE_UPDATE_TIME_SERVICE_UUID);
            }
          }
          else
          {
            TIME_SERVER_MESG_DBG(profiledbgfile,"TimeServer_StateMachine(), TimeServer_StateMachine() FAILED to add Timer Server Profile Service, Characteristics; Error: %02X !!\n", status);
           
            /* Notify to application that Timer Server profile initialization has failed */
            TimeServer_Exit(status);
          }
        }
      }
      break; /* TIME_SERVER_STATE_WAITING_BLE_INIT */
    
      case TIME_SERVER_STATE_INITIALIZED:
      {
        /* Time Server profile has completed its initialization process and voted the 
         * main profile for Advertisable state. Now its waiting for the main profile
         * to set to Advertisable state. */

        /* check main profile's state, if its in advertisable state, set the same for 
         * current profile as well */
        if ((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) && 
            (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_CONNECTABLE_IDLE))
        {
          TSProfile_Write_MainStateMachine(TIME_SERVER_STATE_ADVERTIZE);
          TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_ADVERTISABLE_IDLE);
          cmdCmpltStatus = BLE_STATUS_SUCCESS;
          timeServer.applicationNotifyFunc(EVT_TS_INITIALIZED,1,&cmdCmpltStatus);
        }
      }
      break; /* TIME_SERVER_STATE_INITIALIZED */
      case TIME_SERVER_STATE_ADVERTIZE:
      {
        switch (TSProfile_Read_SubStateMachine())
        {
          case TIME_SERVER_STATE_ADVERTISABLE_IDLE:
          {
          }
          break;

          case TIME_SERVER_STATE_ADVERTISING_FOR_WHITE_LIST:
          break;
          
          case TIME_SERVER_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP:
          {
            TIME_SERVER_MESG_DBG ( profiledbgfile, "STOPPING Advertising to White List devices \n");
            
            hciCmdResult = aci_gap_set_non_discoverable(); 
            if ( hciCmdResult == BLE_STATUS_SUCCESS)
            {
              /* change profile's sub states */
              TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_FAST_DISCOVERABLE_DO_START);
              /* request for main profile's state change */
              BLE_Profile_Change_Advertise_Sub_State ( &timeServer.TStoBLEInf, 
                                                       BLE_PROFILE_STATE_CONNECTABLE_IDLE);
            }
            else
            {
                break;
            }
          }
          case TIME_SERVER_STATE_FAST_DISCOVERABLE_DO_START:
          {

            hciCmdResult = TimeServer_Set_Discoverable((uint16_t)TIP_FAST_CONN_ADV_INTERVAL_MIN,
                                                       (uint16_t)TIP_FAST_CONN_ADV_INTERVAL_MAX);
            if ( hciCmdResult == BLE_STATUS_SUCCESS)
            {
              TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Make_Discoverable(), FAST Limited Discoverable Mode OK \n" );
              /* start a timer of 30 seconds for first phase of discoverable mode (fast discoverable mode) */
              Blue_NRG_Timer_Start (30, TimeServer_Advertise_Period_Timeout_Handler,&timeServer.timerID);
              
              /* change profile's sub states */
              TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_FAST_DISCOVERABLE); //Wait for timeout
            
              /* request for main profile's state change */
              BLE_Profile_Change_Advertise_Sub_State ( &timeServer.TStoBLEInf, 
                                                       BLE_PROFILE_STATE_ADVERTISING);
            }
            else
            {
              /* Request to put the device in limited discoverable mode is 
              * unsuccess, put the device back in advertisable idle mode */
              TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Make_Discoverable(), FAST Limited Discoverable Mode FAILED with hciCmdResult: %02X \n", hciCmdResult );
              TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_ADVERTISABLE_IDLE);
            }                 
          }
          break;
          case TIME_SERVER_STATE_FAST_DISCOVERABLE:
          break;
          
          case TIME_SERVER_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP:
          {
              TIME_SERVER_MESG_DBG ( profiledbgfile, "STOPPING FAST General Discoverable Mode \n");
              
              hciCmdResult = aci_gap_set_non_discoverable(); 
              if ( hciCmdResult == BLE_STATUS_SUCCESS)
              {
                /* request for main profile's state change */
                BLE_Profile_Change_Advertise_Sub_State ( &timeServer.TStoBLEInf, 
                                                         BLE_PROFILE_STATE_CONNECTABLE_IDLE); 
                
                TIME_SERVER_MESG_DBG ( profiledbgfile, "STARTING Low Power General Discoverable Mode \n");
                
                hciCmdResult = TimeServer_Set_Discoverable((uint16_t)TIP_LOW_PWR_ADV_INTERVAL_MIN,
                                                            (uint16_t)TIP_LOW_PWR_ADV_INTERVAL_MAX);
                if ( hciCmdResult == BLE_STATUS_SUCCESS)
                {
                   TIME_SERVER_MESG_DBG ( profiledbgfile, "Command Status received for GAP_SET_LIMITED_DISCOVERABLE \n");
                    /* change profile's sub states */
                   
                    TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_BLE_ADVERTISING);//TBR 
                    
                    /* request for main profile's state change */
                    BLE_Profile_Change_Advertise_Sub_State ( &timeServer.TStoBLEInf, 
                                                 BLE_PROFILE_STATE_ADVERTISING);
                }
                else
                {
                  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Set_Discoverable() FAILED: %02X \n", hciCmdResult );
                  /* Request to put the device in limited discoverable mode is 
                    * unsuccess, put the device back in advertisable idle mode */
                  TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_ADVERTISABLE_IDLE);
                  break;
                }
              }
              else
              {
                TIME_SERVER_MESG_DBG ( profiledbgfile, "aci_gap_set_non_discoverable() FAILED: %02X \n", hciCmdResult );
                /* Request to put the device non discoverable mode is 
                * unsuccess: which state to be set? */  //TBR 
                break;
              }
            }
            break;
            
            case TIME_SERVER_STATE_LOW_POWER_DISCOVERABLE:
            break;
            case TIME_SERVER_STATE_BLE_ADVERTISING:
            {
            }
            break;
            case TIME_SERVER_STATE_PROCESSING_PAIRING:
            break;
        }  /* switch (TSProfile_Read_SubStateMachine()) */
      }
      break; /* TIME_SERVER_STATE_ADVERTIZE */
      case TIME_SERVER_STATE_CONNECTED:
      {
        switch (TSProfile_Read_SubStateMachine())
        {
          case TIME_SERVER_STATE_CONNECTED_IDLE:
          case TIME_SERVER_STATE_CONNECTED_BUSY:
          {
          }
          break;
          case TIME_SERVER_STATE_DO_DISCONNECT:
          break;
          case TIME_SERVER_STATE_DISCONNECTING:
          break;
          case TIME_SERVER_STATE_DISCONNECTED:
          break;
        }
      }
      break; /* TIME_SERVER_STATE_CONNECTED */
  }  /* switch (timeServer.mainState) */
  return (hciCmdResult);
}


/* Function: TimeServer_Make_Discoverable
 * 
 * Desc: This function signals the heart rate sensor task to set to discoverable mode.
 *
 * Param: useBoundedDeviceList: use previously bounded device list for pairing or not.
 *
 * Return: BLE_STATUS_SUCCESS: profile task is signaled successfully,
 *         BLE_STATUS_FAILED: task is not in prorper state to receive the request. 
 */
tBleStatus TimeServer_Make_Discoverable ( uint8_t useBoundedDeviceList )
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Make_Discoverable() \n");
  
  if ((TSProfile_Read_MainStateMachine() == TIME_SERVER_STATE_ADVERTIZE)  &&  
      (TSProfile_Read_SubStateMachine() == TIME_SERVER_STATE_ADVERTISABLE_IDLE))
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Make_Discoverable(), Time Server Set Discoverable mode: %d \n", useBoundedDeviceList );
    if ((useBoundedDeviceList) && (gBLEProfileContext.bleSecurityParam.bonding_mode))
    {

      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Make_Discoverable(), STARTING Advertising for White List devices \n");
      /* start the undirected connectable mode */
      retval = aci_gap_set_undirected_connectable(PUBLIC_ADDR, WHITE_LIST_FOR_ALL);   
      if ( retval == BLE_STATUS_SUCCESS)
      {
        /* start a timer of 10 seconds to try to connnect to white list device */
        Blue_NRG_Timer_Start (10, TimeServer_Advertise_Period_Timeout_Handler,&timeServer.timerID);
    
        /* change profile's sub states */
        TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_ADVERTISING_FOR_WHITE_LIST);
        /* request for main profile's state change */
        BLE_Profile_Change_Advertise_Sub_State ( &timeServer.TStoBLEInf, 
                                                 BLE_PROFILE_STATE_ADVERTISING);
      }
      else
      {
        TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Make_Discoverable(), FAILED to Set Advertising for White List devices: %02X \n", retval );
      }
    }
    else
    {
      TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_FAST_DISCOVERABLE_DO_START);
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Make_Discoverable(), Set state to TIME_SERVER_STATE_FAST_DISCOVERABLE_DO_START \n");
    }
  }
  else
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "Time Server Set Discoverable mode: Invalid Profile State \n");
  }

  return (retval);
}/* end TimeServer_Make_Discoverable() */

/**
 * TimeServer_Advertise_Period_Timeout_Handler
 * 
 * this function is called when the timer started
 * by the heart rate profile for advertising times out
 */ 
void TimeServer_Advertise_Period_Timeout_Handler(void)   
{
  tProfileState nextState = TSProfile_Read_SubStateMachine();
  Blue_NRG_Timer_Stop (timeServer.timerID);
  timeServer.timerID = 0xFF;
  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Advertise_Period_Timeout_Handler() \n");
  if (TSProfile_Read_MainStateMachine() == TIME_SERVER_STATE_ADVERTIZE)
  {
    switch (TSProfile_Read_SubStateMachine())
    {
        case TIME_SERVER_STATE_ADVERTISING_FOR_WHITE_LIST:
            nextState = TIME_SERVER_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP;
            TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Advertise_Period_Timeout_Handler(), White list Advertising Timedout \n");
        break;
        case TIME_SERVER_STATE_FAST_DISCOVERABLE:
            nextState = TIME_SERVER_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP;
            TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Advertise_Period_Timeout_Handler(), Fast Advertising Timedout \n");
        break;
    }

    if (nextState != TSProfile_Read_SubStateMachine())   
    {
        TSProfile_Write_SubStateMachine(nextState);
    }
  }
}/* end TimeServer_Advertise_Period_Timeout_Handler() */

/**
 * TimeServer_CT_Char_Notify_Delay_Timeout_Handler
 */ 
void TimeServer_CT_Char_Notify_Delay_Timeout_Handler(void)
{
  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_CT_Char_Notify_Delay_Timeout_Handler() \n");
  /* Time delay for current time char update notification is timed out, 
   * now the time server can notify to client for any update in current time char */
  ctCharNotifyDelayTimeout = 1;
  Blue_NRG_Timer_Stop(timeServer.delayTimerID);
  timeServer.delayTimerID = 0xFF;
}/* end TimeServer_CT_Char_Notify_Delay_Timeout_Handler() */

/**
 * Allow_Curtime_Char_Read
 * 
 * sends the allow read command to the controller
 */ 
tBleStatus Allow_Curtime_Char_Read()
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  TIME_SERVER_MESG_DBG(profiledbgfile, "Allow_Curtime_Char_Read()\n");
    
  if(timeServer.timeServerAllowRead)
  {
    /* send the allow read command to the controller */
    retval = aci_gatt_allow_read(timeServer.connHandle);
    if (retval == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "Allow_Curtime_Char_Read() successfully \n");
    }
    else
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "Allow_Curtime_Char_Read() FAILED  %02X \n", retval);
    }
    
    /* clear the flag */
    timeServer.timeServerAllowRead = 0x00; 
  }
  
  return retval;
}/* end Allow_Curtime_Char_Read() */

/**
 * TimeServer_Exit
 * 
 * @param[in] errCode: reason for exit
 * 
 * informs the application for the reason
 * the initialization failed and de-registers 
 * the profile
 */ 
void TimeServer_Exit(uint8_t errCode)
{
  /* Notify to application that Heart Rate profile initialization has failed */
  timeServer.applicationNotifyFunc(EVT_TS_INITIALIZED,1,&errCode);
  /* Change profile state to represent uninitialized profile */
  TSProfile_Write_MainStateMachine(TIME_SERVER_STATE_UNINITIALIZED);
  TSProfile_Write_SubStateMachine(TIME_SERVER_STATE_INVALID);
  /* unregister the profile from BLE registered profile list */
  BLE_Profile_Unregister_Profile ( &timeServer.TStoBLEInf );
  /* Need to remove all added services and characteristics ??? */
}  /* TimeServer_Exit() */                


void TimeServer_Notify_Update(uint16_t servHandle,uint16_t charHandle,tBleStatus retval)
{

   uint8_t tempBuf[5];
  /* let the application know that the update was
     * successful
     */ 
  STORE_LE_16(tempBuf,servHandle);
  STORE_LE_16(&tempBuf[2],charHandle);
  tempBuf[4] = retval;
  timeServer.applicationNotifyFunc(EVT_TS_CHAR_UPDATE_CMPLT,5,tempBuf);
}              
