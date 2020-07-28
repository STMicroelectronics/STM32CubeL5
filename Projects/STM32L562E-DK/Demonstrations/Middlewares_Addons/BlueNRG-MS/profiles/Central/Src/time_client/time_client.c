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
*   FILENAME        -  time_client.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/01/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    time client APIs(connect, config charac, enable notification...)
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the time client profile central role.
* 
******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
#include "debug.h"
#include "timer.h"
#include "uuid.h"
#include "hci.h"
#include "hci_le.h"
#include "hci_tl.h"
#include "master_basic_profile.h"
#include "bluenrg_conf.h"

#if (BLE_CURRENT_PROFILE_ROLES & TIME_CLIENT)

#include "string.h"
#include "bluenrg_gatt_server.h"
#include "time_client.h"
#include "time_client_config.h" 

/******************************************************************************
* Macro Declarations
******************************************************************************/
#define WEAK_FUNCTION(x) __weak x
/******************************************************************************
* type definitions
******************************************************************************/

/******************************************************************************
* Variable Declarations
******************************************************************************/

/* time client context */
TimeClientContext_Type TimeClientContext;

/******************************************************************************
* Imported Variable
******************************************************************************/
extern initDevType initParam;

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*******************************************************************************
* Static Functions
*******************************************************************************/

/*******************************************************************************
* exported SAP
*******************************************************************************/

tBleStatus TimeClient_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle)
{
  uint8_t i, j;
  uint16_t uuid;

  j = 0;
  for (i=0; i<TimeClientContext.numpeerServices; i++) {
    if (TimeClientContext.peerServices[j] == UUID_TYPE_16) {
      BLUENRG_memcpy(&uuid, &TimeClientContext.peerServices[j+5], 2);
      j++;
      BLUENRG_memcpy(start_handle, &TimeClientContext.peerServices[j], 2);
      j += 2;
      BLUENRG_memcpy(end_handle, &TimeClientContext.peerServices[j], 2);
      j += 2;
      if (TimeClientContext.peerServices[j-5] == UUID_TYPE_16) {
	j += 2;
      } else {
	j += 16;
      }
      if (uuid == uuid_service) {
	return BLE_STATUS_SUCCESS;
      }
    }
  }
  
  return HCI_INVALID_PARAMETERS;
}

tBleStatus TimeClient_Init(tipcInitDevType param)
{
  //initDevType initParam;
  uint8_t ret;

  BLUENRG_memcpy(&TimeClientContext, 0, sizeof(TimeClientContext_Type));
  
  BLUENRG_memcpy(TimeClientContext.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  initParam.Master_DeviceDiscovery_CB = Master_TC_DeviceDiscovery_CB;
  initParam.Master_Connection_CB = Master_TC_Connection_CB;
  initParam.Master_Pairing_CB = Master_TC_Pairing_CB;
  initParam.Master_PeerDataExchange_CB = Master_TC_PeerDataExchange_CB;
  initParam.Master_PeerDataPermitExchange_CB = Master_TC_PeerDataPermitExchange_CB;
  initParam.Master_ServiceCharacPeerDiscovery_CB = Master_TC_ServiceCharacPeerDiscovery_CB;
  
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    TimeClientContext.state = TIME_CLIENT_INITIALIZED;
    TimeClientContext.fullConf = FALSE;
  } else {
    TimeClientContext.state = TIME_CLIENT_UNINITIALIZED;
    //PROFILE_MESG_DBG(profiledbgfile,"**** Device not initialized\r\n"); //TBR
  }

  return ret;
}

tBleStatus TimeClient_DeviceDiscovery(tipcDevDiscType param)
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
    TimeClientContext.state = TIME_CLIENT_DEVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus TimeClient_SecuritySet(tipcSecurityType param)
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

tBleStatus TimeClient_DeviceConnection(tipcConnDevType param)
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
    TimeClientContext.state = TIME_CLIENT_START_CONNECTION;
  }

  return ret;
}

tBleStatus TimeClient_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(TimeClientContext.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    TimeClientContext.fullConf = FALSE;
  }
  return ret;
}

tBleStatus TimeClient_ConnectionParameterUpdateRsp(uint8_t accepted, tipcConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(TimeClientContext.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus TimeClient_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(TimeClientContext.connHandle, FALSE);
  return ret;
}

tBleStatus TimeClient_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end TimeClient_Clear_Security_Database() */

tBleStatus TimeClient_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(TimeClientContext.connHandle, pinCode);
  
  return ret;
}

