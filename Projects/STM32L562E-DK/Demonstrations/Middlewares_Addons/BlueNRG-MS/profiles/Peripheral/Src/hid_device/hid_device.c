/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  hid_device.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/06/2012
*   $Revision$:  first version
*   $Author$:    
*   Comments:    
*
*   $Date$:      29/09/2014
*   $Revision$:  second version 
*   $Author$:    
*   Comments:    Redesigned the HID Profile according to unified BlueNRG 
*                DK framework     
*******************************************************************************
*
*  File Description 
*  ---------------------
*  This file will have implementation of HID profile main functions
* 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/

#include "hci.h"
#include "bluenrg_aci.h"
#include "bluenrg_gatt_server.h"
#include "hci_const.h"
#include "bluenrg_gap.h"
#include "sm.h"
#include <stdio.h>

#include <host_config.h>
#include <debug.h>
#include <timer.h>
#include <ble_profile.h> 
#include <ble_events.h> 
#include <uuid.h>

#include <hid_device.h>
#include "hid_device_i.h"

/******************************************************************************
 * Macro Declarations
******************************************************************************/
/* protocol Mode */
#define BOOT_PROTOCOL_MODE          (0x00)
#define REPORT_PROTOCOL_MODE        (0x01)

#define ENABLE_SECURITY		     (0x01) /* security is mandatory for HID profile */

/******************************************************************************
* Variable Declarations
******************************************************************************/
tHidDeviceContext hidDevice;
uint8_t reportCharValueBuff[REPORT_CHAR_VAL_BUFF_SIZE];
uint8_t reportMapCharValueBuff[REPORT_MAP_CHAR_BUFF_SIZE_MAX];
uint8_t bootReportCharValueBuff[BOOT_DEVICE_REPORT_CHAR_BUFF_SIZE_MAX];
uint8_t hidInfoCharValueBuff[HID_INFO_CHAR_VAL_BUFF_SIZE_MAX];

/******************************************************************************
 * Imported Variable
******************************************************************************/
extern void *profiledbgfile;
extern uint8_t bnrg_expansion_board;

/******************************************************************************
 * Function Prototypes
******************************************************************************/
void HidDevice_Rx_Event_Handler(void *pckt);
tBleStatus HidDevice_CmdStatus_Event_Handler(uint16_t opcode,uint8_t status);
void HidDevice_Advertise_Period_Timeout_Handler(void);

