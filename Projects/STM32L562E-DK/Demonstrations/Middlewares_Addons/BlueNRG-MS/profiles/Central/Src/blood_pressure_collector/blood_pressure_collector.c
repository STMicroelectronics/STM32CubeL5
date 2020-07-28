/******************** (C) COPYRIGBP 2013 STMicroelectronics ********************
* File Name          : blood_pressure_collector.c
* Author             : AMS - AAS, RF Application Team
* Version            : V1.0.0
* Date               : 09-February-2015
* Description        : This file manages all the BPP Master API
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
#include "math.h"

#include <host_config.h>
#include <hci.h>
#include <hci_le.h>
#include <debug.h>
#include <ble_list.h>
#include <timer.h>
#include <uuid.h>

#if (BLE_CURRENT_PROFILE_ROLES & BLOOD_PRESSURE_COLLECTOR)

#include <blood_pressure_service.h>
#include <blood_pressure_collector.h>
#include <blood_pressure_collector_config.h>

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

/* Blood Pressure collector context */
bloodPressureCollectorContext_Type bloodPressureCollectorContext; 

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
tBleStatus BPC_findHandles(uint16_t uuid_service, uint16_t *start_handle, 
                                    uint16_t *end_handle)
{
  uint8_t i, j;
  uint16_t uuid;

  j = 0;
  for (i=0; i<bloodPressureCollectorContext.numpeerServices; i++) {
    if (bloodPressureCollectorContext.peerServices[j] == UUID_TYPE_16) {
      BLUENRG_memcpy(&uuid, &bloodPressureCollectorContext.peerServices[j+5], 2);
      j++;
      BLUENRG_memcpy(start_handle, &bloodPressureCollectorContext.peerServices[j], 2);
      j += 2;
      BLUENRG_memcpy(end_handle, &bloodPressureCollectorContext.peerServices[j], 2);
      j += 2;
      if (bloodPressureCollectorContext.peerServices[j-5] == UUID_TYPE_16) {
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

tBleStatus BPC_Init(bpcInitDevType param)
{
  //initDevType initParam;
  uint8_t ret;

  BLUENRG_memcpy(&bloodPressureCollectorContext, 0, sizeof(bloodPressureCollectorContext_Type));//TBR
  BLUENRG_memcpy(bloodPressureCollectorContext.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  initParam.Master_DeviceDiscovery_CB = Master_BPC_DeviceDiscovery_CB;
  initParam.Master_Connection_CB = Master_BPC_Connection_CB;
  initParam.Master_Pairing_CB = Master_BPC_Pairing_CB;
  initParam.Master_PeerDataExchange_CB = Master_BPC_PeerDataExchange_CB;
  initParam.Master_PeerDataPermitExchange_CB = Master_BPC_PeerDataPermitExchange_CB;
  initParam.Master_ServiceCharacPeerDiscovery_CB = Master_BPC_ServiceCharacPeerDiscovery_CB;
  
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_INITIALIZED;
    bloodPressureCollectorContext.fullConf = FALSE;
  } else {
    bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_UNINITIALIZED;
  }

  return ret;
}

tBleStatus BPC_DeviceDiscovery(bpcDevDiscType param)
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
    bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_DEVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus BPC_SecuritySet(bpcSecurityType param)
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

tBleStatus BPC_DeviceConnection(bpcConnDevType param)
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
    bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_START_CONNECTION;
  }

  return ret;
}

tBleStatus BPC_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(bloodPressureCollectorContext.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    bloodPressureCollectorContext.fullConf = FALSE;//TBR-----------
  }
  return ret;
}

tBleStatus BPC_ConnectionParameterUpdateRsp(uint8_t accepted, bpcConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(bloodPressureCollectorContext.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus BPC_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(bloodPressureCollectorContext.connHandle, FALSE);
  return ret;
}

tBleStatus BPC_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end BPC_Clear_Security_Database() */

tBleStatus BPC_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(bloodPressureCollectorContext.connHandle, pinCode);
  
  return ret;
}

tBleStatus BPC_ServicesDiscovery(void)
{
  uint8_t ret;

  ret = Master_GetPrimaryServices(bloodPressureCollectorContext.connHandle, 
				  &bloodPressureCollectorContext.numpeerServices, 
				  bloodPressureCollectorContext.peerServices, 
				  PRIMARY_SERVICES_ARRAY_SIZE);
  if (ret == BLE_STATUS_SUCCESS)
  {
    bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_SERVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus BPC_DiscCharacServ(uint16_t uuid_service)
{
  uint8_t ret;
  uint16_t start_handle, end_handle;

  if ((uuid_service == BLOOD_PRESSURE_SERVICE_UUID) || (uuid_service == DEVICE_INFORMATION_SERVICE_UUID)) {
    if (!(bloodPressureCollectorContext.BloodPressureServicePresent && bloodPressureCollectorContext.DeviceInfoServicePresent))
      return HCI_INVALID_PARAMETERS;
  }

  ret = BPC_findHandles(uuid_service, &start_handle, &end_handle);
  if (ret == BLE_STATUS_SUCCESS) {
    ret = Master_GetCharacOfService(bloodPressureCollectorContext.connHandle,
				    start_handle,
				    end_handle,
				    &bloodPressureCollectorContext.numCharac,
				    bloodPressureCollectorContext.charac,
				    CHARAC_OF_SERVICE_ARRAY_SIZE);

    if (ret == BLE_STATUS_SUCCESS)
    {
      bloodPressureCollectorContext.uuid_searched = uuid_service;
    }
  }
  return ret;
}


/**
  * @brief Discovery Blood Pressure Measurement Client Characteristic Descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_Start_Blood_Pressure_Measurement_Client_Char_Descriptor_Discovery(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;

  /* It discovers the characteristic descriptors of the connected Temperature Measurement Characteristic */
  ble_status = Master_GetCharacDescriptors(bloodPressureCollectorContext.connHandle,
                                           bloodPressureCollectorContext.BPMeasurementCharValHandle,   /* Blood Pressure Measurement chararacteristic: Value Handle */
                                           bloodPressureCollectorContext.BPMeasurementCharValHandle+1, /* Blood Pressure Measurement chararacteristic: End Handle */
                                           &bloodPressureCollectorContext.numCharac, 
                                           bloodPressureCollectorContext.charac,
                                           CHARAC_OF_SERVICE_ARRAY_SIZE); 
  return(ble_status);
}

/**
  * @brief Discovery Intermediate Cuff Pressure Client Characteristic Descriptor
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_Start_ICP_Client_Char_Descriptor_Discovery(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;

  /* It discovers the characteristic descriptors of the connected Intermediate Temperature Characteristic */
  ble_status = Master_GetCharacDescriptors(bloodPressureCollectorContext.connHandle,
                                           bloodPressureCollectorContext.ICPCharValHandle,   /* Intermediate Cuff Pressure chararacteristic: Value Handle */
                                           bloodPressureCollectorContext.ICPCharValHandle+1, /* Intermediate Cuff Pressure chararacteristic: End Handle */
                                           &bloodPressureCollectorContext.numCharac, 
                                           bloodPressureCollectorContext.charac,
                                           CHARAC_OF_SERVICE_ARRAY_SIZE); 

  return(ble_status);
}


tBleStatus BPC_ReadDISManufacturerNameChar(void)
{
  tBleStatus ret;

  /* Read Device Info Manufacturer Name Characteristic */
  ret = Master_Read_Value(bloodPressureCollectorContext.connHandle,  
                          bloodPressureCollectorContext.ManufacturerNameCharValHandle,  
                          &bloodPressureCollectorContext.dataLen,  
                          bloodPressureCollectorContext.data,  
                          BP_DIS_MANUFACTURER_NAME_SIZE
                          );
  
  return ret;
}/* end BPC_ReadDISManufacturerNameChar() */

tBleStatus BPC_ReadDISModelNumberChar(void)
{
  tBleStatus ret;
  
  /* Read Device Info Model Number Characteristic */
  ret = Master_Read_Value(bloodPressureCollectorContext.connHandle,  
                          bloodPressureCollectorContext.ModelNumberCharValHandle,  
                          &bloodPressureCollectorContext.dataLen,  
                          bloodPressureCollectorContext.data,  
                          BP_DIS_MODEL_NUMBER_SIZE 
                          );
 return ret; 
}/* end BPC_ReadDISModelNumberChar() */

tBleStatus BPC_ReadDISSystemIDChar(void)
{
  tBleStatus ret;
  
  /* Read Device Info System ID Characteristic */
  ret = Master_Read_Value(bloodPressureCollectorContext.connHandle,  
                          bloodPressureCollectorContext.SystemIDCharValHandle,  
                          &bloodPressureCollectorContext.dataLen,  
                          bloodPressureCollectorContext.data,  
                          BP_DIS_SYSTEM_ID_SIZE 
                          );
  return ret; 
}/* end BPC_ReadDISSystemIDChar() */

/**
  * @brief Enable Blood Pressure Measurement Characteristic for Indication
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_Enable_BP_Measurement_Char_Indication(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Enable Blood Pressure Temperature Char Descriptor for Indication */
 
  ble_status = Master_NotifIndic_Status(bloodPressureCollectorContext.connHandle,
                                          bloodPressureCollectorContext.BPMeasurementCharValHandle + 1, /* Blood Pressure Measurement Char Descriptor Handle */
                                          FALSE, /* no notification */
                                          TRUE /* enable indication */);
  return(ble_status); 
}/* end BPC_Enable_BP_Measurement_Char_Indication() */

