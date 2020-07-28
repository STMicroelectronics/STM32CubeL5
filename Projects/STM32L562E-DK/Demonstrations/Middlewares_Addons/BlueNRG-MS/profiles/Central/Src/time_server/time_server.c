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
*   FILENAME        -  time_server.c
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      02/23/2015
*   $Revision$:  first version
*   $Author$:    
*   Comments:    Time Server Profile Central role implementation
*                
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* This file have Time Profile's Central role's implementation. 
* 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include <host_config.h>
#include <hci.h>
#include <debug.h>
#include <ble_list.h>
#include <timer.h>
#include <uuid.h>

#if (BLE_CURRENT_PROFILE_ROLES & TIME_SERVER)

#include <time_server.h>
#include <time_server_config.h>
/******************************************************************************
* Macro Declarations
******************************************************************************/

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )

/******************************************************************************
* Type Definitions
******************************************************************************/
#define WEAK_FUNCTION(x) __weak x
/******************************************************************************
 * Local Variable Declarations
******************************************************************************/

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
/* profile context */
TimeServerContext_Type TimeServerContext; 
/******************************************************************************
 * Imported Variable
******************************************************************************/
//extern void *profiledbgfile;
/******************************************************************************
 * Function Declarations
******************************************************************************/

void TimeServer_CT_Char_Notify_Delay_Timeout_Handler(void);
/******************************************************************************
* exported SAP
******************************************************************************/

tBleStatus TimeServer_Init(tipsInitDevType param)
{
  initDevType initParam;
  uint8_t ret;

  BLUENRG_memcpy(&TimeServerContext, 0, sizeof(TimeServerContext_Type));//TBR
  
  TimeServerContext.servicesSupported = param.servicesToBeSupported; 
  
  BLUENRG_memcpy(TimeServerContext.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    TimeServerContext.state = TIME_SERVER_STATE_INITIALIZED;
    //TimeServerContext.fullConf = FALSE; //TBR
  } else {
    TimeServerContext.state = TIME_SERVER_STATE_UNINITIALIZED;
  }
  
  return ret;
}