/******************************************************************************
* SAP
******************************************************************************/
tBleStatus HidDevice_Init(uint8_t numOfHIDServices, 
                          tApplDataForHidServ * hidServiceData,
                          uint8_t numOfBatteryServices, 
                          uint16_t deviceInfoCharSupports, 
                          uint8_t scanParamServiceSupport, 
                          uint8_t scanRefreshCharSupport, 
                          BLE_CALLBACK_FUNCTION_TYPE hidDeviceCallbackFunc)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  int indx = 0;
  int indx1 = 0;
  //int j = 0;

  uint8_t * pReportCharBuff = &reportCharValueBuff[0];
  uint8_t * pReportMapCharBuff = &reportMapCharValueBuff[0];
  uint8_t * pBootReportCharBuff = &bootReportCharValueBuff[0];
  /* new code */
  uint8_t * pHidInfoCharBuff = &hidInfoCharValueBuff[0];

  HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Init(), Number of HID services: %d Battery services %d\n",numOfHIDServices, numOfBatteryServices );

  if(Is_Profile_Present(PID_HID_DEVICE) == BLE_STATUS_SUCCESS)
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Init(), HID profile is already present \n" );
    return (BLE_STATUS_PROFILE_ALREADY_INITIALIZED);
  }
      
  if (hidDeviceCallbackFunc == NULL)
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Init(), hidDeviceCallbackFunc is NULL\n" );
    return (BLE_STATUS_NULL_PARAM);
  }

  /* validate the parameters 
   * TODO : check for the report map length as well?
   */
  if((numOfHIDServices > HID_SERVICE_MAX) || 
     (numOfBatteryServices > BATTERY_SERVICE_MAX) ||
     (hidServiceData->inputReportNum > INPUT_REPORT_NUM_MAX) ||
     (hidServiceData->outputReportNum > OUTPUT_REPORT_NUM_MAX) ||
     (hidServiceData->featureReportNum > FEATURE_REPORT_NUM_MAX) ||
     (hidServiceData->extReportCharRefNum > EXT_REPORT_REF_CHAR_NUM_MAX))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Init(), Invalid parameters\n" );
    return (BLE_STATUS_INVALID_PARAMS);
  }

  BLUENRG_memset( &hidDevice, 0, sizeof(tHidDeviceContext) );
  hidDevice.numofHIDServices = numOfHIDServices;
  hidDevice.currHidServIndx = 0;

  for(indx = 0;indx < numOfHIDServices;indx++)
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HID Service Application Data %p\n", hidServiceData );

    /* The default protocol mode for all HID devices on boot should
     * be REPORT_PROTOCOL_MODE. The host changes this if required
     *  using the Set_Protocol method in USB which is equivalnet
     * of writing to the protocol mode characteristic in case of
     * the HID service in LE
     */
    hidDevice.hidService[indx].protocolMode = REPORT_PROTOCOL_MODE; //BOOT_PROTOCOL_MODE;

    /* Input Report Char */
    hidDevice.hidService[indx].inputReportCharNum = hidServiceData->inputReportNum;
    for(indx1 = 0;indx1<hidServiceData->inputReportNum;indx1++)
    {
      hidDevice.hidService[indx].inputReport[indx1].reportType = HID_REPORT_TYPE_INPUT;
      hidDevice.hidService[indx].inputReport[indx1].reportID = hidServiceData->ipReportID[indx1];
      hidDevice.hidService[indx].inputReport[indx1].reportValueLen = hidServiceData->ipReportLen[indx1];
      hidDevice.hidService[indx].inputReport[indx1].pReportValue = pReportCharBuff;
      pReportCharBuff += REPORT_CHAR_SIZE_MAX;
    }

    /* output report char */
    hidDevice.hidService[indx].outputreportCharNum = hidServiceData->outputReportNum;
    for ( indx1 = 0 ; indx1 < hidServiceData->outputReportNum ; indx1++ )
    {
      hidDevice.hidService[indx].outputReport[indx1].reportType = HID_REPORT_TYPE_OUTPUT;
      hidDevice.hidService[indx].outputReport[indx1].reportID = hidServiceData->opReportID[indx1];
      hidDevice.hidService[indx].outputReport[indx1].reportValueLen = hidServiceData->opReportLen[indx1];
      hidDevice.hidService[indx].outputReport[indx1].pReportValue = pReportCharBuff;
      pReportCharBuff += REPORT_CHAR_SIZE_MAX;
    }

    /* feature report char */
    hidDevice.hidService[indx].featureReportCharNum = hidServiceData->featureReportNum;
    for ( indx1 = 0 ; indx1 < hidServiceData->featureReportNum ; indx1++ )
    {
      hidDevice.hidService[indx].featureReport[indx1].reportType = HID_REPORT_TYPE_FEATURE;
      hidDevice.hidService[indx].featureReport[indx1].reportID = hidServiceData->ftrReportID[indx1];
      hidDevice.hidService[indx].featureReport[indx1].reportValueLen = hidServiceData->ftrReportLen[indx1];
      hidDevice.hidService[indx].featureReport[indx1].pReportValue = pReportCharBuff;
      pReportCharBuff += REPORT_CHAR_SIZE_MAX;
    }

    /* report map char */
    hidDevice.hidService[indx].reportMapCharValueLen = hidServiceData->reportMapValueLen;
    hidDevice.hidService[indx].pReportMapCharValue = pReportMapCharBuff;
    BLUENRG_memcpy(pReportMapCharBuff,hidServiceData->reportDesc,hidServiceData->reportMapValueLen);
    pReportMapCharBuff += REPORT_MAP_CHAR_SIZE_MAX;

    /* external report reference descriptor */
    if(hidServiceData->extReportCharRefNum > 0)
    {
      hidDevice.hidService[indx].extReportRefCharDescNum = hidServiceData->extReportCharRefNum;
      /* copy the parameters of the non HID services that have to be added */
      for ( indx1 = 0 ; indx1 < hidServiceData->extReportCharRefNum ; indx1++ )
      {
        BLUENRG_memcpy(&hidDevice.hidService[indx].nonHidServInfo[indx1],&hidServiceData->nonHidServInfo[indx1],sizeof(tnonHIDService));
      }
    }
    /* boot device type */
    if (hidServiceData->bootModeDeviceType & BOOT_DEVICE_KEYBOARD_MASK)
    {
      /* boot keyboard */
      hidDevice.hidService[indx].bootDeviceMode |= BOOT_DEVICE_KEYBOARD_MASK;
      hidDevice.hidService[indx].bootKbdInput.reportValueLen = hidServiceData->bootIpReportLenMax;
      hidDevice.hidService[indx].bootKbdInput.pReportValue = pBootReportCharBuff;
      hidDevice.hidService[indx].bootKbdOutput.reportValueLen = hidServiceData->bootOpReportLenMax;
      hidDevice.hidService[indx].bootKbdOutput.pReportValue = pBootReportCharBuff + BOOT_KBD_IP_REPORT_CHAR_SIZE_MAX;
    }
    else if (hidServiceData->bootModeDeviceType & BOOT_DEVICE_MOUSE_MASK)
    {
      /* boot mouse */
      hidDevice.hidService[indx].bootDeviceMode |= BOOT_DEVICE_MOUSE_MASK;
      hidDevice.hidService[indx].bootMouseInput.reportValueLen = hidServiceData->bootIpReportLenMax;
      hidDevice.hidService[indx].bootMouseInput.pReportValue = pBootReportCharBuff;
    }
    
    pBootReportCharBuff += (BOOT_KBD_IP_REPORT_CHAR_SIZE_MAX + BOOT_KBD_OP_REPORT_CHAR_SIZE_MAX);

    /* hid information char */
    BLUENRG_memcpy(&hidDevice.hidService[indx].hidInfoChar,&hidServiceData->hidInfoChar,sizeof(hidServiceData->hidInfoChar));
    BLUENRG_memcpy(pHidInfoCharBuff,(uint8_t *)&hidDevice.hidService[indx].hidInfoChar,sizeof(hidServiceData->hidInfoChar));
    /* pointer for next hid service application data */
    pHidInfoCharBuff += sizeof(hidServiceData->hidInfoChar);
    hidServiceData++ ;
  } /* end for x numOfHIDServices */

  /* BATTERY SERVICE */
  hidDevice.numofBatteryServices = numOfBatteryServices;

  /* SCAN PARAMETERS SERVICE */
  if (scanParamServiceSupport)
  {
    hidDevice.scanParamService.serviceSupports = SCAN_PARAMETER_SERVICE_MASK;
    if (scanRefreshCharSupport)
    {
        hidDevice.scanParamService.serviceSupports |= SCAN_REFRESH_CHAR_MASK;
    }
  }

  hidDevice.applicationNotifyFunc = hidDeviceCallbackFunc;

  hidDevice.timerID = 0xFF;
  
  /* Initialize HID Profile to BLE main Profile Interface data structure */
  BLUENRG_memset ( &hidDevice.HIDtoBLEInf, 0, sizeof(tProfileInterfaceContext) );
  hidDevice.HIDtoBLEInf.profileID = PID_HID_DEVICE;
  hidDevice.HIDtoBLEInf.callback_func = HidDevice_Rx_Event_Handler;
  hidDevice.HIDtoBLEInf.voteForAdvertisableState = 0;
      
  /* PNP ID characteristic has to be added to the 
   * device information service
   */ 
  hidDevice.HIDtoBLEInf.DISCharRequired = ((deviceInfoCharSupports & 0x1FF) | PNP_ID_CHAR_MASK);
  retval = BLE_Profile_Register_Profile ( &hidDevice.HIDtoBLEInf );
  if (retval != BLE_STATUS_SUCCESS)
  {
    /* Profile could not be registered with the BLE main profile */
    HID_DEVICE_MESG_DBG( profiledbgfile, "HidDevice_Init(), HID Device Profile could not be registered Error: %02X !!\n", retval);
    return (retval);
  }

  /* Initialize profile's state machine's state variables */
  HIDProfile_Write_MainStateMachine(HID_DEVICE_STATE_WAITING_BLE_INIT);
  HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_WAITING_BLE_INIT);

  HID_DEVICE_MESG_DBG( profiledbgfile, "HidDevice_Init(), HID Device Profile Interface is Initialized \n");
  return (BLE_STATUS_SUCCESS);
}/* end HidDevice_Init() */

