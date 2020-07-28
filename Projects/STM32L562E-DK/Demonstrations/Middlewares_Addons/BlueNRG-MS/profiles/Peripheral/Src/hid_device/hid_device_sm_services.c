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
*   FILENAME        -  hid_device_sm_services.c
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
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* 
* HID Profile State Machine function & Services, Characteristics definition
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
// Un-comment the following macro if you want to use the Android/iOS User App
//#define USER_APP
#ifdef USER_APP
# define ATTR_PERMISSION ATTR_PERMISSION_NONE
#else
# define ATTR_PERMISSION (ATTR_PERMISSION_ENCRY_READ|ATTR_PERMISSION_ENCRY_WRITE)
#endif

/* advertising intervals in terms of 625 micro seconds */
#define HIDP_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define HIDP_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define HIDP_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define HIDP_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )

/******************************************************************************
* Variable Declarations
******************************************************************************/
extern tHidDeviceContext hidDevice;
extern uint8_t hidInfoCharValueBuff[HID_INFO_CHAR_VAL_BUFF_SIZE_MAX];

static uint16_t characDescHandle;
/******************************************************************************
* Imported Variable
******************************************************************************/
extern void *profiledbgfile;

/******************************************************************************
* Function Prototypes
******************************************************************************/
extern void HidDevice_Advertise_Period_Timeout_Handler(void);

/******************************************************************************
 * Functions 
******************************************************************************/
/* HID_Sensor_Set_Discoverable() */
tBleStatus HID_Sensor_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'H','I','D'};
  
  retval = aci_gap_set_limited_discoverable(ADV_IND,
                                            AdvIntervMin, 
                                            AdvIntervMax,
                                            PUBLIC_ADDR, 
                                            NO_WHITE_LIST_USE, /* no white list */
                                            4, 
                                            local_name, 
                                            gBLEProfileContext.advtServUUIDlen, 
                                            gBLEProfileContext.advtServUUID,    
                                            0, 
                                            0);
  return retval;
}/* end HID_Sensor_Set_Discoverable() */


