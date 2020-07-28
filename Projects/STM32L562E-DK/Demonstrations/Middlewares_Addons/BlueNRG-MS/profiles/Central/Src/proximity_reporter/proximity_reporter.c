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
*   FILENAME        -  proximity_reporter.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      03/05/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    proximity reporter central role APIs
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the proximity_reporter profile central role.
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

#if (BLE_CURRENT_PROFILE_ROLES & PROXIMITY_REPORTER)

#include <proximity_reporter.h>
#include <proximity_reporter_config.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
#define ALERT_STATUS_CHAR_LENGHT         1
#define RINGER_SETTING_CHAR_LENGHT       1
#define RINGER_CONTROL_POINT_CHAR_LENGHT 1

#define ENCRIPTION_KEY_SIZE 10 //TBR

#define WEAK_FUNCTION(x) __weak x
/******************************************************************************
* type definitions
******************************************************************************/

/******************************************************************************
* Variable Declarations
******************************************************************************/

/* phone alert status server context */
ProximityReporterContext_Type proximity_reporter_context;

/******************************************************************************
* Imported Variable
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*******************************************************************************
* exported SAP
*******************************************************************************/
tBleStatus PXR_Init(pxrInitDevType param)
{
  initDevType initParam;
  uint8_t ret;

  BLUENRG_memcpy(&proximity_reporter_context, 0, sizeof(ProximityReporterContext_Type));
  
  proximity_reporter_context.immAlertTxPowerServSupport = param.immAlertTxPowerServSupport; 
  
  BLUENRG_memcpy(proximity_reporter_context.public_addr, param.public_addr, 6);
  BLUENRG_memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    proximity_reporter_context.state = PROXIMITY_REPORTER_INITIALIZED;
  } else {
    proximity_reporter_context.state = PROXIMITY_REPORTER_UNINITIALIZED;
  }
  
  return ret;
}

tBleStatus PXR_DeviceDiscovery(pxrDevDiscType param)
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
    proximity_reporter_context.state = PROXIMITY_REPORTER_DEVICE_DISCOVERY;
  }
  return ret;
}

tBleStatus PXR_SecuritySet(pxrSecurityType param)
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

tBleStatus PXR_DeviceConnection(pxrConnDevType param)
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
    proximity_reporter_context.state = PROXIMITY_REPORTER_START_CONNECTION;
  }

  return ret;
}

tBleStatus PXR_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(proximity_reporter_context.connHandle);
  if (ret == BLE_STATUS_SUCCESS) 
  {
    /* disable fullConf on disconnection */
    //proximity_reporter_context.fullConf = FALSE;//TBR
  }
  return ret;
}

tBleStatus PXR_ConnectionParameterUpdateRsp(uint8_t accepted, pxrConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(proximity_reporter_context.connHandle, 
					     accepted, (connUpdateParamType*)param);
  return ret;
}

tBleStatus PXR_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(proximity_reporter_context.connHandle, FALSE);
  return ret;
}

tBleStatus PXR_Clear_Security_Database()
{
  tBleStatus ble_status = BLE_STATUS_FAILED; 
  
  /* Clear Profile security database */
  ble_status = Master_ClearSecurityDatabase();
  return (ble_status);
  
}/* end PXR_Clear_Security_Database() */

tBleStatus PXR_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(proximity_reporter_context.connHandle, pinCode);
  
  return ret;
}

void PXR_StateMachine(void)//TBR: Is It needed?
{

  switch(proximity_reporter_context.state) {
   case PROXIMITY_REPORTER_UNINITIALIZED:
    /* Nothing to do */
    break;
  case PROXIMITY_REPORTER_CONNECTED_IDLE:
    {
    }
    break;
  case PROXIMITY_REPORTER_INITIALIZED:
    /* Nothing to do */
    break;
  case PROXIMITY_REPORTER_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case PROXIMITY_REPORTER_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case PROXIMITY_REPORTER_CONNECTED:
    {
      proximity_reporter_context.state = PROXIMITY_REPORTER_CONNECTED_IDLE;
    }
    break;
  }
    
}/* end PXR_StateMachine() */


