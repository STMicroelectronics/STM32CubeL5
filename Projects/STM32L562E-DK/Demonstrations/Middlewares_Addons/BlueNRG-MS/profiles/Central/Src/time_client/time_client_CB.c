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
*   FILENAME        -  time_client_CB.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/07/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    time client callbacks for profile device, services, 
*                characteristics discovery and connection, pairing procedures
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the  time client callbacks handler.
* 
******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
#include <host_config.h>
#include <hci.h>
#include <hci_le.h>
#include <debug.h>
#include <ble_list.h>
#include <timer.h>
#include <uuid.h>
#include "bluenrg_aci.h"
#include <master_basic_profile.h>

#if (BLE_CURRENT_PROFILE_ROLES & TIME_CLIENT)

#include <time_client.h>
#include <time_client_config.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#ifndef TIME_CLIENT_TEST
#define TIME_CLIENT_TEST 1 
#endif

#define WEAK_FUNCTION(x) __weak x
/******************************************************************************
* type definitions
******************************************************************************/

/******************************************************************************
* Variable Declarations
******************************************************************************/

/******************************************************************************
* Imported Variable
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/
static uint8_t findCharacHandle(uint16_t uuid_searched, uint8_t numCharac, uint8_t *charac)
{
  uint8_t ret, i, index, sizeElement;
  uint16_t uuid_charac;

  ret = BLE_STATUS_SUCCESS;

  index = 0;
  for (i=0; i<numCharac; i++) 
  { 
    if (charac[index] == 1) 
    {
      sizeElement = 8;
      BLUENRG_memcpy(&uuid_charac, &charac[index+6], 2);
    } 
    else 
    {
      sizeElement = 22;
    }

    /* Current time Service Characteristics */
    if ((uuid_searched == CURRENT_TIME_SERVICE_UUID) && 
	(uuid_charac == CURRENT_TIME_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&TimeClientContext.currentTimeCharValHandle, &charac[index+4], 2);
    } 
    
    else if ((uuid_searched == CURRENT_TIME_SERVICE_UUID) && 
        (uuid_charac == LOCAL_TIME_INFORMATION_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&TimeClientContext.localTimeInfoCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == CURRENT_TIME_SERVICE_UUID) && 
         (uuid_charac == REFERENCE_TIME_INFORMATION_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&TimeClientContext.refTimeInfoCharValHandle, &charac[index+4], 2);
    } 
    /* Next DST service  characteristics */
    else if ((uuid_searched == NEXT_DST_CHANGE_SERVICE_UUID) && 
        (uuid_charac == TIME_WITH_DST_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&TimeClientContext.timeWithDSTCharValHandle, &charac[index+4], 2);
    } 

    /* Time with DST Service Characteristics */
    else if ((uuid_searched == REFERENCE_UPDATE_TIME_SERVICE_UUID) && 
	     (uuid_charac == TIME_UPDATE_CONTROL_POINT_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&TimeClientContext.timeUpdateCtlPointCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == REFERENCE_UPDATE_TIME_SERVICE_UUID) && 
              (uuid_charac == TIME_UPDATE_STATE_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&TimeClientContext.timeUpdateStateCharValHandle, &charac[index+4], 2);
    } 
    else 
    {
      ret = HCI_INVALID_PARAMETERS;
    }
    index += sizeElement;
  }

  return ret ;
}
/*******************************************************************************
* exported CallBacks
*******************************************************************************/  