tBleStatus TimeServer_DeviceDiscovery(tipsDevDiscType param)
{
  devDiscoveryType devDiscParam;
  uint8_t ret;
  
  devDiscParam.procedure = LIMITED_DISCOVERY_PROCEDURE;
  devDiscParam.scanInterval = param.scanInterval;
  devDiscParam.scanWindow = param.scanWindow;
  devDiscParam.own_address_type = param.own_addr_type;
  ret = Master_DeviceDiscovery(&devDiscParam);
  if (ret == BLE_STATUS_SUCCESS)
  {
    TimeServerContext.state = TIME_SERVER_STATE_DEVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus TimeServer_SecuritySet(tipsSecurityType param)
{
  securitySetType secParam;
  uint8_t ret;

  secParam.ioCapability = param.ioCapability;
  secParam.mitm_mode = param.mitm_mode;
  secParam.oob_enable = param.oob_enable;
  if (secParam.oob_enable == OOB_AUTH_DATA_PRESENT)
    BLUENRG_memcpy(secParam.oob_data, param.oob_data, 16);
  secParam.use_fixed_pin = param.use_fixed_pin;
  if (secParam.use_fixed_pin == USE_FIXED_PIN_FOR_PAIRING)
    secParam.fixed_pin = param.fixed_pin;
  else 
    secParam.fixed_pin = 0;
  secParam.bonding_mode = param.bonding_mode;
  ret = Master_SecuritySet(&secParam);

  return ret;
}

tBleStatus TimeServer_DeviceConnection(tipsConnDevType param)
{
  connDevType master_param;
  uint8_t ret;

  master_param.procedure = DIRECT_CONNECTION_PROCEDURE;
  master_param.fastScanDuration = param.fastScanDuration;
  master_param.fastScanInterval = param.fastScanInterval;
  master_param.fastScanWindow = param.fastScanWindow;
  master_param.reducedPowerScanInterval = param.reducedPowerScanInterval;
  master_param.reducedPowerScanWindow = param.reducedPowerScanWindow;
  master_param.peer_addr_type = param.peer_addr_type;
  BLUENRG_memcpy(master_param.peer_addr, param.peer_addr, 6);
  master_param.own_addr_type = param.own_addr_type;
  master_param.conn_min_interval = param.conn_min_interval;
  master_param.conn_max_interval = param.conn_max_interval;
  master_param.conn_latency = param.conn_latency;
  master_param.supervision_timeout = param.supervision_timeout;
  master_param.min_conn_length = param.min_conn_length;
  master_param.max_conn_length = param.max_conn_length;
  ret = Master_DeviceConnection(&master_param);
  if (ret == BLE_STATUS_SUCCESS) {
    TimeServerContext.state = TIME_SERVER_STATE_START_CONNECTION;
  }

  return ret;
}

tBleStatus TimeServer_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(TimeServerContext.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    //TimeServerContext.fullConf = FALSE;//TBR
  }
  return ret;
}

tBleStatus TimeServer_ConnectionParameterUpdateRsp(uint8_t accepted, tipsConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(TimeServerContext.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus TimeServer_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(TimeServerContext.connHandle, FALSE);
  return ret;
}

tBleStatus TimeServer_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end TimeServer_Clear_Security_Database() */

tBleStatus TimeServer_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(TimeServerContext.connHandle, pinCode);
  
  return ret;
}

void TimeServer_StateMachine(void)//TBR: Is It needed?
{

  switch(TimeServerContext.state) {
   case TIME_SERVER_STATE_UNINITIALIZED:
    /* Nothing to do */
    break;
  case TIME_SERVER_STATE_CONNECTED_IDLE:
    {
    }
    break;
  case TIME_SERVER_STATE_INITIALIZED:
    /* Nothing to do */
    break;
  case TIME_SERVER_STATE_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case TIME_SERVER_STATE_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case TIME_SERVER_STATE_CONNECTED:
    {
      TimeServerContext.subState = TIME_SERVER_STATE_CONNECTED_IDLE;
    }
    break;
  }
    
}/* end TimeServer_StateMachine() */


tBleStatus TimeServer_Update_Current_Time_Value(tCurrentTime timeValue)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  uint8_t canDoUpdateNow = 0;
  
  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(), Received a new Time Value state: %x,  substate: %x,  minutes: %d, ctCharNotificationConfig: %d\n",TimeServerContext.state,TimeServerContext.subState,timeValue.minutes, TimeServerContext.ctCharNotificationConfig);

  if ((TimeServerContext.state == TIME_SERVER_STATE_CONNECTED)  &&  
       (TimeServerContext.subState == TIME_SERVER_STATE_CONNECTED_IDLE))
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
      if (TimeServerContext.ctCharNotificationConfig == 0x01)
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
            if (TimeServerContext.ctCharNotifyDelayTimeout == 0x00)
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
    retval =  aci_gatt_update_char_value(TimeServerContext.currentTimeServiceHandle, 
                                         TimeServerContext.currentTimeCharHandle,
                                         0, /* charValOffset */
                                         10, /* charValueLen */
                                         (uint8_t *) &tsCurrentTimeValue);  
    if (retval == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(),Current Time value has been sent for update (minutes: %d)\n", timeValue.minutes);
     
      /* use a timer to track delay between successive current time updates with notification */
      if (TimeServerContext.delayTimerID == 0xFF)
      {
        Blue_NRG_Timer_Start (60*15, TimeServer_CT_Char_Notify_Delay_Timeout_Handler,&TimeServerContext.delayTimerID);
        TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(),Start 15 minutes timer (timerID: %4X)\n", TimeServerContext.delayTimerID); //TBR
      }
      else
      {
        /* restart the timer */
        Blue_NRG_Timer_Stop(TimeServerContext.delayTimerID);
        Blue_NRG_Timer_Start (60*15, TimeServer_CT_Char_Notify_Delay_Timeout_Handler,&TimeServerContext.delayTimerID);
        TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(),Stop delayTimerID, ReStart 15 minutes timer (timerID: %4X)\n", TimeServerContext.delayTimerID); //TBR
      }
      TimeServerContext.ctCharNotifyDelayTimeout = 0;
    }
    else
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Current_Time_Value(),FAILED to send Current Time value %02X \n", retval);
    }
    
    /* Notify main profile of service, characteristic update & result */
    TimeServer_Notify_State_To_User_Application_CB(EVT_TS_CHAR_UPDATE_CMPLT);
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

  if(TimeServerContext.state == TIME_SERVER_STATE_CONNECTED)  
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "Local Time Information is ready to update \n");
 
    retval =  aci_gatt_update_char_value(TimeServerContext.currentTimeServiceHandle, 
                                         TimeServerContext.localTimeInfoCharHandle,
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
    TimeServer_Notify_State_To_User_Application_CB(EVT_TS_CHAR_UPDATE_CMPLT);
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
  if(TimeServerContext.state == TIME_SERVER_STATE_CONNECTED) 
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "Reference Time Information is ready to update \n");

    retval =  aci_gatt_update_char_value(TimeServerContext.currentTimeServiceHandle, 
                                         TimeServerContext.referenceTimeInfoCharHandle,
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
    TimeServer_Notify_State_To_User_Application_CB(EVT_TS_CHAR_UPDATE_CMPLT);
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

  if(TimeServerContext.state == TIME_SERVER_STATE_CONNECTED)  
  {
    TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_Update_Next_DST_Change(), Next DST Change Time is ready to update \n");

    retval =  aci_gatt_update_char_value(TimeServerContext.nextDSTChangeServiceHandle, 
                                         TimeServerContext.timeWithDSTCharHandle,
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
    TimeServer_Notify_State_To_User_Application_CB(EVT_TS_CHAR_UPDATE_CMPLT);
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
  if(TimeServerContext.subState == TIME_SERVER_STATE_UPDATING_NEW_TIME_VALUE)
  {
    refTimeUpdateStatus.currentStatus = TIME_UPDATE_STATUS_IDLE;
    refTimeUpdateStatus.result = TIME_UPDATE_RESULT_TIMEOUT;
    
    retval =  aci_gatt_update_char_value(TimeServerContext.referenceUpdateTimeServiceHandle, 
                                         TimeServerContext.timeUpdateStateCharHandle,
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
    TimeServer_Notify_State_To_User_Application_CB(EVT_TS_STOP_REFTIME_UPDATE);
    /* reset the time server substate */
    TimeServerContext.subState = TIME_SERVER_STATE_CONNECTED_IDLE;	
  }
}/* end RefTime_Update_Timeout_Handler() */


void Update_Reference_Time(uint8_t errCode,tCurrentTime* newTime) /* TBR : It seems not used */
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  
  TIME_SERVER_MESG_DBG ( profiledbgfile, "Update_Reference_Time() \n");
  
  if(TimeServerContext.subState == TIME_SERVER_STATE_UPDATING_NEW_TIME_VALUE)
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
    
    retval =  aci_gatt_update_char_value(TimeServerContext.referenceUpdateTimeServiceHandle, 
                                         TimeServerContext.timeUpdateStateCharHandle,
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
                                   &(TimeServerContext.currentTimeServiceHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Current Time Service is added Successfully %04X\n", TimeServerContext.currentTimeServiceHandle);
  }
  else
  {
    TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to Current Time Service: Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }  
  /* Add Current Time Characteristic */
  TIME_SERVER_MESG_DBG (profiledbgfile,"TimeServer_Add_Services_Characteristics(), Adding Current Time Characteristic. \n");
  
  uuid = CURRENT_TIME_CHAR_UUID; 
  hciCmdResult = aci_gatt_add_char(TimeServerContext.currentTimeServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &uuid , 
                                   10, 
                                   (CHAR_PROP_READ |CHAR_PROP_NOTIFY), 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, /* this is required for controled notification */
                                   10,  /* encryKeySize */
                                   CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                   &(TimeServerContext.currentTimeCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(),Current Time Characteristic Added Successfully %04X\n", TimeServerContext.currentTimeCharHandle);

  }
  else
  {
    TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Current Time Characteristic: Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }  

  /* Add Local Time Information Characteristic */
  TIME_SERVER_MESG_DBG (profiledbgfile,"TimeServer_Add_Services_Characteristics(), Adding Local Time Information Characteristic. \n");

  uuid = LOCAL_TIME_INFORMATION_CHAR_UUID; 
  hciCmdResult = aci_gatt_add_char(TimeServerContext.currentTimeServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &uuid , 
                                   2, 
                                   CHAR_PROP_READ, 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                   10, /* encryKeySize */
                                   CHAR_VALUE_LEN_CONSTANT, /* isConstant */
                                   &(TimeServerContext.localTimeInfoCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Local Time Information Characteristic Added Successfully %04X\n", TimeServerContext.localTimeInfoCharHandle);
  }
  else
  {
    TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Local Time Information Characteristic: Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }
 
  /* Add Reference Time Information Characteristic */
  TIME_SERVER_MESG_DBG (profiledbgfile,"TimeServer_Add_Services_Characteristics(), Adding Reference Time Information Characteristic. \n");

  uuid = REFERENCE_TIME_INFORMATION_CHAR_UUID; 
  hciCmdResult = aci_gatt_add_char(TimeServerContext.currentTimeServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *) &uuid , 
                                   4, /* bytes */ 
                                   CHAR_PROP_READ, 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                   10, /* encryKeySize */
                                   CHAR_VALUE_LEN_CONSTANT, /* isConstant */
                                   &(TimeServerContext.referenceTimeInfoCharHandle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    TIME_SERVER_MESG_DBG (profiledbgfile, "Reference Time Information Characteristic Added Successfully %04X\n", TimeServerContext.referenceTimeInfoCharHandle);
  }
  else
  {
    TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Reference Time Information Characteristic: Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }                     
       
  /* ***************  Add Next DST Change Service *************************/                           
  if (TimeServerContext.servicesSupported & NEXT_DST_CHANGE_SERVICE_BITMASK)
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
                                     &(TimeServerContext.nextDSTChangeServiceHandle));
    
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Next DST Change Service is added Successfully %04X\n", TimeServerContext.nextDSTChangeServiceHandle);
    }
    else
    {
      TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Next DST Change Service: Error: %02X !!\n", hciCmdResult);
      return hciCmdResult;
    }
   
    /* Add Time with DST Characteristic */
    TIME_SERVER_MESG_DBG (profiledbgfile,"TimeServer_Add_Services_Characteristics(), Adding Time with DST Characteristic. \n");
    
    uuid = TIME_WITH_DST_CHAR_UUID;
    hciCmdResult = aci_gatt_add_char(TimeServerContext.nextDSTChangeServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &uuid , 
                                     8, /* bytes */
                                     CHAR_PROP_READ, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                     10, /* encryKeySize */
                                     CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                     &(TimeServerContext.timeWithDSTCharHandle));
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
        TIME_SERVER_MESG_DBG (profiledbgfile, "Time with DST Characteristic Added Successfully %04X\n", TimeServerContext.timeWithDSTCharHandle);
    }
    else
    {
      TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Time with DST Characteristic: Error: %02X !!\n",hciCmdResult);
      return hciCmdResult;
    }  
  }/* if TimeServerContext.servicesSupported & NEXT_DST_CHANGE_SERVICE_BITMASK */
      
  /* ***************  Add Reference Update TimeChange Service *************************/         
               
  if (TimeServerContext.servicesSupported & REFERENCE_TIME_UPDATE_SERVICE_BITMASK) 
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
                                   &(TimeServerContext.referenceUpdateTimeServiceHandle));
    
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Reference Update Time Service is added Successfully %04X\n", TimeServerContext.referenceUpdateTimeServiceHandle);
    }
    else
    {
      TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Reference Update Time Service: Error: %02X !!\n", hciCmdResult);
      return hciCmdResult;
    }
               
    /* Add Time Update Controle Point Characteristic */
    TIME_SERVER_MESG_DBG (profiledbgfile,"TimeServer_Add_Services_Characteristics(), Adding Time Update Controle Point Characteristic. \n");
    
    uuid = TIME_UPDATE_CONTROL_POINT_CHAR_UUID;    
    hciCmdResult = aci_gatt_add_char(TimeServerContext.referenceUpdateTimeServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &uuid , 
                                     1, /* byte */
                                     CHAR_PROP_WRITE_WITHOUT_RESP, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                     10,  /* encryKeySize */
                                     CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                     &(TimeServerContext.timeUpdateControlPointCharHandle));
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Time Update Controle Point Characteristic Added Successfully %04X\n", TimeServerContext.timeUpdateControlPointCharHandle);
    }
    else
    {
      TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Time Update Controle Point Characteristic: Error: %02X !!\n", hciCmdResult);
      return hciCmdResult;
    }   
                
    /* Add Time Update State Characteristic */
    TIME_SERVER_MESG_DBG (profiledbgfile,"Adding Time Update State Characteristic. \n");
    
    uuid = TIME_UPDATE_STATE_CHAR_UUID;
    hciCmdResult = aci_gatt_add_char(TimeServerContext.referenceUpdateTimeServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *) &uuid , 
                                     2, /* bytes */
                                     CHAR_PROP_READ, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                     10, /* encryKeySize */
                                     CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                     &(TimeServerContext.timeUpdateStateCharHandle));
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG (profiledbgfile, "TimeServer_Add_Services_Characteristics(), Time Update State Characteristic Added Successfully %04X\n", TimeServerContext.timeUpdateStateCharHandle);
    }
    else
    {
      TIME_SERVER_MESG_ERR (profiledbgfile,"TimeServer_Add_Services_Characteristics(), FAILED to add Time Update State Characteristic: Error: %02X !!\n", hciCmdResult);
      return hciCmdResult;
    } 
  }               
  return hciCmdResult;
}/* end TimeServer_Add_Services_Characteristics() */