tBleStatus HidDevice_Make_Discoverable(uint8_t useBoundedDeviceList)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  //tHciResponsePacket response;

  HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Make_Discoverable %x %x\n",HIDProfile_Read_MainStateMachine(),HIDProfile_Read_SubStateMachine());

  if ((HIDProfile_Read_MainStateMachine() == HID_DEVICE_STATE_ADVERTIZE)  &&  
      (HIDProfile_Read_SubStateMachine() == HID_DEVICE_STATE_ADVERTISABLE_IDLE))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HID Device Set Discoverable mode: %d \n", useBoundedDeviceList );
    if ((useBoundedDeviceList) && (gBLEProfileContext.bleSecurityParam.bonding_mode))
    {
      
      HID_DEVICE_MESG_DBG ( profiledbgfile, "STARTING Advertising for White List devices \n");

      retval = aci_gap_set_undirected_connectable(WHITE_LIST_FOR_ALL, 
                                                        PUBLIC_ADDR
                                                        );
      if (retval == BLE_STATUS_SUCCESS)
      {
        /* change profile's sub states */
        HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Make_Discoverable(), STARTING Undirect connectable mode \n");
        /* start a timer of 10 secconds to try to connnect to white list device */
        Blue_NRG_Timer_Start (10, HidDevice_Advertise_Period_Timeout_Handler,&hidDevice.timerID);
        /* change profile's sub states */
        HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_ADVERTISING_FOR_WHITE_LIST);
        /* request for main profile's state change */
        BLE_Profile_Change_Advertise_Sub_State(&hidDevice.HIDtoBLEInf, 
                                               BLE_PROFILE_STATE_ADVERTISING);
      }
      else
      {
        HID_DEVICE_MESG_DBG (profiledbgfile,"HidDevice_Make_Discoverable(), FAILED to start Undirect connectable mode, Error: %02X !!\n", retval);
        return retval; 
      }
    }
    else
    {
      //HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Make_Discoverable(), STARTING Fast discoverable mode \n");
      HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_FAST_DISCOVERABLE_DO_START);
    }
  }
  else
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HID Device Set Discoverable mode: Invalid Profile States %2x %2x\n",HIDProfile_Read_MainStateMachine(),HIDProfile_Read_SubStateMachine());
  }

  return (retval);
}/* end HidDevice_Make_Discoverable() */

/**
 * HidDevice_Update_Input_Report
 * 
 * @param[in] hidServiceIndex : The index of the HID service whose report characteristic
 *            has to be updated
 * @param[in] ipReportIndex : The index of the input report to be updated
 * @param[in] ipReportValLength : Length of the input report
 * @param[in] ipReportValue : value of the input report
 * 
 * updates the report characteristic specified by the ipReportIndex
 * with the value specified in ipReportValue
 * 
 * @return BLE_STATUS_SUCCESS if the update was successfully started
 * else returns error codes
 */ 
tBleStatus HidDevice_Update_Input_Report(uint8_t hidServiceIndex, 
                                         uint8_t ipReportIndex, 
                                         uint8_t ipReportValLength,
                                         uint8_t * ipReportValue)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  
  HID_DEVICE_MESG_DBG ( profiledbgfile, "Received a new input report value \n");

  /* check for valid hid service index */
  if (!(hidServiceIndex < hidDevice.numofHIDServices))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Input_Report(), invalid hid service index\n");
    return (retval);
  }

  /* chick for valid input report index */
  if (!(ipReportIndex < hidDevice.hidService[hidServiceIndex].inputReportCharNum))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Input_Report(), invalid input report index\n");
    return (retval);
  }
  
      /* reports are of fixed lengths. So the value received should be of predefined length */
  if(ipReportValLength != hidDevice.hidService[hidServiceIndex].inputReport[ipReportIndex].reportValueLen)
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Input_Report(), invalid length\n");
    return (retval);
  }

  /* check whether the profile can send the input report */
  if ((HIDProfile_Read_MainStateMachine() == HID_DEVICE_STATE_CONNECTED) &&
     (HIDProfile_Read_SubStateMachine() == HID_DEVICE_STATE_CONNECTED_IDLE) &&
     (hidDevice.hidService[hidServiceIndex].protocolMode == REPORT_PROTOCOL_MODE))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Input_Report(), Input Report Value is ready to send \n");

    retval = aci_gatt_update_char_value(hidDevice.hidService[hidServiceIndex].hidServiceHandle, 
                                        hidDevice.hidService[hidServiceIndex].inputReport[ipReportIndex].reportCharHandle, 
                                        0,/* update from offset 0 */
                                        ipReportValLength, 
                                        ipReportValue);
    if (retval == BLE_STATUS_SUCCESS)
    {
        HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Input_Report(), Input Report value has been sent for update \n");
        hidDevice.opCode = HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL; 
        hidDevice.updater_service_handle = hidDevice.hidService[hidServiceIndex].hidServiceHandle; 
        hidDevice.updater_char_handle = hidDevice.hidService[hidServiceIndex].inputReport[ipReportIndex].reportCharHandle;
    }
    else
    {
        HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Input_Report(), FAILED to send Input Report value %02X \n", retval);
    }
  }
  else
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Input_Report(), HID Device Profile is not in proper state to send I/P report value %2x, %2x, %2x\n",HIDProfile_Read_MainStateMachine(),HIDProfile_Read_SubStateMachine(), hidDevice.hidService[hidServiceIndex].protocolMode);
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
}/* end HidDevice_Update_Input_Report() */