tBleStatus PXR_Add_Services_Characteristics(void)
{
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;
  uint8_t serviceMaxAttributeRecord = 0;
  uint8_t txPwrLevlCharPresFormatDescValue[7] = {0};
  int8_t txPwrLevel=0;
  uint16_t UUID = 0;
  uint16_t charDescHandle;
  
  /* Max_Attribute_Records = 2*no_of_char + 1 */
  /* serviceMaxAttributeRecord = 1 for link loss service itself +
   *                             2 for Alert Level characteristic 
   */
  serviceMaxAttributeRecord = 3;

  /* store the service UUID in LE format */ 
  UUID = LINK_LOSS_SERVICE_UUID;

  hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                   (const uint8_t *)&UUID, 
                                   PRIMARY_SERVICE, 
                                   serviceMaxAttributeRecord,
                                   &proximity_reporter_context.linkLossServiceHandle);
  
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), Link Loss Service is added Successfully %04X\n", proximity_reporter_context.linkLossServiceHandle);
  }
  else 
  {
    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile,"PXR_Add_Services_Characteristics(),FAILED to add Link Loss Service, Error: %02X !!\n", hciCmdResult);
    return hciCmdResult;
  }
  /* add alert level characteristic */
  UUID = ALERT_LEVEL_CHARACTERISTIC_UUID;
  hciCmdResult = aci_gatt_add_char(proximity_reporter_context.linkLossServiceHandle, 
                                   UUID_TYPE_16, 
                                   (const uint8_t *)&UUID, 
                                   1, 
                                   CHAR_PROP_READ | CHAR_PROP_WRITE, 
                                   ATTR_PERMISSION_NONE, 
                                   GATT_NOTIFY_ATTRIBUTE_WRITE, 
                                   10,
                                   CHAR_VALUE_LEN_CONSTANT,
                                   &proximity_reporter_context.linkLossAlertLevelCharHandle);

  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), Alert Level Characteristic Added Successfully:0x%04x \n",
                                 proximity_reporter_context.linkLossAlertLevelCharHandle);
  }
  else
  {
    PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), FAILED to add Link Loss Alert Level Characteristic: Error: 0x%02X !!\n",
                                 hciCmdResult);
    return hciCmdResult;
  }
 
  /* check if Immediate Alert service and the Tx Power service should be supproted */
  if (proximity_reporter_context.immAlertTxPowerServSupport)
  { 
    /* Max_Attribute_Records = 2*no_of_char + 1 */
    /* serviceMaxAttributeRecord = 1 for Immediate Alert service itself +
     *                             2 for Alert Level characteristic
     */
    serviceMaxAttributeRecord = 3;

    /* store the service UUID in LE format */ 
    UUID = IMMEDIATE_ALERT_SERVICE_UUID;
    /* add immediate alert service */
    hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                     (const uint8_t *)&UUID, 
                                     PRIMARY_SERVICE, 
                                     serviceMaxAttributeRecord,
                                     &proximity_reporter_context.immdiateAlertServiceHandle);
    
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), Immediate Alert Service is added Successfully: 0x%04x \n",
                                   proximity_reporter_context.immdiateAlertServiceHandle);
    }
    else
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), FAILED to add Immediate Alert Service: Error: 0x%02X !!\n", 
                                   hciCmdResult);
     return hciCmdResult;
    }
    
    /* add immediate aler level characteristic to immediate alert service */
    UUID = ALERT_LEVEL_CHARACTERISTIC_UUID;
    hciCmdResult = aci_gatt_add_char(proximity_reporter_context.immdiateAlertServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *)&UUID, 
                                     1, 
                                     CHAR_PROP_WRITE_WITHOUT_RESP, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_NOTIFY_ATTRIBUTE_WRITE, 
                                     10,
                                     CHAR_VALUE_LEN_CONSTANT,
                                     &proximity_reporter_context.immediateAlertLevelCharHandle);

    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), Immediate Alert Level Characteristic Added Successfully :0x%04x \n",
                                   proximity_reporter_context.immediateAlertLevelCharHandle);
    }
    else
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), FAILED to add Immediate Alert Level Characteristic: Error: 0x%02X !!\n",
                                   hciCmdResult);
      return hciCmdResult;
    }
   
    /* Max_Attribute_Records = 2*no_of_char + 1 */
    /* serviceMaxAttributeRecord = 1 for Tx Power service itself +
     *                             2 for Tx Power Level characteristic
     *                             1 for client char config descriptor +
     *                             1 for char presentation format descriptor 
     */
    serviceMaxAttributeRecord = 5;
    
    UUID = TX_POWER_SERVICE_UUID;
    /* add tx power service */
    hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16,
                                     (const uint8_t *)&UUID,
                                     PRIMARY_SERVICE, 
                                     serviceMaxAttributeRecord,
                                     &proximity_reporter_context.txPowerServiceHandle);

    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), Tx Power Service Added Successfully 0x%04x \n",
                                   proximity_reporter_context.txPowerServiceHandle);
    }
    else
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), FAILED to add Tx Power Service: Error: 0x%02X !!\n", 
                                   hciCmdResult);
      return hciCmdResult;
    }
    
    /* Add Tx Power Level Characteristic */
    UUID = TX_POWER_LEVEL_CHARACTERISTIC_UUID;
    hciCmdResult = aci_gatt_add_char(proximity_reporter_context.txPowerServiceHandle, 
                                     UUID_TYPE_16, 
                                     (const uint8_t *)&UUID, 
                                     1, 
                                     CHAR_PROP_READ, 
                                     ATTR_PERMISSION_NONE, 
                                     GATT_DONT_NOTIFY_EVENTS, 
                                     10,
                                     CHAR_VALUE_LEN_CONSTANT,
                                     &proximity_reporter_context.txPowerLevelCharHandle);

    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), Tx Power Level Characteristic Added Successfully 0x%04x \n", 
                                   proximity_reporter_context.txPowerLevelCharHandle);
    }
    else
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), FAILED to add Tx Power Level Characteristic: Error: %02X !!\n", 
                                   hciCmdResult);
      return hciCmdResult;
    }

    /* Add Tx Pwr Levl Char Presentation Format Descriptor */
    
    txPwrLevlCharPresFormatDescValue[0] = 0x0C;
    txPwrLevlCharPresFormatDescValue[1] = 0x00;
    txPwrLevlCharPresFormatDescValue[2] = 0x00;
    txPwrLevlCharPresFormatDescValue[3] = 0x27;
    txPwrLevlCharPresFormatDescValue[4] = 0x00;
    txPwrLevlCharPresFormatDescValue[5] = 0x00;
    txPwrLevlCharPresFormatDescValue[6] = 0x00;

    UUID = CHAR_PRESENTATION_FORMAT_DESCRIPTOR_UUID;
    hciCmdResult = aci_gatt_add_char_desc(proximity_reporter_context.txPowerServiceHandle, 
                                          proximity_reporter_context.txPowerLevelCharHandle, 
                                          UUID_TYPE_16, 
                                          (const uint8_t *)&UUID, 
                                          CHAR_PRESENTATION_FORMAT_DESCRIPTOR_LENGTH, 
                                          (uint8_t)CHAR_PRESENTATION_FORMAT_DESCRIPTOR_LENGTH,
                                          (uint8_t *)&txPwrLevlCharPresFormatDescValue,
                                          ATTR_PERMISSION_NONE, 
                                          ATTR_ACCESS_READ_ONLY, 
                                          GATT_DONT_NOTIFY_EVENTS, 
                                          10, 
                                          CHAR_VALUE_LEN_CONSTANT,
                                          &charDescHandle);

    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), Char Presentation Format Descriptor Added Successfully: 0x%04x \n", 
                                   charDescHandle);
    }
    else
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), FAILED to add Tx Power Level Characteristic: Error: 0x%02X !!\n", 
                                   hciCmdResult);
      return hciCmdResult;
    }
    
    /* set the value of the TX power level characteristic.
     * For that we first need to read the current TX power level
     */
    hci_read_transmit_power_level(&proximity_reporter_context.connHandle, //TBR???
                                  READ_CURR_TRANSMIT_POWER_LEVEL, 
                                  &txPwrLevel);
    PROXIMITY_REPORTER_MESG_DBG(profiledbgfile,"PXR_Add_Services_Characteristics(), Read TX power level: 0x%0x \n",txPwrLevel);
    
    /* Updating TX power level */
    hciCmdResult = aci_gatt_update_char_value(proximity_reporter_context.txPowerServiceHandle, 
                                              proximity_reporter_context.txPowerLevelCharHandle, 
                                              0x00,
                                              0x01, /* can it be 0 also ?*/
                                              (uint8_t*)&txPwrLevel);
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), Tw Power Level updated Successfully: 0x%02x \n", 
                                   txPwrLevel);
    }
    else
    {
      PROXIMITY_REPORTER_MESG_DBG (profiledbgfile, "PXR_Add_Services_Characteristics(), FAILED to Update Tw Power Level Characteristic: Error: 0x%02X !!\n", 
                                   hciCmdResult);
      return hciCmdResult;
    }
  }
  return (hciCmdResult);
}