void TimeServer_Update_Control_Point_Handler(tTimeUpdateControlPoint update_control_point_value)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  if (TimeServerContext.state == TIME_SERVER_STATE_CONNECTED)  //TBR
  {
    if (update_control_point_value == GET_REFERENCE_UPDATE)
    {
      /* start the reference time update state machine */
      TimeServer_Notify_State_To_User_Application_CB(EVT_TS_START_REFTIME_UPDATE);
      
      /* update the time update state in the server */
      refTimeUpdateStatus.currentStatus = TIME_UPDATE_STATUS_UPDATE_PENDING;
      
      retval =  aci_gatt_update_char_value(TimeServerContext.referenceUpdateTimeServiceHandle, 
                                           TimeServerContext.timeUpdateStateCharHandle,
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
      Blue_NRG_Timer_Start(REFERENCE_UPDATE_TIMEOUT,RefTime_Update_Timeout_Handler,&TimeServerContext.timerID); //TBR
      TimeServerContext.subState = TIME_SERVER_STATE_UPDATING_NEW_TIME_VALUE;
    }
    else if (update_control_point_value == CANCEL_REFERENCE_UPDATE)
    {
      /* stop the reference time update state machine */
      retval = BLE_STATUS_SUCCESS;
      TimeServer_Notify_State_To_User_Application_CB(EVT_TS_STOP_REFTIME_UPDATE);
      
      /* update the time update state in the server */
      refTimeUpdateStatus.currentStatus = TIME_UPDATE_STATUS_IDLE;
      refTimeUpdateStatus.result = TIME_UPDATE_RESULT_CANCELED;
      
      retval =  aci_gatt_update_char_value(TimeServerContext.referenceUpdateTimeServiceHandle, 
                                           TimeServerContext.timeUpdateStateCharHandle,
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
      if(TimeServerContext.subState == TIME_SERVER_STATE_UPDATING_NEW_TIME_VALUE)
      {
        /* stop the running timer */
        Blue_NRG_Timer_Stop(TimeServerContext.timerID); //TBR
        TimeServerContext.subState = TIME_SERVER_STATE_CONNECTED_IDLE;	
      }
    }
  }
}/* end TimeServer_Update_Control_Point_Handler() */
  

/**
 * TimeServer_CT_Char_Notify_Delay_Timeout_Handler
 */ 
void TimeServer_CT_Char_Notify_Delay_Timeout_Handler(void)
{
  TIME_SERVER_MESG_DBG ( profiledbgfile, "TimeServer_CT_Char_Notify_Delay_Timeout_Handler() \n");
  /* Time delay for current time char update notification is timed out, 
   * now the time server can notify to client for any update in current time char */
  TimeServerContext.ctCharNotifyDelayTimeout = 1;
  Blue_NRG_Timer_Stop(TimeServerContext.delayTimerID);//TBR
  TimeServerContext.delayTimerID = 0xFF;
  
}/* end TimeServer_CT_Char_Notify_Delay_Timeout_Handler() */


tBleStatus TimeServer_Allow_Curtime_Char_Read(void) 
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  TIME_SERVER_MESG_DBG(profiledbgfile, "Allow_Curtime_Char_Read()\n");
    
  if(TimeServerContext.timeServerAllowRead)
  {
    /* send the allow read command to the controller */
    retval = aci_gatt_allow_read(TimeServerContext.connHandle);
    if (retval == BLE_STATUS_SUCCESS)
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "Allow_Curtime_Char_Read() successfully \n");
    }
    else
    {
      TIME_SERVER_MESG_DBG ( profiledbgfile, "Allow_Curtime_Char_Read() FAILED  %02X \n", retval);
    }
    /* clear the flag */
    TimeServerContext.timeServerAllowRead = 0x00; 
  }
  
  return retval;
}/* end Allow_Curtime_Char_Read() */
   
/******************  Weak Callbacks definition ***************************/

WEAK_FUNCTION(void TimeServer_Notify_State_To_User_Application_CB(uint8_t event_value))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & TIME_SERVER) */