/**
 * HidDevice_Update_Feature_Report
 * 
 * @param[in] hidServiceIndex : The index of the HID service whose report characteristic
 *            has to be updated
 * @param[in] ftrReportIndex : The index of the feature report to be updated
 * @param[in] ftrReportValLength : Length of the feature report
 * @param[in] ftrReportValue : value of the feature report
 * 
 * updates the report characteristic specified by the ipReportIndex
 * with the value specified in ftrReportValue
 * 
 * @return BLE_STATUS_SUCCESS if the update was successfully started
 * else returns error codes
 */ 
tBleStatus HidDevice_Update_Feature_Report(uint8_t hidServiceIndex, 
                                           uint8_t ftrReportIndex, 
                                           uint8_t ftrReportValLength,
                                           uint8_t *ftrReportValue)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Feature_Report(), Received a new Feature report value \n");

  /* check for valid hid service index */
  if (!(hidServiceIndex < hidDevice.numofHIDServices))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Feature_Report(),invalid hid service index\n");
    return (retval);
  }

  /* chick for valid feature report index */
  if (!(ftrReportIndex < hidDevice.hidService[hidServiceIndex].featureReportCharNum))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Feature_Report(),invalid feature report index\n");
    return (retval);
  }

  if(ftrReportValLength != hidDevice.hidService[hidServiceIndex].featureReport[ftrReportIndex].reportValueLen)
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Feature_Report(),invalid feature report length\n");
    return (retval);
  }

  /* check if the profile can send the feature report */
  if((HIDProfile_Read_MainStateMachine() == HID_DEVICE_STATE_CONNECTED) &&
     (HIDProfile_Read_SubStateMachine() == HID_DEVICE_STATE_CONNECTED_IDLE) &&
     (hidDevice.hidService[hidServiceIndex].protocolMode == REPORT_PROTOCOL_MODE))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "Feature Report Value is ready to send \n");

    retval = aci_gatt_update_char_value(hidDevice.hidService[hidServiceIndex].hidServiceHandle, 
                                        hidDevice.hidService[hidServiceIndex].featureReport[ftrReportIndex].reportCharHandle, 
                                        0,
                                        ftrReportValLength, 
                                        ftrReportValue);
    if (retval == BLE_STATUS_SUCCESS)
    {
      HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Feature_Report(), Feature Report value has been sent for update \n");
      hidDevice.opCode = HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL; 
      hidDevice.updater_service_handle = hidDevice.hidService[hidServiceIndex].hidServiceHandle; 
      hidDevice.updater_char_handle = hidDevice.hidService[hidServiceIndex].featureReport[ftrReportIndex].reportCharHandle;
      
    }
    else
    {
      HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Feature_Report(), FAILED to send Feature Report value %02X \n", retval);
    }
  }
  else
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Feature_Report(), HID Device Profile is not in proper state to send an feature report value \n");
    retval = BLE_STATUS_FAILED;
  }
      
  return (retval);
}/* end HidDevice_Update_Feature_Report() */

/**
 * HidDevice_Update_Boot_Keyboard_Input_Report
 * 
 * @param[in] hidServiceIndex : The index of the HID service whose boot report characteristic
 *            has to be updated
 * @param[in] bootKbdIpReportValLength : Length of the boot keyboard input report
 * @param[in] bootKbdIpReportValue : value of the boot keyboard input report
 * 
 * updates the boot keyboard input report characteristic 
 * with the value specified in bootKbdIpReportValue
 * 
 * @return BLE_STATUS_SUCCESS if the update was successfully started
 * else returns error codes
 */ 
tBleStatus HidDevice_Update_Boot_Keyboard_Input_Report(uint8_t hidServiceIndex, 
                                                       uint8_t bootKbdIpReportValLength,
                                                       uint8_t *bootKbdIpReportValue)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Keyboard_Input_Report(), Received a Boot Keyboard Input report value %x %x\n",HIDProfile_Read_MainStateMachine(),HIDProfile_Read_SubStateMachine());

  /* check for valid hid service index */
  if (!(hidServiceIndex < hidDevice.numofHIDServices))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Keyboard_Input_Report(),invalid hid service index\n");
    return (retval);
  }
  
  /* check if the device is in the boot protocol mode -- keyboard */
  if((hidDevice.hidService[hidServiceIndex].protocolMode != BOOT_PROTOCOL_MODE) ||
     (!(hidDevice.hidService[hidServiceIndex].bootDeviceMode & BOOT_DEVICE_KEYBOARD_MASK)))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Keyboard_Input_Report(),invalid protocol mode, keyboard mask\n");
    return BLE_STATUS_FAILED;
  }
  HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Keyboard_Input_Report(), bootKbdIpReportValLength %x\n",bootKbdIpReportValLength);
  if (bootKbdIpReportValLength != hidDevice.hidService[hidServiceIndex].bootKbdInput.reportValueLen)
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Keyboard_Input_Report(),invalid report value len\n");
    return (retval);
  }

  /* check if the profile is ready to send the report */
  if((HIDProfile_Read_MainStateMachine() == HID_DEVICE_STATE_CONNECTED) &&
     (HIDProfile_Read_SubStateMachine() == HID_DEVICE_STATE_CONNECTED_IDLE))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Keyboard_Input_Report(), Boot Keyboard Input Report Value is ready to send \n");

    retval = aci_gatt_update_char_value(hidDevice.hidService[hidServiceIndex].hidServiceHandle, 
                                        hidDevice.hidService[hidServiceIndex].bootKbdInput.reportCharHandle, 
                                        0,
                                        bootKbdIpReportValLength, 
                                        bootKbdIpReportValue);
    if (retval == BLE_STATUS_SUCCESS)
    {
      HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Keyboard_Input_Report(), Boot Keyboard Input Report value has been sent for update \n");
      hidDevice.opCode = HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL; 
      hidDevice.updater_service_handle = hidDevice.hidService[hidServiceIndex].hidServiceHandle; 
      hidDevice.updater_char_handle = hidDevice.hidService[hidServiceIndex].bootKbdInput.reportCharHandle;
    }
    else
    {
      HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Keyboard_Input_Report(), HidDevice_Update_Boot_Keyboard_Input_Report(), FAILED to send Boot Keyboard Input Report value %02X \n", retval);
    }
  }
  else
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Keyboard_Input_Report(), HID Device Profile is not in proper state to send boot keyboard I/P report value \n");
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
}/* end HidDevice_Update_Boot_Keyboard_Input_Report() */

