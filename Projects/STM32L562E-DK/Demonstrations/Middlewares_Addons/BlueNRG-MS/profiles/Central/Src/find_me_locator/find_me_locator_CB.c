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
*   FILENAME        -  find_me_locator_CB.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/26/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    find me locator callbacks for profile device, services, 
*                characteristics discovery and connection, pairing procedures
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the find me locator callbacks handler.
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
#include <master_basic_profile.h>

#if (BLE_CURRENT_PROFILE_ROLES & FIND_ME_LOCATOR)

#include <find_me_locator.h>
#include <find_me_locator_config.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#ifndef FML_TEST
#define FML_TEST 1 //TBR 
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

    /* Alert Level Characteristic */
    if ((uuid_searched == IMMEDIATE_ALERT_SERVICE_UUID) && 
	(uuid_charac == ALERT_LEVEL_CHARACTERISTIC_UUID)) 
    {
      BLUENRG_memcpy(&findMeLocatorContext.AlertLevelCharValHandle, &charac[index+4], 2);
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

void Master_FML_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
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
    if (findMeLocatorContext.state != FIND_ME_LOCATOR_START_CONNECTION)
      findMeLocatorContext.state = FIND_ME_LOCATOR_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure ended from the application\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) {
    findMeLocatorContext.state = FIND_ME_LOCATOR_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure Timeout\r\n");    
  }  

  FML_DeviceDiscovery_CB(*status, *addr_type, addr, *data_length, data, *RSSI);
}

void Master_FML_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
			  uint16_t *connection_handle, connUpdateParamType *param)
{
  switch (*connection_evt) {
  case DISCONNECTION_EVT:
    {
      findMeLocatorContext.state = FIND_ME_LOCATOR_INITIALIZED;
      FML_ConnectionStatus_CB(*connection_evt, *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Disconnection Event Received\r\n");
    }
    break;
  case CONNECTION_ESTABLISHED_EVT:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established with Success\r\n");
	findMeLocatorContext.connHandle = *connection_handle;  
	findMeLocatorContext.state = FIND_ME_LOCATOR_CONNECTED;
	FML_ConnectionStatus_CB(*connection_evt, *status);
        
        /* write alert level now */
        //FML_ALert_Target(findMeLocatorContext.requested_alert_level);
      }
    }
    break;
  case CONNECTION_FAILED_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Failed with Status 0x%02x\r\n", *status);
      findMeLocatorContext.state = FIND_ME_LOCATOR_INITIALIZED;
      if (findMeLocatorContext.fullConf)
	FML_FullConfError_CB(FML_ERROR_IN_CONNECTION, *status);
      else
	FML_ConnectionStatus_CB(*connection_evt, *status);
    }
    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Stop Connection Procedure from the application\r\n");
      findMeLocatorContext.state = FIND_ME_LOCATOR_INITIALIZED;
    }
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Request\r\n");
      findMeLocatorContext.state = FIND_ME_LOCATOR_CONN_PARAM_UPDATE_REQ;
      FML_ConnectionParameterUpdateReq_CB((fmlConnUpdateParamType*) param);
    }
    break;
  }
}

