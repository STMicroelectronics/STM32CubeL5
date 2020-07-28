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
*   FILENAME        -  blood_pressure_collector_CB.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      17/02/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    Blood Pressure collector callbacks for profile device, services, 
*                characteristics discovery and connection, pairing procedures
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the  Blood Pressure collector callbacks handler.
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

#if (BLE_CURRENT_PROFILE_ROLES & BLOOD_PRESSURE_COLLECTOR)

#include <blood_pressure_service.h>
#include <blood_pressure_collector.h>
#include <blood_pressure_collector_config.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#ifndef BPC_TEST
#define BPC_TEST 1 //TBR 
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

/*******************************************************************************
* exported CallBacks
*******************************************************************************/  

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
    
    /* Blood Pressure Service Characteristics */
    if ((uuid_searched == BLOOD_PRESSURE_SERVICE_UUID) && 
	(uuid_charac == BLOOD_PRESSURE_MEASUREMENT_CHAR_UUID)) 
    {
      BLUENRG_memcpy(&bloodPressureCollectorContext.BPMeasurementCharValHandle, &charac[index+4], 2);
    } 
    
    else if ((uuid_searched == BLOOD_PRESSURE_SERVICE_UUID) && 
        (uuid_charac == INTERMEDIATE_CUFF_PRESSURE_CHARAC_UUID)) 
    {
      BLUENRG_memcpy(&bloodPressureCollectorContext.ICPCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == BLOOD_PRESSURE_SERVICE_UUID) && 
         (uuid_charac == BLOOD_PRESSURE_FEATURE_CHARAC)) 
    {
      BLUENRG_memcpy(&bloodPressureCollectorContext.BPFeatureCharValHandle, &charac[index+4], 2);
    } 
    
    /* Device Information Service Characteristics */
    else if ((uuid_searched == DEVICE_INFORMATION_SERVICE_UUID) && 
	     (uuid_charac == MANUFACTURER_NAME_UUID)) 
    {
      BLUENRG_memcpy(&bloodPressureCollectorContext.ManufacturerNameCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == DEVICE_INFORMATION_SERVICE_UUID) && 
              (uuid_charac == MODEL_NUMBER_UUID)) 
    {
      BLUENRG_memcpy(&bloodPressureCollectorContext.ModelNumberCharValHandle, &charac[index+4], 2);
    } 
    else if ((uuid_searched == DEVICE_INFORMATION_SERVICE_UUID) && 
             (uuid_charac == SYSTEM_ID_UUID)) 
    {
      BLUENRG_memcpy(&bloodPressureCollectorContext.SystemIDCharValHandle, &charac[index+4], 2);
    } 
    else 
    {
      ret = HCI_INVALID_PARAMETERS;
    }
    index += sizeElement;
  }

  return ret ;
}


void Master_BPC_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
			       uint8_t *data_length, uint8_t *data,
			       uint8_t *RSSI)
{
  uint8_t i;

  if (*status == DEVICE_DISCOVERED) 
  {
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Device Discovered ****\r\n");
    //CALLBACKS_MESG_DBG(profiledbgfile,"status = %d\r\n", *status);
    CALLBACKS_MESG_DBG(profiledbgfile,"ADDR Type = %d\r\n", *addr_type);
    CALLBACKS_MESG_DBG(profiledbgfile,"ADDR = 0x-");
  /*
    for (i=0; i<6; i++)
      CALLBACKS_MESG_DBG(profiledbgfile,"%02x", addr[i]);
    CALLBACKS_MESG_DBG(profiledbgfile,"\r\n");
 */   
    for (i=5; i>0; i--){
      CALLBACKS_MESG_DBG(profiledbgfile,"%02x-", addr[i]);
    }
    CALLBACKS_MESG_DBG(profiledbgfile,"%02X\r\n", addr[0]);
    
    CALLBACKS_MESG_DBG(profiledbgfile,"DATA (Length=%d):\r\n", *data_length);
    for (i=0; i<*data_length; i++)
      CALLBACKS_MESG_DBG(profiledbgfile,"%02x ", data[i]);
    CALLBACKS_MESG_DBG(profiledbgfile,"\r\n");
    CALLBACKS_MESG_DBG(profiledbgfile,"RSSI = 0x%02x\r\n", *RSSI);
    CALLBACKS_MESG_DBG(profiledbgfile, "**************************\r\n");
  }
  
  if (*status == DEVICE_DISCOVERY_PROCEDURE_ENDED) 
  {
    if (bloodPressureCollectorContext.state != BLOOD_PRESSURE_COLLECTOR_START_CONNECTION)
    {  
      bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_INITIALIZED;
    }
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure ended from the application\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) 
  {
    bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure Timeout\r\n");    
  }  

  BPC_DeviceDiscovery_CB(*status, *addr_type, addr, *data_length, data, *RSSI);
}/* end Master_DeviceDiscovery_CB() */

