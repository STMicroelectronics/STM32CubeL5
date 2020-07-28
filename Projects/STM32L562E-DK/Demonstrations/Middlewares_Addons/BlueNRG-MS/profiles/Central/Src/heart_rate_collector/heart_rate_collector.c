/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
* File Name          : heart_rate_collector.c
* Author             : AMS - AAS, RF Application Team
* Version            : V1.0.0
* Date               : 15-October-2014
* Description        : This file manages all the HRP Master API
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

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

#if (BLE_CURRENT_PROFILE_ROLES & HEART_RATE_COLLECTOR)

#include <heart_rate_service.h>
#include <heart_rate_collector.h>
#include <heart_rate_collector_config.h>

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
/* Heart rate collector context */
heartRateCollectorContext_Type heartRateCollectorContext; 

/* Private macros ------------------------------------------------------------*/

/******************************************************************************
* Imported Variable
******************************************************************************/
extern initDevType initParam;

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*******************************************************************************
* exported SAP
*******************************************************************************/

tBleStatus HRC_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle)
{
  uint8_t i, j;
  uint16_t uuid;

  j = 0;
  for (i=0; i<heartRateCollectorContext.numpeerServices; i++) {
    if (heartRateCollectorContext.peerServices[j] == UUID_TYPE_16) {
      BLUENRG_memcpy(&uuid, &heartRateCollectorContext.peerServices[j+5], 2);
      j++;
      BLUENRG_memcpy(start_handle, &heartRateCollectorContext.peerServices[j], 2);
      j += 2;
      BLUENRG_memcpy(end_handle, &heartRateCollectorContext.peerServices[j], 2);
      j += 2;
      if (heartRateCollectorContext.peerServices[j-5] == UUID_TYPE_16) {
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

tBleStatus HRC_Init(hrcInitDevType param)
{
  //initDevType initParam;
  uint8_t ret;

  BLUENRG_memcpy(&heartRateCollectorContext, 0, sizeof(heartRateCollectorContext_Type));//TBR
  
  heartRateCollectorContext.bEnergyExpended = FALSE;//TBR
  
  heartRateCollectorContext.BodySensorLocValue = 0;//TBR
  heartRateCollectorContext.HRControlPointValue = 0; //TBR
  
  BLUENRG_memcpy(heartRateCollectorContext.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  initParam.Master_DeviceDiscovery_CB = Master_HRC_DeviceDiscovery_CB;
  initParam.Master_Connection_CB = Master_HRC_Connection_CB;
  initParam.Master_Pairing_CB = Master_HRC_Pairing_CB;
  initParam.Master_PeerDataExchange_CB = Master_HRC_PeerDataExchange_CB;
  initParam.Master_PeerDataPermitExchange_CB = Master_HRC_PeerDataPermitExchange_CB;
  initParam.Master_ServiceCharacPeerDiscovery_CB = Master_HRC_ServiceCharacPeerDiscovery_CB;
  
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    heartRateCollectorContext.state = HEART_RATE_COLLECTOR_INITIALIZED;
    heartRateCollectorContext.fullConf = FALSE;
  } else {
    heartRateCollectorContext.state = HEART_RATE_COLLECTOR_UNINITIALIZED;
  }

  return ret;
}

tBleStatus HRC_DeviceDiscovery(hrcDevDiscType param)
{
  devDiscoveryType devDiscParam;
  uint8_t ret;
  
  devDiscParam.procedure = GENERAL_DISCOVERY_PROCEDURE;
  devDiscParam.scanInterval = param.scanInterval;
  devDiscParam.scanWindow = param.scanWindow;
  devDiscParam.own_address_type = param.own_addr_type;
  ret = Master_DeviceDiscovery(&devDiscParam);
  if (ret == BLE_STATUS_SUCCESS)
  {
    heartRateCollectorContext.state = HEART_RATE_COLLECTOR_DEVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus HRC_SecuritySet(hrcSecurityType param)
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

tBleStatus HRC_DeviceConnection(hrcConnDevType param)
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
    heartRateCollectorContext.state = HEART_RATE_COLLECTOR_START_CONNECTION;
  }

  return ret;
}

tBleStatus HRC_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(heartRateCollectorContext.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    heartRateCollectorContext.fullConf = FALSE;//TBR-----------
  }
  return ret;
}

tBleStatus HRC_ConnectionParameterUpdateRsp(uint8_t accepted, hrcConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(heartRateCollectorContext.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus HRC_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(heartRateCollectorContext.connHandle, FALSE);
  return ret;
}

tBleStatus HRC_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end HRC_Clear_Security_Database() */

tBleStatus HRC_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(heartRateCollectorContext.connHandle, pinCode);
  
  return ret;
}

tBleStatus HRC_ServicesDiscovery(void)
{
  uint8_t ret;

  ret = Master_GetPrimaryServices(heartRateCollectorContext.connHandle, 
				  &heartRateCollectorContext.numpeerServices, 
				  heartRateCollectorContext.peerServices, 
				  PRIMARY_SERVICES_ARRAY_SIZE);
  if (ret == BLE_STATUS_SUCCESS)
  {
    heartRateCollectorContext.state = HEART_RATE_COLLECTOR_SERVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus HRC_DiscCharacServ(uint16_t uuid_service)
{
  uint8_t ret;
  uint16_t start_handle, end_handle;

  if ((uuid_service == HEART_RATE_SERVICE_UUID) || (uuid_service == DEVICE_INFORMATION_SERVICE_UUID)) {
    if (!(heartRateCollectorContext.HeartRateServicePresent && heartRateCollectorContext.DeviceInfoServicePresent))
      return HCI_INVALID_PARAMETERS;
  }

  ret = HRC_findHandles(uuid_service, &start_handle, &end_handle);
  if (ret == BLE_STATUS_SUCCESS) {
    ret = Master_GetCharacOfService(heartRateCollectorContext.connHandle,
				    start_handle,
				    end_handle,
				    &heartRateCollectorContext.numCharac,
				    heartRateCollectorContext.charac,
				    CHARAC_OF_SERVICE_ARRAY_SIZE);

    if (ret == BLE_STATUS_SUCCESS)
    {
      heartRateCollectorContext.uuid_searched = uuid_service;
    }
  }
  return ret;
}

tBleStatus HRC_Start_Heart_Rate_Measurement_Characteristic_Descriptor_Discovery(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;

  
  /* It discovers the characteristic descriptors of the connected Heart Rate Sensor Measurement Characteristic */
  ble_status = Master_GetCharacDescriptors(heartRateCollectorContext.connHandle,
                                           heartRateCollectorContext.HRMeasurementCharValHandle, /* heart rate sensor measurement chararacteristic: Value Handle */
                                           heartRateCollectorContext.HRMeasurementCharValHandle+1, /* heart rate sensor measurement chararacteristic: End Handle */
                                           &heartRateCollectorContext.numCharac, 
                                           heartRateCollectorContext.charac,
                                           CHARAC_OF_SERVICE_ARRAY_SIZE); 
  return(ble_status);
}

tBleStatus HRC_ReadDISManufacturerNameChar(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Read Device Info Manufacturer Name Characteristic */
  ble_status = Master_Read_Value(heartRateCollectorContext.connHandle,  
                                 heartRateCollectorContext.ManufacturerNameCharValHandle,  
                                 &heartRateCollectorContext.dataLen,  
                                 heartRateCollectorContext.data,  
                                 HR_DIS_MANUFACTURER_NAME_SIZE);
  return(ble_status);
}/* end HRC_ReadFeatureChar() */

/**
  * @brief Enable Heart Rate Measurement Characteristic Notification
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus HRC_Enable_HR_Measurement_Char_Notification(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Enable Heart Rate Measurement Char Descriptor for notifications */
  ble_status = Master_NotifIndic_Status(heartRateCollectorContext.connHandle,
                                        heartRateCollectorContext.HRMeasurementCharValHandle + 1, /* Heart Rate Measurement Char Descriptor Handle */
                                        TRUE, /* enable notification */
                                        FALSE /* no indication */);
  return(ble_status); 
}/* end HRC_Enable_HR_Measurement_Char_Notification() */

tBleStatus HRC_Read_Body_Sensor_Location(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Read Body Sensor Location Characteristic */
  ble_status = Master_Read_Value(heartRateCollectorContext.connHandle,  
                                 heartRateCollectorContext.BodySensorLocValHandle,  
                                 &heartRateCollectorContext.dataLen,  
                                 heartRateCollectorContext.data,  
                                 HR_READ_SIZE);
  return(ble_status);  
}/* end HRC_Read_Body_Sensor_Location */

tBleStatus HRC_Write_HR_Control_Point(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  uint8_t write_val;  
  
  if (heartRateCollectorContext.HRControlPointValHandle != 0) 
#if 0 //TBR???
       && 
      (heartRateCollectorContext.bEnergyExpended))
#endif 
  {
    /* Write Heart Rate Control Point Characteristic */
    write_val = HR_CONTROL_POINT_RESET_ENERGY_EXPEND;
    ble_status =  Master_Write_Value(heartRateCollectorContext.connHandle,  /* connection handle */
                                     heartRateCollectorContext.HRControlPointValHandle, /* attribute value handle */
                                     1, /* charValueLen */
                                     (uint8_t *) &write_val);
    if (ble_status == BLE_STATUS_SUCCESS) 
      heartRateCollectorContext.state = HEART_RATE_COLLECTOR_WAIT_SEND_CP; 
    else
      heartRateCollectorContext.state = HEART_RATE_COLLECTOR_CONNECTED_IDLE;
    
  }
  return(ble_status);  
}

/* This function start the connection and the configuration of the device */
uint8_t HRC_ConnConf(hrcConnDevType connParam)
{
  uint8_t ret;

  ret = HRC_DeviceConnection(connParam);
  
  if (ret == BLE_STATUS_SUCCESS) {
    heartRateCollectorContext.fullConf = TRUE;
  }

  return ret;
}

void HRC_StateMachine(void)
{
  uint8_t ret;

  switch(heartRateCollectorContext.state) {
  case HEART_RATE_COLLECTOR_UNINITIALIZED:
    /* Nothing to do */
    break;
  case HEART_RATE_COLLECTOR_CONNECTED_IDLE:
    {
    }
    break;
  case HEART_RATE_COLLECTOR_INITIALIZED:
    /* Nothing to do */
    break;
  case HEART_RATE_COLLECTOR_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case HEART_RATE_COLLECTOR_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case HEART_RATE_COLLECTOR_CONNECTED:
    {
      ret = HRC_ServicesDiscovery(); 
      if (ret == BLE_STATUS_SUCCESS) {
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_SERVICE_DISCOVERY;
      } else {
	HRC_FullConfError_CB(HRC_ERROR_IN_SERVICE_DISCOVERY, ret);
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case HEART_RATE_COLLECTOR_SERVICE_DISCOVERY:
    /* Waiting the end of the service discovery procedure */
    break;
  case HEART_RATE_COLLECTOR_MEASUREMENT_CHARAC_DISCOVERY:
    {
      ret = HRC_DiscCharacServ(HEART_RATE_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY;
      } else {
	HRC_FullConfError_CB(HRC_ERROR_IN_CHARAC_DISCOVERY, ret);
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case HEART_RATE_COLLECTOR_WAIT_MEASUREMENT_CHARAC_DISCOVERY:
    /* Waiting the end of the Glucose Measurement Characteristic discovery */ 
    break;
    
  case HEART_RATE_COLLECTOR_ENABLE_MEASUREMENT_NOTIFICATION:                 
   {
      ret = HRC_Enable_HR_Measurement_Char_Notification();
      if (ret == BLE_STATUS_SUCCESS) {
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_NOTIFICATION;
      } else {
	HRC_FullConfError_CB(HRC_ERROR_IN_ENABLE_NOTIFICATION, ret);
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_DISCONNECTION;
      }
    }
    break; 
  case HEART_RATE_COLLECTOR_WAIT_ENABLE_MEASUREMENT_NOTIFICATION:             
     break;
 
  case HEART_RATE_COLLECTOR_SEND_CP: 
    {
    }
    break;
  case HEART_RATE_COLLECTOR_WAIT_SEND_CP:
    /* Waiting the end of RACP procedure response */
    break;
    
  case HEART_RATE_COLLECTOR_READ_BODY_SENSOR:
    {
      ret = HRC_Read_Body_Sensor_Location();
      if (ret == BLE_STATUS_SUCCESS) {
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_WAIT_READ_BODY_SENSOR;
      } else {
	HRC_FullConfError_CB(HRC_ERROR_IN_READ, ret);
	heartRateCollectorContext.state = HEART_RATE_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case HEART_RATE_COLLECTOR_WAIT_READ_BODY_SENSOR:
    /* Waiting the end of the read body sensor procedure */
    break;
    
  case HEART_RATE_COLLECTOR_DISCONNECTION:
    {
      ret = HRC_DeviceDisconnection();
      if (ret != BLE_STATUS_SUCCESS) {
	HRC_FullConfError_CB(HRC_ERROR_IN_DISCONNECTION, ret);
      }
      heartRateCollectorContext.state = HEART_RATE_COLLECTOR_INITIALIZED;
    }
    break;
  }
}

void HRC_Read_Body_Sensor_Location_Handler(void)
{  
  /* store the read body sensor location characteristic value */
  heartRateCollectorContext.BodySensorLocValue = heartRateCollectorContext.data[0];    //TBR???
  switch(heartRateCollectorContext.BodySensorLocValue) {
    case BODY_SENSOR_LOCATION_OTHER: {
      HR_INFO_MSG (profiledbgfile,"HR_Collector_Read_Body_Sensor_Location(), Body Sensor Location Value: Other\n");
      break;
    }  
    case BODY_SENSOR_LOCATION_CHEST: {
      HR_INFO_MSG (profiledbgfile,"HR_Collector_Read_Body_Sensor_Location(), Body Sensor Location Value: Chest\n");
      break;
    } 
    case BODY_SENSOR_LOCATION_WRIST: {
      HR_INFO_MSG (profiledbgfile,"HR_Collector_Read_Body_Sensor_Location(), Body Sensor Location Value: Wrist\n");
      break;
    } 
    case BODY_SENSOR_LOCATION_FINGER: {
      HR_INFO_MSG (profiledbgfile,"HR_Collector_Read_Body_Sensor_Location(), Body Sensor Location Value: Finger\n");
      break;
    } 
    case BODY_SENSOR_LOCATION_HAND: {
      HR_INFO_MSG (profiledbgfile,"HR_Collector_Read_Body_Sensor_Location(), Body Sensor Location Value: Hand\n");
      break;
    } 
    case BODY_SENSOR_LOCATION_EARLOBE: {
      HR_INFO_MSG (profiledbgfile,"HR_Collector_Read_Body_Sensor_Location(), Body Sensor Location Value: Ear Lobe\n");
      break;
    } 
    case BODY_SENSOR_LOCATION_FOOT: {
      HR_INFO_MSG (profiledbgfile,"HR_Collector_Read_Body_Sensor_Location(), Body Sensor Location Value: Foot\n");
      break;
    } 
    default: {
      HR_INFO_MSG (profiledbgfile,"HR_Collector_Read_Body_Sensor_Location(), Body Sensor Location Value: RFU\n");
      break;
    }
  } /* end switch(heartRateCollectorContext.BodySensorLocValue) */
} /* end HRC_Read_Body_Sensor_Location_Handler() */

/**
* @brief  It checks the write response status and error related to the 
*         Heart Rate Control Point (HRCP) write request to reset energy expended      
*         It should be called within the Profile event handler, on EVT_BLUE_GATT_ERROR_RESP, 
*         EVT_BLUE_GATT_PROCEDURE_COMPLETE (TBR?)  
* @param  err_code:  HRCP write response error code (from Heart Rate Sensor) 
*/
void HRC_CP_Check_Write_Response_Handler(uint8_t err_code)
{
  HR_INFO_MSG(profiledbgfile,"HR_Collector_HRCP_Check_Write_Response_CB().\n");
  
  if (err_code == 0)
  {
    HR_INFO_MSG(profiledbgfile, "Heart Rate Control Point Value Write procedure completed with success!\n");
  }
  else if (err_code == HR_CNTL_POINT_VALUE_NOT_SUPPORTED) 
  {
    HR_INFO_MSG(profiledbgfile,"Heart Rate Control Point Value not supported!\n");
  } 
  else 
  {
    HR_INFO_MSG(profiledbgfile, "Error during the Write Characteristic Value Procedure!\n");
  } 
  
  HRC_CP_Write_Response_CB(err_code);
}/* end HRC_CP_Check_Write_Response_Handler() */

void HRC_Notification_Handler(uint16_t attr_handle, uint8_t data_lenght, uint8_t * value)
{
  uint8_t dimAttrValue, index, i;
  uint8_t bHRMformat16 = FALSE;  
  /* heart rate measurement characteristic */
  tHeartRateMeasurementVal HRMeasurement;
  
  /* Check notification handle: heart rate measurement characteristic notification */ 
  if (attr_handle == heartRateCollectorContext.HRMeasurementCharValHandle)
  {
    HR_INFO_MSG(profiledbgfile, "HR_Notification_Handler()\n");
    BLUENRG_memset(&HRMeasurement, 0 , sizeof(HRMeasurement)); 
  
    /* data_length  is the lenght of attribute value + handle (2 bytes).*/
    dimAttrValue = data_lenght;
    
    /* To check the Flags value field to determine the contents of Heart  
       Rate Measurement (HRM) and the format of HR value field. */
    index = 0; 

    HRMeasurement.flags = value[index];  
    /* If the first bit is 0 then index++ because HRM value is uint8
       Heart Rate Value Format is setted to UINT8 */
    if (!(HRMeasurement.flags & HEART_RATE_VALUE_FORMAT_MASK))
    {
      index++;
      HRMeasurement.HRM_value8 = value[index];
      HR_INFO_MSG(profiledbgfile, "Heart Rate Measurement Value (uint8): 0x%02x bpm\n", HRMeasurement.HRM_value8);
    } 
    else 
    { /* If the first bit is 1 then index+=2 because HRM value is uint16 */
      /* Heart Rate Value Format is setted to UINT16 */
      index += 2;
      HRMeasurement.HRM_value16 = (uint16_t)(value[index]<<8) | value[index-1];
      bHRMformat16 = TRUE;
      HR_INFO_MSG(profiledbgfile, "Heart Rate Measurement Value (uint16): 0x%04x bpm\n", HRMeasurement.HRM_value16);
    }
    /* Sensor contact feature is supported */
    if ((HRMeasurement.flags & SENSOR_CONTACT_SUPPORTED_STATUS_MASK))
    {
      /* Sensor contact feature is supported and contact is detected */
      if ((HRMeasurement.flags & SENSOR_CONTACT_DETECTED_STATUS_MASK))
      {
        HR_INFO_MSG(profiledbgfile, "Sensor Contact feature is supported and contact is detected!\n");  
      } 
      else 
      {
        HR_INFO_MSG(profiledbgfile, "Sensor Contact feature is supported and contact is not detected!\n");  
      }    
    } 
    else 
    {
      HR_INFO_MSG(profiledbgfile, "Sensor Contact feature is not supported in the current connection!\n");  
    }
    /* Energy Expended Field is present */
    if ((HRMeasurement.flags & ENERGY_EXPENDED_STATUS_MASK))
    {
      index += 2;
      HRMeasurement.energyExpended = (uint16_t)(value[index]<<8) | value[index-1];
      heartRateCollectorContext.bEnergyExpended = TRUE;
      HR_INFO_MSG(profiledbgfile, "Energy Expended Value: 0x%04x kJ\n", HRMeasurement.energyExpended);
    } 
    else 
    {
      HR_INFO_MSG(profiledbgfile, "Energy Expended Field is not present\n");
      heartRateCollectorContext.bEnergyExpended = FALSE;
    }
    /* one or more RR-interval are present */
    if ((HRMeasurement.flags & RR_INTERVAL_MASK))
    {
      /* Calculate the maximum number of RR-interval values that can be notified, this depend
       * by the charac length, by the HRM format and by the presence of Energy Expended Field */
      HRMeasurement.maxNumRR_interval = (dimAttrValue - 1); /* 1 byte for Flag field */
      if (bHRMformat16) 
        HRMeasurement.maxNumRR_interval-=2;
      else 
        HRMeasurement.maxNumRR_interval--;
      if (heartRateCollectorContext.bEnergyExpended) 
        HRMeasurement.maxNumRR_interval-=2;
      HRMeasurement.maxNumRR_interval = HRMeasurement.maxNumRR_interval / 2;   
      for (i=0; i<HRMeasurement.maxNumRR_interval; i++) 
      { 
        index += 2;
        if (index < dimAttrValue)
        { 
          HRMeasurement.RR_interval[i] = (uint16_t)(value[index]<<8) | value[index-1];
          HR_INFO_MSG(profiledbgfile, "RR_interval Value 0x%02x: 0x%04x ms\n", i, HRMeasurement.RR_interval[i]);
        }
      }   
    } 
    else 
    {
      HR_INFO_MSG(profiledbgfile, "RR-interval values are not present\n");
    }  
    BLUENRG_memcpy(&heartRateCollectorContext.HRMeasurementValue, &HRMeasurement, sizeof(HRMeasurement));
  }
}

/****************** Profile Weak Callbacks definition ***************************/

WEAK_FUNCTION(void HRC_CP_Write_Response_CB(uint8_t err_code))
{
}

WEAK_FUNCTION(void HRC_FullConfError_CB(uint8_t error_type, uint8_t code))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & HEART_RATE_COLLECTOR) */