/**
 * HIDProfile_Add_Services_Characteristics
 * 
 * @param[in]: None
 *            
 * It adds HID service & related characteristics
*/ 
tBleStatus HIDProfile_Add_Services_Characteristics(void)
{
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;
  uint8_t serviceMaxAttributeRecord = 0;
  uint8_t tempIndx = 0;
  uint8_t tempIndx1 = 0;
  uint8_t tempBuf[12];
  uint16_t UUID = 0;
           
  /* Add HID Services */
  HID_DEVICE_MESG_DBG( profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Add HID Services \n");
  
  for (hidDevice.currHidServIndx = 0; hidDevice.currHidServIndx < hidDevice.numofHIDServices; hidDevice.currHidServIndx++) 
  {
    /* serviceMaxAttributeRecord = 1 for hid service itself +
     *                             2 for protocol mode characteristic +
     *                             4 for each input report characteristic( 2 for charac,1 for report ref desc, 1 for desc) +
     *                             3 for each output report characteristic +
     *                             3 for each feature report characteristic +
     *                             2 for report map characteristic +
     *                             1 for each external report reference descriptor +
     *                             3 for boot keyboard i/p report char, if supported +
     *                             2 for boot keyboard o/p report char, if supported +
     *                             3 for boot mouse i/p report char, if supported +
     *                             2 for hid information characteristic +
     *                             2 for hid control point characteristic
     * The maximum number of records required is dependent on the number of reports
     * the HID device has to expose. The number of HID services is also not limited
     * but multiple HID services should be used when the report descriptor is longer
     * than 512 octets as maybe the case in case of composite devices
     */
    
    tempIndx = hidDevice.currHidServIndx;
    
    
    serviceMaxAttributeRecord = ( 1                                                     /* for hid service */
                                  + 2                                                     /* for protocol mode char */
                                  + (4 * hidDevice.hidService[tempIndx].inputReportCharNum)      /* for each input report characteristic */
                                  + (3 * hidDevice.hidService[tempIndx].outputreportCharNum)     /* for each output report characteristic */
                                  + (3 * hidDevice.hidService[tempIndx].featureReportCharNum)    /* for each feature report characteristic */
                                  + 2                                                     /* for report map characteristic */
                                  /* if there are external services, then they have to be included as
                                  * include service in the HID service and a descriptor has to be added
                                  * to the report map characteristic
                                  */
                                  + (2 * hidDevice.hidService[tempIndx].extReportRefCharDescNum)
                                  /* for hid information characteristic */
                                  + 2
                                  /* for hid control point characteristic */
                                  + 2 );
    /* boot mode reports */
    if(hidDevice.hidService[tempIndx].bootDeviceMode & BOOT_DEVICE_KEYBOARD_MASK)
    {
      serviceMaxAttributeRecord += 5;
    }
    else if(hidDevice.hidService[tempIndx].bootDeviceMode & BOOT_DEVICE_MOUSE_MASK)
    {
      serviceMaxAttributeRecord += 3;
    }
    HID_DEVICE_MESG_DBG( profiledbgfile, "HID Service (%d) Max Attribute Record %d \n", tempIndx, serviceMaxAttributeRecord);
    
    /* store the HID Service UUID */
    UUID = HUMAN_INTERFACE_DEVICE_SERVICE_UUID;
    
    hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                     (const uint8_t *) &UUID, 
                                     PRIMARY_SERVICE, 
                                     serviceMaxAttributeRecord,
                                     &(hidDevice.hidService[tempIndx].hidServiceHandle));

    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), HID Service (%d) is added Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].hidServiceHandle);
    }
    else 
    {
      HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add HID Service (%d), Error: %02X !!\n",tempIndx, hciCmdResult);
      return hciCmdResult;
    }
    
  }/****************end for (hidDevice.currHidServIndx = 0): Add HID Services */  
    
 
  /* Requested num of HID services have been added:  //TBR After HID service add non hid + included services
   * now we need to add the non hid services and then the included services (for eeach hid service?) 
   */
  
  for (hidDevice.currHidServIndx = 0; hidDevice.currHidServIndx < hidDevice.numofHIDServices; hidDevice.currHidServIndx++) 
  {
  
    tempIndx = hidDevice.currHidServIndx;
    
    /* add non HID services */
    
    if (hidDevice.hidService[tempIndx].extReportRefCharDescNum == 0)
    {
      HID_DEVICE_MESG_DBG(profiledbgfile, "No non hid Services, hidservice(%d) %x \n",tempIndx,hidDevice.currHidServIndx);
    }
    else
    {
      for (hidDevice.hidService[tempIndx].currExtReportRefIndx = 0; hidDevice.hidService[tempIndx].currExtReportRefIndx < hidDevice.hidService[tempIndx].extReportRefCharDescNum; hidDevice.hidService[tempIndx].currExtReportRefIndx++)
      {
        tempIndx1 = hidDevice.hidService[tempIndx].currExtReportRefIndx;
        
        HID_DEVICE_MESG_DBG(profiledbgfile, "Adding non hid Services (%d,%d), UUID %x \n",tempIndx, tempIndx1,hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].servUUID);
        
        hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                         (const uint8_t *) &hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].servUUID, 
                                         PRIMARY_SERVICE, 
                                         3,
                                         &(hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].servhandle));

        if (hciCmdResult == BLE_STATUS_SUCCESS)
        {
          HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Non HID Service (%d, %d) is added Successfully %04X\n",tempIndx, tempIndx1, hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].servhandle);
        }
        else 
        {
          HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Non HID Service (%d, %d), Error: %02X !!\n",tempIndx,tempIndx1, hciCmdResult);
          return hciCmdResult;
        }
        
        /* add characteristic for non HID services */
        HID_DEVICE_MESG_DBG( profiledbgfile, "Adding char for non hid Service (%d, %d)\n",tempIndx, tempIndx1);
        
        hciCmdResult = aci_gatt_add_char(hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].servhandle, 
                                         UUID_TYPE_16, 
                                         (const uint8_t *) &hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].charUUID , 
                                         hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].charValLen, /* size of the characteristic */ 
                                         CHAR_PROP_READ, 
                                         ATTR_PERMISSION,
                                         GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                         10, /* encryKeySize */
                                         hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].isVariable, /* isVariable */
                                         &(hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].charHandle));
        if (hciCmdResult == BLE_STATUS_SUCCESS)
        {
          HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Char. x non HID Service (%d, %d) is added Successfully %04X\n",tempIndx, tempIndx1, hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].charHandle);
        }
        else 
        {
          HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Char. for non HID Service (%d, %d), Error: %02X !!\n",tempIndx,tempIndx1, hciCmdResult);
          return hciCmdResult;
        }       
      }/* end for(hidDevice.hidService[tempIndx].currExtReportRefIndx): adding non HID services */
    }/* else if (hidDevice.hidService[tempIndx].extReportRefCharDescNum == 0) */
    
    /* no more non hid services to be added */
    /* reset the index. we will use it again to add
     * included services and descriptor
    */
    hidDevice.hidService[tempIndx].currExtReportRefIndx = 0;
    tempIndx1 = hidDevice.hidService[tempIndx].currExtReportRefIndx;
    
    /* Add include service */   
    if (hidDevice.hidService[tempIndx].extReportRefCharDescNum == 0)
    {
      HID_DEVICE_MESG_DBG(profiledbgfile, "No Include service, hidservice(%d) %x \n",tempIndx,hidDevice.currHidServIndx);
    }
    else
    {
      for (tempIndx1 = 0; tempIndx1 < hidDevice.hidService[tempIndx].extReportRefCharDescNum; tempIndx1++) //num of extReportRefCharDescNum
      {
            HID_DEVICE_MESG_DBG(profiledbgfile,"Adding include service %d %d %d\n",hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].servhandle,hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].charHandle+1,hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].servUUID);
            
            hciCmdResult = aci_gatt_include_service(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                                    hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].servhandle,    /* startHandle */
                                                    hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].charHandle+1, 
                                                    UUID_TYPE_16, 
                                                    (uint8_t*)&hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].servUUID,
                                                    &(characDescHandle)); //includedHandle[tempIndx][tempIndx1]
            
            
            if (hciCmdResult == BLE_STATUS_SUCCESS)
            {
              HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Include x non HID Service (%d, %d) is added Successfully %04X\n",tempIndx, tempIndx1, hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].charHandle);
            }
            else 
            {
              HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Include for non HID Service (%d, %d), Error: %02X !!\n",tempIndx,tempIndx1, hciCmdResult);
              return hciCmdResult;
            }       
            
      }/* end for (tempIndx1 = 0): Add include service  */
    }/* if (hidDevice.hidService[tempIndx].extReportRefCharDescNum == 0)) */

    /* TO BE DONE: Add from Protocol mode charact. to ..... HID ctrl point charact. */
    
    
    /* Add from Protocol mode characteristic */
    UUID = PROTOCOL_MODE_CHAR_UUID;
   
    hciCmdResult = aci_gatt_add_char(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                     UUID_TYPE_16, 
                                     (uint8_t *)&UUID , 
                                     (uint8_t)1, 
                                     (CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RESP), 
                                     ATTR_PERMISSION,
                                     GATT_NOTIFY_ATTRIBUTE_WRITE, 
                                     10,
                                     CHAR_VALUE_LEN_CONSTANT,
                                     &(hidDevice.hidService[tempIndx].protocolModeCharHandle));
    
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Protocol mode charact(%d) is added Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].protocolModeCharHandle);
    }
    else 
    {
      HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Protocol mode charact(%d), Error: %02X !!\n",tempIndx,hciCmdResult);
      return hciCmdResult;
    }   
    
    /* Update Protocol mode characteristic */
    hciCmdResult = aci_gatt_update_char_value(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                              hidDevice.hidService[tempIndx].protocolModeCharHandle, 
                                              (uint8_t)0,
                                              (uint8_t)1, 
                                              &(hidDevice.hidService[tempIndx].protocolMode));
    
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Protocol mode charact(%d) update is Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].protocolModeCharHandle);
    }
    else 
    {
      HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to update Protocol mode charact(%d), Error: %02X !!\n",tempIndx,hciCmdResult);
      return hciCmdResult;
    }   
    
    for (tempIndx1 = 0; tempIndx1 < hidDevice.hidService[tempIndx].inputReportCharNum; tempIndx1++ ) 
    {
        /* Add Input Report Characteristic */
        HID_DEVICE_MESG_DBG (profiledbgfile,"HIDProfile_Add_Services_Characteristics(), Adding Input Report Characteristic (%d, %d). \n", tempIndx, tempIndx1);
        
        UUID = REPORT_CHAR_UUID;
        hciCmdResult = aci_gatt_add_char(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                         UUID_TYPE_16,
                                         (uint8_t *)&UUID ,
                                         hidDevice.hidService[tempIndx].inputReport[tempIndx1].reportValueLen, 
                                         (CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_NOTIFY),
                                         ATTR_PERMISSION,
                                         GATT_NOTIFY_ATTRIBUTE_WRITE,
                                         10, 
                                         CHAR_VALUE_LEN_VARIABLE,/* there is no harm in keeping it as variable */
                                         &(hidDevice.hidService[tempIndx].inputReport[tempIndx1].reportCharHandle));
        if (hciCmdResult == BLE_STATUS_SUCCESS)
        {
          HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Input Report charact(%d) update is Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].inputReport[tempIndx1].reportCharHandle);
        }
        else 
        {
          HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to update Input Report characteristic (%d, %d), Error: %02X !!\n",tempIndx,tempIndx1, hciCmdResult);
          return hciCmdResult;
        }  

        HID_DEVICE_MESG_DBG (profiledbgfile,"HIDProfile_Add_Services_Characteristics(), Adding Input Report Reference Characteristic Descriptor (%d, %d). \n", tempIndx, tempIndx1);

        tempBuf[0] = hidDevice.hidService[tempIndx].inputReport[tempIndx1].reportID; 
        tempBuf[1] = HID_REPORT_TYPE_INPUT;

        UUID = REPORT_REFERENCE_DESCRIPTOR_UUID;
        hciCmdResult = aci_gatt_add_char_desc(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                              hidDevice.hidService[tempIndx].inputReport[tempIndx1].reportCharHandle, 
                                              UUID_TYPE_16, 
                                              (uint8_t *)&UUID, 
                                              (uint8_t)REPORT_REFERENCE_DESCRIPTOR_LENGTH, 
                                              (uint8_t)REPORT_REFERENCE_DESCRIPTOR_LENGTH,
                                              tempBuf,
                                              ATTR_PERMISSION,
                                              ATTR_ACCESS_READ_ONLY, 
                                              GATT_DONT_NOTIFY_EVENTS, 
                                              10, 
                                              CHAR_VALUE_LEN_CONSTANT,  
                                              &(characDescHandle));//descHandle_input_report_reference[tempIndx][tempIndx1]
                                              
        if (hciCmdResult == BLE_STATUS_SUCCESS)
        {
          HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Input Report charact(%d,%d) descriptor added Successfully %04X\n",tempIndx, tempIndx1, characDescHandle);
        }
        else 
        {
          HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Input Report charact(%d, %d), Error: %02X !!\n",tempIndx,tempIndx1, hciCmdResult);
          return hciCmdResult;
        }                                        
    }/* end for (tempIndx1 = 0) */     
                                              
    for(tempIndx1 = 0; tempIndx1 < hidDevice.hidService[tempIndx].outputreportCharNum; tempIndx1++)
    {
      /* Add Output Report Characteristic */
      HID_DEVICE_MESG_DBG (profiledbgfile,"Adding Output Report Characteristic (%d, %d) \n", tempIndx, tempIndx1);
      
      UUID = REPORT_CHAR_UUID;
      hciCmdResult = aci_gatt_add_char(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                       UUID_TYPE_16,
                                       (uint8_t *)&UUID ,
                                       hidDevice.hidService[tempIndx].outputReport[tempIndx1].reportValueLen, 
                                       (CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP),
                                       ATTR_PERMISSION,
                                       GATT_NOTIFY_ATTRIBUTE_WRITE,
                                       10, 
                                       CHAR_VALUE_LEN_VARIABLE,
                                       &(hidDevice.hidService[tempIndx].outputReport[tempIndx1].reportCharHandle));
      
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Output Report charact(%d) update is Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].outputReport[tempIndx1].reportCharHandle);
      }
      else 
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to update Output Report characteristic (%d, %d), Error: %02X !!\n",tempIndx,tempIndx1, hciCmdResult);
        return hciCmdResult;
      }   
      
      tempBuf[0] = hidDevice.hidService[tempIndx].outputReport[tempIndx1].reportID;
      tempBuf[1] = HID_REPORT_TYPE_OUTPUT;

      UUID = REPORT_REFERENCE_DESCRIPTOR_UUID;
      hciCmdResult = aci_gatt_add_char_desc(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                            hidDevice.hidService[tempIndx].outputReport[tempIndx1].reportCharHandle, 
                                            UUID_TYPE_16, 
                                            (uint8_t *)&UUID, 
                                            (uint8_t)REPORT_REFERENCE_DESCRIPTOR_LENGTH, 
                                            (uint8_t)REPORT_REFERENCE_DESCRIPTOR_LENGTH,
                                            tempBuf,
                                            ATTR_PERMISSION,
                                            ATTR_ACCESS_READ_ONLY, 
                                            GATT_DONT_NOTIFY_EVENTS, 
                                            10, 
                                            CHAR_VALUE_LEN_CONSTANT,
                                            &(characDescHandle));//descHandle_output_report_reference[tempIndx][tempIndx1]
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Output Report charact(%d,%d) descriptor added Successfully %04X\n",tempIndx, tempIndx1, characDescHandle);
      }
      else 
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Output Report charact (%d, %d), Error: %02X !!\n",tempIndx,tempIndx1, hciCmdResult);
        return hciCmdResult;
      }      
    }  /* end for (tempIndx1=0) */
    
    /* (current index) < (num_of_input_reports) */
    for (tempIndx1 = 0;  tempIndx1< hidDevice.hidService[tempIndx].featureReportCharNum; tempIndx1++)
    {
      /* Add Feature Report Characteristic */
      HID_DEVICE_MESG_DBG (profiledbgfile,"Adding Feature Report Characteristic(%d, %d)\n", tempIndx, tempIndx1);

      UUID = REPORT_CHAR_UUID;
      hciCmdResult = aci_gatt_add_char(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                       UUID_TYPE_16,
                                       (uint8_t *)&UUID ,
                                       hidDevice.hidService[tempIndx].featureReport[tempIndx1].reportValueLen, 
                                       (CHAR_PROP_READ | CHAR_PROP_WRITE),
                                       ATTR_PERMISSION, //xxxxxxxxxxxxxxxxxxxxxxxxxx
                                       GATT_NOTIFY_ATTRIBUTE_WRITE, 
                                       10, 
                                       CHAR_VALUE_LEN_VARIABLE,
                                       &(hidDevice.hidService[tempIndx].featureReport[tempIndx1].reportCharHandle)
                                       );
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Feature Report charact(%d) update is Successfully %04X\n",tempIndx,hidDevice.hidService[tempIndx].featureReport[tempIndx1].reportCharHandle);
      }
      else 
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to update Feature Report characteristic (%d, %d), Error: %02X !!\n",tempIndx,tempIndx1, hciCmdResult);
        return hciCmdResult;
      }   
      
      HID_DEVICE_MESG_DBG (profiledbgfile,"Adding Feature Report Reference Characteristic Descriptor (%d, %d)\n",tempIndx,tempIndx1);

      tempBuf[0] = hidDevice.hidService[tempIndx].featureReport[tempIndx1].reportID;
      tempBuf[1] = HID_REPORT_TYPE_FEATURE;

      UUID = REPORT_REFERENCE_DESCRIPTOR_UUID;
      hciCmdResult = aci_gatt_add_char_desc(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                            hidDevice.hidService[tempIndx].featureReport[tempIndx1].reportCharHandle, 
                                            UUID_TYPE_16, 
                                            (uint8_t *)&UUID, 
                                            (uint8_t)REPORT_REFERENCE_DESCRIPTOR_LENGTH, 
                                            (uint8_t)REPORT_REFERENCE_DESCRIPTOR_LENGTH,
                                            tempBuf,
                                            ATTR_PERMISSION,
                                            ATTR_ACCESS_READ_ONLY, 
                                            GATT_DONT_NOTIFY_EVENTS,  
                                            10, 
                                            CHAR_VALUE_LEN_CONSTANT, 
                                           &(characDescHandle));//descHandle_feature_report[tempIndx][tempIndx1]
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Feature Report charact(%d,%d) descriptor added Successfully %04X\n",tempIndx, tempIndx1, characDescHandle);
      }
      else 
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Feature Report charact descriptor(%d, %d), Error: %02X !!\n",tempIndx,tempIndx1, hciCmdResult);
        return hciCmdResult;
      }    
    }/* end for (tempIndx1=0) */
  
    /* Add Report Map Characteristic */
    HID_DEVICE_MESG_DBG (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),Adding Report Map Characteristic(%d, %d)\n",tempIndx,tempIndx1);                                     
    UUID = REPORT_MAP_CHAR_UUID;
    hciCmdResult = aci_gatt_add_char(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                     UUID_TYPE_16,
                                     (uint8_t *)&UUID ,
                                     hidDevice.hidService[tempIndx].reportMapCharValueLen, 
                                     (CHAR_PROP_READ),
                                     ATTR_PERMISSION,
                                     GATT_DONT_NOTIFY_EVENTS, 
                                     10, 
                                     CHAR_VALUE_LEN_VARIABLE,  //xxxxxxxxxxxxxxxxxxxxxx
                                     &(hidDevice.hidService[tempIndx].reportMapCharHandle));
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Added Report Map charact(%d) is Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].reportMapCharHandle);
    }
    else 
    {
      HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Report Map charact(%d), Error: %02X !!\n",tempIndx,hciCmdResult);
      return hciCmdResult;
    }   
    
    hciCmdResult = aci_gatt_update_char_value(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                              hidDevice.hidService[tempIndx].reportMapCharHandle, 
                                              0,
                                              hidDevice.hidService[tempIndx].reportMapCharValueLen, 
                                              hidDevice.hidService[tempIndx].pReportMapCharValue);
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Report Map charact(%d) update is Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].reportMapCharHandle);
    }
    else 
    {
      HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to update Report Map charact(%d), Error: %02X !!\n",tempIndx,hciCmdResult);
      return hciCmdResult;
    }   
    
    /* Add External Report Reference Characteristic Descriptor */
    //tempIndx1 = hidDevice.hidService[tempIndx].currExtReportRefIndx;
    /* (already_added_descriptors) < (total_num_of_descriptors) */
    /* (current index) < (total_num_of_descriptors) */
    for (tempIndx1 = 0;  tempIndx1 < hidDevice.hidService[tempIndx].extReportRefCharDescNum; tempIndx1++ )
    {
      /* Add External Report Reference Characteristic Descriptor */
      HID_DEVICE_MESG_DBG (profiledbgfile,"Adding External Report Reference Characteristic Descriptor (%d, %d)\n",tempIndx,tempIndx1);

      UUID = hidDevice.hidService[tempIndx].nonHidServInfo[tempIndx1].charUUID;
      tempBuf[0] = (uint8_t)(UUID & 0x00FF);
      tempBuf[1] = (uint8_t)((UUID >> 8) & 0x00FF);

      UUID = EXTERNAL_REPORT_REFERENCE_DESCRIPTOR_UUID;
      hciCmdResult = aci_gatt_add_char_desc(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                            hidDevice.hidService[tempIndx].reportMapCharHandle, 
                                            UUID_TYPE_16, 
                                            (uint8_t *)&UUID, 
                                            (uint8_t)2, 
                                            (uint8_t)2,
                                            (uint8_t *)&tempBuf,
                                            ATTR_PERMISSION,
                                            ATTR_ACCESS_READ_ONLY, 
                                            GATT_DONT_NOTIFY_EVENTS,  
                                            10, 
                                            CHAR_VALUE_LEN_CONSTANT,
                                            &(characDescHandle));//descHandle_external_report_reference[tempIndx][tempIndx1]
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), External Report Reference charact(%d,%d) descriptor added Successfully %04X\n",tempIndx, tempIndx1,characDescHandle);
      }
      else 
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add External Report Reference descriptor (%d, %d), Error: %02X !!\n",tempIndx,tempIndx1, hciCmdResult);
        return hciCmdResult;
      }                                          
    }  /* end for (tempIndx1 = 0) */                                     
                                            
    /* check if the device is boot keyboard */
    if (hidDevice.hidService[tempIndx].bootDeviceMode & BOOT_DEVICE_KEYBOARD_MASK)
    {
      /* Add Boot Keyboard Input Report Characteristic */
      HID_DEVICE_MESG_DBG (profiledbgfile,"HIDProfile_Add_Services_Characteristics(), Adding Boot Keyboard Input Report Characteristic (%d) \n",tempIndx);

      UUID = BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID;
      hciCmdResult = aci_gatt_add_char(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                       UUID_TYPE_16,
                                       (uint8_t *)&UUID ,
                                       (uint8_t)hidDevice.hidService[tempIndx].bootKbdInput.reportValueLen, 
                                       (CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_NOTIFY),
                                       ATTR_PERMISSION,
                                       GATT_NOTIFY_ATTRIBUTE_WRITE, 
                                       10, 
                                       CHAR_VALUE_LEN_VARIABLE,
                                       &(hidDevice.hidService[tempIndx].bootKbdInput.reportCharHandle));
      
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Added Boot Keyboard Input Report charact(%d) is Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].bootKbdInput.reportCharHandle);
      }
      else 
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Boot Keyboard Input Report charact(%d), Error: %02X !!\n",tempIndx,hciCmdResult);
        return hciCmdResult;
      }  
      
      /* Add Boot Keyboard Output Report Characteristic */
      HID_DEVICE_MESG_DBG (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),Adding Boot Keyboard Output Report Characteristic(%d) \n",tempIndx);


      UUID = BOOT_KEYBOARD_OUTPUT_REPORT_CHAR_UUID;
      hciCmdResult = aci_gatt_add_char(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                       UUID_TYPE_16,
                                       (uint8_t *)&UUID ,
                                       (uint8_t)hidDevice.hidService[tempIndx].bootKbdOutput.reportValueLen,
                                       (CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP),
                                       ATTR_PERMISSION,
                                       GATT_NOTIFY_ATTRIBUTE_WRITE, 
                                       10, 
                                       CHAR_VALUE_LEN_VARIABLE,
                                       &(hidDevice.hidService[tempIndx].bootKbdOutput.reportCharHandle));
      
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Added Boot Keyboard Output Report charact(%d) is Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].bootKbdOutput.reportCharHandle);
      }
      else 
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Boot Keyboard Output Report charact(%d), Error: %02X !!\n",tempIndx,hciCmdResult);
        return hciCmdResult;
      }             
    } /* end if (hidDevice.hidService[tempIndx].bootDeviceMode & BOOT_DEVICE_KEYBOARD_MASK) */                                      
    else if (hidDevice.hidService[tempIndx].bootDeviceMode & BOOT_DEVICE_MOUSE_MASK)
    {
      /* Add code for : HID_DEVICE_STATE_ADDED_BOOT_KBD_OP_REPORT_CHAR */
      
      /* Add Boot Mouse Input Report Characteristic */
      HID_DEVICE_MESG_DBG (profiledbgfile,"Adding Boot  Mouse Report Characteristic. \n");

      UUID = BOOT_MOUSE_INPUT_REPORT_CHAR_UUID;
      hciCmdResult = aci_gatt_add_char(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                       UUID_TYPE_16,
                                       (uint8_t *)&UUID ,
                                       (uint8_t)hidDevice.hidService[tempIndx].bootMouseInput.reportValueLen, 
                                       (CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_NOTIFY), 
                                       ATTR_PERMISSION,
                                       GATT_NOTIFY_ATTRIBUTE_WRITE, 
                                       10, 
                                       CHAR_VALUE_LEN_VARIABLE,
                                       &(hidDevice.hidService[tempIndx].bootMouseInput.reportCharHandle));
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Added Boot Mouse Input Report charact(%d) is Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].bootMouseInput.reportCharHandle);
      }
      else 
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Boot Mouse Input Report charact(%d), Error: %02X !!\n",tempIndx,hciCmdResult);
        return hciCmdResult;
      } 
    }/* end else if (hidDevice.hidService[tempIndx].bootDeviceMode & BOOT_DEVICE_MOUSE_MASK) */                                   
                                            
    /* Add HID Information Characteristic */
    HID_DEVICE_MESG_DBG (profiledbgfile,"Adding HID Information Characteristic(%d) \n",tempIndx);

    tempIndx = hidDevice.currHidServIndx;

    UUID = HID_INFORMATION_CHAR_UUID;
    hciCmdResult = aci_gatt_add_char(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                     UUID_TYPE_16,
                                     (uint8_t *)&UUID ,
                                     4, 
                                     (CHAR_PROP_READ),
                                     ATTR_PERMISSION, //xxxxxxxxxxxxxxxxxxxxxxxxxx
                                     GATT_DONT_NOTIFY_EVENTS, 
                                     10, 
                                     CHAR_VALUE_LEN_VARIABLE,  //xxxxxxxxxxxxxxxxxxxxxx
                                     &(hidDevice.hidService[tempIndx].hidInfoCharHandle));
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Added HID Information charact(%d) is Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].hidInfoCharHandle);
    }
    else 
    {
      HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add HID Information charact(%d), Error: %02X !!\n",tempIndx,hciCmdResult);
      return hciCmdResult;
    } 
      
    /* NEW code TBR */
    HID_DEVICE_MESG_DBG (profiledbgfile,"*************Updating HID information Characteristic\n**************");

    tempIndx = hidDevice.currHidServIndx;
    // value length and value parameters needs to be added to this command.
    hciCmdResult = aci_gatt_update_char_value(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                              hidDevice.hidService[tempIndx].hidInfoCharHandle,
                                              0,
                                              sizeof( hidDevice.hidService[tempIndx].hidInfoChar), 
                                              hidInfoCharValueBuff);                                           
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HID_DEVICE_MESG_DBG ( profiledbgfile, "HIDProfile_Add_Services_Characteristics(), HID info characteristic value has been sent for update \n");
    }
    else
    {
      HID_DEVICE_MESG_DBG ( profiledbgfile, "HIDProfile_Add_Services_Characteristics(), FAILED to send HID info characteristic value %02X \n", hciCmdResult);
      return hciCmdResult;
    }			
          
    /* end NEW code */                               
  
    /* Add HID Control Point Characteristic */
    HID_DEVICE_MESG_DBG (profiledbgfile,"Adding HID Control Point Char(%d) \n",tempIndx);
    UUID = HID_CONTROL_POINT_CHAR_UUID;
    hciCmdResult = aci_gatt_add_char(hidDevice.hidService[tempIndx].hidServiceHandle, 
                                     UUID_TYPE_16,
                                     (uint8_t *)&UUID ,
                                     (uint8_t)1, 
                                     (CHAR_PROP_WRITE_WITHOUT_RESP),
                                     ATTR_PERMISSION,
                                     GATT_NOTIFY_ATTRIBUTE_WRITE, 
                                     10, 
                                     CHAR_VALUE_LEN_CONSTANT,
                                     &(hidDevice.hidService[tempIndx].hidCtlPointCharHandle));
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Added HID Control Point charact(%d) is Successfully %04X\n",tempIndx, hidDevice.hidService[tempIndx].hidCtlPointCharHandle);
    }
    else 
    {
      HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add HID Control Point charact(%d), Error: %02X !!\n",tempIndx,hciCmdResult);
      return hciCmdResult;
    }                                 
  } /**************************end second for (hidDevice.currHidServIndx = 0) */
  
  /* ************************** Add Battery Services ************************* */
  HID_DEVICE_MESG_DBG( profiledbgfile, "Adding Battery Service(%d) \n",tempIndx);

  for (tempIndx = 0; tempIndx < hidDevice.numofBatteryServices; tempIndx ++)
  {
    /* serviceMaxAttributeRecord = 1 for battery service itself + 
     *                             2 for battery level characteristic + 
     *                             1 for Notifications +
     *                             1 for char presentation format descriptor(if multiple battery services are present)
     */
    serviceMaxAttributeRecord = 4;

    if(hidDevice.numofBatteryServices > 1)
    {
      serviceMaxAttributeRecord += 1;
    }
                        
    /* store the Battery Service UUID */
    UUID = BATTERY_SERVICE_SERVICE_UUID;
    HID_DEVICE_MESG_DBG( profiledbgfile, "Battery Service UUID %x \n",UUID);
    hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                     (const uint8_t *)&UUID, 
                                     PRIMARY_SERVICE, 
                                     serviceMaxAttributeRecord,
                                     &(hidDevice.batteryService[tempIndx].batteryServHandle));
    
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Battery Service (%d) is added Successfully %04X\n",tempIndx, hidDevice.batteryService[tempIndx].batteryServHandle);
    }
    else 
    {
      HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Battery Service (%d), Error: %02X !!\n",tempIndx, hciCmdResult);
      return hciCmdResult;
    }
    
  }/* end for (tempIndx = 0) */
  
  /* requested num of Battery services have been added, now proceed further */
  /* Now start adding characteristics and descriptors from
   * first Battery service with index zero. */
  
  for (tempIndx = 0; tempIndx < hidDevice.numofBatteryServices; tempIndx++)
  {
    /* Add Battery Level Characteristic */
    HID_DEVICE_MESG_DBG (profiledbgfile,"Adding Battery Level Characteristic(%d) \n",tempIndx);

    UUID = BATTERY_LEVEL_CHAR_UUID;
   
    /* the battery level characteristic should be added with the
     * event bit to notify on write enabled. So that whenever
     * the peer tries to read the battery level, the application
     * can be notified and it updates the battery level with the
     * latest value 
     */ 
    hciCmdResult = aci_gatt_add_char(hidDevice.batteryService[tempIndx].batteryServHandle, 
                                     UUID_TYPE_16, 
                                     (uint8_t *)&UUID , 
                                     (uint8_t)1, 
                                     CHAR_PROP_READ|CHAR_PROP_NOTIFY, 
                                     ATTR_PERMISSION,
                                     GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                                     10,
                                     CHAR_VALUE_LEN_CONSTANT,
                                     &(hidDevice.batteryService[tempIndx].batteryLevlCharHandle));
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Added Battery Level charact(%d) is Successfully %04X\n",tempIndx, hidDevice.batteryService[tempIndx].batteryLevlCharHandle);
    }
    else 
    {
      HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Battery Level charact(%d), Error: %02X !!\n",tempIndx,hciCmdResult);
      return hciCmdResult;
    } 
    
    /* add char presentation format descriptor only if multiple battery services are supported */
    if (hidDevice.numofBatteryServices > 1)
    {
      HID_DEVICE_MESG_DBG (profiledbgfile,"Adding Battery Level Char Presentation Format Descriptor(%d)\n",tempIndx);
                              
     /* TODO ???  : the presentation descriptor value
      * has to be assigned. Add while implementing
      * the battery profile
      */ 
      tempBuf[0] = 0x0;
      tempBuf[1] = 0x0;
      tempBuf[2] = 0x0;
      tempBuf[3] = 0x0;
      tempBuf[4] = 0x0;
      tempBuf[5] = 0x0;
      tempBuf[6] = 0x0;

      UUID = CHAR_PRESENTATION_FORMAT_DESCRIPTOR_UUID;
      hciCmdResult = aci_gatt_add_char_desc(hidDevice.batteryService[tempIndx].batteryServHandle, 
                                            hidDevice.batteryService[tempIndx].batteryLevlCharHandle, 
                                            UUID_TYPE_16, 
                                            (uint8_t *)&UUID, 
                                            CHAR_PRESENTATION_FORMAT_DESCRIPTOR_LENGTH, 
                                            (uint8_t)CHAR_PRESENTATION_FORMAT_DESCRIPTOR_LENGTH,
                                            (uint8_t *)&tempBuf,
                                            ATTR_PERMISSION,
                                            ATTR_ACCESS_READ_ONLY,
                                            GATT_DONT_NOTIFY_EVENTS,
                                            10, 
                                            CHAR_VALUE_LEN_CONSTANT,
                                            &(characDescHandle));//descHandle_battery_level[tempIndx]
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Battery Level charact descriptor (%d)  added Successfully %04X\n",tempIndx,characDescHandle);
      }
      else 
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Battery Level descriptor (%d), Error: %02X !!\n",tempIndx, hciCmdResult);
        return hciCmdResult;
      }            
    }
    else
    {
        /* skip the char presentation format descriptor addition step */
        /* change profile's sub states */
      //HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_ADDED_BTRY_LEVL_CHAR_PRESEN_FORMAT_DESC); TBR: is it needed? --<-----
    }
  }/* end for (tempIndx = 0) */
                                            
  if (hidDevice.scanParamService.serviceSupports & SCAN_PARAMETER_SERVICE_MASK)
  {
      /* need to support scan parameter service */

      /* Add Scan Parameter Service */
      HID_DEVICE_MESG_DBG( profiledbgfile, "HIDProfile_Add_Services_Characteristics() Adding Scan Parameter Service \n");

      /* serviceMaxAttributeRecord = 1 for Scan Parameter service itself + 
       *                             2 for scan interval window characteristic + 
       *                             2 for scan refresh characteristic, optional + 
       *                             1 for client char configuration descriptor, optional
       */
      serviceMaxAttributeRecord = 3;
      if (hidDevice.scanParamService.serviceSupports & SCAN_REFRESH_CHAR_MASK)
      {
        serviceMaxAttributeRecord += 3;
      }

      /* store the scan parameter Service UUID */
      UUID = SCAN_PARAMETER_SERVICE_UUID;

      hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                       (uint8_t *)&UUID, 
                                       PRIMARY_SERVICE, 
                                       serviceMaxAttributeRecord,
                                       &(hidDevice.scanParamService.scanParamServHandle));
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Scan Parameter Service is added Successfully %04X\n",hidDevice.scanParamService.scanParamServHandle);
      }
      else 
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Scan Parameter Service Error: %02X !!\n",hciCmdResult);
        return hciCmdResult;
      }
      
      /* Add Scan Interval Window Characteristic */
      HID_DEVICE_MESG_DBG (profiledbgfile,"Adding Scan Interval Window Characteristic. \n");

      UUID = SCAN_INTERVAL_WINDOW_CHAR_UUID;
      hciCmdResult = aci_gatt_add_char(hidDevice.scanParamService.scanParamServHandle, 
                                       UUID_TYPE_16, 
                                       (uint8_t *)&UUID , 
                                       (uint8_t)4,
                                       (CHAR_PROP_WRITE_WITHOUT_RESP), 
                                       ATTR_PERMISSION,
                                       GATT_NOTIFY_ATTRIBUTE_WRITE,
                                       10,
                                       CHAR_VALUE_LEN_CONSTANT,
                                       &(hidDevice.scanParamService.scanIntervalWinCharHandle));
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Added Scan Interval Window charact is Successfully %04X\n",hidDevice.scanParamService.scanIntervalWinCharHandle);
      }
      else 
      {
        HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Scan Interval Window charact, Error: %02X !!\n",hciCmdResult);
        return hciCmdResult;
      } 
    
      if (hidDevice.scanParamService.serviceSupports & SCAN_REFRESH_CHAR_MASK)
      {
          /* Add Scan Refresh Characteristic */
          HID_DEVICE_MESG_DBG (profiledbgfile,"Adding Scan Refresh Characteristic. \n");

          UUID = SCAN_REFRESH_CHAR_UUID;
          hciCmdResult = aci_gatt_add_char(hidDevice.scanParamService.scanParamServHandle, 
                                           UUID_TYPE_16,
                                           (uint8_t *)&UUID,
                                           (uint8_t)1,
                                           (CHAR_PROP_NOTIFY),
                                           ATTR_PERMISSION,
                                           GATT_DONT_NOTIFY_EVENTS,
                                           10,
                                           CHAR_VALUE_LEN_CONSTANT,
                                           &(hidDevice.scanParamService.scanRefreshCharHandle));
          if (hciCmdResult == BLE_STATUS_SUCCESS)
          {
            HRPROFILE_MESG_DBG (profiledbgfile, "HIDProfile_Add_Services_Characteristics(), Added Scan Refresh charact is Successfully %04X\n",hidDevice.scanParamService.scanRefreshCharHandle);
          }
          else 
          {
            HRPROFILE_MESG_ERR (profiledbgfile,"HIDProfile_Add_Services_Characteristics(),FAILED to add Scan Refresh charact, Error: %02X !!\n",hciCmdResult);
            return hciCmdResult;
          } 
      }  /* end if (hidDevice.scanParamService.serviceSupports & SCAN_REFRESH_CHAR_MASK) */
  }/* end if (hidDevice.scanParamService.serviceSupports & SCAN_PARAMETER_SERVICE_MASK) */
       
  return hciCmdResult;
}/* end HIDProfile_Add_Services_Characteristics() */

                                                                                   
/**
 * HIDProfile_StateMachine
 * 
 * @param[in] None
 * 
   * HID profile's state machine: to be called on application main loop. 
 */ 