void Master_BPC_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
                          uint16_t *connection_handle, connUpdateParamType *param)
{
  
  switch (*connection_evt) {
  case DISCONNECTION_EVT:
    {
      bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_INITIALIZED;
      BPC_ConnectionStatus_CB(*connection_evt, *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Disconnection Event Received\r\n");
    }
    break;
  case CONNECTION_ESTABLISHED_EVT:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established with Success\r\n");
	bloodPressureCollectorContext.connHandle = *connection_handle;  
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_CONNECTED;
	BPC_ConnectionStatus_CB(*connection_evt, *status);
      }
    }
    break;
  case CONNECTION_FAILED_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Failed with Status 0x%02x\r\n", *status);
      bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_INITIALIZED;
      if (bloodPressureCollectorContext.fullConf)
	BPC_FullConfError_CB(BPC_ERROR_IN_CONNECTION, *status);
      else
	BPC_ConnectionStatus_CB(*connection_evt, *status);
    }
    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Stop Connection Procedure from the application\r\n");
      bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_INITIALIZED;
    }
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Request\r\n");
      bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_CONN_PARAM_UPDATE_REQ;
      BPC_ConnectionParameterUpdateReq_CB((bpcConnUpdateParamType*) param);
    }
    break;
  }
}/* end Master_Connection_CB() */