tBleStatus TimeClient_ServicesDiscovery(void)
{
  uint8_t ret;

  ret = Master_GetPrimaryServices(TimeClientContext.connHandle, 
				  &TimeClientContext.numpeerServices, 
				  TimeClientContext.peerServices, 
				  PRIMARY_SERVICES_ARRAY_SIZE);
  if (ret == BLE_STATUS_SUCCESS)
  {
    TimeClientContext.state = TIME_CLIENT_SERVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus TimeClient_DiscCharacServ(uint16_t uuid_service)
{
  uint8_t ret;
  uint16_t start_handle, end_handle;

  if ((uuid_service == CURRENT_TIME_SERVICE_UUID) || (uuid_service == NEXT_DST_CHANGE_SERVICE_UUID)|| (uuid_service == REFERENCE_UPDATE_TIME_SERVICE_UUID)) {
    if (!(TimeClientContext.currentTimeServicePresent))
      return HCI_INVALID_PARAMETERS;
  }

  ret = TimeClient_findHandles(uuid_service, &start_handle, &end_handle);
  if (ret == BLE_STATUS_SUCCESS) {
    ret = Master_GetCharacOfService(TimeClientContext.connHandle,
				    start_handle,
				    end_handle,
				    &TimeClientContext.numCharac,
				    TimeClientContext.charac,
				    CHARAC_OF_SERVICE_ARRAY_SIZE);

    if (ret == BLE_STATUS_SUCCESS)
    {
      TimeClientContext.uuid_searched = uuid_service;
    }
  }
  return ret;
}

tBleStatus TimeClient_Start_Current_Time_Characteristic_Descriptor_Discovery(void)
{
  tBleStatus ret;
  
  /* It discovers the characteristic descriptors of the connected Current Time Characteristic */
  ret= Master_GetCharacDescriptors(TimeClientContext.connHandle,
                                   TimeClientContext.currentTimeCharValHandle, /* current time chararacteristic: Value Handle */
                                   TimeClientContext.currentTimeCharValHandle+1, /* current time chararacteristic: End Handle */
                                   &TimeClientContext.numCharac,
                                   TimeClientContext.charac,
                                   CHARAC_OF_SERVICE_ARRAY_SIZE);
  return(ret);
}/* end TimeClient_Start_Glucose_Measurement_Characteristic_Descriptor_Discovery() */


/**
  * @brief Read Current Time Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus TimeClient_ReadCurrentTimeChar(void)
{
  tBleStatus ret = BLE_STATUS_FAILED; 

  /* Read current  time  Characteristic */
  ret = Master_Read_Value(TimeClientContext.connHandle,  
                          TimeClientContext.currentTimeCharValHandle,  
                          &TimeClientContext.dataLen,  
                          TimeClientContext.data,  
                          CURRENT_TIME_READ_SIZE 
                          );
  if (ret == BLE_STATUS_SUCCESS)
  {
    TimeClientContext.state = TIME_CLIENT_WAIT_READ_CURRENT_TIME_VALUE;
  }
   else
  {
    TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE;
  }
  return ret;
}

/**
  * @brief Read Local Time Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus TimeClient_ReadLocalTimeChar(void)
{
  tBleStatus ret = BLE_STATUS_FAILED; 
  
  /* Read local time info Characteristic */
  ret = Master_Read_Value(TimeClientContext.connHandle,  
                          TimeClientContext.localTimeInfoCharValHandle,  
                          &TimeClientContext.dataLen,  
                          TimeClientContext.data,  
                          LOCAL_TIME_READ_SIZE 
                          );
  if (ret == BLE_STATUS_SUCCESS)
  {
    TimeClientContext.state = TIME_CLIENT_WAIT_READ_LOCAL_TIME_INFO;
  }
   else
  {
    TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE;
  }
  return ret;
}

tBleStatus TimeClient_ReadReferenceTimeInfoChar(void)
{
  tBleStatus ret = BLE_STATUS_FAILED; 
  
  /* Read reference time  Characteristic */
  ret = Master_Read_Value(TimeClientContext.connHandle,  
                          TimeClientContext.refTimeInfoCharValHandle,  
                          &TimeClientContext.dataLen,  
                          TimeClientContext.data,  
                          REFERENCE_TIME_READ_SIZE 
                            );
  if (ret == BLE_STATUS_SUCCESS)
  {
    TimeClientContext.state = TIME_CLIENT_WAIT_READ_REFERENCE_TIME_INFO;
  }
  else
  {
    TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE;
  }
    
  return ret;
}/* end TimeClient_ReadReferenceTimeUpdateChar() */     