/**
 * HidDevice_Update_Boot_Mouse_Input_Report
 * 
 * @param[in] hidServiceIndex : The index of the HID service whose boot report characteristic
 *            has to be updated
 * @param[in] bootMouseIpReportValLength : Length of the boot mouse input report
 * @param[in] bootMouseIpReportValue : value of the boot mouse input report
 * 
 * updates the boot mouse input report characteristic 
 * with the value specified in bootMouseIpReportValue
 * 
 * @return BLE_STATUS_SUCCESS if the update was successfully started
 * else returns error codes
 */ 
tBleStatus HidDevice_Update_Boot_Mouse_Input_Report(uint8_t hidServiceIndex, 
                                                    uint8_t bootMouseIpReportValLength,
                                                    uint8_t * bootMouseIpReportValue)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Mouse_Input_Report(), Received a Boot Mouse Input report value \n");

  /* check for valid hid service index */
  if (hidServiceIndex >= hidDevice.numofHIDServices)
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Mouse_Input_Report(), invalid hid service index\n");
    return (retval);
  }
      
  /* check if the device is in the boot protocol mode -- mouse */
  if((hidDevice.hidService[hidServiceIndex].protocolMode != BOOT_PROTOCOL_MODE) ||
    (!(hidDevice.hidService[hidServiceIndex].bootDeviceMode & BOOT_DEVICE_MOUSE_MASK)))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Mouse_Input_Report(), Invalid boot protocol mode, mouse \n");
    return BLE_STATUS_FAILED;
  }
      
  if (bootMouseIpReportValLength != hidDevice.hidService[hidServiceIndex].bootMouseInput.reportValueLen)
  {
    return (retval);
  }

  /* check if the profile is ready to send the report */
  if((HIDProfile_Read_MainStateMachine() == HID_DEVICE_STATE_CONNECTED) &&
     (HIDProfile_Read_SubStateMachine() == HID_DEVICE_STATE_CONNECTED_IDLE))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Mouse_Input_Report(), Boot Mouse Input Report Value is ready to send \n");

    retval = aci_gatt_update_char_value(hidDevice.hidService[hidServiceIndex].hidServiceHandle, 
                                        hidDevice.hidService[hidServiceIndex].bootMouseInput.reportCharHandle, 
                                        0,
                                        bootMouseIpReportValLength, 
                                        bootMouseIpReportValue);
    if (retval == BLE_STATUS_SUCCESS)
    {
      HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Mouse_Input_Report(), Boot Mouse Input Report value has been sent for update %x \n",hidDevice.hidService[hidServiceIndex].bootMouseInput.reportCharHandle);
      hidDevice.opCode = HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL; 
      hidDevice.updater_service_handle = hidDevice.hidService[hidServiceIndex].hidServiceHandle; 
      hidDevice.updater_char_handle = hidDevice.hidService[hidServiceIndex].bootMouseInput.reportCharHandle;
    }
    else
    {
      HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Mouse_Input_Report(), FAILED to send Boot Mouse Input Report value %02X \n", retval);
    }
  }
  else
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Boot_Mouse_Input_Report(), HID Device Profile is not in proper state to send boot Mouse I/P report value \n");
    retval = BLE_STATUS_FAILED;
  }
      
  return (retval);
}/* end HidDevice_Update_Boot_Mouse_Input_Report() */

/**
 * HidDevice_Update_Battery_Level
 * 
 * @param[in] batteryServiceIndex : the battery service whose  characteristic
 *            has to be updated
 * @param[in] batteryLevel : value of the battery level characteristic
 * 
 * starts the update for the battery level characteristic
 * 
 * @return BLE_STATUS_SUCCESS if the update was successfully started
 */ 
tBleStatus HidDevice_Update_Battery_Level(uint8_t batteryServiceIndex, 
                                          uint8_t batteryLevel)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Battery_Level(), Received Battery Level value to update \n");

  /* check for valid hid service index */
  if (!(batteryServiceIndex < hidDevice.numofBatteryServices))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Battery_Level(), Invalid hid battery service index \n");
    return (retval);
  }

  if((HIDProfile_Read_MainStateMachine() > HID_DEVICE_STATE_INITIALIZED) && 
     (HIDProfile_Read_SubStateMachine() != HID_DEVICE_STATE_CONNECTED_BUSY))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Battery_Level(), Requesting to Update battery level characteritsic Value \n");

    retval = aci_gatt_update_char_value(hidDevice.batteryService[batteryServiceIndex].batteryServHandle, 
                                        hidDevice.batteryService[batteryServiceIndex].batteryLevlCharHandle, 
                                        0,
                                        1, 
                                        &batteryLevel);
    if (retval == BLE_STATUS_SUCCESS)
    {
      PROFILE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Battery_Level(), Battery Level Char value has been send for update \n");
      hidDevice.opCode = HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL; 
      hidDevice.updater_service_handle = hidDevice.batteryService[batteryServiceIndex].batteryServHandle; 
      hidDevice.updater_char_handle = hidDevice.batteryService[batteryServiceIndex].batteryLevlCharHandle;
    }
    else
    {
      HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Battery_Level(),FAILED to Update Battery Level Char Value %02X \n", retval);
    }
  }
  else
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Battery_Level(),HID Device Profile is not in proper state to set Battery level value \n");
    retval = BLE_STATUS_FAILED;
  }

  return (retval);
}/* end HidDevice_Update_Battery_Level() */

/**
 * HidDevice_Update_Scan_Refresh_Char
 * 
 * @param[in] scanRefresh : value of the scan refresh characteristic
 * 
 * starts the update for the scan refresh characteristic
 * 
 * @return BLE_STATUS_SUCCESS if the update was successfully started
 */ 