tBleStatus HIDProfile_StateMachine(void)
{
  uint8_t cmdCmpltStatus = BLE_STATUS_SUCCESS;
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;
  uint8_t tempBuf[12];

  /* hidDevice.mainState, hidDevice.subState */
  switch (HIDProfile_Read_MainStateMachine())
  {
    case HID_DEVICE_STATE_UNINITIALIZED:
    break;
    case HID_DEVICE_STATE_WAITING_BLE_INIT:
    {
      /* chech whether the main profile has done with ble init or not */
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
      {
        /* Add HID service & characteristics */ 
        hciCmdResult = HIDProfile_Add_Services_Characteristics();
        if (hciCmdResult == BLE_STATUS_SUCCESS)
        {
          HID_DEVICE_MESG_DBG ( profiledbgfile, "HIDProfile_StateMachine(), HID Profile is Initialized with Service & Characteristics  \n");

          hidDevice.currHidServIndx = 0;
          /* change current profiles Main and Sub both states */
          /* change profiles main and sub states */
          HIDProfile_Write_MainStateMachine(HID_DEVICE_STATE_INITIALIZED);
          HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_INVALID);

          /* vote to the main profile to go to advertise state */
          (void) BLE_Profile_Vote_For_Advertisable_State (&hidDevice.HIDtoBLEInf);

          /* add the profile specific service UUIDs into global list of supported UUIDs*/
          BLE_Profile_Add_Advertisment_Service_UUID (HUMAN_INTERFACE_DEVICE_SERVICE_UUID);
          BLE_Profile_Add_Advertisment_Service_UUID (BATTERY_SERVICE_SERVICE_UUID);
          BLE_Profile_Add_Advertisment_Service_UUID (DEVICE_INFORMATION_SERVICE_UUID);        
        }
       else
       {
          HID_DEVICE_MESG_DBG ( profiledbgfile, "HIDProfile_StateMachine(), FAILED HID Profile Initialization with Service & Characteristics  Error: %02X !!\n", hciCmdResult);
          cmdCmpltStatus = BLE_STATUS_FAILED;
          /* Notify to application that HID profile initialization has failed */
          hidDevice.applicationNotifyFunc(EVT_HID_INITIALIZED,1,&cmdCmpltStatus);
          /* Change profile state to represent uninitialized profile */
          HIDProfile_Write_MainStateMachine(HID_DEVICE_STATE_UNINITIALIZED);
          HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_INVALID);
          /* unregister the profile from BLE registered profile list */
          BLE_Profile_Unregister_Profile ( &hidDevice.HIDtoBLEInf );
          /* Need to remove all added services and characteristics ??? */
       }
      }
    }
    break; /* HID_DEVICE_STATE_WAITING_BLE_INIT */
    
    case HID_DEVICE_STATE_INITIALIZED: 
    {
      /* HID Device profile has completed its initialization process and voted the 
       * main profile for Advertisable state. Now its waiting for the main profile
       * to set to Advertisable state. */

      /* check main profile's state, if its in advertisable state, set the same for 
       * current profile as well */
      if ((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) && 
          (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_CONNECTABLE_IDLE))
      {
        HIDProfile_Write_MainStateMachine(HID_DEVICE_STATE_ADVERTIZE);
        HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_ADVERTISABLE_IDLE);
        cmdCmpltStatus = BLE_STATUS_SUCCESS;
        hidDevice.applicationNotifyFunc(EVT_HID_INITIALIZED,1,&cmdCmpltStatus);
      }
    }
    break; /* HID_DEVICE_STATE_INITIALIZED */
    case HID_DEVICE_STATE_ADVERTIZE:
    {
      switch (HIDProfile_Read_SubStateMachine())
      {
        case HID_DEVICE_STATE_ADVERTISABLE_IDLE:
        {
        }
        break;
        case HID_DEVICE_STATE_ADVERTISING_FOR_WHITE_LIST_DO_START:
        {
        }
        break;
        case HID_DEVICE_STATE_ADVERTISING_FOR_WHITE_LIST_STARTING: 
        {
        }
        break;
        case HID_DEVICE_STATE_ADVERTISING_FOR_WHITE_LIST:
        break;
        case HID_DEVICE_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP: 
        {
          HID_DEVICE_MESG_DBG ( profiledbgfile, "HIDProfile_StateMachine(), STOPPING Advertising to White List devices \n");
          
          hciCmdResult = aci_gap_set_non_discoverable();
          if ( hciCmdResult == BLE_STATUS_SUCCESS)
          {
            /* change profile's sub states */
            HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_FAST_DISCOVERABLE_DO_START);
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State ( &hidDevice.HIDtoBLEInf, 
                                                     BLE_PROFILE_STATE_CONNECTABLE_IDLE);
          }
          else
          {
            HID_DEVICE_MESG_DBG (profiledbgfile,"HIDProfile_StateMachine(), FAILED to start Fast discoverable mode, Error: %02X !!\n", hciCmdResult);
            return hciCmdResult;
          }
        }
        break;
        case HID_DEVICE_STATE_FAST_DISCOVERABLE_DO_START: 
        {
          //tempBuf[0] = 0x08;
          //BLUENRG_memcpy ((void *)(tempBuf+1), (void *)"HID", 0x03);
          hciCmdResult = HID_Sensor_Set_Discoverable(HIDP_FAST_CONN_ADV_INTERVAL_MIN,HIDP_FAST_CONN_ADV_INTERVAL_MAX);  
          if ( hciCmdResult == BLE_STATUS_SUCCESS)
          {
              HID_DEVICE_MESG_DBG ( profiledbgfile, "HIDProfile_StateMachine(), STARTED FAST Limited Discoverable Mode %d\n",gBLEProfileContext.advtServUUIDlen);
            /* change profile's sub states */
            HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_FAST_DISCOVERABLE); 
           
            /* start a timer of 30 seconds for first phase of discoverable mode (fast discoverable mode) */
            Blue_NRG_Timer_Start (30, HidDevice_Advertise_Period_Timeout_Handler,&hidDevice.timerID);
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State (&hidDevice.HIDtoBLEInf, 
                                                BLE_PROFILE_STATE_ADVERTISING);
          }
          else
          {
            HID_DEVICE_MESG_DBG (profiledbgfile,"HIDProfile_StateMachine(), FAILED to start Fast discoverable mode, Error: %02X !!\n", hciCmdResult);
            /* Request to put the device in limited discoverable mode is 
               * unsuccess, put the device back in advrtisable idle mode */
            HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_ADVERTISABLE_IDLE);
            return hciCmdResult; 
          }
        }
        break;
        case HID_DEVICE_STATE_FAST_DISCOVERABLE_STARTING:
        break;
        case HID_DEVICE_STATE_FAST_DISCOVERABLE:
        break;
        case HID_DEVICE_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP:
        {
          hciCmdResult = aci_gap_set_non_discoverable(); 
          if ( hciCmdResult == BLE_STATUS_SUCCESS)
          {
            /* change profile's sub states */
            //HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_FAST_DISCOVERABLE_STOPPING);
            HID_DEVICE_MESG_DBG ( profiledbgfile, "HIDProfile_StateMachine(), HIDProfile_StateMachine(), STOPPED FAST Limited Discoverable Mode \n");
            /* change profile's sub states */
            HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_LOW_POWER_DISCOVERABLE_DO_START);
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State(&hidDevice.HIDtoBLEInf, 
                                                   BLE_PROFILE_STATE_CONNECTABLE_IDLE);
            
            hciCmdResult = HID_Sensor_Set_Discoverable(HIDP_LOW_PWR_ADV_INTERVAL_MIN,HIDP_LOW_PWR_ADV_INTERVAL_MAX);  
            if ( hciCmdResult == BLE_STATUS_SUCCESS)
            {
              HID_DEVICE_MESG_DBG ( profiledbgfile, "IDProfile_StateMachine(),STARTED Low Power Limited Discoverable Mode \n");
              /* change profile's sub states */
              HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_LOW_POWER_DISCOVERABLE); //HID_DEVICE_STATE_LOW_POWER_DISCOVERABLE_STARTING;
              /* request for main profile's state change */
              BLE_Profile_Change_Advertise_Sub_State (&hidDevice.HIDtoBLEInf, 
                                                BLE_PROFILE_STATE_ADVERTISING);
            }
            else
            {
              HID_DEVICE_MESG_DBG (profiledbgfile,"HIDProfile_StateMachine(), FAILED to start Low Power Limited Discoverable mode, Error: %02X !!\n", hciCmdResult);
              /* Request to put the device in limited discoverable mode is 
              * unsuccess, put the device back in advrtisable idle mode */
              HIDProfile_Write_SubStateMachine(HID_DEVICE_STATE_ADVERTISABLE_IDLE);
              return hciCmdResult; 
            }
          }
          else
          {
            HID_DEVICE_MESG_DBG (profiledbgfile,"HIDProfile_StateMachine(), FAILED to STOP FAST Limited Discoverable Mode, Error: %02X !!\n", hciCmdResult);
            return hciCmdResult; 
          }
        }
        break;

        case HID_DEVICE_STATE_FAST_DISCOVERABLE_STOPPING:
        {
        }
        break;

        case HID_DEVICE_STATE_LOW_POWER_DISCOVERABLE_STARTING:
        break;
        case HID_DEVICE_STATE_LOW_POWER_DISCOVERABLE:
        break;
   
        case HID_DEVICE_STATE_BLE_ADVERTISING: 
        {
        }
        break;

        case HID_DEVICE_STATE_PROCESSING_PAIRING:
        break;
      }  /* switch (HIDProfile_Read_SubStateMachine()) */
      
      if(hidDevice.opCode == HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL) //TBR
      {
        STORE_LE_16(tempBuf,hidDevice.updater_service_handle);
        STORE_LE_16(&tempBuf[2],hidDevice.updater_char_handle);
        tempBuf[4] = cmdCmpltStatus;
        /* the update has completed. Notify the application */
        hidDevice.applicationNotifyFunc(EVT_HID_UPDATE_CMPLT,5,tempBuf);
        hidDevice.opCode = HCI_CMD_NO_OPERATION; //TBR
      }
    }
    break; /* HID_DEVICE_STATE_ADVERTIZE */
    case HID_DEVICE_STATE_CONNECTED: 
    {
      switch (HIDProfile_Read_SubStateMachine())
      {
        case HID_DEVICE_STATE_CONNECTED_IDLE:
        {
          if(hidDevice.opCode == HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL)
          {
            STORE_LE_16(tempBuf,hidDevice.updater_service_handle);
            STORE_LE_16(&tempBuf[2],hidDevice.updater_char_handle);
            tempBuf[4] = cmdCmpltStatus;
            /* the update has completed. Notify the application */
            hidDevice.applicationNotifyFunc(EVT_HID_UPDATE_CMPLT,5,tempBuf);
            hidDevice.opCode = HCI_CMD_NO_OPERATION; //TBR
          }
        }
        break;
      }
    }
    break; /* HID_DEVICE_STATE_CONNECTED */
  }  /* switch (HIDProfile_Read_MainStateMachine()) */
  return (hciCmdResult);
}/* end HIDProfile_StateMachine() */      