void Master_TC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
			       uint8_t *data_length, uint8_t *data, 
			       uint8_t *RSSI)
{
  uint8_t i;

  if (*status == DEVICE_DISCOVERED) {
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Device Discovered ****\r\n");
    CALLBACKS_MESG_DBG(profiledbgfile, "Addr Type = %d\r\n", *addr_type);
    CALLBACKS_MESG_DBG(profiledbgfile, "Addr = 0x");
    for (i=0; i<6; i++)
      CALLBACKS_MESG_DBG(profiledbgfile, "%02x", addr[i]);
    CALLBACKS_MESG_DBG(profiledbgfile, "\r\n");
    CALLBACKS_MESG_DBG(profiledbgfile, "Data (Length=%d):\r\n", *data_length);
    for (i=0; i<*data_length; i++)
      CALLBACKS_MESG_DBG(profiledbgfile, "%02x ", data[i]);
    CALLBACKS_MESG_DBG(profiledbgfile, "\r\n");
    CALLBACKS_MESG_DBG(profiledbgfile, "RSSI = 0x%02x\r\n", *RSSI);
    CALLBACKS_MESG_DBG(profiledbgfile, "**************************\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_ENDED) {
    if (TimeClientContext.state != TIME_CLIENT_START_CONNECTION)
      TimeClientContext.state = TIME_CLIENT_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure ended from the application\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) {
    TimeClientContext.state = TIME_CLIENT_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure Timeout\r\n");    
  }  

  TimeClient_DeviceDiscovery_CB(*status, *addr_type, addr, *data_length, data, *RSSI);
}

void Master_TC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
			  uint16_t *connection_handle, connUpdateParamType *param)
{
  switch (*connection_evt) {
  case DISCONNECTION_EVT:
    {
      TimeClientContext.state = TIME_CLIENT_INITIALIZED;
      TimeClient_ConnectionStatus_CB(*connection_evt, *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Disconnection Event Received\r\n");
    }
    break;
  case CONNECTION_ESTABLISHED_EVT:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established with Success\r\n");
	TimeClientContext.connHandle = *connection_handle;  
	TimeClientContext.state = TIME_CLIENT_CONNECTED;
	TimeClient_ConnectionStatus_CB(*connection_evt, *status);
        
        //aci_gatt_exchange_configuration(TimeClientContext.connHandle); //TBR
      }
    }
    break;
  case CONNECTION_FAILED_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Failed with Status 0x%02x\r\n", *status);
      TimeClientContext.state = TIME_CLIENT_INITIALIZED;
      if (TimeClientContext.fullConf)
	TimeClient_FullConfError_CB(TIPC_ERROR_IN_CONNECTION, *status);
      else
	TimeClient_ConnectionStatus_CB(*connection_evt, *status);
    }
    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Stop Connection Procedure from the application\r\n");
      TimeClientContext.state = TIME_CLIENT_INITIALIZED;
    }
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Request\r\n");
      TimeClientContext.state = TIME_CLIENT_CONN_PARAM_UPDATE_REQ;
      TimeClient_ConnectionParameterUpdateReq_CB((tipcConnUpdateParamType*) param);
    }
    break;
  }
}

