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
*   FILENAME        -  find_me_target_CB.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/16/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    find me target central role callbacks for profile device, services, 
*                characteristics discovery and connection, pairing procedures
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the find me target central role callbacks handler.
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
#include <master_basic_profile.h>

#if (BLE_CURRENT_PROFILE_ROLES & FIND_ME_TARGET)

#include <find_me_target.h>
#include <find_me_target_config.h>

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

/******************************************************************************
* Imported Variable
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*******************************************************************************
* exported CallBacks
*******************************************************************************/  

void Master_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
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
    if (find_me_target_context.state != FIND_ME_TARGET_START_CONNECTION)
      find_me_target_context.state = FIND_ME_TARGET_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure ended from the application\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) {
    find_me_target_context.state = FIND_ME_TARGET_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure Timeout\r\n");    
  }  

  FMT_DeviceDiscovery_CB(*status, *addr_type, addr, *data_length, data, *RSSI);
}

void Master_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
			  uint16_t *connection_handle, connUpdateParamType *param)
{
  switch (*connection_evt) {
  case DISCONNECTION_EVT:
    {
      find_me_target_context.state = FIND_ME_TARGET_INITIALIZED;
      
      FMT_ConnectionStatus_CB(*connection_evt, *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Disconnection Event Received\r\n");
    }
    break;
  case CONNECTION_ESTABLISHED_EVT:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established with Success\r\n");
	find_me_target_context.connHandle = *connection_handle;  
	find_me_target_context.state = FIND_ME_TARGET_CONNECTED;
	FMT_ConnectionStatus_CB(*connection_evt, *status);
      }
    }
    break;
  case CONNECTION_FAILED_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Failed with Status 0x%02x\r\n", *status);
      find_me_target_context.state = FIND_ME_TARGET_INITIALIZED;
#if 0 //TBR??
      if (find_me_target_context.fullConf)
	FMT_FullConfError_CB(FMT_ERROR_IN_CONNECTION, *status);
      else
#endif
	FMT_ConnectionStatus_CB(*connection_evt, *status);
    }
    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Stop Connection Procedure from the application\r\n");
      find_me_target_context.state = FIND_ME_TARGET_INITIALIZED;
    }
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Request\r\n");
      find_me_target_context.state = FIND_ME_TARGET_CONN_PARAM_UPDATE_REQ;
      FMT_ConnectionParameterUpdateReq_CB((fmtConnUpdateParamType*) param);
    }
    break;
  }
}

void Master_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
}

void Master_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle, dataReceivedType *data)
{ 
  CALLBACKS_MESG_DBG(profiledbgfile,"Conn handle: 0x%04x Procedure 0x%02x Ended with status = 0x%02x\n", *connection_handle, *procedure, *status);
  
  if (*procedure == NOTIFICATION_INDICATION_CHANGE_STATUS) 
  {
    CALLBACKS_MESG_DBG(profiledbgfile,"Master_PeerDataExchange_CB(), event: NOTIFICATION_INDICATION_CHANGE_STATUS\r\n");
  }

  if ((*procedure == READ_VALUE_STATUS) ||  
      (*procedure == READ_LONG_VALUE_STATUS)) 
  {
    CALLBACKS_MESG_DBG(profiledbgfile,"Read Procedure %d= \r\n", *procedure);
  }

  if (*procedure == WRITE_VALUE_STATUS) 
  {
    CALLBACKS_MESG_DBG(profiledbgfile,"WRITE_VALUE_STATUS, Write Status = 0x%02x\r\n", *status);
  }

  if (*procedure == NOTIFICATION_DATA_RECEIVED) 
  {
    CALLBACKS_MESG_DBG(profiledbgfile,"NOTIFICATION_DATA_RECEIVED: handle=0x%04x, data length=%d, data:\r\n",
	   data->attr_handle, data->data_length);      
  }
  
  if (*procedure == INDICATION_DATA_RECEIVED ) 
  {
    
    CALLBACKS_MESG_DBG(profiledbgfile,"INDICATION_DATA_RECEIVED: handle=0x%04X, data length=%d\n",data->attr_handle,data->data_length);
  }
  
  if (*procedure == ATTRIBUTE_MODIFICATION_RECEIVED)
  {
    CALLBACKS_MESG_DBG(profiledbgfile,"ATTRIBUTE_MODIFICATION_RECEIVED: handle=0x%04X, data length=%d\n",data->attr_handle,data->data_length);
    for (uint8_t i=0; i<data->data_length; i++) {
      CALLBACKS_MESG_DBG(profiledbgfile,"%02x ", data->data_value[i]);
    }
    CALLBACKS_MESG_DBG(profiledbgfile,"\r\n");
    if(data->attr_handle == (find_me_target_context.AlertLevelCharHandle + 1))
    {
      /* This event is received only when the client writes to the Alert Level 
         characteristic */ 
      
      /* Store the received alert level */
      find_me_target_context.alert_level_value = data->data_value[0];
      CALLBACKS_MESG_DBG(profiledbgfile,"Received alert level:%01x ", data->data_value[0]);
      /* Call user callback */
      FMT_Alert_Level_Value_CB(find_me_target_context.alert_level_value);
    }
  }
  
}/* end Master_PeerDataExchange_CB() */

void Master_Pairing_CB(uint16_t *conn_handle, uint8_t *status)
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Pairing CB conn_handle = 0x%04x with status = 0x%02x***************\n", *conn_handle, *status);
  if (*status == BLE_STATUS_SUCCESS)
  {
    if (aci_gap_configure_whitelist()!= BLE_STATUS_SUCCESS) //TBR
        CALLBACKS_MESG_DBG(profiledbgfile,"Error during aci_gap_configure_whitelist() function call\r\n");
  }
  FMT_Pairing_CB(*conn_handle, *status);
  if (*status == PASS_KEY_REQUEST) {
     FMT_PinCodeRequired_CB();
  }
}/* end Master_Pairing_CB() */

void Master_PeerDataPermitExchange_CB(uint8_t * procedure, uint16_t * connection_handle, dataPermitExchangeType * data) 
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Conn handle: 0x%04x Procedure 0x%02x \n", *connection_handle, *procedure);
}/* end Master_PeerDataPermitExchange_CB() */

/******************  Weak Callbacks definition ***************************/

WEAK_FUNCTION(void FMT_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
                                                   uint8_t data_length, uint8_t *data, 
					           uint8_t RSSI))
{
}

WEAK_FUNCTION(void FMT_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status))
{
}

WEAK_FUNCTION(void FMT_ConnectionParameterUpdateReq_CB(fmtConnUpdateParamType *param))
{
}

WEAK_FUNCTION(void FMT_PinCodeRequired_CB(void))
{
}

WEAK_FUNCTION(void FMT_Pairing_CB(uint16_t conn_handle, uint8_t status))
{
}

WEAK_FUNCTION(void FMT_Alert_Level_Value_CB(uint8_t alert_level))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & FIND_ME_TARGET) */