tBleStatus HidDevice_Update_Scan_Refresh_Char(uint8_t scanRefresh)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  if(!(hidDevice.scanParamService.serviceSupports & SCAN_REFRESH_CHAR_MASK))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Scan_Refresh_Char(), Invalid scan param serviceSupports \n");
    return (BLE_STATUS_FAILED);
  }

  HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Scan_Refresh_Char(), Received Scan Refresh value to update char \n");

  if ((HIDProfile_Read_MainStateMachine() == HID_DEVICE_STATE_CONNECTED) && 
      (HIDProfile_Read_SubStateMachine() == HID_DEVICE_STATE_CONNECTED_IDLE))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Scan_Refresh_Char(), Requesting to Update Scan Refresh Char's Attribute Value \n");

    retval = aci_gatt_update_char_value(hidDevice.scanParamService.scanParamServHandle, 
                                        hidDevice.scanParamService.scanRefreshCharHandle, 
                                        0,
                                        1, 
                                        &scanRefresh);
    if (retval == BLE_STATUS_SUCCESS)
    {
      PROFILE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Scan_Refresh_Char(), Scan Refresh Char value has been send for update \n");
      hidDevice.opCode = HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL; 
      hidDevice.updater_service_handle = hidDevice.scanParamService.scanParamServHandle;
      hidDevice.updater_char_handle = hidDevice.scanParamService.scanRefreshCharHandle;
    }
    else
    {
      HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Scan_Refresh_Char(), FAILED to Update Scan Refresh Char Value %02X \n", retval);
    }
  }
  else
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Update_Scan_Refresh_Char(),HID Device Profile is not in proper state to set Scan Refresh Char value %02X, %02X\n", HIDProfile_Read_MainStateMachine(),HIDProfile_Read_SubStateMachine() == HID_DEVICE_STATE_CONNECTED_IDLE);
    retval = BLE_STATUS_FAILED;
  }

  return (retval);
}/* end HidDevice_Update_Scan_Refresh_Char() */

/**
 * Allow_BatteryLevel_Char_Read
 * 
 * @param[in] batteryServiceIndex : The battery servcie index
 * for whose characteristic the read has to be allowed
 * 
 * sends the allow read command to the controller
 */ 
tBleStatus Allow_BatteryLevel_Char_Read(uint8_t batteryServiceIndex)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  HID_DEVICE_MESG_DBG ( profiledbgfile, "Allow_BatteryLevel_Char_Read(), Allow_Read %d\n",batteryServiceIndex);

  /* check for valid battery service index */
  if (!(batteryServiceIndex < hidDevice.numofBatteryServices))
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "Allow_BatteryLevel_Char_Read(), Invalid battery service index \n");
    return (BLE_STATUS_INVALID_PARAMS);
  }
      
  if(hidDevice.batteryService[batteryServiceIndex].battLvlAllowRead)
  {
     HID_DEVICE_MESG_DBG ( profiledbgfile, "Allow_BatteryLevel_Char_Read(), Allow_Read\n");
    /* send the allow read command to the controller */
    retval = aci_gatt_allow_read(hidDevice.connHandle);
    
    if (retval == BLE_STATUS_SUCCESS)
    {
       /* clear the flag */
      hidDevice.batteryService[batteryServiceIndex].battLvlAllowRead = 0x00;
      PROFILE_MESG_DBG ( profiledbgfile, "Allow_BatteryLevel_Char_Read(), Allow read has been send\n");
    }
    else
    {
      HID_DEVICE_MESG_DBG ( profiledbgfile, "Allow_BatteryLevel_Char_Read(), FAILED to perform allow read  %02X \n", retval);
    }
  }
  
  return retval;
}/* end Allow_BatteryLevel_Char_Read() */

