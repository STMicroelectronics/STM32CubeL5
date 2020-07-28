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
*   FILENAME        -  glucose_collector.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/01/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    glucose collector APIs(connect, config charac, enable notification...)
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the glucose collector profile central role.
* 
******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
#include "debug.h"
#include "timer.h"
#include "uuid.h"
#include "hci_le.h"
#include "hal.h"
#include "master_basic_profile.h"

#if (BLE_CURRENT_PROFILE_ROLES & GLUCOSE_COLLECTOR)

#include "string.h"
#include "bluenrg_gatt_server.h"
#include "glucose_service.h"
#include "glucose_collector.h"
#include "glucose_collector_config.h" 
#include "glucose_collector_racp_CB.h"

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
/* racp characteristic value */
static uint8_t racp_charval[RACP_COMMAND_MAX_LEN]; 

/* glucose collector context */
glucoseCollectorContext_Type glucoseCollectorContext;

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

tBleStatus GL_Collector_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle)
{
  uint8_t i, j;
  uint16_t uuid;

  j = 0;
  for (i=0; i<glucoseCollectorContext.numpeerServices; i++) {
    if (glucoseCollectorContext.peerServices[j] == UUID_TYPE_16) {
      BLUENRG_memcpy(&uuid, &glucoseCollectorContext.peerServices[j+5], 2);
      j++;
      BLUENRG_memcpy(start_handle, &glucoseCollectorContext.peerServices[j], 2);
      j += 2;
      BLUENRG_memcpy(end_handle, &glucoseCollectorContext.peerServices[j], 2);
      j += 2;
      if (glucoseCollectorContext.peerServices[j-5] == UUID_TYPE_16) {
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

tBleStatus GL_Collector_Init(glcInitDevType param)
{
  //initDevType initParam;
  uint8_t ret;

  BLUENRG_memcpy(&glucoseCollectorContext, 0, sizeof(glucoseCollectorContext_Type));//TBR
  
  glucoseCollectorContext.Is_Write_Ongoing = FALSE;
  glucoseCollectorContext.MeasurementContext_Is_Expected = FALSE;
 
  glucoseCollectorContext.number_of_notifications_for_racp = 0;
  glucoseCollectorContext.start_racp_notifications_analyis = 0; 
  glucoseCollectorContext.racp_notifications_index = 0;
  
  glucoseCollectorContext.number_of_measurement_notifications = 0;  //TBR
  glucoseCollectorContext.number_of_measurement_context_notifications = 0;  //TBR
  
  glucoseCollectorContext.glucose_feature = 0;
  
  glucoseCollectorContext.racp_notifications_postprocessig_is_enabled = param.racp_post_processing; 
  glucoseCollectorContext.max_num_of_single_racp_notifications = param.max_num_of_single_racp_notifications;
  
  BLUENRG_memcpy(glucoseCollectorContext.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  initParam.Master_DeviceDiscovery_CB = Master_GC_DeviceDiscovery_CB;
  initParam.Master_Connection_CB = Master_GC_Connection_CB;
  initParam.Master_Pairing_CB = Master_GC_Pairing_CB;
  initParam.Master_PeerDataExchange_CB = Master_GC_PeerDataExchange_CB;
  initParam.Master_PeerDataPermitExchange_CB = Master_GC_PeerDataPermitExchange_CB;
  initParam.Master_ServiceCharacPeerDiscovery_CB = Master_GC_ServiceCharacPeerDiscovery_CB;
  
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    glucoseCollectorContext.state = GLUCOSE_COLLECTOR_INITIALIZED;
    glucoseCollectorContext.fullConf = FALSE;
    //PROFILE_MESG_DBG(profiledbgfile,"**** Device initialized\r\n"); //TBR
  } else {
    glucoseCollectorContext.state = GLUCOSE_COLLECTOR_UNINITIALIZED;
    //PROFILE_MESG_DBG(profiledbgfile,"**** Device not initialized\r\n"); //TBR
  }

  return ret;
}

tBleStatus GL_Collector_DeviceDiscovery(glcDevDiscType param)
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
    glucoseCollectorContext.state = GLUCOSE_COLLECTOR_DEVICE_DISCOVERY;
    //PROFILE_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure started\n"); //TBR
  }
  return ret;
}

tBleStatus GL_Collector_SecuritySet(glcSecurityType param)
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

tBleStatus GL_Collector_DeviceConnection(glcConnDevType param)
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
    glucoseCollectorContext.state = GLUCOSE_COLLECTOR_START_CONNECTION;
  }

  return ret;
}