/**
  * @brief Read Next DST Change Time Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus TimeClient_ReadNextDSTChangeTimeChar(void)
{
  tBleStatus ret = BLE_STATUS_FAILED; 
  
  /* Read Next DST Change Time Characteristic */
  ret = Master_Read_Value(TimeClientContext.connHandle,  
                          TimeClientContext.timeWithDSTCharValHandle,  
                          &TimeClientContext.dataLen,  
                          TimeClientContext.data,  
                          NEXT_DST_CHANGE_TIME_READ_SIZE 
                          );
  if (ret == BLE_STATUS_SUCCESS)
  {
    TimeClientContext.state = TIME_CLIENT_WAIT_READ_NEXT_DST_CHANGE_TIME;
  }
  else
  {
    TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE;
  }
  return ret;
}

/**
  * @brief Read Server Time Update Status Characteristic
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus TimeClient_ReadServerTimeUpdateStatusChar(void)
{
  tBleStatus ret = BLE_STATUS_FAILED; 
  
  /* Read Server Time Update Status Characteristic */
  ret = Master_Read_Value(TimeClientContext.connHandle,  
                          TimeClientContext.timeUpdateStateCharValHandle,  
                          &TimeClientContext.dataLen,  
                          TimeClientContext.data,  
                          SERVER_TIME_UPDATE_STATUS_READ_SIZE 
                          );
  if (ret == BLE_STATUS_SUCCESS)
  {
    TimeClientContext.state = TIME_CLIENT_WAIT_READ_SERVER_TIME_UPDATE_STATUS;
  }
   else
  {
    TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE;
  }
    
  return ret;
}
                              
tBleStatus TimeClient_Set_Current_Time_Char_Notification(BOOL value)
{
  tBleStatus ret; 
  ret = Master_NotifIndic_Status(TimeClientContext.connHandle,
                                 TimeClientContext.currentTimeCharValHandle + 1, /* current Time Descriptor Handle */
                                 value, /* enable/disable notification */
                                 FALSE /* no indication */
                                 );
  return ret;

}/* end TimeClient_Set_Current_Time_Char_Notification() */

/**
 * TimeClient_Update_Reference_Time_On_Server
 * 
 * @param[in] ctlValue: the value passed starts
 * a procedure to cancel or start a update process\n
 * 
 * Starts a gatt write without response procedure to 
 * write the time update control point characteristic  
 * on the server. The information read will be passed 
 * to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Update_Reference_Time_On_Server(uint8_t ctlValue)
{
  /* write without response for Time Update Control Point Char*/
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  if ((ctlValue != GET_REFERENCE_UPDATE) && (ctlValue != CANCEL_REFERENCE_UPDATE))
  {
    return (retval);
  }

  retval = Master_WriteWithoutResponse_Value(TimeClientContext.connHandle,
                                             TimeClientContext.timeUpdateCtlPointCharValHandle, 
                                             (uint8_t)1, /* control point value is of 1 byte */
                                             &ctlValue);
  return (retval);
}/* end TimeClient_Update_Reference_Time_On_Server() */


/* This function start the connection and the configuration of the time client device */
uint8_t TimeClient_ConnConf(tipcConnDevType connParam)
{
  uint8_t ret;

  ret = TimeClient_DeviceConnection(connParam);
  
  if (ret == BLE_STATUS_SUCCESS) {
    TimeClientContext.fullConf = TRUE;
  }

  return ret;
}