void Master_TC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
  switch(*procedure) {
  case PRIMARY_SERVICE_DISCOVERY:
    {
      uint16_t start_handle, end_handle;

      /* Verify if the Current time, next DST Change and RTU Service are  present */
      if (TimeClient_findHandles(CURRENT_TIME_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	TimeClientContext.currentTimeServicePresent = TRUE;
      } else {
	TimeClientContext.currentTimeServicePresent = FALSE;
      }
      if (TimeClient_findHandles(NEXT_DST_CHANGE_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	TimeClientContext.nextDSTServicePresent = TRUE;
      } else {
	TimeClientContext.nextDSTServicePresent = FALSE;
      }
      if (TimeClient_findHandles(REFERENCE_UPDATE_TIME_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	TimeClientContext.RTUServicePresent = TRUE;
      } else {
	TimeClientContext.RTUServicePresent = FALSE;
      }
      
      if (TimeClientContext.fullConf) 
      {
        if (TimeClientContext.currentTimeServicePresent)
        {  
        /* start CTS characteristic discovery */
	TimeClientContext.state = TIME_CLIENT_CTS_CHARAC_DISCOVERY; 
#if TIME_CLIENT_TEST
	TimeClient_ServicesDiscovery_CB(*status, TimeClientContext.numpeerServices, 
				          TimeClientContext.peerServices);
#endif
        }
      } else {
	TimeClient_ServicesDiscovery_CB(*status, TimeClientContext.numpeerServices, 
				           TimeClientContext.peerServices);
      }
    }
    break;
  case GET_CHARACTERISTICS_OF_A_SERVICE:
    {
      CALLBACKS_MESG_DBG(profiledbgfile,"------ *status = 0x%04X, uuid_searched = 0x%04X, numCharac = 0x%04X\n", *status,TimeClientContext.uuid_searched, TimeClientContext.numCharac); 
      if (*status != BLE_STATUS_SUCCESS) {
	TimeClientContext.state = TIME_CLIENT_DISCONNECTION;
	TimeClient_FullConfError_CB(TIPC_ERROR_IN_CHARAC_DISCOVERY, *status);
      } else {
	findCharacHandle(TimeClientContext.uuid_searched, 
			 TimeClientContext.numCharac, 
			 TimeClientContext.charac);
	if (TimeClientContext.fullConf) {   
	  switch (TimeClientContext.uuid_searched) 
          {
            case CURRENT_TIME_SERVICE_UUID:
              if (TimeClientContext.currentTimeServicePresent &&   
                  TimeClientContext.state == TIME_CLIENT_WAIT_CTS_CHARAC_DISCOVERY)
              {
                /* set current time characteristic descriptor to 0x0000 */
                TimeClientContext.state = TIME_CLIENT_SET_CURRENT_TIME_UPDATE_NOTIFICATION;
              }
            break;
            case NEXT_DST_CHANGE_SERVICE_UUID:
              if (TimeClientContext.nextDSTServicePresent &&   
                  TimeClientContext.state == TIME_CLIENT_WAIT_NEXT_DSTS_CHARAC_DISCOVERY)
              {
                if (TimeClientContext.RTUServicePresent) 
                {
                  TimeClientContext.state = TIME_CLIENT_RTUS_CHARAC_DISCOVERY;
                }
                else /* no Next  RTU service is  present */
                {
                  TimeClientContext.fullConf = FALSE; 
                  TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE; 
                }
              }
            break;
            case REFERENCE_UPDATE_TIME_SERVICE_UUID:
              if (TimeClientContext.RTUServicePresent &&   
                  TimeClientContext.state == TIME_CLIENT_WAIT_RTUS_CHARAC_DISCOVERY)
              {
                 TimeClientContext.fullConf = FALSE; 
                 TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE; 
              }
            break;
	  }
#if TIME_CLIENT_TEST
          if (TimeClientContext.fullConf)
          {
            TimeClient_CharacOfService_CB(*status, TimeClientContext.numCharac, TimeClientContext.charac); 
          }
#endif
	} else {
	  TimeClient_CharacOfService_CB(*status, TimeClientContext.numCharac, TimeClientContext.charac);
	}
      }
    }
    break;
  case FIND_INCLUDED_SERVICES:
    {
    }
    break;
  case FIND_CHARAC_DESCRIPTORS:
    {
      TimeClient_CharacDesc_CB(*status, TimeClientContext.numCharac, TimeClientContext.charac);
    }
    break;
  }
}

void Master_TC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				uint16_t *connection_handle, dataReceivedType *data)
{

  switch(*procedure) {
  case NOTIFICATION_INDICATION_CHANGE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Indication Change Status (0x%02x)\r\n", *status);
    
    if (TimeClientContext.fullConf && 
        (TimeClientContext.state == TIME_CLIENT_WAIT_SET_CURRENT_TIME_UPDATE_NOTIFICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	TimeClient_FullConfError_CB(TIPC_ERROR_IN_WRITE_PROCEDURE, *status);
        TimeClientContext.state = TIME_CLIENT_DISCONNECTION; //TBR??
      }
      else
      {
        if (TimeClientContext.nextDSTServicePresent)
        {
          TimeClientContext.state = TIME_CLIENT_NEXT_DSTS_CHARAC_DISCOVERY;
        }
        else if (TimeClientContext.RTUServicePresent)
        {
          TimeClientContext.state = TIME_CLIENT_RTUS_CHARAC_DISCOVERY;
        }
        else /* no Next DST, RTU services are present */
        {
          TimeClientContext.fullConf = FALSE; 
          TimeClientContext.state = TIME_CLIENT_CONNECTED_IDLE; 
        }
      }
    } 
    /* Call user callback */
    TimeClient_EnableNotification_CB(*status);
    break;
  case READ_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Value Status (0x%02x)\r\n", *status);
    
    if (*status != BLE_STATUS_SUCCESS) 
    {
	TimeClientContext.state = TIME_CLIENT_DISCONNECTION;
	TimeClient_FullConfError_CB(TIPC_ERROR_IN_READ, *status);
    } 
    else 
    {
      /* Provide read data to the application */
      TimeClient_DataValueRead_CB(*status, TimeClientContext.dataLen, TimeClientContext.data); 
      
      /*Decode and store the received read value */
      TimeClient_Decode_Read_Characteristic_Value(TimeClientContext.dataLen, &(TimeClientContext.data[0]));
    }
    break;
  case READ_LONG_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Long Value Status (0x%02x)\r\n", *status);
    break;
  case WRITE_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Write Value Status (0x%02x)\r\n", *status);
    break;
  case NOTIFICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Data Received (0x%02x)\r\n", *status);
    if (*status == BLE_STATUS_SUCCESS)
    {
      if (data->attr_handle == TimeClientContext.currentTimeCharValHandle)
      {
        /* Call user callback (it provides the  received notification value) */
        TimeClient_NotificationReceived_CB(data->attr_handle, data->data_length, &(data->data_value[0])); 
        /* Store notified value */
        BLUENRG_memcpy(&(TimeClientContext.notified_current_time_value), &(data->data_value[0]),CURRENT_TIME_READ_SIZE); 
        /* Display the current time values */
        TimeClient_DisplayCurrentTimeCharacteristicValue(TimeClientContext.notified_current_time_value);
      }
    }
    break;
  case INDICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile,"INDICATION_DATA_RECEIVED (0x%02x): handle=0x%04X, data length=%d\n", *status, data->attr_handle,data->data_length);
   
    break;
  case ATTRIBUTE_MODIFICATION_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Attribute Modification Received (0x%02x)\r\n", *status);
    break;
  }

}