void Master_BPC_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
  switch(*procedure) {
  case PRIMARY_SERVICE_DISCOVERY:
    {
      uint16_t start_handle, end_handle;

      /* Verify if the Blood Pressure Service and Device Info  Service are both present */
      if (BPC_findHandles(BLOOD_PRESSURE_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	bloodPressureCollectorContext.BloodPressureServicePresent = TRUE;
      } else {
	bloodPressureCollectorContext.BloodPressureServicePresent = FALSE;
      }
      if (BPC_findHandles(DEVICE_INFORMATION_SERVICE_UUID, &start_handle, &end_handle) == BLE_STATUS_SUCCESS) {
	bloodPressureCollectorContext.DeviceInfoServicePresent = TRUE;
      } else {
	bloodPressureCollectorContext.DeviceInfoServicePresent = FALSE;
      }

      if (bloodPressureCollectorContext.fullConf) {
        /* start glucose measurement characteristic discovery */
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_CHARAC_DISCOVERY;
#if BPC_TEST
	BPC_ServicesDiscovery_CB(*status, bloodPressureCollectorContext.numpeerServices, 
				          bloodPressureCollectorContext.peerServices);
#endif
      } else {
	BPC_ServicesDiscovery_CB(*status, bloodPressureCollectorContext.numpeerServices, 
				           bloodPressureCollectorContext.peerServices);
      }
    }
    break;
  case GET_CHARACTERISTICS_OF_A_SERVICE:
    {
      CALLBACKS_MESG_DBG(profiledbgfile,"------ *status = 0x%04X, uuid_searched = 0x%04X, numCharac = 0x%04X\n", *status,bloodPressureCollectorContext.uuid_searched, bloodPressureCollectorContext.numCharac); //TBR???
      if (*status != BLE_STATUS_SUCCESS) {
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_DISCONNECTION;
	BPC_FullConfError_CB(BPC_ERROR_IN_CHARAC_DISCOVERY, *status);
      } else {
	findCharacHandle(bloodPressureCollectorContext.uuid_searched, 
			 bloodPressureCollectorContext.numCharac, 
			 bloodPressureCollectorContext.charac);
	if (bloodPressureCollectorContext.fullConf) {   //TBR 
	  switch (bloodPressureCollectorContext.uuid_searched) {
	  case BLOOD_PRESSURE_SERVICE_UUID:
	    if (bloodPressureCollectorContext.BloodPressureServicePresent && 
                (bloodPressureCollectorContext.state == BLOOD_PRESSURE_COLLECTOR_WAIT_CHARAC_DISCOVERY))
            {
	      bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_READ_BP_FEATURE;
	    }
	    break;
           
	  }
#if BPC_TEST
          if (bloodPressureCollectorContext.fullConf)
          {
            BPC_CharacOfService_CB(*status, bloodPressureCollectorContext.numCharac, bloodPressureCollectorContext.charac);
          }
#endif
	} else {
	  BPC_CharacOfService_CB(*status, bloodPressureCollectorContext.numCharac, bloodPressureCollectorContext.charac);
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
      BPC_CharacDesc_CB(*status, bloodPressureCollectorContext.numCharac, bloodPressureCollectorContext.charac);
    }
    break;
  }
}/* end Master_ServiceCharacPeerDiscovery_CB() */


void Master_BPC_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle, dataReceivedType *data)
{  
  switch(*procedure) {
  case NOTIFICATION_INDICATION_CHANGE_STATUS:
    if (bloodPressureCollectorContext.fullConf && 
        (bloodPressureCollectorContext.state == BLOOD_PRESSURE_COLLECTOR_WAIT_ENABLE_BP_MEASUREMENT_INDICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	BPC_FullConfError_CB(BPC_ERROR_IN_ENABLE_INDICATION, *status);
      }
      BPC_EnableIndication_CB(*status);
      bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_ENABLE_ICP_NOTIFICATION;
    } 
    else if (bloodPressureCollectorContext.fullConf && 
        (bloodPressureCollectorContext.state == BLOOD_PRESSURE_COLLECTOR_WAIT_ENABLE_ICP_NOTIFICATION))
    {
      if (*status != BLE_STATUS_SUCCESS) {
	BPC_FullConfError_CB(BPC_ERROR_IN_ENABLE_NOTIFICATION, *status);
      }
      BPC_EnableNotification_CB(*status);
      bloodPressureCollectorContext.fullConf = FALSE; //TBR
      bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_CONNECTED_IDLE; //TBR
    } 
    //BPC_EnableNotificationIndication_CB(*status);
    break;
  case READ_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Value Status (0x%02x)\r\n", *status);
    
    if (*status != BLE_STATUS_SUCCESS) 
    {
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_DISCONNECTION;
	BPC_FullConfError_CB(BPC_ERROR_IN_READ, *status);
    } 
    else 
    { 
      if (bloodPressureCollectorContext.dataLen == BP_FEATURE_READ_SIZE)
      {
        /* Store read Temperature Type characteristic value */      
        BPC_Read_BP_Feature_CB();
      }
      BPC_DataValueRead_CB(*status, bloodPressureCollectorContext.dataLen, bloodPressureCollectorContext.data); 
      
      if ((bloodPressureCollectorContext.fullConf) && (bloodPressureCollectorContext.state == BLOOD_PRESSURE_COLLECTOR_WAIT_READ_BP_FEATURE))
      {
        /* Start enable blood pressure measurement indication */
        bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_ENABLE_BP_MEASUREMENT_INDICATION;
      } 
      else 
        bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_DISCONNECTION; //TBR
    }
    break;
  case READ_LONG_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Long Value Status (0x%02x)\r\n", *status);
    break;
  /*
  case WRITE_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Write Value Status (0x%02x)\r\n", *status);
    // check write response status to the performed write request (APPL_WRITE_HR_CTRL_POINT_CHAR)
    BPC_Measurement_Interval_Check_Write_Response_CB(*status);
    break;
  */
  case NOTIFICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Data Received (0x%02x)\r\n", *status);
    if (*status == BLE_STATUS_SUCCESS)
    {
      /* Call user callback (display received notification value) */
      BPC_NotificationReceived_CB(data->attr_handle, data->data_length, &(data->data_value[0])); 
      
      /* Store the received Intermediate Temperature */
      BPC_ICP_Notification_CB(data->attr_handle, data->data_length, &(data->data_value[0]));         
    }
    break;
  case INDICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Indication Data Received (0x%02x)\r\n", *status);
    if (*status == BLE_STATUS_SUCCESS)
    {
      BPC_IndicationReceived_CB(data->attr_handle, data->data_length, &(data->data_value[0])); 
      
      if (data->attr_handle == bloodPressureCollectorContext.BPMeasurementCharValHandle)
      {
        /* Store the received Blood Pressure Measurement */
        BPC_BP_Measurement_Indication_CB(data->attr_handle, data->data_length, &(data->data_value[0]));      
      }
    }
    break;
  case ATTRIBUTE_MODIFICATION_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Attribute Modification Received (0x%02x)\r\n", *status);
    break;
  }  
  
}/* end Master_PeerDataExchange_CB() */


void Master_BPC_Pairing_CB(uint16_t *conn_handle, uint8_t *status)
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Pairing CB conn_handle = 0x%04x with status = 0x%02x***************\n", *conn_handle, *status);
  if (*status == BLE_STATUS_SUCCESS)
  {
    if (aci_gap_configure_whitelist()!= BLE_STATUS_SUCCESS) //TBR
        CALLBACKS_MESG_DBG(profiledbgfile,"Error during aci_gap_configure_whitelist() function call\r\n");
  }
  BPC_Pairing_CB(*conn_handle, *status);
  
  if (*status == PASS_KEY_REQUEST) {
    BPC_PinCodeRequired_CB();
  }
}
 