void Master_FML_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
  switch(*procedure) {
  case PRIMARY_SERVICE_DISCOVERY:
    {
      uint16_t start_handle, end_handle;

      /* Verify if the Immediate Alert Service is present */
      if (FML_findHandles(IMMEDIATE_ALERT_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	findMeLocatorContext.IASServicePresent = TRUE;
      } else {
	findMeLocatorContext.IASServicePresent = FALSE;
      }

      if (findMeLocatorContext.fullConf) {
        /* start alert level characteristic discovery */
	findMeLocatorContext.state = FIND_ME_LOCATOR_ALERT_LEVEL_CHARAC_DISCOVERY;
#if FML_TEST
	FML_ServicesDiscovery_CB(*status, findMeLocatorContext.numpeerServices, 
				          findMeLocatorContext.peerServices);
#endif
      } else {
	FML_ServicesDiscovery_CB(*status, findMeLocatorContext.numpeerServices, 
				           findMeLocatorContext.peerServices);
      }
    }
    break;
  case GET_CHARACTERISTICS_OF_A_SERVICE:
    {
      if (*status != BLE_STATUS_SUCCESS) {
	findMeLocatorContext.state = FIND_ME_LOCATOR_DISCONNECTION;
	FML_FullConfError_CB(FML_ERROR_IN_CHARAC_DISCOVERY, *status);
      } else {
	findCharacHandle(findMeLocatorContext.uuid_searched, 
			 findMeLocatorContext.numCharac, 
			 findMeLocatorContext.charac);
	if (findMeLocatorContext.fullConf) {   //TBR 
	  switch (findMeLocatorContext.uuid_searched) {
	  case IMMEDIATE_ALERT_SERVICE_UUID:
	    if (findMeLocatorContext.IASServicePresent  && 
                (findMeLocatorContext.state == FIND_ME_LOCATOR_WAIT_ALERT_LEVEL_CHARAC_DISCOVERY))
            {
              findMeLocatorContext.fullConf = FALSE; //TBR
              findMeLocatorContext.state = FIND_ME_LOCATOR_CONNECTED_IDLE; //TBR
	    }
	    break;
	  }
#if FML_TEST
          if (findMeLocatorContext.fullConf)
          {
            FML_CharacOfService_CB(*status, findMeLocatorContext.numCharac, findMeLocatorContext.charac);
          }
#endif
	} else {
	  FML_CharacOfService_CB(*status, findMeLocatorContext.numCharac, findMeLocatorContext.charac);
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
    }
    break;
  }
}
void Master_FML_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				uint16_t *connection_handle, dataReceivedType *data)
{

  switch(*procedure) {
  case NOTIFICATION_INDICATION_CHANGE_STATUS:
   
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Indication Change Status (0x%02x)\r\n", *status);
    break;
  case READ_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Value Status (0x%02x)\r\n", *status);
    break;
  case READ_LONG_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Long Value Status (0x%02x)\r\n", *status);
    break;
  case WRITE_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Write Value Status (0x%02x)\r\n", *status);
    
    break;
  case NOTIFICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Data Received (0x%02x)\r\n", *status);
    
    break;
  case INDICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile,"INDICATION_DATA_RECEIVED (0x%02x): handle=0x%04X, data length=%d\n", *status, data->attr_handle,data->data_length);
    
    break;
  case ATTRIBUTE_MODIFICATION_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Attribute Modification Received (0x%02x)\r\n", *status);
    break;
  }

}


void Master_FML_Pairing_CB(uint16_t *conn_handle, uint8_t *status)
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Pairing CB conn_handle = 0x%04x with status = 0x%02x***************\n", *conn_handle, *status);
  if (*status == BLE_STATUS_SUCCESS)
  {
    if (aci_gap_configure_whitelist()!= BLE_STATUS_SUCCESS) //TBR
        CALLBACKS_MESG_DBG(profiledbgfile,"Error during aci_gap_configure_whitelist() function call\r\n");
  }
  FML_Pairing_CB(*conn_handle, *status);
  
  if (*status == PASS_KEY_REQUEST) {
    FML_PinCodeRequired_CB();
  }
}

void Master_FML_PeerDataPermitExchange_CB(uint8_t * procedure, uint16_t * connection_handle, dataPermitExchangeType * data) 
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Conn handle: 0x%04x Procedure 0x%02x \n", *connection_handle, *procedure);
}/* end Master_PeerDataPermitExchange_CB() */

/****************** glucose collector Weak Callbacks definition ***************************/

WEAK_FUNCTION(void FML_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
                                                   uint8_t data_length, uint8_t *data, 
					           uint8_t RSSI))
{
}

WEAK_FUNCTION(void FML_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services))
{
}

WEAK_FUNCTION(void FML_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status))
{
}

WEAK_FUNCTION(void FML_ConnectionParameterUpdateReq_CB(fmlConnUpdateParamType *param))
{
}

WEAK_FUNCTION(void FML_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void FML_PinCodeRequired_CB(void))
{
}

WEAK_FUNCTION(void FML_Pairing_CB(uint16_t conn_handle, uint8_t status))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & FIND_ME_LOCATOR) */