void Master_TC_Pairing_CB(uint16_t *conn_handle, uint8_t *status)
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Pairing CB conn_handle = 0x%04x with status = 0x%02x***************\n", *conn_handle, *status);
  if (*status == BLE_STATUS_SUCCESS)
  {
    if (aci_gap_configure_whitelist()!= BLE_STATUS_SUCCESS) //TBR
        CALLBACKS_MESG_DBG(profiledbgfile,"Error during aci_gap_configure_whitelist() function call\r\n");
  }
  TimeClient_Pairing_CB(*conn_handle, *status);
  
  if (*status == PASS_KEY_REQUEST) {
    TimeClient_PinCodeRequired_CB();
  }
}

void Master_TC_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data)
{
  // Nothing to do 
}
 
/****************** time client Weak Callbacks definition ***************************/

WEAK_FUNCTION(void TimeClient_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
                                                   uint8_t data_length, uint8_t *data, 
					           uint8_t RSSI))
{
}

WEAK_FUNCTION(void TimeClient_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services))
{
}

WEAK_FUNCTION(void TimeClient_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status))
{
}

WEAK_FUNCTION(void TimeClient_ConnectionParameterUpdateReq_CB(tipcConnUpdateParamType *param))
{
}

WEAK_FUNCTION(void TimeClient_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void TimeClient_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void TimeClient_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data))
{
}

WEAK_FUNCTION(void TimeClient_PinCodeRequired_CB(void))
{
}

WEAK_FUNCTION(void TimeClient_EnableNotification_CB(uint8_t status))
{
}

WEAK_FUNCTION(void TimeClient_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value))
{
}

WEAK_FUNCTION(void TimeClient_Pairing_CB(uint16_t conn_handle, uint8_t status))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & TIME_CLIENT) */