void Master_BPC_PeerDataPermitExchange_CB(uint8_t * procedure, uint16_t * connection_handle, dataPermitExchangeType * data) 
{
  CALLBACKS_MESG_DBG(profiledbgfile,"Conn handle: 0x%04x Procedure 0x%02x \n", *connection_handle, *procedure);
  
}/* end Master_PeerDataPermitExchange_CB() */


/****************** Blood Pressure collector Weak Callbacks definition ***************************/

WEAK_FUNCTION(void BPC_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
                                                   uint8_t data_length, uint8_t *data, 
					           uint8_t RSSI))
{
}

WEAK_FUNCTION(void BPC_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services))
{
}

WEAK_FUNCTION(void BPC_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status))
{
}

WEAK_FUNCTION(void BPC_ConnectionParameterUpdateReq_CB(bpcConnUpdateParamType *param))
{
}

WEAK_FUNCTION(void BPC_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void BPC_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void BPC_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data))
{
}

WEAK_FUNCTION(void BPC_PinCodeRequired_CB(void))
{
}

WEAK_FUNCTION(void BPC_EnableNotificationIndication_CB(uint8_t status))
{
}

WEAK_FUNCTION(void BPC_NotificationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value))
{
}

WEAK_FUNCTION(void BPC_IndicationReceived_CB(uint8_t handle, uint8_t length, uint8_t * data_value))
{
}

WEAK_FUNCTION(void BPC_Pairing_CB(uint16_t conn_handle, uint8_t status))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & BLOOD_PRESSURE_COLLECTOR) */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