void TimeClient_StateMachine(void)
{
  uint8_t ret;

  switch(TimeClientContext.state) {
  case TIME_CLIENT_UNINITIALIZED:
    /* Nothing to do */
    break;
  case TIME_CLIENT_CONNECTED_IDLE:
    {
    }
    break;
  case TIME_CLIENT_INITIALIZED:
    /* Nothing to do */
    break;
  case TIME_CLIENT_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case TIME_CLIENT_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case TIME_CLIENT_CONNECTED:
    {
      ret = TimeClient_ServicesDiscovery(); 
      if (ret == BLE_STATUS_SUCCESS) {
	TimeClientContext.state = TIME_CLIENT_SERVICE_DISCOVERY;
      } else {
	TimeClient_FullConfError_CB(TIPC_ERROR_IN_SERVICE_DISCOVERY, ret);
	TimeClientContext.state = TIME_CLIENT_DISCONNECTION;
      }
    }
    break;
  case TIME_CLIENT_SERVICE_DISCOVERY:
    /* Waiting the end of the service discovery procedure */
    break;
    
  case TIME_CLIENT_CTS_CHARAC_DISCOVERY:
    {
      ret = TimeClient_DiscCharacServ(CURRENT_TIME_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	TimeClientContext.state = TIME_CLIENT_WAIT_CTS_CHARAC_DISCOVERY;
      } else {
	TimeClient_FullConfError_CB(TIPC_ERROR_IN_CHARAC_DISCOVERY, ret);
	TimeClientContext.state = TIME_CLIENT_DISCONNECTION;
      }
    }
    break;
  case TIME_CLIENT_WAIT_CTS_CHARAC_DISCOVERY:
    /* Waiting the end of the current time Characteristic discovery */ 
    break;
    
    case TIME_CLIENT_NEXT_DSTS_CHARAC_DISCOVERY:
    {
      ret = TimeClient_DiscCharacServ(NEXT_DST_CHANGE_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	TimeClientContext.state = TIME_CLIENT_WAIT_NEXT_DSTS_CHARAC_DISCOVERY;
      } else {
	TimeClient_FullConfError_CB(TIPC_ERROR_IN_CHARAC_DISCOVERY, ret);
	TimeClientContext.state = TIME_CLIENT_DISCONNECTION;
      }
    }
    break;
  case TIME_CLIENT_WAIT_NEXT_DSTS_CHARAC_DISCOVERY:
    /* Waiting the end of the next dst service Characteristics discovery */ 
    break;
    
  case TIME_CLIENT_RTUS_CHARAC_DISCOVERY:
    {
      ret = TimeClient_DiscCharacServ(REFERENCE_UPDATE_TIME_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	TimeClientContext.state = TIME_CLIENT_WAIT_RTUS_CHARAC_DISCOVERY;
      } else {
	TimeClient_FullConfError_CB(TIPC_ERROR_IN_CHARAC_DISCOVERY, ret);
	TimeClientContext.state = TIME_CLIENT_DISCONNECTION;
      }
    }
    break;
  case TIME_CLIENT_WAIT_RTUS_CHARAC_DISCOVERY:
    /* Waiting the end of the RTU service characteristics discovery */ 
    break;  
       
  case TIME_CLIENT_SET_CURRENT_TIME_UPDATE_NOTIFICATION:                 
   {
      ret = TimeClient_Set_Current_Time_Char_Notification(FALSE);
      if (ret == BLE_STATUS_SUCCESS) {
	TimeClientContext.state = TIME_CLIENT_WAIT_SET_CURRENT_TIME_UPDATE_NOTIFICATION;
      } else {
	TimeClient_FullConfError_CB(TIPC_ERROR_IN_ENABLE_NOTIFICATION, ret);
	TimeClientContext.state = TIME_CLIENT_DISCONNECTION;
      }
    }
    break; 
  case TIME_CLIENT_WAIT_SET_CURRENT_TIME_UPDATE_NOTIFICATION:             
     break;
    
  case TIME_CLIENT_DISCONNECTION:
    {
      ret = TimeClient_DeviceDisconnection();
      if (ret != BLE_STATUS_SUCCESS) {
	TimeClient_FullConfError_CB(TIPC_ERROR_IN_DISCONNECTION, ret);
      }
      TimeClientContext.state = TIME_CLIENT_INITIALIZED;
    }
    break;
  }
}

void TimeClient_DisplayCurrentTimeCharacteristicValue(tCurrentTime data_value)
{
  
  TIME_CLIENT_MESG_INFO(profiledbgfile,"year: %d, month: %d,date: %d, hours: %d, minutes: %d, seconds: %d, day_of_week: %d, fractions256: %d, adjustReason: %d\n",
                        data_value.year,data_value.month,data_value.date,
                        data_value.hours,data_value.minutes, data_value.seconds, 
                        data_value.day_of_week, data_value.fractions256, data_value.adjustReason);
}