/**
  * @brief Enable Intermediate Cuff Pressure Characteristic for Notification
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_Enable_ICP_Char_Notification(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Enable Intermediate Cuff Pressure Char Descriptor for Notification */
  ble_status = Master_NotifIndic_Status(bloodPressureCollectorContext.connHandle,
                                        bloodPressureCollectorContext.ICPCharValHandle + 1, /* Intermediate Cuff Pressure Char Descriptor Handle */
                                        TRUE, /* enable notification */
                                        FALSE /* no indication */);
  return(ble_status); 
}/* end BPC_Enable_ICP_Char_Notification() */


/**
  * @brief Read Blood Pressure Feature characteristic 
  * @param [in] None
  * @retval tBleStatus: BLE status   
  */
tBleStatus BPC_Read_BP_Feature(void)
{
  tBleStatus ble_status = BLE_STATUS_SUCCESS;
  
  /* Read Measurement Interval Characteristic */
  ble_status = Master_Read_Value(bloodPressureCollectorContext.connHandle,  
                                 bloodPressureCollectorContext.BPFeatureCharValHandle,  
                                 &bloodPressureCollectorContext.dataLen,  
                                 bloodPressureCollectorContext.data,  
                                 BP_FEATURE_READ_SIZE);
  
  return(ble_status);  
}/* end BPC_Read_Measurement_Interval() */


/* This function start the connection and the configuration of the blood pressure collector device */
uint8_t BPC_ConnConf(bpcConnDevType connParam)
{
  uint8_t ret;

  ret = BPC_DeviceConnection(connParam);
  
  if (ret == BLE_STATUS_SUCCESS) {
    bloodPressureCollectorContext.fullConf = TRUE;
  }

  return ret;
}