tBleStatus GL_Collector_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(glucoseCollectorContext.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    glucoseCollectorContext.fullConf = FALSE;//TBR-----------
  }
  return ret;
}

tBleStatus GL_Collector_ConnectionParameterUpdateRsp(uint8_t accepted, glcConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(glucoseCollectorContext.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus GL_Collector_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(glucoseCollectorContext.connHandle, FALSE);
  return ret;
}

tBleStatus GL_Collector_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end GL_Collector_Clear_Security_Database() */

tBleStatus GL_Collector_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(glucoseCollectorContext.connHandle, pinCode);
  
  return ret;
}

tBleStatus GL_Collector_ServicesDiscovery(void)
{
  uint8_t ret;

  ret = Master_GetPrimaryServices(glucoseCollectorContext.connHandle, 
				  &glucoseCollectorContext.numpeerServices, 
				  glucoseCollectorContext.peerServices, 
				  PRIMARY_SERVICES_ARRAY_SIZE);
  if (ret == BLE_STATUS_SUCCESS)
  {
    glucoseCollectorContext.state = GLUCOSE_COLLECTOR_SERVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus GL_Collector_DiscCharacServ(uint16_t uuid_service)
{
  uint8_t ret;
  uint16_t start_handle, end_handle;

  if ((uuid_service == GLUCOSE_SERVICE_UUID) || (uuid_service == DEVICE_INFORMATION_SERVICE_UUID)) {
    if (!(glucoseCollectorContext.glucoseServicePresent && glucoseCollectorContext.DeviceInfoServicePresent))
      return HCI_INVALID_PARAMETERS;
  }

  ret = GL_Collector_findHandles(uuid_service, &start_handle, &end_handle);
  if (ret == BLE_STATUS_SUCCESS) {
    ret = Master_GetCharacOfService(glucoseCollectorContext.connHandle,
				    start_handle,
				    end_handle,
				    &glucoseCollectorContext.numCharac,
				    glucoseCollectorContext.charac,
				    CHARAC_OF_SERVICE_ARRAY_SIZE);

    if (ret == BLE_STATUS_SUCCESS)
    {
      glucoseCollectorContext.uuid_searched = uuid_service;
    }
  }
  return ret;
}

tBleStatus GL_Collector_Start_Glucose_Measurement_Characteristic_Descriptor_Discovery(void)
{
  tBleStatus ret;
  
  /* It discovers the characteristic descriptors of the connected Glucose Sensor Measurement Characteristic */
  ret= Master_GetCharacDescriptors(glucoseCollectorContext.connHandle,
                                   glucoseCollectorContext.MeasurementCharValHandle, /* glucose sensor measurement chararacteristic: Value Handle */
                                   glucoseCollectorContext.MeasurementCharValHandle+1, /* glucose sensor measurement chararacteristic: End Handle */
                                   &glucoseCollectorContext.numCharac,
                                   glucoseCollectorContext.charac,
                                   CHARAC_OF_SERVICE_ARRAY_SIZE);
  return(ret);
}/* end GL_Collector_Start_Glucose_Measurement_Characteristic_Descriptor_Discovery() */

tBleStatus GL_Collector_Start_Glucose_Measurement_Context_Characteristic_Descriptor_Discovery(void)
{
  tBleStatus ret;
  
  /* It discovers the characteristic descriptors of the connected Glucose Sensor Measurement Context Characteristic */
  ret= Master_GetCharacDescriptors(glucoseCollectorContext.connHandle,
                                   glucoseCollectorContext.MeasurementContextCharValHandle, /* glucose sensor measurement context chararacteristic: Value Handle */
                                   glucoseCollectorContext.MeasurementContextCharValHandle+1, /* glucose sensor measurement context chararacteristic: End Handle */
                                   &glucoseCollectorContext.numCharac,
                                   glucoseCollectorContext.charac,
                                   CHARAC_OF_SERVICE_ARRAY_SIZE);
 
  return(ret);
}/* end GL_Collector_Start_Glucose_Measurement_Context_Characteristic_Descriptor_Discovery() */

tBleStatus GL_Collector_Start_RACP_Characteristic_Descriptor_Discovery(void)
{
  uint8_t ret;

  ret = Master_GetCharacDescriptors(glucoseCollectorContext.connHandle,
				    glucoseCollectorContext.RACPCharValHandle,
				    glucoseCollectorContext.RACPCharValHandle +1,
				    &glucoseCollectorContext.numCharac,
				    glucoseCollectorContext.charac,
				    CHARAC_OF_SERVICE_ARRAY_SIZE);

  return ret;
}/* end GL_Collector_Start_RACP_Characteristic_Descriptor_Discovery(); */

tBleStatus GL_Collector_ReadFeatureChar(void)
{
  tBleStatus ret; 
  /* Read glucose feature Characteristic */
  ret = Master_Read_Value(glucoseCollectorContext.connHandle,  
                          glucoseCollectorContext.FeatureCharValHandle,  
                          &glucoseCollectorContext.dataLen,  
                          glucoseCollectorContext.data,  
                          GLUCOSE_READ_SIZE 
                          );
  return ret;
}/* end GL_Collector_ReadFeatureChar() */

tBleStatus GL_Collector_ReadDISManufacturerNameChar(void)
{
  tBleStatus ret;

  /* Read Device Info Manufacturer Name Characteristic */
  ret = Master_Read_Value(glucoseCollectorContext.connHandle,  
                          glucoseCollectorContext.ManufacturerNameCharValHandle,  
                          &glucoseCollectorContext.dataLen,  
                          glucoseCollectorContext.data,  
                          GLUCOSE_DIS_MANUFACTURER_NAME_SIZE
                          );
  
  return ret;
}/* end GL_Collector_ReadDISManufacturerNameChar() */

tBleStatus GL_Collector_ReadDISModelNumberChar(void)
{
  tBleStatus ret;
  
  /* Read Device Info Model Number Characteristic */
  ret = Master_Read_Value(glucoseCollectorContext.connHandle,  
                          glucoseCollectorContext.ModelNumberCharValHandle,  
                          &glucoseCollectorContext.dataLen,  
                          glucoseCollectorContext.data,  
                          GLUCOSE_DIS_MODEL_NUMBER_SIZE 
                          );
 return ret; 
}/* end GL_Collector_ReadDISModelNumberChar() */

tBleStatus GL_Collector_ReadDISSystemIDChar(void)
{
  tBleStatus ret;
  
  /* Read Device Info System ID Characteristic */
  ret = Master_Read_Value(glucoseCollectorContext.connHandle,  
                          glucoseCollectorContext.SystemIDCharValHandle,  
                          &glucoseCollectorContext.dataLen,  
                          glucoseCollectorContext.data,  
                          GLUCOSE_DIS_SYSTEM_ID_SIZE 
                          );
  return ret; 
}/* end GL_Collector_ReadDISSystemIDChar() */
    
tBleStatus GL_Collector_Enable_Glucose_Measurement_Char_Notification(void)
{
  tBleStatus ret; 
  ret = Master_NotifIndic_Status(glucoseCollectorContext.connHandle,
                                 glucoseCollectorContext.MeasurementCharValHandle + 1, /* Glucose Measurement Char Descriptor Handle */
                                 TRUE, /* enable notification */
                                 FALSE /* no indication */
                                 );
  return ret;

}/* end GL_Collector_Enable_Glucose_Measurement_Char_Notification() */

tBleStatus GL_Collector_Enable_Glucose_Measurement_Context_Char_Notification(void)
{
  tBleStatus ret;
  
  ret =  Master_NotifIndic_Status(glucoseCollectorContext.connHandle,
                                  glucoseCollectorContext.MeasurementContextCharValHandle + 1, /* Glucose Measurement Context Char Descriptor Handle */
                                  TRUE, /* enable notification */
                                  FALSE/* no indication */
                                  );
  return ret; 
  
} /* end GL_Collector_Enable_Glucose_Measurement_Context_Char_Notification() */

tBleStatus GL_Collector_Enable_Glucose_RACP_Char_Indication(void)
{
  tBleStatus ret;
  ret =  Master_NotifIndic_Status(glucoseCollectorContext.connHandle,
                                  glucoseCollectorContext.RACPCharValHandle + 1, /* Glucose RACP Char Descriptor Handle */
                                  FALSE, /* no notification */
                                  TRUE/* enable indication */
                                  );
  return(ret); 
} /* end GL_Collector_Enable_Glucose_RACP_Char_Indication() */


static void GL_Collector_Set_BaseTime(uint8_t index, tBasetime timevalue)
{
  /* store time value */
  STORE_LE_16(&racp_charval[index], timevalue.year);
  index+= 2; 
  racp_charval[index++] = timevalue.month;
  racp_charval[index++] = timevalue.day;
  racp_charval[index++] = timevalue.hours;
  racp_charval[index++] = timevalue.minutes;
  racp_charval[index++] = timevalue.seconds;
}

tBleStatus GL_Collector_Send_RACP(uint8_t racp_opcode,
                                  uint8_t racp_operator,
                                  uint8_t racp_filter_type, 
                                  tfilterTypeParameter* racp_filter_parameter_1,
                                  tfilterTypeParameter* racp_filter_parameter_2)
{
  uint8_t index = 0;
  
  tBleStatus retval = BLE_STATUS_FAILED; 
    
  GL_DBG_MSG(profiledbgfile,"GL_Collector_Send_RACP(), RACPCharValHandle: 0x%04X,opcode: 0x%02X, operator: 0x%02X, filter_type: 0x%02X\n", glucoseCollectorContext.RACPCharValHandle,racp_opcode,racp_operator,racp_filter_type);
  
  if ((glucoseCollectorContext.glucoseServicePresent) && (glucoseCollectorContext.RACPCharValHandle != 0)) 
  {
    BLUENRG_memset(racp_charval,0x00,RACP_COMMAND_MAX_LEN);
    
    glucoseCollectorContext.number_of_notifications_for_racp = 0; //TBR???
    
    /* set racp procedure opcode and update index */
    racp_charval[index++] = racp_opcode;
    
    /* set racp procedure operator and update index */
    racp_charval[index++] = racp_operator;
    
    /* Check racp operator for proper configuring the racp procedure operands where needed */
    switch(racp_operator)
    {
      /* operator is <= or >=: only 1 operand is provided */
      case RACP_OPERATOR_LESS_EQUAL:
      case RACP_OPERATOR_GREATER_EQUAL:
      {
        switch(racp_filter_type)
        {
          /* filter type is sequence number */
          case RACP_FILTER_TYPE_SEQUENCE_NUMBER:
          {
            /* set racp procedure filter type and update index */
            racp_charval[index++] = racp_filter_type;
            
            /* store sequence number */
            STORE_LE_16(&racp_charval[index], racp_filter_parameter_1->sequence_number);
            index += 2;                                  
          }
          break;
          /* filter type is time*/
          case RACP_FILTER_TYPE_USER_FACING_TIME:
          {
            /* set racp procedure filter type and update index */
            racp_charval[index++] = racp_filter_type;
            
            /* store time value */
            GL_Collector_Set_BaseTime(index, racp_filter_parameter_1->user_facing_time);
            index += RACP_BASE_TIME_SIZE;
          }
          break;
          default:
          {
          }
          break;
        }/* end switch(racp_filter_type) */
      }/* end case RACP_OPERATOR_LESS_EQUAL, RACP_OPERATOR_GREATER_EQUAL */
      break; 
      /* operator is within a range: 2 operands are provided */
      case RACP_OPERATOR_WITHIN_RANGE:
      {
        switch(racp_filter_type)
        {
          case RACP_FILTER_TYPE_SEQUENCE_NUMBER:
          {
            /* set racp procedure filter type and update index */
            racp_charval[index++] = racp_filter_type;
            
            /* store sequence number 1 */
            STORE_LE_16(&racp_charval[index], racp_filter_parameter_1->sequence_number);
            index += 2; 
            /* store sequence number 2 */
            STORE_LE_16(&racp_charval[index], racp_filter_parameter_2->sequence_number);
            index += 2; 
          }
          break;
          case RACP_FILTER_TYPE_USER_FACING_TIME:
          {
            /* set racp procedure filter type and update index */
            racp_charval[index++] = racp_filter_type;
            
            /* store time value 1 */
            GL_Collector_Set_BaseTime(index, racp_filter_parameter_1->user_facing_time);
            index += RACP_BASE_TIME_SIZE; 
            /* store time value 2 */
            GL_Collector_Set_BaseTime(index, racp_filter_parameter_2->user_facing_time);
            index += RACP_BASE_TIME_SIZE; 
          }
          break;
          default:
          {
          }
          break;
        }/* end switch(racp_filter_type) */  
      }/* end case RACP_OPERATOR_WITHIN_RANGE */
      break;
      
      default:
      break;
    }/* end switch(racp_operator)*/
    
#if 0 /* ONLY FOR DEBUG: TBR */
    GL_DBG_MSG(profiledbgfile,"-D- index: %d\n",index);
    for (uint8_t i=0; i<(index-1); i++) 
       GL_DBG_MSG(profiledbgfile,"%02X ", racp_charval[i]); 
    GL_DBG_MSG(profiledbgfile,"%02X\n", racp_charval[index-1]); 
#endif 
    
    retval =  Master_Write_Value(glucoseCollectorContext.connHandle, /* connection handle */
                                 glucoseCollectorContext.RACPCharValHandle , /* attribute value handle */
                                 index, /* charValueLen */
			         (uint8_t *) &racp_charval);
    if (retval == BLE_STATUS_SUCCESS) 
      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_WAIT_SEND_RACP; 
    else
      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_CONNECTED_IDLE;
    
  }/* end if(glucose_collector.state == GL_CONNECTED) */
  else
  {
    retval = HCI_INVALID_PARAMETERS; 
  } 
  return (retval);
}/* end GL_Collector_Send_RACP() */


/* This function start the connection and the configuration of the glucose collector device */
uint8_t GL_Collector_ConnConf(glcConnDevType connParam)
{
  uint8_t ret;

  ret = GL_Collector_DeviceConnection(connParam);
  
  if (ret == BLE_STATUS_SUCCESS) {
    glucoseCollectorContext.fullConf = TRUE;
  }

  return ret;
}

void GL_Collector_StateMachine(void)
{
  uint8_t ret;

  switch(glucoseCollectorContext.state) {
  case GLUCOSE_COLLECTOR_UNINITIALIZED:
    /* Nothing to do */
    break;
  case GLUCOSE_COLLECTOR_CONNECTED_IDLE:
    {
    }
    break;
  case GLUCOSE_COLLECTOR_INITIALIZED:
    /* Nothing to do */
    break;
  case GLUCOSE_COLLECTOR_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case GLUCOSE_COLLECTOR_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case GLUCOSE_COLLECTOR_CONNECTED:
    {
      ret = GL_Collector_ServicesDiscovery(); 
      if (ret == BLE_STATUS_SUCCESS) {
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_SERVICE_DISCOVERY;
      } else {
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_SERVICE_DISCOVERY, ret);
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case GLUCOSE_COLLECTOR_SERVICE_DISCOVERY:
    /* Waiting the end of the service discovery procedure */
    break;
  case GLUCOSE_COLLECTOR_MEASUREMENT_CHARAC_DISCOVERY:
    {
      ret = GL_Collector_DiscCharacServ(GLUCOSE_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY;
      } else {
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_CHARAC_DISCOVERY, ret);
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case GLUCOSE_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY:
    /* Waiting the end of the Glucose Measurement Characteristic discovery */ 
    break;
       
  case GLUCOSE_COLLECTOR_ENABLE_MEASUREMENT_NOTIFICATION:                 
   {
      ret = GL_Collector_Enable_Glucose_Measurement_Char_Notification();
      if (ret == BLE_STATUS_SUCCESS) {
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_NOTIFICATION;
      } else {
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_ENABLE_NOTIFICATION, ret);
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_DISCONNECTION;
      }
    }
    break; 
  case GLUCOSE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_NOTIFICATION:             
     break;
  case GLUCOSE_COLLECTOR_ENABLE_MEASUREMENT_CONTEXT_NOTIFICATION:       
    {
      ret = GL_Collector_Enable_Glucose_Measurement_Context_Char_Notification();
      if (ret == BLE_STATUS_SUCCESS) {
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_CONTEXT_NOTIFICATION;
      } else {
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_ENABLE_NOTIFICATION, ret);
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_DISCONNECTION;
      }
    }
    break; 
  case GLUCOSE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_CONTEXT_NOTIFICATION:       
     break;
    
  case GLUCOSE_COLLECTOR_ENABLE_RACP_INDICATION:
    {
      ret = GL_Collector_Enable_Glucose_RACP_Char_Indication();
      if (ret == BLE_STATUS_SUCCESS) {
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_WAIT_ENABLE_RACP_INDICATION;
      } else {
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_ENABLE_INDICATION, ret);
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case GLUCOSE_COLLECTOR_WAIT_ENABLE_RACP_INDICATION:
    /* Waiting the end of the Tx Power Enable Notification */
    break;
  case GLUCOSE_COLLECTOR_SEND_RACP: 
    {
    }
    break;
  case GLUCOSE_COLLECTOR_WAIT_SEND_RACP:
    /* Waiting the end of RACP procedure response */
    break;
  case GLUCOSE_COLLECTOR_READ_FEATURE:
    {
      ret = GL_Collector_ReadFeatureChar();
      if (ret == BLE_STATUS_SUCCESS) {
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_WAIT_READ_FEATURE;
      } else {
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_READ, ret);
	glucoseCollectorContext.state = GLUCOSE_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case GLUCOSE_COLLECTOR_WAIT_READ_FEATURE:
    /* Waiting the end of the read glucose feature procedure */
    break;
    
  case GLUCOSE_COLLECTOR_DISCONNECTION:
    {
      ret = GL_Collector_DeviceDisconnection();
      if (ret != BLE_STATUS_SUCCESS) {
	GL_Collector_FullConfError_CB(GLC_ERROR_IN_DISCONNECTION, ret);
      }
      glucoseCollectorContext.state = GLUCOSE_COLLECTOR_INITIALIZED;
    }
    break;
  }
}

BOOL GL_Collector_Util_Perform_RACP_Post_processing(void)
{
  if ((glucoseCollectorContext.racp_notifications_postprocessig_is_enabled) &&(glucoseCollectorContext.number_of_notifications_for_racp > 0))
  {
    glucoseCollectorContext.start_racp_notifications_analyis = TRUE;
    return TRUE;
  }
  return FALSE;
  
}/* GL_Collector_Util_Perform_RACP_Post_processing() */

void GL_Collector_PostProcess_RACP_Notification_SM() 
{
  if (glucoseCollectorContext.start_racp_notifications_analyis)
  {  
    GL_Collector_RACP_Notifications_PostProcessing();
    glucoseCollectorContext.number_of_notifications_for_racp -=1;
    if (glucoseCollectorContext.number_of_notifications_for_racp == 0)
    {
      glucoseCollectorContext.start_racp_notifications_analyis = FALSE;
      glucoseCollectorContext.racp_notifications_index =0;
    }
  }
}

/**
* @brief  It checks the write response status and error related to the RACP procedure 
*         It should be called within the Profile event handler, on EVT_BLUE_GATT_ERROR_RESP (TBR?), 
*         EVT_BLUE_GATT_PROCEDURE_COMPLETE (TBR?)  
* @param  err_code:  RACP write response error code (from Glucose Sensor) 
*/
void GL_Collector_RACP_Check_Write_Response(uint8_t err_code)
{
  //GL_INFO_MSG(profiledbgfile,"GL_Collector_RACP_Check_Write_Response()\n");
  
  if (err_code == 0)
  {
    /* Requested RACP procedure has been started on Glucose Sensor */
   
    /* RACP procedure has been completed with success, now we can start the procedure timer */
    Blue_NRG_Timer_Start(GL_COLLECTOR_PROCEDURE_TIMEOUT,GL_Collector_ProcedureTimeoutHandler,&glucoseCollectorContext.timerID);
    GL_INFO_MSG(profiledbgfile,"GL_Collector_RACP_Check_Write_Response(): BLE_STATUS_SUCCESS\n");
    glucoseCollectorContext.Is_Write_Ongoing = TRUE; 
  }
  
  else if (err_code == RACP_RESPONSE_PROCEDURE_ALREADY_IN_PROGRESS) /* err_code is not 0: some errors occurs */
  {
    /* Possible error code sent from the glucose sensor trough aci_gatt_write_response(): 
       RACP_RESPONSE_PROCEDURE_ALREADY_IN_PROGRESS; 
       RACP_RESPONSE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED.
    */
    GL_INFO_MSG(profiledbgfile,"GL_Collector_RACP_Check_Write_Response(), RACP procedure Already In progress: 0x%04X\n", err_code);
  }
  else if (err_code == RACP_RESPONSE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED)
  {
    GL_INFO_MSG(profiledbgfile,"GL_Collector_RACP_Check_Write_Response(), Client Char Configuration Not Properly Configured: 0x%04X\n", err_code);
  }
  //else
    //GL_INFO_MSG(profiledbgfile,"GL_Collector_RACP_Check_Write_Response(), Unknown error code : 0x%04X\n", err_code);
  GL_Collector_RACP_Write_Response_CB(err_code);
}/* end GL_Collector_RACP_Check_Write_Response() */

void GL_Collector_RACP_Indications(uint16_t attr_handle, uint8_t data_lenght,uint8_t * value)
{ 
  tBleStatus retval = BLE_STATUS_FAILED; 
  uint16_t glucose_sensor_num_stored_records = 0;
  
  if (attr_handle == glucoseCollectorContext.RACPCharValHandle)
  {
    /* stop the  timer */
    Blue_NRG_Timer_Stop(glucoseCollectorContext.timerID); //TBR Does we need to stop only when Op Code is set to Response Code? (what about Number of Stored Records Response Op code ?)
    glucoseCollectorContext.timerID = 0xFF; 
    glucoseCollectorContext.Is_Write_Ongoing = FALSE;
    
    glucoseCollectorContext.number_of_measurement_notifications=0; 
    glucoseCollectorContext.number_of_measurement_context_notifications=0; 
    
    if(*value == RACP_NUMBER_STORED_RECORDS_RESPONSE_OP_CODE)  
    {
      glucose_sensor_num_stored_records = LE_TO_HOST_16(value+2);
      GL_INFO_MSG(profiledbgfile,"GL_Collector_Indications_CB(), Number of Stored Records = 0x%04X\n",glucose_sensor_num_stored_records );
      
      GL_Collector_RACP_Received_Indication_CB(*value, 0, glucose_sensor_num_stored_records);
    }
    else if(*value == RACP_RESPONSE_OP_CODE)  
    {
      GL_INFO_MSG(profiledbgfile,"GL_Collector_Indications_CB(), Glucose RACP: Operator of Null= 0x%02X, requested Op Code = 0x%02X, , Response Code = 0x%02X\n", *(value+1), *(value+2), *(value+3));
      /* Parse received Response OP Code */
      switch (*(value+3))
      {
        case RACP_RESPONSE_SUCCESS :
          GL_INFO_MSG(profiledbgfile,"RACP_RESPONSE_SUCCESS\n");
        break;
        case RACP_RESPONSE_OPCODE_NOT_SUPPORTED:
          GL_INFO_MSG(profiledbgfile,"RACP_RESPONSE_OPCODE_NOT_SUPPORTED\n");
        break;
        case RACP_RESPONSE_INVALID_OPERATOR:
          GL_INFO_MSG(profiledbgfile,"RACP_RESPONSE_INVALID_OPERATOR\n");
        break;
        case RACP_RESPONSE_OPERATOR_NOT_SUPPORTED:
          GL_INFO_MSG(profiledbgfile,"RACP_RESPONSE_OPERATOR_NOT_SUPPORTED\n");
        break;
        case RACP_RESPONSE_INVALID_OPERAND:
          GL_INFO_MSG(profiledbgfile,"RACP_RESPONSE_INVALID_OPERAND\n");
        break;
        case RACP_RESPONSE_NO_RECORDS:
          GL_INFO_MSG(profiledbgfile,"RACP_RESPONSE_NO_RECORDS\n");
        break;
        case RACP_RESPONSE_ABORT_FAILED:
          GL_INFO_MSG(profiledbgfile,"RACP_RESPONSE_ABORT_FAILED\n"); //ABORT
        break;
        case RACP_RESPONSE_PROCEDURE_NOT_COMPLETED:
          GL_INFO_MSG(profiledbgfile,"RACP_RESPONSE_PROCEDURE_NOT_COMPLETED\n");
        break;
        case RACP_RESPONSE_OPERAND_NOT_SUPPORTED: 
         GL_INFO_MSG(profiledbgfile,"RACP_RESPONSE_OPERAND_NOT_SUPPORTED\n"); /* TBR: RACP_RESPONSE_FILTER_TYPE_NOT_SUPPORTED */
        break;
        }
        GL_Collector_RACP_Received_Indication_CB(*value, *(value+3), 0);
    }
             
    /* Send confirm for indication */
    retval =  aci_gatt_confirm_indication(glucoseCollectorContext.connHandle);  /* connection handle */  
    if(retval == BLE_STATUS_SUCCESS)
    {
      GL_INFO_MSG(profiledbgfile,"GL_Collector_Indications_Handler(), aci_gatt_confirm_indication() OK\n");
    }
    else
    {
      GL_INFO_MSG(profiledbgfile,"GL_Collector_Indications_Handler(), aci_gatt_confirm_indication() Failed %04X \n", retval);
    }   
    glucoseCollectorContext.state = GLUCOSE_COLLECTOR_CONNECTED_IDLE; //TBR
  }/* end (glucose_indication->attr_handle == RACPCharHandle) */
  
}/* end GL_Collector_RACP_Indications() */

/**
 * GL_Collector_ProcedureTimeoutHandler
 * 
 * It informs the application that the RACP write procedure has timed out
 */ 
void GL_Collector_ProcedureTimeoutHandler(void)
{
  /* stop the  timer */
  Blue_NRG_Timer_Stop(glucoseCollectorContext.timerID); //TBR
  glucoseCollectorContext.timerID = 0xFF;
  GL_INFO_MSG(profiledbgfile,"GL_Collector_ProcedureTimeoutHandler() timeout: RACP procedure failed!\n");
  glucoseCollectorContext.Is_Write_Ongoing = FALSE; //TBR
}/* end GL_Collector_ProcedureTimeoutHandler() */
         

/****************** glucose collector Weak Callbacks definition ***************************/

WEAK_FUNCTION(void GL_Collector_RACP_Write_Response_CB(uint8_t err_code))
{
}

WEAK_FUNCTION(void GL_Collector_RACP_Received_Indication_CB(uint8_t racp_response, uint8_t value, uint8_t  num_records))
{
}

WEAK_FUNCTION(void GL_Collector_FullConfError_CB(uint8_t error_type, uint8_t code))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & GLUCOSE_COLLECTOR) */