void TimeClient_Decode_Read_Characteristic_Value(uint8_t data_length, uint8_t *data_value)
{ 
  if (TimeClientContext.state == TIME_CLIENT_WAIT_READ_CURRENT_TIME_VALUE)
  {
    TIME_CLIENT_MESG_INFO ( profiledbgfile, " length: %d (READING_CURRENT_TIME_INFO)\n", data_length);
    /* store read value */
    BLUENRG_memcpy(&(TimeClientContext.read_current_time_value), data_value,CURRENT_TIME_READ_SIZE);  
    
    /* Display read value according to the characteristic type */
    TimeClient_DisplayCurrentTimeCharacteristicValue(TimeClientContext.read_current_time_value);
    TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE;
  }
  else if (TimeClientContext.state == TIME_CLIENT_WAIT_READ_LOCAL_TIME_INFO)
  {
    TIME_CLIENT_MESG_INFO ( profiledbgfile, " length: %d (READING_LOCAL_TIME_INFO)\n", data_length);
    /* store read value */
    BLUENRG_memcpy(&(TimeClientContext.read_local_time_value), data_value,LOCAL_TIME_READ_SIZE); 
    /* Display read value according to the characteristic type */
    TIME_CLIENT_MESG_INFO(profiledbgfile,"timeZone: %d, dstOffset: %d\n",TimeClientContext.read_local_time_value.timeZone, TimeClientContext.read_local_time_value.dstOffset);
    TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE;
  }  
  else if (TimeClientContext.state == TIME_CLIENT_WAIT_READ_REFERENCE_TIME_INFO)
  {
    TIME_CLIENT_MESG_INFO ( profiledbgfile, " length: %d (READING_REFERENCE_TIME_INFO)\n", data_length);
    /* store read value */
    BLUENRG_memcpy(&(TimeClientContext.read_reference_time_value), data_value,REFERENCE_TIME_READ_SIZE);
    /* Display read value according to the characteristic type */
    TIME_CLIENT_MESG_INFO(profiledbgfile,"source: %d, accuracy: %d, daysSinceUpdate: %d,hoursSinceUpdate: %d \n",TimeClientContext.read_reference_time_value.source,
                          TimeClientContext.read_reference_time_value.accuracy,TimeClientContext.read_reference_time_value.daysSinceUpdate,TimeClientContext.read_reference_time_value.hoursSinceUpdate);
    TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE;
  }
  else if (TimeClientContext.state == TIME_CLIENT_WAIT_READ_NEXT_DST_CHANGE_TIME)
  {
    TIME_CLIENT_MESG_INFO ( profiledbgfile, " length: %d (READING_NEXT_DST_CHANGE_TIME_INFO)\n", data_length);
    /* store read value */
    BLUENRG_memcpy(&(TimeClientContext.read_dst_change_time_value), data_value, NEXT_DST_CHANGE_TIME_READ_SIZE); 
    /* Display read value according to the characteristic type */
    TIME_CLIENT_MESG_INFO(profiledbgfile,"year: %d, month: %d,date: %d, hours: %d, minutes: %d, seconds: %d, dstOffset: %d\n",
                        TimeClientContext.read_dst_change_time_value.year,TimeClientContext.read_dst_change_time_value.month,TimeClientContext.read_dst_change_time_value.date, TimeClientContext.read_dst_change_time_value.hours, 
                        TimeClientContext.read_dst_change_time_value.minutes, TimeClientContext.read_dst_change_time_value.seconds, TimeClientContext.read_dst_change_time_value.dstOffset);
    TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE;
  }
  else if (TimeClientContext.state == TIME_CLIENT_WAIT_READ_SERVER_TIME_UPDATE_STATUS)
  {
    TIME_CLIENT_MESG_INFO ( profiledbgfile, " length: %d (READING_SERVER_TIME_UPDATE_STATE)\n", data_length);
    /* store read value */
    BLUENRG_memcpy(&(TimeClientContext.read_server_time_update_status_value),data_value, SERVER_TIME_UPDATE_STATUS_READ_SIZE); 
    /* Display read value according to the characteristic type */
    TIME_CLIENT_MESG_INFO(profiledbgfile,"source: %d, result: %d\n",TimeClientContext.read_server_time_update_status_value.currentStatus, TimeClientContext.read_server_time_update_status_value.result);
    TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE;
  }  
}
 
/****************** time client Weak Callbacks definition ***************************/

WEAK_FUNCTION(void TimeClient_FullConfError_CB(uint8_t error_type, uint8_t code))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & TIME_CLIENT) */