void BPC_StateMachine(void)
{
  uint8_t ret;

  switch(bloodPressureCollectorContext.state) {
  case BLOOD_PRESSURE_COLLECTOR_UNINITIALIZED:
    /* Nothing to do */
    break;
  case BLOOD_PRESSURE_COLLECTOR_CONNECTED_IDLE:
    {
    }
    break;
  case BLOOD_PRESSURE_COLLECTOR_INITIALIZED:
    /* Nothing to do */
    break;
  case BLOOD_PRESSURE_COLLECTOR_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case BLOOD_PRESSURE_COLLECTOR_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case BLOOD_PRESSURE_COLLECTOR_CONNECTED:
    {
      ret = BPC_ServicesDiscovery(); 
      if (ret == BLE_STATUS_SUCCESS) {
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_SERVICE_DISCOVERY;
      } else {
	BPC_FullConfError_CB(BPC_ERROR_IN_SERVICE_DISCOVERY, ret);
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case BLOOD_PRESSURE_COLLECTOR_SERVICE_DISCOVERY:
    /* Waiting the end of the service discovery procedure */
    break;
  case BLOOD_PRESSURE_COLLECTOR_CHARAC_DISCOVERY:
    {
      ret = BPC_DiscCharacServ(BLOOD_PRESSURE_SERVICE_UUID);
      if (ret == BLE_STATUS_SUCCESS) {
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_WAIT_CHARAC_DISCOVERY;
      } else {
	BPC_FullConfError_CB(BPC_ERROR_IN_CHARAC_DISCOVERY, ret);
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case BLOOD_PRESSURE_COLLECTOR_WAIT_CHARAC_DISCOVERY:
    /* Waiting the end of the Blood Pressure Measurement Characteristic discovery */ 
    break;
       
  case BLOOD_PRESSURE_COLLECTOR_ENABLE_BP_MEASUREMENT_INDICATION:                 
   {
      ret = BPC_Enable_BP_Measurement_Char_Indication();
      if (ret == BLE_STATUS_SUCCESS) {
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_WAIT_ENABLE_BP_MEASUREMENT_INDICATION;
      } else {
	BPC_FullConfError_CB(BPC_ERROR_IN_ENABLE_NOTIFICATION, ret);
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_DISCONNECTION;
      }
    }
    break; 
  case BLOOD_PRESSURE_COLLECTOR_WAIT_ENABLE_BP_MEASUREMENT_INDICATION:             
     break;
     
  case BLOOD_PRESSURE_COLLECTOR_ENABLE_ICP_NOTIFICATION:       
    {
      ret = BPC_Enable_ICP_Char_Notification();
      if (ret == BLE_STATUS_SUCCESS) {
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_WAIT_ENABLE_ICP_NOTIFICATION;
      } else {
	BPC_FullConfError_CB(BPC_ERROR_IN_ENABLE_NOTIFICATION, ret);
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_DISCONNECTION;
      }
    }
    break; 
  case BLOOD_PRESSURE_COLLECTOR_WAIT_ENABLE_ICP_NOTIFICATION:       
     break;
       
  case BLOOD_PRESSURE_COLLECTOR_READ_BP_FEATURE:
    {
      ret = BPC_Read_BP_Feature();
      if (ret == BLE_STATUS_SUCCESS) {
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_WAIT_READ_BP_FEATURE;
      } else {
	BPC_FullConfError_CB(BPC_ERROR_IN_READ, ret);
	bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_DISCONNECTION;
      }
    }
    break;
  case BLOOD_PRESSURE_COLLECTOR_WAIT_READ_BP_FEATURE:
    /* Waiting the end of the read  procedure */
    break;
    
  case BLOOD_PRESSURE_COLLECTOR_DISCONNECTION:
    {
      ret = BPC_DeviceDisconnection();
      if (ret != BLE_STATUS_SUCCESS) {
	BPC_FullConfError_CB(BPC_ERROR_IN_DISCONNECTION, ret);
      }
      bloodPressureCollectorContext.state = BLOOD_PRESSURE_COLLECTOR_INITIALIZED;
    }
    break;
  }
}


/**
  * @brief  Store & analyse the Characteristic Value for Blood Pressure Feature Char
  * @param [in] None
  * @retval Error Code
  */
void BPC_Read_BP_Feature_CB(void)
{ 
  uint16_t BPFeatureV;
  tMeasurementStatus measurStatus;
  
  measurStatus = bloodPressureCollectorContext.BPMeasurementValue.measurementStatus;
      
  BPFeatureV = (uint16_t)(bloodPressureCollectorContext.data[1] <<8) 
               | bloodPressureCollectorContext.data[0];
  
  BP_INFO_MSG(profiledbgfile, "Blood Pressure Feature Read Characteristic: 0x%04x \n", BPFeatureV);
  
  if (BPFeatureV & BODY_MOVEMENT_DETECTION_SUPPORT_BIT) 
  {
    BP_INFO_MSG(profiledbgfile, "Body Movement Detection feature supported.\n"); 
    if (measurStatus.bodyMovementDetectionFlag == 1)
      BP_INFO_MSG(profiledbgfile, "Body movement during measurement.\n");
    else
      BP_INFO_MSG(profiledbgfile, "No body movement.\n");
  }
  
  if (BPFeatureV &  CUFF_FIT_DETECTION_SUPPORT_BIT) 
  {
    BP_INFO_MSG(profiledbgfile, "Cuff Fit Detection feature supported.\n");
    if (measurStatus.cuffFitDetectionFlag == 1)
      BP_INFO_MSG(profiledbgfile, "Cuff too loose.\n");
    else
      BP_INFO_MSG(profiledbgfile, "Cuff fits properly.\n");   
  } 
  
  if (BPFeatureV & IRREGULAR_PULSE_DETECTION_SUPPORT_BIT)
  {
    BP_INFO_MSG(profiledbgfile, "Irregular Pulse Detection feature supported.\n");
    if (measurStatus.irregularPulseDetectionFlag == 1)
      BP_INFO_MSG(profiledbgfile, "Irregular pulse detected.\n");
    else
      BP_INFO_MSG(profiledbgfile, "No irregular pulse detected.\n"); 
  } 
  
  if (BPFeatureV & PULSE_RATE_RANGE_DETECTION_SUPPORT_BIT)
  {
    BP_INFO_MSG(profiledbgfile, "Pulse Rate Range Detection feature supported.\n");
    if (measurStatus.irregularPulseDetectionFlag == 0)
      BP_INFO_MSG(profiledbgfile, "Pulse rate is within the range.\n");
    else if (measurStatus.irregularPulseDetectionFlag == 1)
      BP_INFO_MSG(profiledbgfile, "Pulse rate exceeds upper limit.\n");
    else if (measurStatus.irregularPulseDetectionFlag == 2)
      BP_INFO_MSG(profiledbgfile, "Pulse rate is less than lower limit.\n");
    else if (measurStatus.irregularPulseDetectionFlag == 3)     
      BP_INFO_MSG(profiledbgfile, "Pulse rate: Reserved for future use.\n"); 
  } 
  
  if (BPFeatureV & MEASUREMENT_POSITION_DETECTION_SUPPORT_BIT)
  {
    BP_INFO_MSG(profiledbgfile, "Measurement Position Detection feature supported.\n");   
    if (measurStatus.measurementPositionDetectionFlag ==1)
      BP_INFO_MSG(profiledbgfile, "Improper measurement position.\n");
    else
      BP_INFO_MSG(profiledbgfile, "Proper measurement position.\n");     
  } 
  
  if (BPFeatureV & MULTIPLE_BOND_SUPPORT_BIT)
    BP_INFO_MSG(profiledbgfile, "Multiple Bonds supported.\n");  
  
  if ((BPFeatureV & RFU_6) || (BPFeatureV & RFU_7)  || (BPFeatureV & RFU_7) || 
      (BPFeatureV & RFU_8)  || (BPFeatureV & RFU_9) || (BPFeatureV & RFU_10) || 
      (BPFeatureV & RFU_11)  || (BPFeatureV & RFU_12) || (BPFeatureV & RFU_13) || 
      (BPFeatureV & RFU_14)  || (BPFeatureV & RFU_15))
    BP_INFO_MSG(profiledbgfile, "Blood Pressure Feature Char Value: Reserved for future use.\n");
  
  bloodPressureCollectorContext.BPFeatureValue = BPFeatureV;
  
} /* end BPC_Read_Temperature_Type_CB() */


static tDateTime printTimeStamp(uint8_t index, uint8_t *value) 
{
  tDateTime TimeStamp;
  
  index += 2;
  TimeStamp.year = (uint16_t)(value[index]<<8) | value[index-1];
  index++;
  TimeStamp.month = value[index];
  index++;
  TimeStamp.day = value[index];
  index++;
  TimeStamp.hours = value[index];
  index++;
  TimeStamp.minutes = value[index];
  index++;
  TimeStamp.seconds = value[index];  
  BP_INFO_MSG(profiledbgfile,"Time Stamp: %04d-%02d-%02d, %02d:%02d:%02d\n",  TimeStamp.year, 
                     TimeStamp.month, TimeStamp.day, TimeStamp.hours, TimeStamp.minutes, TimeStamp.seconds);  
  return (TimeStamp);
}

static tMeasurementStatus printMeasurementStatus(uint16_t MeasurStatusFlag)
{
  tMeasurementStatus MeasurStatus;
  
  BP_INFO_MSG(profiledbgfile, "Measurement Status Field Value: 0x%04x \n", MeasurStatusFlag);
  
  if (MeasurStatusFlag & BODY_MOVEMENT_DETECTION_FLAG) 
    MeasurStatus.bodyMovementDetectionFlag = 1;
  else
    MeasurStatus.bodyMovementDetectionFlag = 0;
  
  if (MeasurStatusFlag &  CUFF_FIT_DETECTION_FLAG) 
    MeasurStatus.cuffFitDetectionFlag = 1;
  else
    MeasurStatus.cuffFitDetectionFlag = 0;
  
  if (MeasurStatusFlag & IRREGULAR_PULSE_DETECTION_FLAG)
    MeasurStatus.irregularPulseDetectionFlag = 1;
  else
    MeasurStatus.irregularPulseDetectionFlag = 0;
  
  if (MeasurStatusFlag & PULSE_RATE_RANGE_EXCEEDS_UPPER_LIMIT)
    MeasurStatus.pulseRateRangeDetectionFlag = 1;
  else
    MeasurStatus.pulseRateRangeDetectionFlag = 0;
  if (MeasurStatusFlag & PULSE_RATE_RANGE_BELOW_LOWER_LIMIT)
    MeasurStatus.pulseRateRangeDetectionFlag = 3;
  else
    MeasurStatus.pulseRateRangeDetectionFlag = 2;
  
  if (MeasurStatusFlag & MEASUREMENT_POSITION_DETECTION_FLAG)
    MeasurStatus.measurementPositionDetectionFlag = 1;
  else
    MeasurStatus.measurementPositionDetectionFlag = 0;
  
  if ((MeasurStatusFlag & RFU_6) || (MeasurStatusFlag & RFU_7)  || (MeasurStatusFlag & RFU_7) || 
      (MeasurStatusFlag & RFU_8)  || (MeasurStatusFlag & RFU_9) || (MeasurStatusFlag & RFU_10) || 
      (MeasurStatusFlag & RFU_11)  || (MeasurStatusFlag & RFU_12) || (MeasurStatusFlag & RFU_13) || 
      (MeasurStatusFlag & RFU_14)  || (MeasurStatusFlag & RFU_15))
    BP_INFO_MSG(profiledbgfile, "Measurement Status Field: Reserved for future use.\n");
  
  return MeasurStatus;
}


/**
* @brief  It stores the Temperature Measurement Characteristics Indication
* @param  attr_handle: Temperature Measurement characteristic handle
* @param  data_lenght: Temperature Measurement characteristic value lenght
* @param  value: Temperature Measurement characteristic value 
*/

void BPC_BP_Measurement_Indication_CB(uint16_t attr_handle, uint8_t data_length, uint8_t * value)
{
  uint8_t index;
  int8_t exponent; 
  int16_t mantissa;
  uint16_t charValue, MeasurStatusFlag;
  float BPMeasFinal;
  tBleStatus retval = BLE_STATUS_SUCCESS;
  /* Blood Pressure measurement characteristic */
  tBPMValue BPMeasurement;
  
  /* Check indication value handle: Blood Pressure measurement characteristic indication */ 
  if (attr_handle == bloodPressureCollectorContext.BPMeasurementCharValHandle)
  {
    BP_INFO_MSG(profiledbgfile, "BPC_BP_Measurement_Indication_CB()\n");
    BLUENRG_memset(&BPMeasurement, 0 , sizeof(tBPMValue)); 
    
    /* To check the Flags value field to determine the contents of  
       Blood Pressure Measurement and the format of Char value field. */
    index = 0; 
    BPMeasurement.flags = value[index];  
    
    if ((BPMeasurement.flags & RFU_5) || (BPMeasurement.flags & RFU_6) || (BPMeasurement.flags & RFU_7))
      BP_INFO_MSG(profiledbgfile, "Reserved flag (Optional):  0x%02x\n", BPMeasurement.flags);
   
    /** 
    * The Blood Pressure Measurement Field is a SFLOAT-Type 16-bit data type that contains
    * an 4-bit signed exponent to base 10, followed by a 12-bit signed integer (mantissa).
    */
    if (BPMeasurement.flags & FLAG_BLOOD_PRESSURE_UNITS_KPA)
    {
      /* Pressure measurement Unit: kilo Pascal (kPa) */
      BPMeasurement.bloodPressureUnit = UNIT_BP_kPa;
      
      //Systolic
      index += 2;       
      charValue = (uint16_t)(value[index]<<8) | value[index-1];    
      exponent = (int8_t) (charValue >> 12);      //take the first 4 bits
      mantissa = (uint16_t) (charValue & 0x0FFF); //reset the first  4 bits
      BPMeasurement.systolic_mantissa = mantissa;
      BPMeasurement.systolic_exponent = exponent;
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Mantissa - Systolic (kPa): 0x%04x (%d)\n", mantissa, mantissa);
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Exponent - Systolic (kPa): 0x%02x (%d)\n", exponent, exponent);
      if (mantissa == 0x07FF)
      {
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - Systolic: NaN.\n");
      }
      else
      {
        /* Bload Pressure Measurement Value = Mantissa * pow(10, Exponent)*/
        if (exponent == 0x0f)
          exponent = -1;
        BPMeasFinal = (float)(mantissa * pow(10, exponent)); 
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - Systolic:  %g kPa\n", BPMeasFinal);
      }  
      //Diastolic
      index += 2;       
      charValue = (uint16_t)(value[index]<<8) | value[index-1];    
      exponent = (int8_t) (charValue >> 12);      //take the first 4 bits
      mantissa = (uint16_t) (charValue & 0x0FFF); //reset the first  4 bits
      BPMeasurement.diastolic_mantissa = mantissa;
      BPMeasurement.diastolic_exponent = exponent;
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Mantissa - Diastolic (kPa): 0x%04x (%d)\n", mantissa, mantissa);
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Exponent - Diastolic (kPa): 0x%02x (%d)\n", exponent, exponent);
      if (mantissa == 0x07FF)
      {
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - Diastolic: NaN.\n");
      }
      else
      {
        /* Bload Pressure Measurement Value = Mantissa * pow(10, Exponent)*/
        if (exponent == 0x0f)
          exponent = -1;
        BPMeasFinal = (float)(mantissa * pow(10, exponent)); 
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - Diastolic:  %g kPa\n", BPMeasFinal);
      } 
      //Mean Arterial Pressure (mpa)
      index += 2;       
      charValue = (uint16_t)(value[index]<<8) | value[index-1];  
      exponent = (int8_t) (charValue >> 12);      //take the first 4 bits
      mantissa = (uint16_t) (charValue & 0x0FFF); //reset the first  4 bits
      BPMeasurement.map_mantissa = mantissa;
      BPMeasurement.map_exponent = exponent;
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Mantissa - MPA (kPa): 0x%04x (%d)\n", mantissa, mantissa);
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Exponent - MPA (kPa): 0x%02x (%d)\n", exponent, exponent);
      if (mantissa == 0x07FF)
      {
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - MPA: NaN.\n");
      }
      else
      {
        /* Bload Pressure Measurement Value = Mantissa * pow(10, Exponent)*/
        if (exponent == 0x0f)
          exponent = -1;
        BPMeasFinal = (float)(mantissa * pow(10, exponent)); 
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - MPA:  %g kPa\n", BPMeasFinal);
      }  
    }
    else  
    {
      /* Pressure measurement Unit: millimetre of mercury (mmHg)*/
      BPMeasurement.bloodPressureUnit = UNIT_BP_mmHg;
      
      //Systolic
      index += 2;       
      charValue = (uint16_t)(value[index]<<8) | value[index-1];    
      exponent = (int8_t) (charValue >> 12);      //take the first 4 bits
      mantissa = (int16_t) (charValue & 0x0FFF);  //reset the first  4 bits
      BPMeasurement.systolic_mantissa = mantissa;
      BPMeasurement.systolic_exponent = exponent;
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Mantissa - Systolic (mmHg): 0x%04x (%d)\n", mantissa, mantissa);
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Exponent - Systolic (mmHg): 0x%02x (%d)\n", exponent, exponent);
      if (mantissa == 0x07FF)
      {
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - Systolic: NaN.\n");
      }
      else
      {
        /* Bload Pressure Measurement Value = Mantissa * pow(10, Exponent)*/
        if (exponent == 0x0f)
          exponent = -1;
        BPMeasFinal = (float)(mantissa * pow(10, exponent)); 
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - Systolic:  %g mmHg\n", BPMeasFinal);
      }  
      //Diastolic
      index += 2;       
      charValue = (uint16_t)(value[index]<<8) | value[index-1];   
      exponent = (int8_t) (charValue >> 12);      //take the first 4 bits
      mantissa = (int16_t) (charValue & 0x0FFF);  //reset the first  4 bits
      BPMeasurement.diastolic_mantissa = mantissa;
      BPMeasurement.diastolic_exponent = exponent;
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Mantissa - Diastolic (mmHg): 0x%04x (%d)\n", mantissa, mantissa);
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Exponent - Diastolic (mmHg): 0x%02x (%d)\n", exponent, exponent);
      if (mantissa == 0x07FF)
      {
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - Diastolic: NaN.\n");
      }
      else
      {
        /* Bload Pressure Measurement Value = Mantissa * pow(10, Exponent)*/
        if (exponent == 0x0f)
          exponent = -1;
        BPMeasFinal = (float)(mantissa * pow(10, exponent)); 
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - Diastolic:  %g mmHg\n", BPMeasFinal);
      } 
      //Mean Arterial Pressure (mpa)
      index += 2;       
      charValue = (uint16_t)(value[index]<<8) | value[index-1];    
      exponent = (int8_t) (charValue >> 12);      //take the first 4 bits
      mantissa = (int16_t) (charValue & 0x0FFF);  //reset the first  4 bits
      BPMeasurement.map_mantissa = mantissa;
      BPMeasurement.map_exponent = exponent;
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Mantissa - MPA (mmHg): 0x%04x (%d)\n", mantissa, mantissa);
      BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value Exponent - MPA (mmHg): 0x%02x (%d)\n", exponent, exponent);
      if (mantissa == 0x07FF)
      {
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - MPA: NaN.\n");
      }
      else
      {
        /* Bload Pressure Measurement Value = Mantissa * pow(10, Exponent)*/
        if (exponent == 0x0f)
          exponent = -1;
        BPMeasFinal = (float)(mantissa * pow(10, exponent)); 
        BP_INFO_MSG(profiledbgfile, "Blood Pressure Measurement Value - MPA:  %g mmHg\n", BPMeasFinal);
      }          
    }  
  
    /* Time Stamp field is present */
    if (BPMeasurement.flags & FLAG_TIME_STAMP_PRESENT )
    {  
      BPMeasurement.timeStamp = printTimeStamp(index, value); 
      index += 7;
    }
    else   /* Time Stamp field is not present */
      BP_INFO_MSG(profiledbgfile, "Time Stamp is not present\n");  

    /* Pulse Rate field is present */
    if ((BPMeasurement.flags & FLAG_PULSE_RATE_PRESENT))
    {    
      index += 2;       
      charValue = (uint16_t)(value[index]<<8) | value[index-1];    
      exponent = (int8_t) (charValue >> 12);      //take the first 4 bits
      mantissa = (uint16_t) (charValue & 0x0FFF); //reset the first  4 bits
      BPMeasurement.pulseRate_mantissa = mantissa;
      BPMeasurement.pulseRate_exponent = exponent;
      BP_INFO_MSG(profiledbgfile, "Pulse Rate Field Value Mantissa: 0x%04x (%d)\n", mantissa, mantissa);
      BP_INFO_MSG(profiledbgfile, "Pulse Rate Field Value Exponent: 0x%02x (%d)\n", exponent, exponent);
      if (mantissa == 0x07FF)  
        BP_INFO_MSG(profiledbgfile, "Pulse Rate Field Value: NaN.\n");
      else
      {
        /* Bload Pressure Measurement Value = Mantissa * pow(10, Exponent)*/
        if (exponent == 0x0f)
          exponent = -1;
        BPMeasFinal = (float)(mantissa * pow(10, exponent)); 
        BP_INFO_MSG(profiledbgfile, "Pulse Rate Field Value:  %g bpm\n", BPMeasFinal);
      }
    } 
    else /* Pulse Rate field is not present */
      BP_INFO_MSG(profiledbgfile, "Pulse Rate Field is not present.\n");
    
    /* User ID field is present */
    if (BPMeasurement.flags & FLAG_USER_ID_PRESENT)
    {
      index++;
      BPMeasurement.userID = value[index];
      if (BPMeasurement.userID == 0xFF)
        BP_INFO_MSG(profiledbgfile, "User ID Field Value: Unknown User\n");
      else
        BP_INFO_MSG(profiledbgfile, "User ID Field Value: %d \n", BPMeasurement.userID);
    }
    else /* User ID field is not present */
      BP_INFO_MSG(profiledbgfile, "User ID Field Value is not present.\n");              
                    
    /* Measurement Status field is present */
    if (BPMeasurement.flags & FLAG_MEASUREMENT_STATUS_PRESENT)
    {
      index += 2;  
      MeasurStatusFlag = (uint16_t)(value[index]<<8) | value[index-1]; 
      BPMeasurement.measurementStatus = printMeasurementStatus(MeasurStatusFlag);
    }
    else /* Measurement Status field is not present */
     BP_INFO_MSG(profiledbgfile, "Measurement Status Field Value is not present.\n"); 
    
    /* Additional Octets */
    if (data_length > index +1)
    {  
      BP_INFO_MSG(profiledbgfile, "Additional Octets: 0x"); 
      for (uint8_t i=index+1; i<data_length; i++)
        BP_INFO_MSG(profiledbgfile, "%02x", value[i]);
      BP_INFO_MSG(profiledbgfile, "\n"); 
    }
    
    BLUENRG_memcpy(&bloodPressureCollectorContext.BPMeasurementValue, &BPMeasurement, sizeof(BPMeasurement));
    
    /* Send confirm for indication */
    retval =  aci_gatt_confirm_indication(bloodPressureCollectorContext.connHandle);  /* connection handle */  
    if(retval == BLE_STATUS_SUCCESS)
    {
      BP_INFO_MSG(profiledbgfile,"BPC_BP_Measurement_Indication_CB(), aci_gatt_confirm_indication() OK\n");
    }
    else
    {
      BP_INFO_MSG(profiledbgfile,"BPC_BP_Measurement_Indication_CB(), aci_gatt_confirm_indication() Failed %04X \n", retval);
    }
  }

} /* end BPC_BP_Measurement_Indication_CB() */


/**
* @brief  It stores the Intermediate Temperature Characteristics Notification
* @param  attr_handle: Intermediate Temperature characteristic handle
* @param  data_lenght: Intermediate Temperature characteristic value lenght
* @param  value: Intermediate Temperature characteristic value 
*/
void BPC_ICP_Notification_CB(uint16_t attr_handle, uint8_t data_length,uint8_t * value)
{
  uint8_t index;
  int8_t exponent; 
  int16_t mantissa;
  uint16_t charValue, MeasurStatusFlag;
  float ICPFinal;
  /* Intermediate Cuff Pressure (ICP) characteristic */
  tICPValue ICPValue;
  
  /* Check indication value handle: Blood Pressure measurement characteristic indication */ 
  if (attr_handle == bloodPressureCollectorContext.ICPCharValHandle)
  {
    BP_INFO_MSG(profiledbgfile, "BPC_ICP_Notification_CB()\n");
    BLUENRG_memset(&ICPValue, 0 , sizeof(tICPValue)); 
    
    /* To check the Flags value field to determine the contents of  
       Intermediate Cuff Pressure and the format of Char value field. */
    index = 0; 
    ICPValue.flags = value[index];  
   
    if ((ICPValue.flags & RFU_5) || (ICPValue.flags & RFU_6) || (ICPValue.flags & RFU_7))
      BP_INFO_MSG(profiledbgfile, "Reserved flag (Optional):  0x%02x\n", ICPValue.flags);
   
    /** 
    * The ICP Field is a SFLOAT-Type 16-bit data type that contains
    * an 4-bit signed exponent to base 10, followed by a 12-bit signed integer (mantissa).
    */
    
    /* ICP compound value is composed of three subfields:
     *   Current Cuff Pressure and two unused subfields */
    if (ICPValue.flags & FLAG_BLOOD_PRESSURE_UNITS_KPA)
    {
       /* Pressure measurement Unit: kilo Pascal (kPa) */  
      ICPValue.bloodPressureUnit = UNIT_BP_kPa;
      
      index += 2;       
      charValue = (uint16_t)(value[index]<<8) | value[index-1];    
      exponent = (int8_t) (charValue >> 12);      //take the first 4 bits
      mantissa = (int16_t) (charValue & 0x0FFF);  //reset the first  4 bits
      ICPValue.icp_mantissa = mantissa;
      ICPValue.icp_exponent = exponent;
      BP_INFO_MSG(profiledbgfile, "Intermediate Cuff Pressure Value Mantissa (kPa): 0x%04x (%d)\n", mantissa, mantissa);
      BP_INFO_MSG(profiledbgfile, "Intermediate Cuff Pressure Value Exponent (kPa): 0x%02x (%d)\n", exponent, exponent);
      if (mantissa == 0x07FF)
        BP_INFO_MSG(profiledbgfile, "Intermediate Cuff Pressure Value: NaN.\n");
      else
      {
        /* Bload Pressure Measurement Value = Mantissa * pow(10, Exponent)*/
        if (exponent == 0x0f)
          exponent = -1;
        ICPFinal = (float)(mantissa * pow(10, exponent)); 
        BP_INFO_MSG(profiledbgfile, "Intermediate Cuff Pressure Value:  %g kPa\n", ICPFinal);
      }   
    }
    else 
    {
      /* Pressure measurement Unit: millimetre of mercury (mmHg)*/
      ICPValue.bloodPressureUnit = UNIT_BP_mmHg;      
      index += 2;       
      charValue = (uint16_t)(value[index]<<8) | value[index-1];    
      exponent = (int8_t) (charValue >> 12);      //take the first 4 bits
      mantissa = (int16_t) (charValue & 0x0FFF);  //reset the first  4 bits
      ICPValue.icp_mantissa = mantissa;
      ICPValue.icp_exponent = exponent;
      BP_INFO_MSG(profiledbgfile, "Intermediate Cuff Pressure Value Mantissa (mmHg): 0x%04x (%d)\n", mantissa, mantissa);
      BP_INFO_MSG(profiledbgfile, "Intermediate Cuff Pressure Value Exponent (mmHg): 0x%02x (%d)\n", exponent, exponent);
      if (mantissa == 0x07FF)
        BP_INFO_MSG(profiledbgfile, "Intermediate Cuff Pressure Value: NaN.\n");
      else
      {
        /* Bload Pressure Measurement Value = Mantissa * pow(10, Exponent)*/
        if (exponent == 0x0f)
          exponent = -1;
        ICPFinal = (float)(mantissa * pow(10, exponent)); 
        BP_INFO_MSG(profiledbgfile, "Intermediate Cuff Pressure Value:  %g mmHg\n", ICPFinal);
      }  
    }
    /* ICP compound value is composed of three subfields:
     * these are the two unused subfields (they shall be set to special value NaN) */
    index += 2; 
    charValue = (uint16_t)(value[index]<<8) | value[index-1]; 
    if (charValue != 0x07FF)
      BP_INFO_MSG(profiledbgfile, "Subfield 2 of ICP Compund Value is different from NaN.\n");
    index += 2; 
    charValue = (uint16_t)(value[index]<<8) | value[index-1];  
    if (charValue != 0x07FF)  
      BP_INFO_MSG(profiledbgfile, "Subfield 3 of ICP Compund Value is different from NaN.\n");
    
    /* Time Stamp field is present */
    if (ICPValue.flags & FLAG_TIME_STAMP_PRESENT)
    {  
      ICPValue.timeStamp = printTimeStamp(index, value); 
      index += 7;
    }
    else   /* Time Stamp field is not present */
      BP_INFO_MSG(profiledbgfile, "Time Stamp is not present\n");  

    /* Pulse Rate field is present */
    if (ICPValue.flags & FLAG_PULSE_RATE_PRESENT)
    {    
      index += 2;       
      charValue = (uint16_t)(value[index]<<8) | value[index-1];    
      exponent = (int8_t) (charValue >> 12);      //take the first 4 bits
      mantissa = (uint16_t) (charValue & 0x0FFF); //reset the first  4 bits
      ICPValue.pulseRate_mantissa = mantissa;
      ICPValue.pulseRate_exponent = exponent;
      BP_INFO_MSG(profiledbgfile, "Pulse Rate Field Value Mantissa: 0x%04x (%d)\n", mantissa, mantissa);
      BP_INFO_MSG(profiledbgfile, "Pulse Rate Field Value Exponent: 0x%02x (%d)\n", exponent, exponent);
      if (mantissa == 0x07FF)
        BP_INFO_MSG(profiledbgfile, "Pulse Rate Field Value: NaN.\n");
      else
      {
        /* Bload Pressure Measurement Value = Mantissa * pow(10, Exponent)*/
        if (exponent == 0x0f)
          exponent = -1;
        ICPFinal = (float)(mantissa * pow(10, exponent)); 
        BP_INFO_MSG(profiledbgfile, "Pulse Rate Field Value:  %g bpm\n", ICPFinal);
      }
    } 
    else /* Pulse Rate field is not present */
      BP_INFO_MSG(profiledbgfile, "Pulse Rate Field is not present.\n");
    
    /* User ID field is present */
    if (ICPValue.flags & FLAG_USER_ID_PRESENT)
    {
      index++;
      ICPValue.userID = value[index];
      if (ICPValue.userID == 0xFF)
        BP_INFO_MSG(profiledbgfile, "User ID Field Value: Unknown User\n");
      else
        BP_INFO_MSG(profiledbgfile, "User ID Field Value: %d \n", ICPValue.userID);
    }
    else /* User ID field is not present */
      BP_INFO_MSG(profiledbgfile, "User ID Field Value is not present.\n");              
                    
    /* Measurement Status field is present */
    if (ICPValue.flags & FLAG_MEASUREMENT_STATUS_PRESENT)
    {
      index += 2;  
      MeasurStatusFlag = (uint16_t)(value[index]<<8) | value[index-1]; 
      ICPValue.measurementStatus = printMeasurementStatus(MeasurStatusFlag);
    }
    else /* Measurement Status field is not present */
     BP_INFO_MSG(profiledbgfile, "Measurement Status Field Value is not present.\n"); 
    
    /* Additional Octets */
    if (data_length > index +1)
    {  
      BP_INFO_MSG(profiledbgfile, "Additional Octets: : 0x"); 
      for (uint8_t i=index+1; i<data_length; i++)
        BP_INFO_MSG(profiledbgfile, "%02x", value[i]);
      BP_INFO_MSG(profiledbgfile, "\n"); 
    }
       
    BLUENRG_memcpy(&bloodPressureCollectorContext.ICPValue, &ICPValue, sizeof(ICPValue));
  }
    
} /* end BPC_ICP_Notification_CB() */

/****************** blood pressure collector Weak Callbacks definition ***************************/

WEAK_FUNCTION(void BPC_FullConfError_CB(uint8_t error_type, uint8_t code))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & BLOOD_PRESSURE_COLLECTOR) */