/******************************************************************************
* Local Functions
******************************************************************************/
void HIDProfile_Write_MainStateMachine(tProfileState localmainState)
{
  hidDevice.mainState = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

tProfileState HIDProfile_Read_MainStateMachine(void)
{
  return(hidDevice.mainState);
}

void HIDProfile_Write_SubStateMachine(tProfileState localsubState)
{
  hidDevice.subState = localsubState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

/*
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
static void HIDProfile_Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  uint16_t attrHandle = 0;
  uint8_t attValue;
  uint8_t indx1 = 0;
  uint8_t indx2 = 0;
  uint8_t doCurrAttrWritten = 0;
  
  attrHandle = handle;
  attValue = att_data[0]; 
  
  HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Rx_Event_Handler(), EVT_BLUE_GATT_ATTRIBUTE_MODIFIED %x %x\n",attrHandle,attValue); 
  
  /* check for all hid services */
  for ( indx1 = 0 ; indx1 < hidDevice.numofHIDServices ; indx1++ )
  {
    HID_DEVICE_MESG_DBG ( profiledbgfile, "handle %x\n",hidDevice.hidService[indx1].protocolModeCharHandle);
    /* check for protocol mode char */
    if (attrHandle == (hidDevice.hidService[indx1].protocolModeCharHandle + 1))
    {
      /* current protocol mode char attribute is written by the client */
      HID_DEVICE_MESG_DBG ( profiledbgfile, "current protocol mode char attribute is written by the client\n");
      doCurrAttrWritten = 1;
      /* update the protocol mode here */
      hidDevice.hidService[indx1].protocolMode = attValue; 
    }
    if (doCurrAttrWritten)
    {
      return;
    }
    /* check for input report char */
    for ( indx2 = 0 ; indx2 < hidDevice.hidService[indx1].inputReportCharNum ; indx2++ )
    {
      if (attrHandle == (hidDevice.hidService[indx1].inputReport[indx2].reportCharHandle + 1))
      {
        /* current input report char attribute is written by the client */
        HID_DEVICE_MESG_DBG ( profiledbgfile, "current input report char attribute is written by the client\n");
        doCurrAttrWritten = 1;
        return;
      }
    }
    if (doCurrAttrWritten)
    {
      return;
    }
    /* check for output report char */
    for ( indx2 = 0 ; indx2 < hidDevice.hidService[indx1].outputreportCharNum ; indx2++ )
    {
      if (attrHandle == (hidDevice.hidService[indx1].outputReport[indx2].reportCharHandle + 1))
      {
        /* current output report char attribute is written by the client */
        HID_DEVICE_MESG_DBG ( profiledbgfile, "current output report char attribute is written by the client\n");
        doCurrAttrWritten = 1;
        return;
      }
    }
    if (doCurrAttrWritten)
    {
      return;
    }
    /* check for feature report char */
    for ( indx2 = 0 ; indx2 < hidDevice.hidService[indx1].featureReportCharNum ; indx2++ )
    {
      if (attrHandle == (hidDevice.hidService[indx1].featureReport[indx2].reportCharHandle + 1))
      {
        /* current feature report char attribute is written by the client */
        HID_DEVICE_MESG_DBG ( profiledbgfile, "current feature report char attribute is written by the client\n");
        doCurrAttrWritten = 1;
        return;
      }
    }
    if (doCurrAttrWritten)
    {
      return;
    }
    /* check for boot keyboard input report char */
    if (attrHandle == (hidDevice.hidService[indx1].bootKbdInput.reportCharHandle + 1))
    {
      /* current protocol mode char attribute is written by the client */
      HID_DEVICE_MESG_DBG ( profiledbgfile, "current boot keyboard input report char attribute is written by the client\n");
      doCurrAttrWritten = 1;
    }
    if (doCurrAttrWritten)
    {
      return;
    }
    /* check for boot keyboard output report char */
    if (attrHandle == (hidDevice.hidService[indx1].bootKbdOutput.reportCharHandle + 1))
    {
      /* current protocol mode char attribute is written by the client */
      HID_DEVICE_MESG_DBG ( profiledbgfile, "current boot keyboard output report char attribute is written by the client\n");
      doCurrAttrWritten = 1;
    }
    if (doCurrAttrWritten)
    {
      return;
    }
    /* check for boot mouse input report char */
    if (attrHandle == (hidDevice.hidService[indx1].bootMouseInput.reportCharHandle + 1))
    {
      /* current protocol mode char attribute is written by the client */
      HID_DEVICE_MESG_DBG ( profiledbgfile, "current boot mouse input report char attribute is written by the client\n");
      doCurrAttrWritten = 1;
    }
    if (doCurrAttrWritten)
    {
      return;
    }
    /* check for hid control point char */
    if (attrHandle == (hidDevice.hidService[indx1].hidCtlPointCharHandle + 1))
    {
      /* current protocol mode char attribute is written by the client */
      HID_DEVICE_MESG_DBG ( profiledbgfile, "currenthid control point char attribute is written by the client\n");
      doCurrAttrWritten = 1;
    }
    if (doCurrAttrWritten)
    {
      return;
    }
  }/* for ( indx1 = 0 ; indx1 < hidDevice.numofHIDServices ; indx1++ ) */
  if (doCurrAttrWritten == 0)
  {
    if (attrHandle == (hidDevice.scanParamService.scanIntervalWinCharHandle + 1))
    {
      /* scan interval window char attribute is wirtten by the client */
    }
  }  
}

tProfileState HIDProfile_Read_SubStateMachine(void)
{
  return(hidDevice.subState);
}

/**
* HidDevice_Rx_Event_Handler
* 
* @param[in] pckt : Pointer to the ACI packet
* 
* It parses the events received from ACI according to the profile's state machine.
* Inside this function each event must be identified and correctly parsed.
* NOTE: It is the event handler to be called inside HCI_Event_CB() 
*/ 
void HidDevice_Rx_Event_Handler(void *pckt)
{
  uint16_t attrHandle = 0;
  //uint8_t attValue;
  uint8_t indx1 = 0;
  //uint8_t indx2 = 0;
  //uint8_t doCurrAttrWritten = 0;
  
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  HRPROFILE_MESG_DBG(profiledbgfile, "HidDevice_Rx_Event_Handler(), HID Device States: %02X %02X \n", 
                     HIDProfile_Read_MainStateMachine(), HIDProfile_Read_SubStateMachine() );
  
  if(hci_pckt->type != HCI_EVENT_PKT)
  {
    HRPROFILE_MESG_ERR (profiledbgfile,"HidDevice_Rx_Event_Handler(), Recevied packed is not an HCI Event: %02X !!\n", hci_pckt->type);
    return;
  }
  
  switch(event_pckt->evt)
  {
    case EVT_DISCONN_COMPLETE:
    {
      if (HIDProfile_Read_MainStateMachine() >= HID_DEVICE_STATE_INITIALIZED)
      {
        evt_disconn_complete *cc = (void *)event_pckt->data;
        if (cc->status == BLE_STATUS_SUCCESS)
        {
            /* Change Profile's State */
            HIDProfile_Write_MainStateMachine(HID_DEVICE_STATE_ADVERTIZE);
            HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_ADVERTISABLE_IDLE);
        }
      }
      HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Rx_Event_Handler(), Received event EVT_DISCONN_COMPLETE %x %x\n",HIDProfile_Read_MainStateMachine(),HIDProfile_Read_SubStateMachine());
    }
    break;
    
    case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
     
      switch(evt->subevent)
      {
        case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
          
          HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Rx_Event_Handler(), Received event EVT_LE_CONN_COMPLETE %2x\n", cc->status);
          if (cc->status == BLE_STATUS_SUCCESS)
          {
            /* store connection handle */
            hidDevice.connHandle = cc->handle;
          }
#if (ENABLE_SECURITY == 0) /* security is mandatory for HID profile. This macro is defined only to
                            * enable testing without security
                                                */
          HIDProfile_Write_MainStateMachine(HID_DEVICE_STATE_CONNECTED);
          HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_CONNECTED_IDLE);
#else
          /* check event status */
          if (cc->status == BLE_STATUS_SUCCESS)
          {
            /* connection req is accepted and pairing process started */
            HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_PROCESSING_PAIRING);
          }
          else
          {
            /* connection was not successful, now we need to wait for
             * the application to put the device in discoverable mode 
             */
            HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_ADVERTISABLE_IDLE);
          }