void PXR_Received_Alert_Handler(uint16_t attrHandle,uint8_t attValue)
{
  
  if (attrHandle == proximity_reporter_context.linkLossAlertLevelCharHandle+1)
  {
    PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Received Link Loss Alert Level Configuration \n");
    /* when the alert level for the link loss is written by the client,
     * just store the value and it has to be used to start an alert when
     * a disconnection happens
     */ 
    PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Link Loss Service, Alert Level value: %d\n",attValue);
    if((attValue == NO_ALERT) ||
       (attValue == MILD_ALERT) ||
       (attValue == HIGH_ALERT))
    {
      proximity_reporter_context.linkLossAlertLevel = attValue;
    }
    else
    {
      /* invalid attribute value, neglect it 
       * TODO when an invalid value is received, it should be just
       * ignored or the alert level should be taken as NO_ALERT
       */
      PROXIMITY_REPORTER_MESG_DBG(profiledbgfile,"Link Loss Service, received invalid alert level\n");
    }
  }
  else if (attrHandle == proximity_reporter_context.immediateAlertLevelCharHandle+1)
  {
    /**
     * the alert level of the immediate alert service is written when
     * the proximity monitor detects a path loss. The application has to be 
     * notified so that it can start the alert procedure
     */ 
    PROXIMITY_REPORTER_MESG_DBG(profiledbgfile, "Immediate Alert Service, Alert Level %x\n",proximity_reporter_context.state);
    if (attValue == NO_ALERT)
    {
      PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Received Path Loss NO_ALERT \n");
      
      PXR_Alert_CB(EVT_PR_PATH_LOSS_ALERT,attValue);
    }
    else if (attValue == MILD_ALERT)
    {
      PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Received Path Loss MILD_ALERT \n");
      PXR_Alert_CB(EVT_PR_PATH_LOSS_ALERT,attValue);
    }
    else if (attValue == HIGH_ALERT)
    {
      PROXIMITY_REPORTER_MESG_DBG ( profiledbgfile, "Received Path Loss HIGH_ALERT \n");
      PXR_Alert_CB(EVT_PR_PATH_LOSS_ALERT,attValue);
    }
    else
    {
      /* invalid attribute value ignored*/
      PROXIMITY_REPORTER_MESG_DBG(profiledbgfile,"Received invalid alert level\n");
      PXR_Alert_CB(EVT_PR_PATH_LOSS_ALERT,attValue);
    }
  }
}
/****************** Proximity Reporter Weak Callbacks definition ***************************/

WEAK_FUNCTION(void PXR_Alert_CB(uint8_t alert_type, uint8_t alert_value))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES & PROXIMITY_REPORTER) */