#endif /* (ENABLE_SECURITY == 0) */
          /* Here there is no need to do anything extra if this profile is not 
           * controlling the advertising process. Above if-else statement does
           * the needful. */

          /* Stop if any timer is running */
          Blue_NRG_Timer_Stop (hidDevice.timerID);
          hidDevice.timerID = 0xFF;
        }
        break; /* EVT_LE_CONN_COMPLETE */
      } /* switch(response->leEvent.subEventCode) */
    }
    break; /* EVT_LE_META_EVENT */
    
    case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      
      switch(blue_evt->ecode)
      {
        case EVT_BLUE_GAP_LIMITED_DISCOVERABLE:
        {
          HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Rx_Event_Handler(), EVT_BLUE_GAP_SET_LIMITED_DISCOVERABLE \n");

          if ((HIDProfile_Read_MainStateMachine() == HID_DEVICE_STATE_ADVERTIZE) && 
              ((HIDProfile_Read_SubStateMachine() == HID_DEVICE_STATE_FAST_DISCOVERABLE) ||
               (HIDProfile_Read_SubStateMachine() == HID_DEVICE_STATE_LOW_POWER_DISCOVERABLE)))
          {
            /* Put the main profile in connectable idle state */
            BLE_Profile_Change_Advertise_Sub_State(&hidDevice.HIDtoBLEInf, 
                                                   BLE_PROFILE_STATE_CONNECTABLE_IDLE);
                                                                                             
            if(HIDProfile_Read_SubStateMachine() == HID_DEVICE_STATE_LOW_POWER_DISCOVERABLE)
            {
              /* Limited Discoverable mode has timed out */
              HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_ADVERTISABLE_IDLE);
              
              /* notify the application */
              hidDevice.applicationNotifyFunc(EVT_MP_ADVERTISING_TIMEOUT,0x00,NULL);
            }
          }
        }
        break;/* EVT_BLUE_GAP_LIMITED_DISCOVERABLE */
        case EVT_BLUE_GAP_PAIRING_CMPLT:
        {
          evt_gap_pairing_cmplt *pairing = (void*)blue_evt->data;
	 
          HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Rx_Event_Handler(), EVT_BLUE_GAP_PAIRING_CMPLT %2x\n", pairing->status);
         
          if ((HIDProfile_Read_MainStateMachine() == HID_DEVICE_STATE_ADVERTIZE) && 
              (HIDProfile_Read_SubStateMachine() == HID_DEVICE_STATE_PROCESSING_PAIRING))
          {
            if (pairing->status == BLE_STATUS_SUCCESS) 
            {
              HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Rx_Event_Handler(), EVT_BLUE_GAP_PAIRING_CMPLT \n");
              HIDProfile_Write_MainStateMachine(HID_DEVICE_STATE_CONNECTED);
              HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_CONNECTED_IDLE);
            }
          }
        }
        break; /* EVT_BLUE_GAP_PAIRING_CMPLT */
        case EVT_BLUE_GATT_READ_PERMIT_REQ:
        {
          evt_gatt_read_permit_req * read_permit_req = (void*)blue_evt->data;
          attrHandle = read_permit_req->attr_handle;
          
          HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Rx_Event_Handler(), EVT_BLUE_GATT_READ_PERMIT_REQ, handle: %2x\n", attrHandle);
          for(indx1=0;indx1<hidDevice.numofBatteryServices;indx1++)
          {
            if(attrHandle == (hidDevice.batteryService[indx1].batteryLevlCharHandle + 1))
            {
              /* send the allow read command */			
              hidDevice.batteryService[indx1].battLvlAllowRead = 1;
              /* let the application know that the battery level is
               * being requested for read. the event data contains the
               * index of the battery service whose characteristic is
               * being requested to be read
               */
              HID_DEVICE_MESG_DBG ( profiledbgfile, "HidDevice_Rx_Event_Handler(), EVT_BLUE_GATT_READ_PERMIT_REQ, notify application\n");
              hidDevice.applicationNotifyFunc(EVT_BATT_LEVEL_READ_REQ,2,&indx1);
            }
          }
        }
        break;
        case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
        {
          /* Check for attribute handle */
          if (bnrg_expansion_board == IDB05A1) {
            evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
            HIDProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
          else {
            evt_gatt_attr_modified_IDB04A1 *evt = (evt_gatt_attr_modified_IDB04A1*)blue_evt->data;
            HIDProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }      
        }
        break; /* HCI_EVT_BLUE_GATT_ATTRIBUTE_MODIFIED */
      }/* switch(blue_evt->ecode) */
    }/* EVT_VENDOR */
    break; /* HCI_EVT_VENDOR_SPECIFIC */
  }
}/* end HidDevice_Rx_Event_Handler() */


/**
 * HidDevice_Advertise_Period_Timeout_Handler
 * 
 * this function is called when the timer started
 * by the HID profile for advertising times out
 */ 
void HidDevice_Advertise_Period_Timeout_Handler (void)
{
  tProfileState nextState = HIDProfile_Read_SubStateMachine();
  
  Blue_NRG_Timer_Stop(hidDevice.timerID);
  hidDevice.timerID = 0xFF; 
  

  HID_DEVICE_MESG_DBG ( profiledbgfile, "Advertising Period Timedout \n");
  if (HIDProfile_Read_MainStateMachine() == HID_DEVICE_STATE_ADVERTIZE)
  {
    switch (HIDProfile_Read_SubStateMachine())
    {
      case HID_DEVICE_STATE_ADVERTISING_FOR_WHITE_LIST:
        nextState = HID_DEVICE_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP;
        HID_DEVICE_MESG_DBG ( profiledbgfile, "White list Advertising Timedout \n");
      break;
      case HID_DEVICE_STATE_FAST_DISCOVERABLE:
        nextState = HID_DEVICE_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP;
        HID_DEVICE_MESG_DBG ( profiledbgfile, "Fast Advertising Timedout \n");
      break;
      case HID_DEVICE_STATE_LOW_POWER_DISCOVERABLE:
        nextState = HID_DEVICE_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP;
        HID_DEVICE_MESG_DBG ( profiledbgfile, "Low Power Advertising Timedout \n");
      break;
    }

    if (nextState != HIDProfile_Read_SubStateMachine())
    {
      HIDProfile_Write_SubStateMachine(nextState);

      hidDevice.opCode = HCI_CMD_NO_OPERATION;//TBR Is it needed?
      /* Call profile's State Machine engine for state change be effective. */
      // [CL]: Disable state machine call to avoid possible race condition
      //       since the function is called also in the main loop (see main.c -- profileApplContext.profileStateMachineFunc();)
      //HIDProfile_StateMachine();
    }
  }
}/* end HidDevice_Advertise_Period_Timeout_Handler() */
