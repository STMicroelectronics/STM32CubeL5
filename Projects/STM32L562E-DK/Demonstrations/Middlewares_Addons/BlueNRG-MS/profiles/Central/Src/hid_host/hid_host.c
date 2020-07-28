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
*   FILENAME        -  hid_host.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/01/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    HID Host APIs(connect, config charac, enable notification...)
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the HID host profile central role.
* 
******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
#include "debug.h"
#include "timer.h"
#include "uuid.h"
#include "hci.h"
#include "hci_le.h"
#include "hci_tl.h"
#include "hal.h"
#include "master_basic_profile.h"

#if (BLE_CURRENT_PROFILE_ROLES & HID_HOST)

#include "string.h"
#include "bluenrg_gatt_server.h"
#include "hid_host.h"

#define WEAK_FUNCTION(x) __weak x

#ifndef HID_HOST_TEST
#define HID_HOST_TEST 1
#endif

/******************************************************************************
* Macro Declarations
******************************************************************************/

/* HID Host States */
#define HID_HOST_UNINITIALIZED                                            0x0000
#define HID_HOST_CONNECTED_IDLE                                           0x0001
#define HID_HOST_INITIALIZED                                              0x0002
#define HID_HOST_DEVICE_DISCOVERY                                         0x0003
#define HID_HOST_START_CONNECTION                                         0x0004
#define HID_HOST_CONNECTED                                                0x0005
#define HID_HOST_DISCONNECTION                                            0x0006
#define HID_HOST_CONN_PARAM_UPDATE_REQ                                    0x0007
#define HID_HOST_SERVICE_DISCOVERY                                        0x0008
#define HID_HOST_GET_INCLUDED_SERVICES                                    0x0009
#define HID_HOST_WAIT_INCLUDED_SERVICES                                   0x000A
#define HID_HOST_HID_SERVICE_CHARAC_DISCOVERY                             0x000B
#define HID_HOST_WAIT_HID_SERVICE_CHARAC_DISCOVERY                        0x000C
#define HID_HOST_REPORT_MAP_CHARAC_DESC_DISCOVERY                         0x000D
#define HID_HOST_WAIT_REPORT_MAP_CHARAC_DESC_DISCOVERY                    0x000E
#define HID_HOST_REPORT_CHARAC_DESC_DISCOVERY                             0x000F
#define HID_HOST_WAIT_REPORT_CHARAC_DESC_DISCOVERY                        0x0010
#define HID_HOST_BOOT_KEYBOARD_REPORT_INPUT_CHARAC_DESC_DISCOVERY         0x0011
#define HID_HOST_WAIT_BOOT_KEYBOARD_REPORT_INPUT_CHARAC_DESC_DISCOVERY    0x0012
#define HID_HOST_BOOT_MOUSE_REPORT_INPUT_CHARAC_DESC_DISCOVERY            0x0013
#define HID_HOST_WAIT_BOOT_MOUSE_REPORT_INPUT_CHARAC_DESC_DISCOVERY       0x0014
#define HID_HOST_BATTERY_SERVICE_CHARAC_DISCOVERY                         0x0015
#define HID_HOST_WAIT_BATTERY_SERVICE_CHARAC_DISCOVERY                    0x0016
#define HID_HOST_BATTERY_LEVEL_CHARAC_DESC_DISCOVERY                      0x0017
#define HID_HOST_WAIT_BATTERY_LEVEL_CHARAC_DESC_DISCOVERY                 0x0018
#define HID_HOST_DEVICE_INFORMATION_SERVICE_CHARAC_DISCOVERY              0x0019
#define HID_HOST_WAIT_DEVICE_INFORMATION_SERVICE_CHARAC_DISCOVERY         0x001A
#define HID_HOST_READ_REPORT_MAP_DESCRIPTOR                               0x001B
#define HID_HOST_WAIT_READ_REPORT_MAP_DESCRIPTOR                          0x001C
#define HID_HOST_READ_REPORT_REFERENCE_VALUE                              0x001D
#define HID_HOST_WAIT_READ_REPORT_REFERENCE_VALUE                         0x001E
#define HID_HOST_READ_HID_INFORMATION_VALUE                               0x001F
#define HID_HOST_WAIT_READ_HID_INFORMATION_VALUE                          0x0020
#define HID_HOST_READ_BATTERY_LEVEL_CHARAC_PRESENTATION_FORMAT            0x0021
#define HID_HOST_WAIT_READ_BATTERY_LEVEL_CHARAC_PRESENTATION_FORMAT       0x0022
#define HID_HOST_READ_BATTERY_LEVEL_VALUE                                 0x0023
#define HID_HOST_WAIT_READ_BATTERY_LEVEL_VALUE                            0x0024
#define HID_HOST_READ_PNP_ID_CHARAC                                       0x0025
#define HID_HOST_WAIT_READ_PNP_ID_CHARAC                                  0x0026
#define HID_HOST_READ_PROTOCOL_MODE                                       0x0027
#define HID_HOST_WAIT_READ_PROTOCOL_MODE                                  0x0028
#define HID_HOST_WAIT_READ_REPORT                                         0x0029
#define HID_HOST_WAIT_READ_REPORT_CHARAC_DESC                             0x002A
#define HID_HOST_SCAN_PARAM_SERVICE_SCHARAC_DISCOVERY                     0x002B
#define HID_HOST_WAIT_SCAN_PARAM_SERVICE_SCHARAC_DISCOVERY                0x002C
#define HID_HOST_WRITE_SCAN_INTERVAL_WINDOW_VALUE                         0x002D
#define HID_HOST_SCAN_REFRESH_DESCRIPTOR                                  0x002E
#define HID_HOST_WAIT_SCAN_REFRESH_DESCRIPTOR                             0x002F
#define HID_HOST_SCAN_REFRESH_ENABLE_NOTIFICATION                         0x0030
#define HID_HOST_WAIT_SCAN_REFRESH_ENABLE_NOTIFICATION                    0x0031
#define HID_HOST_INPUT_REPORT_ENABLE_NOTIFICATION                         0x0032
#define HID_HOST_WAIT_INPUT_REPORT_ENABLE_NOTIFICATION                    0x0033
#define HID_HOST_BOOT_KEYBOARD_INPUT_ENABLE_NOTIFICATION                  0x0034
#define HID_HOST_WAIT_BOOT_KEYBOARD_INPUT_ENABLE_NOTIFICATION             0x0035
#define HID_HOST_BOOT_MOUSE_INPUT_ENABLE_NOTIFICATION                     0x0036
#define HID_HOST_WAIT_BOOT_MOUSE_INPUT_ENABLE_NOTIFICATION                0x0037
#define HID_HOST_RECONNECTION                                             0x0038

#define PRIMARY_SERVICES_ARRAY_SIZE 50
#define CHARAC_OF_SERVICE_ARRAY_SIZE 100
#define INCLUDED_SERVICES_ARRAY_SIZE 5
#define NUM_MAX_HID_SERVICES 5
#define NUM_MAX_HID_CHARAC_REPORT 7
#define NUM_MAX_EXTERNAL_REPORT_REFERENCE 5
#define NUM_MAX_BATTERY_SERVICES 5

/******************************************************************************
* type definitions
******************************************************************************/

/* Start & End handle */
typedef struct start_endS {
  uint16_t start;
  uint16_t end;
} start_end_handleType;

typedef struct reportCharacS {
  uint16_t start;
  uint16_t end;
  uint8_t properties;
  uint16_t clientCharacDescHandle;
  uint16_t reportReferenceHandle;
  uint8_t reportType;
  uint8_t reportID;
} reportCharacType;

typedef struct hidServiceS {
  uint8_t protocolModePresent;
  start_end_handleType protocolMode;
  uint8_t numReport;
  reportCharacType report[NUM_MAX_HID_CHARAC_REPORT];
  start_end_handleType reportMap;
  uint8_t bootKeyboardInputPresent;
  start_end_handleType bootKeyboardInput;
  uint8_t bootKeyboardOutputPresent;
  start_end_handleType bootKeyboardOutput;
  uint16_t bootKeyboardClientCharacDescHandle;
  uint8_t bootMouseInputPresent;
  start_end_handleType bootMouseInput;
  uint16_t bootMouseClientCharacDescHandle;
  start_end_handleType hidInformation;
  start_end_handleType hidControlPoint;
  uint8_t numExternalReport;
  uint16_t externalReportHandle[NUM_MAX_EXTERNAL_REPORT_REFERENCE];
} hidServiceType;

typedef struct batteryServiceS {
  uint16_t start;
  uint16_t end;
  uint8_t properties;
  uint16_t characPresentationFormat;
  uint16_t clientCharacDescHandle;
  uint16_t reportReferenceHandle;
  uint8_t nameSpace;
  uint16_t description;
} batteryServiceType;

typedef struct devInfServiceS {
  uint16_t pnpIdHandle;
} devInfServiceType;

typedef struct scanParameterServiceS {
  start_end_handleType scanIntervalWindow;
  start_end_handleType scanRefresh;
  uint16_t scanRefreshCharacDescHandle;
  uint8_t scanRefreshPresent;
  uint16_t le_scan_interval; 
  uint16_t le_scan_window; 
} scanParameterServiceType;

/* HID Host Context variable */
typedef struct hidContextS {
  uint8_t hostMode;
  uint32_t state;
  uint8_t public_addr[6];
  connDevType master_param;
  uint16_t connHandle;
  uint8_t numDeviceServices;
  uint8_t deviceServices[PRIMARY_SERVICES_ARRAY_SIZE];
  uint8_t numCharac;
  uint8_t charac[CHARAC_OF_SERVICE_ARRAY_SIZE];
  uint16_t uuid_searched;
  uint16_t characDesc_searched;
  uint8_t data[20];
  uint16_t dataLen;
  uint8_t fullConf;
  uint8_t fullConfEnded;
  uint8_t fullConfReconnection;
  uint16_t startHandle;
  uint16_t endHandle;
  uint8_t numIncludedServices;
  uint16_t includedServicesHandle[INCLUDED_SERVICES_ARRAY_SIZE];
  uint8_t hidPresent;
  uint8_t batteryPresent;
  uint8_t scanParamPresent;
  uint8_t otherServicePresent;
  uint8_t hidCharacDiscovered;
  uint8_t otherCharacDiscovered;
  uint8_t numReportDiscovered;
  hidServiceType hidService[NUM_MAX_HID_SERVICES];
  batteryServiceType batteryService[NUM_MAX_BATTERY_SERVICES];
  devInfServiceType devInfService;
  scanParameterServiceType scanParamService;
  uint8_t maxNmbReportDesc;
  uint16_t *reportDescLen;
  uint8_t *reportData;
  uint8_t *numReportDescPresent;
  uint16_t reportDataOffset;
  uint16_t maxReportDataSize;
  uint8_t infoDiscovered;
} hidContext_Type;

/******************************************************************************
* Variable Declarations
******************************************************************************/

/* HID Host context */
hidContext_Type hidContext;

/******************************************************************************
* Imported Variable
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*******************************************************************************
* Static Functions
*******************************************************************************/

static void runReadReportState(uint16_t state)
{
  uint8_t ret;
  uint16_t next_state, next_procedure, readUUID;

  switch (state) {
  case HID_HOST_READ_REPORT_REFERENCE_VALUE:
    {
      readUUID = REPORT_REFERENCE_DESCRIPTOR_UUID;
      next_state = HID_HOST_WAIT_READ_REPORT_REFERENCE_VALUE;
      next_procedure = HID_HOST_READ_HID_INFORMATION_VALUE;
    }
    break;
  }

  ret = HID_ReadReportValue(readUUID);
  if (ret != BLE_STATUS_SUCCESS) {
    hidContext.hidCharacDiscovered++;
    hidContext.infoDiscovered = 0;
    if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
      hidContext.state = state;
    } else {
      hidContext.hidCharacDiscovered = 0;
      hidContext.state = next_procedure;
    }
  } else {
    hidContext.state = next_state;
  } 
}

static void runReadReportReceivedState(uint16_t state, uint8_t dataLen, uint8_t *data)
{
  hidServiceType *hid;
  uint16_t next_state, next_procedure;

  hid = &hidContext.hidService[hidContext.hidCharacDiscovered];
  
  switch (state) {
  case HID_HOST_WAIT_READ_REPORT_REFERENCE_VALUE:
    {
      hid->report[hidContext.infoDiscovered].reportID = data[0];
      hid->report[hidContext.infoDiscovered].reportType = data[1];
      next_state = HID_HOST_READ_REPORT_REFERENCE_VALUE;
      next_procedure = HID_HOST_READ_HID_INFORMATION_VALUE;
    }
    break;
  }

  if (hidContext.fullConf) {
    if ((hidContext.infoDiscovered < hid->numReport)  &&  (hidContext.hidCharacDiscovered < hidContext.hidPresent)) {
      hidContext.infoDiscovered++;
      hidContext.state = next_state;
    } else {
      hidContext.hidCharacDiscovered = 0;
      hidContext.infoDiscovered = 0;
      hidContext.state = next_procedure;
    }
  } else {
    hidContext.state = HID_HOST_INITIALIZED;
    hidContext.infoDiscovered++;
    if (hidContext.infoDiscovered == hid->numReport) {
      hidContext.infoDiscovered = 0;
      hidContext.hidCharacDiscovered++;
      if (hidContext.hidCharacDiscovered == hidContext.hidPresent) 
        hidContext.hidCharacDiscovered = 0;
    }
  }
}

static void runReadHidInformationReceivedState(uint8_t status, uint8_t dataLen, uint8_t *data)
{
  uint16_t version;
  uint8_t countryCode, remoteWake, normallyConnectable;
  
  version = 0;
  countryCode = 0;
  remoteWake = 0;
  normallyConnectable = 0;
  if (status == BLE_STATUS_SUCCESS) {
    version = LE_TO_HOST_16(data);
    countryCode = data[2];
    remoteWake = data[3] & 0x01;
    normallyConnectable = (data[3] &0x02) >> 1;
  }
  HID_InformationData_CB(status, version, countryCode,
                         remoteWake, normallyConnectable);

  hidContext.hidCharacDiscovered++;
  if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
    hidContext.state = HID_HOST_READ_HID_INFORMATION_VALUE;
  } else {
    hidContext.hidCharacDiscovered = 0;
    hidContext.state = HID_HOST_READ_BATTERY_LEVEL_CHARAC_PRESENTATION_FORMAT;
  }

}

static void batteryCharac(uint16_t state, uint8_t status, uint8_t dataLen, uint8_t *data)
{
  batteryServiceType *battery;
  uint8_t next_state, next_procedure;

  battery = &hidContext.batteryService[hidContext.hidCharacDiscovered];
  switch (state) {
  case HID_HOST_WAIT_READ_BATTERY_LEVEL_CHARAC_PRESENTATION_FORMAT:
    battery->nameSpace = data[3];
    battery->description = LE_TO_HOST_16(&data[4]);
    next_state = HID_HOST_READ_BATTERY_LEVEL_CHARAC_PRESENTATION_FORMAT;
    next_procedure = HID_HOST_READ_BATTERY_LEVEL_VALUE;
    break;
  case HID_HOST_WAIT_READ_BATTERY_LEVEL_VALUE:    
    HID_BetteryLevelData_CB(status, battery->nameSpace, battery->description, data[0]);
    next_state = HID_HOST_READ_BATTERY_LEVEL_VALUE;
    next_procedure = HID_HOST_READ_PNP_ID_CHARAC;
    break;
  }

  hidContext.hidCharacDiscovered++;
  if (hidContext.hidCharacDiscovered < hidContext.batteryPresent) {
    hidContext.state = next_state;
  } else {
    hidContext.hidCharacDiscovered = 0;
    hidContext.state = next_procedure;
  }
}

static void pnpIdCharac(uint8_t status, uint8_t dataLen, uint8_t *data)
{
  uint8_t vendorIdSource;
  uint16_t vendorId, productId, productVersion;
  
  vendorIdSource = data[0];
  vendorId = LE_TO_HOST_16(&data[1]);
  productId = LE_TO_HOST_16(&data[3]);
  productVersion = LE_TO_HOST_16(&data[5]);
  HID_PnPID_CB(status, vendorIdSource, vendorId, productId, productVersion);

  hidContext.state = HID_HOST_READ_PROTOCOL_MODE;
  hidContext.hidCharacDiscovered = 0;
}

static void readReport(uint8_t status, uint8_t dataLen, uint8_t *data)
{
  switch (hidContext.uuid_searched) {
  case BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID:
  case BOOT_KEYBOARD_OUTPUT_REPORT_CHAR_UUID:
  case BOOT_MOUSE_INPUT_REPORT_CHAR_UUID:
    HID_BootReportValue_CB(status, dataLen, data);
    break;
  }
  hidContext.state = HID_HOST_CONNECTED_IDLE;
  hidContext.hidCharacDiscovered = 0;

}

static void readCharacDesc(uint8_t status, uint8_t dataLen, uint8_t *data)
{
  switch (hidContext.uuid_searched) {
  case BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID:
  case BOOT_MOUSE_INPUT_REPORT_CHAR_UUID:
    HID_ReadBootReportClientCharacDesc_CB(status, data[0], data[1]);
    break;
  }

  hidContext.state = HID_HOST_CONNECTED_IDLE;
  hidContext.hidCharacDiscovered = 0;
}


static uint8_t verifyServices(void)
{
  uint8_t i, index, deviceInfPresent;
  uint16_t uuid;

  hidContext.hidPresent = 0;
  hidContext.batteryPresent = 0;
  deviceInfPresent = 0;
  hidContext.scanParamPresent = 0;
  hidContext.otherServicePresent = 0;
  hidContext.hidCharacDiscovered = 0;
  hidContext.otherCharacDiscovered = 0;
  hidContext.numIncludedServices = 0;
  hidContext.numReportDiscovered = 0;

  index = 0;
  for (i=0; i<hidContext.numDeviceServices; i++) {
    if (hidContext.deviceServices[index] == UUID_TYPE_16) {
      uuid = LE_TO_HOST_16(&hidContext.deviceServices[index+5]);
      switch (uuid) {
      case HUMAN_INTERFACE_DEVICE_SERVICE_UUID:
	hidContext.hidPresent++;
	break;
      case BATTERY_SERVICE_SERVICE_UUID:
	hidContext.batteryPresent++;
	break;
      case DEVICE_INFORMATION_SERVICE_UUID:
	deviceInfPresent++;
	break;
      case SCAN_PARAMETER_SERVICE_UUID:
	hidContext.scanParamPresent++;
        break;
      default:
	hidContext.otherServicePresent++;
      }
    }
    if (hidContext.deviceServices[index] == UUID_TYPE_16) {
      index += 7;
    } else {
      index += 21;
    }
  }

  if ((hidContext.hidPresent > NUM_MAX_HID_SERVICES) || 
      (hidContext.batteryPresent > NUM_MAX_BATTERY_SERVICES))
    return FALSE;

  if (hidContext.hidPresent && hidContext.batteryPresent && deviceInfPresent)
    return TRUE;
  else
    return FALSE;
}

static uint8_t findHandles(uint16_t uuid_service, uint16_t *start_handle, 
			   uint16_t *end_handle)
{
  uint8_t i, j, uuid_num, found;
  uint16_t uuid;

  j = 0;
  found = FALSE;
  uuid_num = 0;
  for (i=0; i<hidContext.numDeviceServices; i++) {
    if (hidContext.deviceServices[j] == UUID_TYPE_16) {
      uuid = LE_TO_HOST_16(&hidContext.deviceServices[j+5]);
      j++;
      *start_handle = LE_TO_HOST_16(&hidContext.deviceServices[j]);
      j += 2;
      *end_handle = LE_TO_HOST_16(&hidContext.deviceServices[j]);
      j += 2;
      if (hidContext.deviceServices[j-5] == UUID_TYPE_16) {
	j += 2;
      } else {
	j += 16;
      }
      if (uuid == uuid_service) {
	switch (uuid_service) {
	case HUMAN_INTERFACE_DEVICE_SERVICE_UUID:
	  if (uuid_num == hidContext. hidCharacDiscovered) {
	    hidContext. hidCharacDiscovered++;
	    found = TRUE;
	  } else {
	    uuid_num++;
	  }
	  break;
	case BATTERY_SERVICE_SERVICE_UUID:
	  if (uuid_num ==  hidContext.hidCharacDiscovered) {
	    hidContext.hidCharacDiscovered++;
	    found = TRUE;
	  } else {
	    uuid_num++;
	  }
	  break;
	case DEVICE_INFORMATION_SERVICE_UUID:	  
	  found = TRUE;
	  break;
	case SCAN_PARAMETER_SERVICE_UUID:
	  found = TRUE;
	  break;
	default:
	  if (uuid_num == hidContext.otherCharacDiscovered) {
	    hidContext.otherCharacDiscovered++;
	    found = TRUE;
	  } else {
	    uuid_num++;
	  }
	}
	if (found) {
	  return BLE_STATUS_SUCCESS;
	}
      }
    }
  }
  
  return HCI_INVALID_PARAMETERS;
}

static void findIncludedHandle(void)
{
  uint8_t i, index, numAlreadyInc;
  uint16_t uuid, handle;

  numAlreadyInc = hidContext.numIncludedServices;
  index = 0;

  if (numAlreadyInc == INCLUDED_SERVICES_ARRAY_SIZE)
    return;

  for (i=0; i<hidContext.numCharac; i++) {
    if (hidContext.charac[index] == 8) {
      uuid = LE_TO_HOST_16(&hidContext.charac[index+7]);
      if (uuid == BATTERY_SERVICE_SERVICE_UUID) {
        handle = LE_TO_HOST_16(&hidContext.charac[index+1]);
	hidContext.includedServicesHandle[i+numAlreadyInc] = handle;
	hidContext.numIncludedServices++;
	if (hidContext.numIncludedServices ==  INCLUDED_SERVICES_ARRAY_SIZE)
	  break;
      } 
      index += 9;
    } else {
      index += 7;
    }
  }
}

static uint8_t findCharacHandle(uint16_t uuid_searched, uint8_t numCharac, uint8_t *charac)
{
  uint8_t ret, i, index, sizeElement, properties;
  uint16_t value_handle, uuid_charac, end_handle;
  hidServiceType *hid;
  batteryServiceType *battery;

  if (uuid_searched == HUMAN_INTERFACE_DEVICE_SERVICE_UUID) {
    hid = &hidContext.hidService[hidContext.hidCharacDiscovered-1];
    hid->numReport = 0;
    hid->numExternalReport = 0;
    hid->bootKeyboardInputPresent = 0;
    hid->bootKeyboardOutputPresent = 0;
    hid->bootMouseInputPresent = 0;
    hid->protocolModePresent = 0;
  }

  if (uuid_searched == BATTERY_SERVICE_SERVICE_UUID) {
    battery = &hidContext.batteryService[hidContext.hidCharacDiscovered-1];
  }

  ret = BLE_STATUS_SUCCESS;
  
  index = 0;
  for (i=0; i<numCharac; i++) { 
    if (charac[index] == 1) {
      sizeElement = 8;
      uuid_charac = LE_TO_HOST_16(&charac[index+6]);
    } else {
      sizeElement = 22;
    }

    properties = charac[index+3];
    value_handle = LE_TO_HOST_16(&charac[index+4]);
    index += sizeElement;
    if (i != (numCharac-1)) {
      end_handle = LE_TO_HOST_16(&charac[index+1]);
      end_handle--;
    } else {
      end_handle = hidContext.endHandle;
    }

    switch(uuid_charac) {
    case PROTOCOL_MODE_CHAR_UUID:
      hid->protocolModePresent = 1;
      hid->protocolMode.start = value_handle;
      hid->protocolMode.end = end_handle;
      break;
    case REPORT_CHAR_UUID:
      hid->report[hid->numReport].start = value_handle;
      hid->report[hid->numReport].end = end_handle;
      hid->report[hid->numReport].properties = properties;
      hid->numReport++;
      break;
    case REPORT_MAP_CHAR_UUID:
      hid->reportMap.start = value_handle;
      hid->reportMap.end = end_handle;
      break;
    case BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID:
      hid->bootKeyboardInputPresent = 1;
      hid->bootKeyboardInput.start = value_handle;
      hid->bootKeyboardInput.end = end_handle;
      break;
    case BOOT_KEYBOARD_OUTPUT_REPORT_CHAR_UUID:
      hid->bootKeyboardOutputPresent = 1;
      hid->bootKeyboardOutput.start = value_handle;
      hid->bootKeyboardOutput.end = end_handle;
      break;
    case BOOT_MOUSE_INPUT_REPORT_CHAR_UUID:
      hid->bootMouseInputPresent = 1;
      hid->bootMouseInput.start = value_handle;
      hid->bootMouseInput.end = end_handle;
      break;
    case HID_INFORMATION_CHAR_UUID:
      hid->hidInformation.start = value_handle;
      hid->hidInformation.end = end_handle;
      break;
    case HID_CONTROL_POINT_CHAR_UUID:
      hid->hidControlPoint.start = value_handle;
      hid->hidControlPoint.end = end_handle;
      break;
    case BATTERY_LEVEL_CHAR_UUID:
      battery->start = value_handle;
      battery->end = end_handle;
      battery->properties = properties;
      break;
    case PNP_ID_UUID:
      hidContext.devInfService.pnpIdHandle = value_handle;
      break;
    case SCAN_INTERVAL_WINDOW_CHAR_UUID:
      hidContext.scanParamService.scanIntervalWindow.start = value_handle;
      hidContext.scanParamService.scanIntervalWindow.end = end_handle;
      break;
    case SCAN_REFRESH_CHAR_UUID:
      hidContext.scanParamService.scanRefreshPresent = 1;
      hidContext.scanParamService.scanRefresh.start = value_handle;
      hidContext.scanParamService.scanRefresh.end = end_handle;
      break;
    default:
      // To add a state ???
      break;
    }
  }

  return ret ;
}

static void findCharacDesc(uint8_t numCharac, uint8_t *charac)
{
  uint8_t i, index, sizeElement;
  uint16_t handle_characDesc, uuid_characDesc;
  hidServiceType *hid;
  batteryServiceType *battery;

  if (hidContext.uuid_searched == HUMAN_INTERFACE_DEVICE_SERVICE_UUID) {
    hid = &hidContext.hidService[hidContext.hidCharacDiscovered];
  }

  if (hidContext.uuid_searched == BATTERY_SERVICE_SERVICE_UUID) {
    battery = &hidContext.batteryService[hidContext.hidCharacDiscovered];
  }

  index = 0;
  for (i=0; i<numCharac; i++) { 
    if (charac[index] == 1) {
      sizeElement = 5;
      uuid_characDesc = LE_TO_HOST_16(&charac[index+3]);
    } else {
      sizeElement = 19;
      uuid_characDesc = 0;
    }

    handle_characDesc = LE_TO_HOST_16(&charac[index+1]);

    switch(uuid_characDesc) {
    case EXTERNAL_REPORT_REFERENCE_DESCRIPTOR_UUID:
      hid->externalReportHandle[hid->numExternalReport] = handle_characDesc;
      hid->numExternalReport++;
      break;
    case REPORT_REFERENCE_DESCRIPTOR_UUID:
      if (hidContext.uuid_searched == HUMAN_INTERFACE_DEVICE_SERVICE_UUID) 
	hid->report[hidContext.numReportDiscovered].reportReferenceHandle = handle_characDesc;
      if (hidContext.uuid_searched == BATTERY_SERVICE_SERVICE_UUID)
	battery->reportReferenceHandle = handle_characDesc;
      break;
    case CHAR_PRESENTATION_FORMAT_DESCRIPTOR_UUID:
      if (hidContext.uuid_searched == BATTERY_SERVICE_SERVICE_UUID) {
	battery->characPresentationFormat = handle_characDesc;
        battery->nameSpace = 0;
        battery->description = 0;
      }
      break;
    case CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID:
      switch (hidContext.characDesc_searched) {
      case REPORT_CHAR_UUID:
	hid->report[hidContext.numReportDiscovered].clientCharacDescHandle = handle_characDesc;
	break;
      case BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID:
	hid->bootKeyboardClientCharacDescHandle = handle_characDesc;
	break;
      case BOOT_MOUSE_INPUT_REPORT_CHAR_UUID:
	hid->bootMouseClientCharacDescHandle = handle_characDesc;
	break;
      case BATTERY_LEVEL_CHAR_UUID:
	battery->clientCharacDescHandle = handle_characDesc;
	break;
      case SCAN_REFRESH_CHAR_UUID:
        hidContext.scanParamService.scanRefreshCharacDescHandle = handle_characDesc;
      }
      break;
    default:
      // To add ???
      break;
    }

    index += sizeElement;
  }
}

static uint8_t findNotificationSource(uint16_t attr_handle, uint8_t *type, uint8_t *id)
{
  uint8_t i, j;
  hidServiceType *hid;

  for (i=0; i<hidContext.hidPresent; i++) {
    hid = &hidContext.hidService[i];
    if (hid->bootKeyboardInputPresent && (hid->bootKeyboardInput.start == attr_handle)) {
      *type = BOOT_KEYBOARD_INPUT_REPORT;
      if (hidContext.hostMode == BOOT_PROTOCOL_MODE)
        return TRUE;
      else 
        return FALSE;
    }
    if (hid->bootMouseInputPresent && (hid->bootMouseInput.start == attr_handle)) {
      *type = BOOT_MOUSE_INPUT_REPORT;
      if (hidContext.hostMode == BOOT_PROTOCOL_MODE)
        return TRUE;
      else
        return FALSE;
    }
    for (j=0; j<hid->numReport; j++) {
      if (hid->report[j].start == attr_handle) {
        *type = hid->report[j].reportType;
        *id = hid->report[j].reportID;
        if (hidContext.hostMode == REPORT_PROTOCOL_MODE)
          return TRUE;
        else
          return FALSE;
      }
    }
  }
  return FALSE;
}

/*******************************************************************************
* exported SAP
*******************************************************************************/

uint8_t HID_Init(hidInitDevType param)
{
  initDevType initParam;
  uint8_t ret;

  memcpy(hidContext.public_addr, param.public_addr, 6);
  memcpy(initParam.public_addr, param.public_addr, 6);
  initParam.txPowerLevel = param.txPower;
  initParam.device_name_len = param.device_name_len;
  initParam.device_name = param.device_name;
  initParam.BLE_HCI_Event_CB = NULL;
  ret = Master_Init(&initParam);
  if (ret == BLE_STATUS_SUCCESS) {
    hidContext.state = HID_HOST_INITIALIZED;
    hidContext.fullConf = FALSE;
    hidContext.hostMode = param.hostMode;
    PROFILE_MESG_DBG(profiledbgfile,"**** Device initialized\r\n");
  } else {
    hidContext.state = HID_HOST_UNINITIALIZED;
    PROFILE_MESG_DBG(profiledbgfile,"**** Device not initialized\r\n");
  }

  return ret;
}

uint8_t HID_SecuritySet(hidSecurityType param)
{
  securitySetType secParam;
  uint8_t ret;

  secParam.ioCapability = param.ioCapability;
  secParam.mitm_mode = param.mitm_mode;
  secParam.oob_enable = param.oob_enable;
  if (secParam.oob_enable == OOB_AUTH_DATA_PRESENT)
    memcpy(secParam.oob_data, param.oob_data, 16);
  secParam.use_fixed_pin = param.use_fixed_pin;
  if (secParam.use_fixed_pin == USE_FIXED_PIN_FOR_PAIRING)
    secParam.fixed_pin = param.fixed_pin;
  else 
    secParam.fixed_pin = 0;
  secParam.bonding_mode = param.bonding_mode;
  ret = Master_SecuritySet(&secParam);
  if (ret != BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Set Security failed\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Set Security success\r\n");
  }

  return ret;
}

uint8_t HID_DeviceDiscovery(hidDevDiscType param)
{
  devDiscoveryType devDiscParam;
  uint8_t ret;
  
  devDiscParam.procedure = LIMITED_DISCOVERY_PROCEDURE;
  devDiscParam.scanInterval = param.scanInterval;
  devDiscParam.scanWindow = param.scanWindow;
  devDiscParam.own_address_type = param.own_addr_type;
  ret = Master_DeviceDiscovery(&devDiscParam);
  if (ret != BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure failed\n");
  } else {
    hidContext.state = HID_HOST_DEVICE_DISCOVERY;
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure started\n");
  }
  return ret;
}

uint8_t HID_DeviceConnection(hidConnDevType param)
{
  uint8_t ret;
  
  hidContext.master_param.procedure = DIRECT_CONNECTION_PROCEDURE;
  hidContext.master_param.fastScanDuration = param.fastScanDuration;
  hidContext.master_param.fastScanInterval = param.fastScanInterval;
  hidContext.master_param.fastScanWindow = param.fastScanWindow;
  hidContext.master_param.reducedPowerScanInterval = param.reducedPowerScanInterval;
  hidContext.master_param.reducedPowerScanWindow = param.reducedPowerScanWindow;
  hidContext.master_param.peer_addr_type = param.peer_addr_type;
  memcpy(hidContext.master_param.peer_addr, param.peer_addr, 6);
  hidContext.master_param.own_addr_type = param.own_addr_type;
  hidContext.master_param.conn_min_interval = param.conn_min_interval;
  hidContext.master_param.conn_max_interval = param.conn_max_interval;
  hidContext.master_param.conn_latency = param.conn_latency;
  hidContext.master_param.supervision_timeout = param.supervision_timeout;
  hidContext.master_param.min_conn_length = param.min_conn_length;
  hidContext.master_param.max_conn_length = param.max_conn_length;
  ret = Master_DeviceConnection(&hidContext.master_param);
  if (ret == BLE_STATUS_SUCCESS) {
    hidContext.scanParamService.le_scan_interval = param.fastScanInterval;
    hidContext.scanParamService.le_scan_window = param.fastScanWindow;
    hidContext.state = HID_HOST_START_CONNECTION;
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Connection success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Connection failed\r\n");
  }

  return ret;
}

uint8_t HID_DeviceDisconnection(void)
{
  uint8_t ret;

  ret = Master_CloseConnection(hidContext.connHandle);
  if (ret == BLE_STATUS_SUCCESS) {    
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Disconnection success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Device Disconnection failed\r\n");
  }
  return ret;
}

uint8_t HID_ConnectionParameterUpdateRsp(uint8_t accepted, hidConnUpdateParamType *param)
{
  uint8_t ret;

  ret = Master_ConnectionUpdateParamResponse(hidContext.connHandle, 
					     accepted, (connUpdateParamType*)param);
  if (ret == BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Connection Update Parameter Response success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Connection Update Parameter Response failed\r\n");
  }

  return ret;
}

uint8_t HID_StartPairing(void)
{
  uint8_t ret;

  ret = Master_Start_Pairing_Procedure(hidContext.connHandle, FALSE);
  if (ret == BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Pairing Procedure success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Pairing Procedure failed\r\n");
  }
  return ret;
}

uint8_t HID_ClearBondedDevices(void)
{
  uint8_t ret;

  ret = Master_ClearSecurityDatabase();
  if (ret == BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Clear Security Database success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Clear Security Database failed\r\n");
  }
  return ret;
}

uint8_t HID_SendPinCode(uint32_t pinCode)
{
  uint8_t ret;

  ret = Master_Passkey_Response(hidContext.connHandle, pinCode);
  
  return ret;
}

uint8_t HID_ServicesDiscovery(void)
{
  uint8_t ret;

  ret = Master_GetPrimaryServices(hidContext.connHandle, 
				  &hidContext.numDeviceServices, 
				  hidContext.deviceServices, 
				  PRIMARY_SERVICES_ARRAY_SIZE);
  if (ret != BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Service Discovery Procedure failed\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Service Discovery Procedure success\r\n");
    hidContext.state = HID_HOST_SERVICE_DISCOVERY;
  }

  return ret;
}

uint8_t HID_GetIncludedBatterySerivces(void)
{
  uint8_t ret;

  findHandles(HUMAN_INTERFACE_DEVICE_SERVICE_UUID, &hidContext.startHandle, &hidContext.endHandle);
  ret = Master_GetIncludeServices(hidContext.connHandle, hidContext.startHandle, hidContext.endHandle,
				  &hidContext.numCharac, hidContext.charac, CHARAC_OF_SERVICE_ARRAY_SIZE);
  return ret;
}

uint8_t HID_DiscCharacServ(uint16_t uuid_service)
{
  uint8_t ret;

  ret = findHandles(uuid_service, &hidContext.startHandle, &hidContext.endHandle);
  if (ret == BLE_STATUS_SUCCESS) {
    ret = Master_GetCharacOfService(hidContext.connHandle,
				    hidContext.startHandle,
				    hidContext.endHandle,
				    &hidContext.numCharac,
				    hidContext.charac,
				    CHARAC_OF_SERVICE_ARRAY_SIZE);

    if (ret == BLE_STATUS_SUCCESS) {
      hidContext.uuid_searched = uuid_service;
      PROFILE_MESG_DBG(profiledbgfile,"**** Discovery All Characteristic of a Service success\r\n");
    } else {
      PROFILE_MESG_DBG(profiledbgfile,"**** Discovery All Characteristic of a Service failed\r\n");
    }
  }

  return ret;
}

uint8_t HID_DiscCharacDesc(uint16_t uuid_charac)
{
  uint8_t ret;
  hidServiceType *hid;
  batteryServiceType *battery;
  uint16_t start, end, uuid_service;

  ret = BLE_STATUS_SUCCESS;
  switch (uuid_charac) {
  case REPORT_MAP_CHAR_UUID:
    hid = &hidContext.hidService[hidContext.hidCharacDiscovered];
    uuid_service = HUMAN_INTERFACE_DEVICE_SERVICE_UUID;
    start = hid->reportMap.start;
    end = hid->reportMap.end; 
    break;
  case REPORT_CHAR_UUID:
    hid = &hidContext.hidService[hidContext.hidCharacDiscovered];
    uuid_service = HUMAN_INTERFACE_DEVICE_SERVICE_UUID;
    start  = hid->report[hidContext.numReportDiscovered].start;
    end = hid->report[hidContext.numReportDiscovered].end;
    break;
  case BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID:
    hid = &hidContext.hidService[hidContext.hidCharacDiscovered];
    uuid_service = HUMAN_INTERFACE_DEVICE_SERVICE_UUID;
    start = hid->bootKeyboardInput.start;
    end = hid->bootKeyboardInput.end;
    break;
  case BOOT_MOUSE_INPUT_REPORT_CHAR_UUID:
    hid = &hidContext.hidService[hidContext.hidCharacDiscovered];
    uuid_service = HUMAN_INTERFACE_DEVICE_SERVICE_UUID;
    start = hid->bootMouseInput.start;
    end = hid->bootMouseInput.end;
    break;
  case BATTERY_LEVEL_CHAR_UUID:
    battery = &hidContext.batteryService[hidContext.hidCharacDiscovered];
    uuid_service = BATTERY_SERVICE_SERVICE_UUID;
    start = battery->start;
    end = battery->end;
    break;
  case SCAN_REFRESH_CHAR_UUID:
    if (hidContext.scanParamService.scanRefreshPresent) {
      uuid_service = SCAN_PARAMETER_SERVICE_UUID;
      start = hidContext.scanParamService.scanRefresh.start;
      end = hidContext.scanParamService.scanRefresh.end;
    } else {
      ret = HCI_INVALID_PARAMETERS;
    }
    break;
  default:
    ret = HCI_INVALID_PARAMETERS;
  }

  if (ret == BLE_STATUS_SUCCESS) {
    ret = Master_GetCharacDescriptors(hidContext.connHandle, 
				      start,
				      end,
				      &hidContext.numCharac, 
				      hidContext.charac, 
				      CHARAC_OF_SERVICE_ARRAY_SIZE);
  }
  if (ret == BLE_STATUS_SUCCESS) {
    hidContext.uuid_searched = uuid_service;
    hidContext.characDesc_searched = uuid_charac;
    PROFILE_MESG_DBG(profiledbgfile,"**** Discovery All Characteristic Descriptor success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Discovery All Characteristic Descriptor failed\r\n");
  }

  return ret;
}

uint8_t HID_NumberOfReportDescriptor(void)
{
  return hidContext.hidPresent;
}

uint8_t HID_ReadReportDescriptor(uint8_t reportToRead, uint16_t *reportDataLen, uint8_t *reportData, uint16_t maxSize)
{
  uint8_t ret;
  hidServiceType *hid;
  
  if ((reportToRead > hidContext.hidPresent) ||  
      (reportToRead == 0)) {
    ret = HCI_INVALID_PARAMETERS;
  } else {
    hid = &hidContext.hidService[reportToRead-1];
    hidContext.startHandle =  hid->reportMap.start;
    ret = Master_Read_Long_Value(hidContext.connHandle, hid->reportMap.start, 
				 reportDataLen, reportData, 
				 0, maxSize);
  }

  return ret;
}

uint8_t HID_ReadReportValue(uint16_t characToRead)
{
  uint8_t ret, ableToRead, numReport;
  hidServiceType *hid;

  ableToRead = FALSE;
  hid = &hidContext.hidService[hidContext.hidCharacDiscovered];

  do {
    switch (characToRead) {
    case EXTERNAL_REPORT_REFERENCE_DESCRIPTOR_UUID:
      {
        numReport = hid->numExternalReport;
        if (hidContext.infoDiscovered < hid->numExternalReport) {
          hidContext.startHandle = hid->externalReportHandle[hidContext.infoDiscovered];
          ableToRead = TRUE;
        }
      }
      break;
    case REPORT_REFERENCE_DESCRIPTOR_UUID:
      {
        numReport = hid->numReport;
        if (hidContext.infoDiscovered < hid->numReport) {
          hidContext.startHandle = hid->report[hidContext.infoDiscovered].reportReferenceHandle;
          ableToRead = TRUE;
        }
      }
      break;
    case CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID:
      {
        numReport = hid->numReport;
        if (hidContext.infoDiscovered < hid->numReport) {
          if (hid->report[hidContext.infoDiscovered].reportType == INPUT_REPORT) {
            hidContext.startHandle = hid->report[hidContext.infoDiscovered].clientCharacDescHandle;
            ableToRead = TRUE;
          }
        }
      }
      break;
    case INPUT_REPORT:
    case OUTPUT_REPORT:
    case FEATURE_REPORT:
      {
        numReport = hid->numReport;
        if (hidContext.infoDiscovered < hid->numReport) {
          if (hid->report[hidContext.infoDiscovered].reportType == characToRead) {
            hidContext.startHandle = hid->report[hidContext.infoDiscovered].start;
            ableToRead = TRUE;
          }
        }
      }
      break;
    }
    if (!ableToRead)
      hidContext.infoDiscovered++;
  } while (!ableToRead && (hidContext.infoDiscovered < numReport));

  if (!hidContext.fullConf) {  
    if (characToRead == REPORT_REFERENCE_DESCRIPTOR_UUID) {
      hidContext.state = HID_HOST_WAIT_READ_REPORT_REFERENCE_VALUE;
    } else {
      hidContext.infoDiscovered++;
      if (hidContext.infoDiscovered >= numReport) {
        hidContext.hidCharacDiscovered++;
        hidContext.infoDiscovered = 0;
        if (hidContext.hidCharacDiscovered == hidContext.hidPresent) 
          hidContext.hidCharacDiscovered = 0; 
      }
    }
  }

  if(ableToRead) {
    ret = Master_Read_Value(hidContext.connHandle, hidContext.startHandle, 
                            &hidContext.dataLen, hidContext.data, CHARAC_OF_SERVICE_ARRAY_SIZE);
  } else {
    ret = BLE_STATUS_ERROR;
  }

  return ret;  
}

uint8_t HID_ReadHidInformation(void)
{
  uint8_t ret;
  hidServiceType *hid;
  
  hid = &hidContext.hidService[hidContext.hidCharacDiscovered];
  ret = Master_Read_Value(hidContext.connHandle, hid->hidInformation.start, 
                          &hidContext.dataLen, hidContext.data, CHARAC_OF_SERVICE_ARRAY_SIZE);
  if (ret == BLE_STATUS_SUCCESS) {
    hidContext.startHandle = hid->hidInformation.start;
    hidContext.uuid_searched = HID_INFORMATION_CHAR_UUID;
    hidContext.state = HID_HOST_WAIT_READ_HID_INFORMATION_VALUE;
    PROFILE_MESG_DBG(profiledbgfile,"**** Read HID Information value success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Read HID Information value failed\r\n");
  }
  
  return ret;
}

uint8_t HID_ReadBatteryLevel(void)
{
  uint8_t ret;
  batteryServiceType *battery;

  battery = &hidContext.batteryService[hidContext.hidCharacDiscovered];
  ret = Master_Read_Value(hidContext.connHandle, battery->start, 
                          &hidContext.dataLen, hidContext.data, CHARAC_OF_SERVICE_ARRAY_SIZE);
  if (ret == BLE_STATUS_SUCCESS) {
    hidContext.startHandle = battery->start;
    hidContext.state = HID_HOST_WAIT_READ_BATTERY_LEVEL_VALUE;
    PROFILE_MESG_DBG(profiledbgfile,"**** Read Battery Level value success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Read Battery Level value failed\r\n");
  }
  
  return ret;
}

uint8_t HID_ReadBatteryClientCharacDesc(void)
{
  uint8_t ret;
  batteryServiceType *battery;

  battery = &hidContext.batteryService[hidContext.hidCharacDiscovered];
  ret = Master_Read_Value(hidContext.connHandle, battery->clientCharacDescHandle, 
                          &hidContext.dataLen, hidContext.data, CHARAC_OF_SERVICE_ARRAY_SIZE);
  if (ret == BLE_STATUS_SUCCESS) {
    hidContext.startHandle = battery->clientCharacDescHandle;
    hidContext.state = HID_HOST_CONNECTED_IDLE;
    PROFILE_MESG_DBG(profiledbgfile,"**** Read Battery Level Client Charac Desc value success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Read Battery Level Client Charac Desc value failed\r\n");
  }
 
  return ret;
}

uint8_t HID_ReadPnPID(void)
{
  uint8_t ret;

  ret = Master_Read_Value(hidContext.connHandle, hidContext.devInfService.pnpIdHandle, 
                          &hidContext.dataLen, hidContext.data, CHARAC_OF_SERVICE_ARRAY_SIZE);
  if (ret == BLE_STATUS_SUCCESS) {
    hidContext.startHandle = hidContext.devInfService.pnpIdHandle;
    hidContext.state = HID_HOST_WAIT_READ_PNP_ID_CHARAC;
    PROFILE_MESG_DBG(profiledbgfile,"**** Read PnP ID value success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Read PnP ID value failed\r\n");
  }  
  
  return ret;
}

uint8_t HID_ReadBootReport(uint16_t bootReportUUID)
{
  uint8_t ret, readyToRead, found;
  uint16_t handle;
  hidServiceType *hid;
  
  readyToRead = FALSE;
  found = FALSE;
  hidContext.hidCharacDiscovered = 0;
  do {
    hid =  &hidContext.hidService[hidContext.hidCharacDiscovered];
    switch (bootReportUUID) {
    case BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID:
      {
        if (hid->bootKeyboardInputPresent) {
          readyToRead = TRUE;
          handle = hid->bootKeyboardInput.start;
          hidContext.uuid_searched = BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID;
        }
      }
      break;
    case BOOT_KEYBOARD_OUTPUT_REPORT_CHAR_UUID:
      {
        if (hid->bootKeyboardOutputPresent) {
          readyToRead = TRUE;
          handle = hid->bootKeyboardOutput.start;
          hidContext.uuid_searched = BOOT_KEYBOARD_OUTPUT_REPORT_CHAR_UUID;
        }
      }
      break;
    case  BOOT_MOUSE_INPUT_REPORT_CHAR_UUID:
      {
        if (hid->bootMouseInputPresent) {
          readyToRead = TRUE;
          handle = hid->bootMouseInput.start;
          hidContext.uuid_searched = BOOT_MOUSE_INPUT_REPORT_CHAR_UUID; 
        }
      }
      break;
    }
    if (readyToRead) {
      ret = Master_Read_Value(hidContext.connHandle, handle, 
                              &hidContext.dataLen, hidContext.data, CHARAC_OF_SERVICE_ARRAY_SIZE);
      found = TRUE;
      if (ret == BLE_STATUS_SUCCESS) {
        hidContext.startHandle = handle;
        hidContext.state = HID_HOST_WAIT_READ_REPORT;
        PROFILE_MESG_DBG(profiledbgfile,"**** Read Boot Report value success\r\n");
      } else {
        PROFILE_MESG_DBG(profiledbgfile,"**** Read Boot Report value failed\r\n");
      }  
    } else {
      hidContext.hidCharacDiscovered++;
    }
  } while (!found && (hidContext.hidCharacDiscovered < hidContext.hidPresent));

  if (!readyToRead) {
    ret = HID_ERROR_BOOT_REPORT_NOT_PRESENT;
  }

  return ret;
}

uint8_t HID_ReadBootReportClientCharacDesc(uint16_t bootReportUUID)
{
  uint8_t ret, readyToRead, found;
  uint16_t handle;
  hidServiceType *hid;
  
  readyToRead = FALSE;
  found = FALSE;
  hidContext.hidCharacDiscovered = 0;
  do {
    hid =  &hidContext.hidService[hidContext.hidCharacDiscovered];
    switch (bootReportUUID) {
    case BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID:
      {
        if (hid->bootKeyboardInputPresent) {
          readyToRead = TRUE;
          handle = hid->bootKeyboardClientCharacDescHandle;
          hidContext.uuid_searched = BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID;
        }
      }
      break;
      case BOOT_MOUSE_INPUT_REPORT_CHAR_UUID:
      {
        if (hid->bootMouseInputPresent) {
          readyToRead = TRUE;
          handle = hid->bootMouseClientCharacDescHandle;
          hidContext.uuid_searched = BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID;          
        }
      }
      break;
    }
    if (readyToRead) {
      ret = Master_Read_Value(hidContext.connHandle, handle, 
                              &hidContext.dataLen, hidContext.data, CHARAC_OF_SERVICE_ARRAY_SIZE);
      found = TRUE;
      if (ret == BLE_STATUS_SUCCESS) {
        hidContext.startHandle = handle;
        hidContext.state = HID_HOST_WAIT_READ_REPORT_CHARAC_DESC;
        PROFILE_MESG_DBG(profiledbgfile,"**** Read Boot Report Client Characteristic Config Descriptor value success\r\n");
      } else {
        PROFILE_MESG_DBG(profiledbgfile,"**** Read Boot Report Client Characteristic Config Descriptor value failed\r\n");
      }  
    } else {
      hidContext.hidCharacDiscovered++;
    }
  } while (!found && (hidContext.hidCharacDiscovered < hidContext.hidPresent));

  if (!readyToRead) {
    ret = HID_ERROR_BOOT_REPORT_NOT_PRESENT;
  }

  return ret;
}

uint8_t HID_SetNotificationStatus(uint8_t type,  uint8_t enabled)
{
  uint8_t ret, reportFound;
  hidServiceType *hid;

  ret = HID_ERROR_REPORT_NOT_PRESENT;
  reportFound = FALSE;
  while((hidContext.hidCharacDiscovered < hidContext.hidPresent) && !reportFound) {
    hid = &hidContext.hidService[hidContext.hidCharacDiscovered];
    if (type == INPUT_REPORT) {
      do {
        if (hid->report[hidContext.infoDiscovered].reportType == type) {
          hidContext.startHandle = hid->report[hidContext.infoDiscovered].clientCharacDescHandle;
          reportFound = TRUE;
        }
        hidContext.infoDiscovered++;
      } while((hidContext.infoDiscovered < hid->numReport) && !reportFound);
      if (hidContext.infoDiscovered == hid->numReport) {
        hidContext.hidCharacDiscovered++;
        hidContext.infoDiscovered = 0;
      }
    } else {
      if ((type == BOOT_KEYBOARD_INPUT_REPORT) && hid->bootKeyboardInputPresent) {
        hidContext.startHandle = hid->bootKeyboardClientCharacDescHandle;
        reportFound = TRUE;
      }
      if ((type == BOOT_MOUSE_INPUT_REPORT) && hid->bootMouseInputPresent) {
        hidContext.startHandle = hid->bootMouseClientCharacDescHandle;
        reportFound = TRUE;
      }
      hidContext.hidCharacDiscovered++;
    }
  }

  if (reportFound)
    ret = Master_NotifIndic_Status(hidContext.connHandle, hidContext.startHandle, enabled, FALSE);

  return ret; 
}

uint8_t HID_WriteScanIntervalWindowParam(uint16_t scanInterval, uint16_t scanWindow)
{
  uint8_t ret;
  scanParameterServiceType *scanParam;
  uint8_t data[4];

  if (!hidContext.scanParamPresent)
    return HCI_COMMAND_DISALLOWED;

  hidContext.scanParamService.le_scan_interval = scanInterval;
  hidContext.scanParamService.le_scan_window = scanWindow;
  scanParam = &hidContext.scanParamService;
  HOST_TO_LE_16(data, scanInterval);
  HOST_TO_LE_16(&data[2], scanWindow);
  ret = Master_WriteWithoutResponse_Value(hidContext.connHandle, scanParam->scanIntervalWindow.start, 4, data);

  if (ret == BLE_STATUS_SUCCESS) {
    PROFILE_MESG_DBG(profiledbgfile,"**** Write Scan Interval and Scan Window value success\r\n");
  } else {
    PROFILE_MESG_DBG(profiledbgfile,"**** Write Scan Interval and Scan Window value failed\r\n");
  }

  return ret;
}

uint8_t HID_ScanRefreshNotificationStatus(uint8_t enabled)
{
  uint8_t ret;

  if (!hidContext.scanParamPresent || !hidContext.scanParamService.scanRefreshPresent) {
    return HCI_COMMAND_DISALLOWED;
  }

  hidContext.startHandle = hidContext.scanParamService.scanRefreshCharacDescHandle;
  ret = Master_NotifIndic_Status(hidContext.connHandle, hidContext.startHandle, enabled, FALSE);

  return ret;
}

/* This function start the connection and the configuration of the HID host device */
uint8_t HID_ConnConf(hidConnDevType connParam, hidConfDevType confParam)
{
  uint8_t ret;

  ret = HID_DeviceConnection(connParam);
  
  if (ret == BLE_STATUS_SUCCESS) {
    hidContext.fullConf = TRUE;
    hidContext.fullConfReconnection = FALSE;
    hidContext.maxNmbReportDesc = confParam.maxNmbReportDesc;
    hidContext.numReportDescPresent = confParam.numReportDescPresent;
    hidContext.reportDescLen = confParam.reportDescLen;
    hidContext.reportData = confParam.reportData;
    hidContext.maxReportDataSize = confParam.maxReportDataSize;
  }

  return ret;
}

void HID_GetReportId(uint8_t type, uint8_t *numReport, uint8_t *ID)
{
  uint8_t i, j;
  hidServiceType *hid;

  *numReport = 0;
  for (i=0; i<hidContext.hidPresent; i++) {
    hid = &hidContext.hidService[i];
    for (j=0; j<hid->numReport; j++) {
      if (hid->report[j].reportType == type) {
        ID[*numReport] = hid->report[j].reportID;
        (*numReport)++;
      }
    }
  }
}

uint8_t HID_SetReport(uint8_t noResponseFlag, uint8_t type, uint8_t ID, uint8_t dataLen, uint8_t *data)
{
  uint8_t i, j, reportFound, ret;
  hidServiceType *hid;

  if (noResponseFlag && (type != OUTPUT_REPORT))
    return HCI_COMMAND_DISALLOWED;

  reportFound = FALSE;
  for (i=0; i<hidContext.hidPresent; i++) {
    hid = &hidContext.hidService[i];
    for (j=0; j<hid->numReport; j++) {
      if ((hid->report[j].reportType == type) && (hid->report[j].reportID == ID)) {
        if (noResponseFlag) {
          ret = Master_WriteWithoutResponse_Value(hidContext.connHandle, hid->report[j].start, dataLen, data);
        } else {
          ret = Master_Write_Value(hidContext.connHandle, hid->report[j].start, dataLen, data);
        }
        reportFound = TRUE;
      }
    }
  }
  if (!reportFound) {
    ret = HID_ERROR_REPORT_NOT_PRESENT; 
  }

  return ret;
}

uint8_t HID_GetReport(uint8_t type, uint8_t ID)
{
  uint8_t i, j, reportFound, ret;
  hidServiceType *hid;

  reportFound = FALSE;
  for (i=0; i<hidContext.hidPresent; i++) {
    hid = &hidContext.hidService[i];
    for (j=0; j<hid->numReport; j++) {
      if ((hid->report[j].reportType == type) && (hid->report[j].reportID == ID)) {
        hidContext.startHandle = hid->report[j].start;
        ret = Master_Read_Value(hidContext.connHandle, hid->report[j].start, 
                                &hidContext.dataLen, hidContext.data, CHARAC_OF_SERVICE_ARRAY_SIZE);
        reportFound = TRUE;
      }
    }
  }

  if (!reportFound) {
    ret = HID_ERROR_REPORT_NOT_PRESENT; 
  }

  return ret;
}

uint8_t HID_SetControlPoint(uint8_t suspend)
{
  uint8_t i, ret, value;
  hidServiceType *hid;

  if (suspend)
    value = 0;
  else
    value = 1;

  for (i=0; i<hidContext.hidPresent; i++) {
    HAL_Delay(10); // ???
    hid = &hidContext.hidService[i];
    ret = Master_WriteWithoutResponse_Value(hidContext.connHandle, hid->hidControlPoint.start, 1, &value);
    if (ret != BLE_STATUS_SUCCESS) {
      PROFILE_MESG_DBG(profiledbgfile,"**** Set Control Point procedure failed HID[%d] status 0x%02x\r\n", i, ret);
      return ret;
    }
  }
  return ret;
}

uint8_t HID_SetProtocol(uint8_t mode)
{
  uint8_t i, ret;
  hidServiceType *hid;

  for (i=0; i<hidContext.hidPresent; i++) {
    HAL_Delay(10); // ???
    hid = &hidContext.hidService[i];
    if (hid->protocolModePresent) {
      ret = Master_WriteWithoutResponse_Value(hidContext.connHandle, hid->protocolMode.start, 1, &mode);
    }
    if (ret != BLE_STATUS_SUCCESS) {
      PROFILE_MESG_DBG(profiledbgfile,"**** Set Protocol Mode procedure failed HID[%d] status 0x%02x\r\n", i, ret);
      return ret;
    }
  }
  return ret;  
}

uint8_t HID_GetProtocol(void)
{
  uint8_t ret, readyToRead;
  uint16_t handle, state;
  hidServiceType *hid;
  
  readyToRead = FALSE;
  do {
    hid =  &hidContext.hidService[hidContext.hidCharacDiscovered];
    if (hid->protocolModePresent) {
      readyToRead = TRUE;
      handle = hid->protocolMode.start;
      state = HID_HOST_WAIT_READ_PROTOCOL_MODE;
    }
    if (readyToRead) {
      ret = Master_Read_Value(hidContext.connHandle, handle, 
                              &hidContext.dataLen, hidContext.data, CHARAC_OF_SERVICE_ARRAY_SIZE);
      if (ret == BLE_STATUS_SUCCESS) {
        hidContext.startHandle = handle;
        hidContext.state = state;
        PROFILE_MESG_DBG(profiledbgfile,"**** Read Protocol Mode value success\r\n");
      } else {
        PROFILE_MESG_DBG(profiledbgfile,"**** Read Protocol Mode value failed\r\n");
      }  
    }
    hidContext.hidCharacDiscovered++;
  } while (!readyToRead && (hidContext.hidCharacDiscovered < hidContext.hidPresent));

  if (!readyToRead) {
    HID_ProtocolMode_CB(BLE_STATUS_SUCCESS, 1);
    ret = HCI_COMMAND_DISALLOWED;
  }

  return ret;
}

uint8_t HID_SetBootReport(uint8_t type, uint8_t dataLen, uint8_t *data)
{
  uint8_t i, ret, bootReportPresent;
  uint16_t handle;
  hidServiceType *hid;

  ret = HID_ERROR_REPORT_NOT_PRESENT;
  bootReportPresent = FALSE;
  for (i=0; i<hidContext.hidPresent; i++) {
    hid = &hidContext.hidService[i];

    if ((type == BOOT_KEYBOARD_INPUT_REPORT) && hid->bootKeyboardInputPresent) {
      bootReportPresent = TRUE;
      handle = hid->bootKeyboardInput.start;
    }

    if ((type == BOOT_KEYBOARD_OUTPUT_REPORT) && hid->bootKeyboardOutputPresent) {
      bootReportPresent = TRUE;
      handle = hid->bootKeyboardOutput.start;
    }

    if ((type == BOOT_MOUSE_INPUT_REPORT) && hid->bootMouseInputPresent) {
      bootReportPresent = TRUE;
      handle = hid->bootMouseInput.start;
    }      
    
    if (bootReportPresent) {
      ret = Master_Write_Value(hidContext.connHandle, handle, dataLen, data);
      if (ret != BLE_STATUS_SUCCESS) 
        PROFILE_MESG_DBG(profiledbgfile,"**** Set Protocol Mode procedure failed HID[%d] status 0x%02x\r\n", i, ret);
      return ret;
    }
  }
  return ret;  
}

uint8_t HID_SetHostMode(uint8_t mode)
{ 
  hidContext.hostMode = mode;

  return BLE_STATUS_SUCCESS;
}

void HID_StateMachine(void)
{
  uint8_t ret;

  switch(hidContext.state) {
  case HID_HOST_UNINITIALIZED:
    /* Nothing to do */
    break;
  case HID_HOST_CONNECTED_IDLE:
    {
      // Add the connected idle state management ???
    }
    break;
  case HID_HOST_INITIALIZED:
    /* Nothing to do */
    break;
  case HID_HOST_DEVICE_DISCOVERY:
    /* Nothing to do */
    break;
  case HID_HOST_START_CONNECTION:
    /* Start connection and all the service discovery */
    break;
  case HID_HOST_DISCONNECTION:
    {
      ret = HID_DeviceDisconnection();
      if (ret != BLE_STATUS_SUCCESS) {
	HID_FullConfError_CB(HID_ERROR_IN_DISCONNECTION, ret);
      }
      hidContext.state = HID_HOST_INITIALIZED;
    }
    break;
  case HID_HOST_RECONNECTION:
    {
      ret = Master_DeviceConnection(&hidContext.master_param);
      if (ret == BLE_STATUS_SUCCESS) {
        PROFILE_MESG_DBG(profiledbgfile,"**** Reconnection procedure success\r\n");
        hidContext.state = HID_HOST_CONNECTED_IDLE;
      } else {
        PROFILE_MESG_DBG(profiledbgfile,"**** Reconnection procedure failed with status 0x%02x\r\n", ret);
        hidContext.state = HID_HOST_INITIALIZED;
      }
    }
    break;
  case HID_HOST_CONNECTED:
    {
      ret = HID_ServicesDiscovery(); 
      if (ret == BLE_STATUS_SUCCESS) {
	hidContext.state = HID_HOST_SERVICE_DISCOVERY;
      } else {
	HID_FullConfError_CB(HID_ERROR_IN_SERVICE_DISCOVERY, ret);
	hidContext.state = HID_HOST_DISCONNECTION;
      }
    }
    break;
  case HID_HOST_SERVICE_DISCOVERY:
    /* Waiting the end of the service discovery procedure */
    break;
  case HID_HOST_GET_INCLUDED_SERVICES:
    if (hidContext.numIncludedServices > 0) {
      ret = HID_GetIncludedBatterySerivces();
      if (ret != BLE_STATUS_SUCCESS) {
        hidContext.state = HID_HOST_HID_SERVICE_CHARAC_DISCOVERY;
        HID_FullConfError_CB(HID_ERROR_IN_INCLUDE_SERVICE_DISCOVERY, ret);
      } else {
        hidContext.state = HID_HOST_WAIT_INCLUDED_SERVICES;
      }
    } else {
      hidContext.state = HID_HOST_HID_SERVICE_CHARAC_DISCOVERY;
    }
    break;
  case HID_HOST_WAIT_INCLUDED_SERVICES:
    /* Waiting the end of the included service discovery procedure */
    break;
  case HID_HOST_HID_SERVICE_CHARAC_DISCOVERY:
    ret = HID_DiscCharacServ(HUMAN_INTERFACE_DEVICE_SERVICE_UUID);
    if (ret != BLE_STATUS_SUCCESS) {
      hidContext.state = HID_HOST_DISCONNECTION;
      HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DISCOVERY, ret);
    } else {
      hidContext.state = HID_HOST_WAIT_HID_SERVICE_CHARAC_DISCOVERY;
    }
    break;
  case HID_HOST_WAIT_HID_SERVICE_CHARAC_DISCOVERY:
    /* Waiting the end of characteristics discovery for HID service */
    break;
  case HID_HOST_REPORT_MAP_CHARAC_DESC_DISCOVERY:
    {
      hidServiceType *hid;
      
      hid =  &hidContext.hidService[hidContext.hidCharacDiscovered];
      if (hid->reportMap.start == hid->reportMap.end) {
        hidContext.state = HID_HOST_REPORT_CHARAC_DESC_DISCOVERY;
      } else {
        ret = HID_DiscCharacDesc(REPORT_MAP_CHAR_UUID);
        if (ret != BLE_STATUS_SUCCESS) {
          hidContext.state = HID_HOST_DISCONNECTION;
          HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DESC_DISCOVERY, ret);
        } else {
          hidContext.state = HID_HOST_WAIT_REPORT_MAP_CHARAC_DESC_DISCOVERY;
        }
      }
    }
    break;
  case HID_HOST_WAIT_REPORT_MAP_CHARAC_DESC_DISCOVERY:
    /* Waiting the end of Report Map characteristic descriptor discovery for Report Map */
    break;
  case HID_HOST_REPORT_CHARAC_DESC_DISCOVERY:
    {
      hidServiceType *hid;
      
      hid =  &hidContext.hidService[hidContext.hidCharacDiscovered];
      if (hid->numReport != 0) {
	ret = HID_DiscCharacDesc(REPORT_CHAR_UUID);
	if (ret != BLE_STATUS_SUCCESS) {
	  hidContext.state = HID_HOST_DISCONNECTION;
	  HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DESC_DISCOVERY, ret);
	} else {
	  hidContext.state = HID_HOST_WAIT_REPORT_CHARAC_DESC_DISCOVERY;
	}
      } else {
	hidContext.hidCharacDiscovered++;
	if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
	  hidContext.state = HID_HOST_REPORT_CHARAC_DESC_DISCOVERY;
	} else {
	  hidContext.hidCharacDiscovered = 0;
	  hidContext.state = HID_HOST_BOOT_KEYBOARD_REPORT_INPUT_CHARAC_DESC_DISCOVERY;
	}
      }
    }
    break;
  case HID_HOST_WAIT_REPORT_CHARAC_DESC_DISCOVERY:
    /* Waiting the end of Report Characteristics Descriptor discovery for all the hid service */
    break;
  case HID_HOST_BOOT_KEYBOARD_REPORT_INPUT_CHARAC_DESC_DISCOVERY:
    {
      hidServiceType *hid;
      
      hid =  &hidContext.hidService[hidContext.hidCharacDiscovered];
      if (hid->bootKeyboardInputPresent) {
	ret = HID_DiscCharacDesc(BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID);
	if (ret != BLE_STATUS_SUCCESS) {
	  hidContext.state = HID_HOST_DISCONNECTION;
	  HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DESC_DISCOVERY, ret);
	} else {
	  hidContext.state = HID_HOST_WAIT_BOOT_KEYBOARD_REPORT_INPUT_CHARAC_DESC_DISCOVERY;
	}
      } else {
	hidContext. hidCharacDiscovered++;
	if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
	  hidContext.state = HID_HOST_BOOT_KEYBOARD_REPORT_INPUT_CHARAC_DESC_DISCOVERY;
	} else {
	  hidContext.hidCharacDiscovered = 0;
	  hidContext.state = HID_HOST_BOOT_MOUSE_REPORT_INPUT_CHARAC_DESC_DISCOVERY;
	}
      }
    }
    break;
  case HID_HOST_WAIT_BOOT_KEYBOARD_REPORT_INPUT_CHARAC_DESC_DISCOVERY:
    /* Waiting the end of Boot Keyboard Report Input Characteristics Descriptor discovery for all the hid service */
    break;
  case HID_HOST_BOOT_MOUSE_REPORT_INPUT_CHARAC_DESC_DISCOVERY:
    {
      hidServiceType *hid;
      
      hid =  &hidContext.hidService[hidContext.hidCharacDiscovered];
      if (hid->bootMouseInputPresent) {
	ret = HID_DiscCharacDesc(BOOT_MOUSE_INPUT_REPORT_CHAR_UUID);
	if (ret != BLE_STATUS_SUCCESS) {
	  hidContext.state = HID_HOST_DISCONNECTION;
	  HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DESC_DISCOVERY, ret);
	} else {
	  hidContext.state = HID_HOST_WAIT_BOOT_MOUSE_REPORT_INPUT_CHARAC_DESC_DISCOVERY;
	}   
      } else {
	hidContext. hidCharacDiscovered++;
	if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
	  hidContext.state = HID_HOST_BOOT_MOUSE_REPORT_INPUT_CHARAC_DESC_DISCOVERY;
	} else {
	  hidContext.hidCharacDiscovered = 0;
	  hidContext.state = HID_HOST_BATTERY_SERVICE_CHARAC_DISCOVERY;
	}
      }
    }
    break;
  case HID_HOST_WAIT_BOOT_MOUSE_REPORT_INPUT_CHARAC_DESC_DISCOVERY:
    /* Waiting the end of Boot Mouse Report Input Characteristics Descriptor discovery for all the hid service */
    break;
  case HID_HOST_BATTERY_SERVICE_CHARAC_DISCOVERY:
    {
      ret = HID_DiscCharacServ(BATTERY_SERVICE_SERVICE_UUID);
      if (ret != BLE_STATUS_SUCCESS) {
	hidContext.state = HID_HOST_DISCONNECTION;
	HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DISCOVERY, ret);
      } else {
	hidContext.state = HID_HOST_WAIT_BATTERY_SERVICE_CHARAC_DISCOVERY;
      }
    }
    break;
  case HID_HOST_WAIT_BATTERY_SERVICE_CHARAC_DISCOVERY:
    /* Waiting the end of characteristics discovery for Battery service */
    break;
  case  HID_HOST_BATTERY_LEVEL_CHARAC_DESC_DISCOVERY:
    {
      ret = HID_DiscCharacDesc(BATTERY_LEVEL_CHAR_UUID);
      if (ret != BLE_STATUS_SUCCESS) {
	hidContext.state = HID_HOST_DISCONNECTION;
	HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DESC_DISCOVERY, ret);
      } else {
	hidContext.state = HID_HOST_WAIT_BATTERY_LEVEL_CHARAC_DESC_DISCOVERY;
      }   
    }
    break;
  case HID_HOST_WAIT_BATTERY_LEVEL_CHARAC_DESC_DISCOVERY:
    /* Waiting the end of characteristic descriptors discovery for Battery Level */
    break;
  case HID_HOST_DEVICE_INFORMATION_SERVICE_CHARAC_DISCOVERY:
    {
      ret = HID_DiscCharacServ(DEVICE_INFORMATION_SERVICE_UUID);
      if (ret != BLE_STATUS_SUCCESS) {
	hidContext.state = HID_HOST_DISCONNECTION;
	HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DESC_DISCOVERY, ret);
      } else {
	hidContext.state = HID_HOST_WAIT_DEVICE_INFORMATION_SERVICE_CHARAC_DISCOVERY;
      } 
    }  
    break;
  case HID_HOST_WAIT_DEVICE_INFORMATION_SERVICE_CHARAC_DISCOVERY:
    /* Waiting the end of characteristic discovery for the Device Informtation */
    break;
  case HID_HOST_READ_REPORT_MAP_DESCRIPTOR:
    {
      if (hidContext.hidPresent > hidContext.maxNmbReportDesc) {
	ret = BLE_STATUS_INSUFFICIENT_RESOURCES;
      } else {
	if (hidContext.hidCharacDiscovered == 0) {
	  hidContext.reportDataOffset = 0;
	}
        hidContext.reportDescLen[hidContext.hidCharacDiscovered] = 0;
	ret = HID_ReadReportDescriptor(hidContext.hidCharacDiscovered+1, 
				       &hidContext.reportDescLen[hidContext.hidCharacDiscovered], 
				       &hidContext.reportData[hidContext.reportDataOffset], 
				       (hidContext.maxReportDataSize-hidContext.reportDataOffset));
      }
      if (ret != BLE_STATUS_SUCCESS) {
	hidContext.state = HID_HOST_DISCONNECTION;
	HID_FullConfError_CB(HID_ERROR_IN_READ_REPORT_DESCRIPTOR, ret);
      } else {
	*hidContext.numReportDescPresent = hidContext.hidPresent;
	hidContext.state = HID_HOST_WAIT_READ_REPORT_MAP_DESCRIPTOR;
      } 
    }
    break;
  case HID_HOST_WAIT_READ_REPORT_MAP_DESCRIPTOR:
    /* Waiting the end of Read Report Map characteristic procedure */
    break;
  case HID_HOST_READ_REPORT_REFERENCE_VALUE:
    {
      runReadReportState(HID_HOST_READ_REPORT_REFERENCE_VALUE);
    }
    break;
  case HID_HOST_WAIT_READ_REPORT_REFERENCE_VALUE:
    /* Waiting the end of Read Report Reference characteristic descriptor value */
    break;
  case HID_HOST_READ_HID_INFORMATION_VALUE:
    {
      ret = HID_ReadHidInformation();
      if (ret != BLE_STATUS_SUCCESS) {
        hidContext.state = HID_HOST_DISCONNECTION;
	HID_FullConfError_CB(HID_ERROR_IN_READ_HID_INFORMATION, ret);
      } else {
        hidContext.state = HID_HOST_WAIT_READ_HID_INFORMATION_VALUE;
      }
    }
    break;
  case HID_HOST_WAIT_READ_HID_INFORMATION_VALUE:
    /* Waiting the end of Read HID Information value */
    break;
  case HID_HOST_READ_BATTERY_LEVEL_CHARAC_PRESENTATION_FORMAT:
    {
      batteryServiceType *battery;

      if (hidContext.batteryPresent > 1) {
        battery = &hidContext.batteryService[hidContext.hidCharacDiscovered];
        ret = Master_Read_Value(hidContext.connHandle, battery->characPresentationFormat, 
                                &hidContext.dataLen, hidContext.data, CHARAC_OF_SERVICE_ARRAY_SIZE);
        if (ret != BLE_STATUS_SUCCESS) {
          hidContext.state = HID_HOST_DISCONNECTION;
          HID_FullConfError_CB(HID_ERROR_IN_READ_BATTERY_PRESENTATION_FORMAT, ret);
        } else {
          hidContext.state = HID_HOST_WAIT_READ_BATTERY_LEVEL_CHARAC_PRESENTATION_FORMAT;
        }
      } else {
        hidContext.state = HID_HOST_READ_BATTERY_LEVEL_VALUE;
      }
    }
    break;
  case HID_HOST_WAIT_READ_BATTERY_LEVEL_CHARAC_PRESENTATION_FORMAT:
    /* Waiting the end of Read Characteristic Presentation Format value */
    break;
  case HID_HOST_READ_BATTERY_LEVEL_VALUE:
    {
      ret = HID_ReadBatteryLevel();
      if (ret != BLE_STATUS_SUCCESS) {
        hidContext.state = HID_HOST_DISCONNECTION;
        HID_FullConfError_CB(HID_ERROR_IN_READ_BATTERY_LEVEL, ret);
      } else {
        hidContext.state = HID_HOST_WAIT_READ_BATTERY_LEVEL_VALUE;
      }
    }
    break;
  case HID_HOST_WAIT_READ_BATTERY_LEVEL_VALUE:
    /* Waiting the end of the Read Battery Level value */
    break;
  case HID_HOST_READ_PNP_ID_CHARAC:
    {
      ret = HID_ReadPnPID();
      if (ret != BLE_STATUS_SUCCESS) {
        hidContext.state = HID_HOST_DISCONNECTION;
        HID_FullConfError_CB(HID_ERROR_IN_READ_PNP_ID, ret);
      } else {
        hidContext.state = HID_HOST_WAIT_READ_PNP_ID_CHARAC;
      }
    }
    break;
  case HID_HOST_WAIT_READ_PNP_ID_CHARAC:
    /* Waiting the end of the Read PnP ID characteristic procedure */
    break;
  case HID_HOST_READ_PROTOCOL_MODE:
    {
      ret = HID_GetProtocol();
      if (ret == BLE_STATUS_SUCCESS) {
        hidContext.state = HID_HOST_WAIT_READ_PROTOCOL_MODE;
      } else {
        hidContext.hidCharacDiscovered = 0;
        if (hidContext.scanParamPresent && (hidContext.hostMode == REPORT_PROTOCOL_MODE))  {
          hidContext.state = HID_HOST_SCAN_PARAM_SERVICE_SCHARAC_DISCOVERY;
        } else {
          hidContext.hidCharacDiscovered = 0;
          hidContext.infoDiscovered = 0;
          hidContext.state = HID_HOST_INPUT_REPORT_ENABLE_NOTIFICATION;
        }
      }
    }
    break;
  case HID_HOST_WAIT_READ_PROTOCOL_MODE:
    /* Waiting the end of the Get Protocol Mode procedure */
    break;
  case HID_HOST_SCAN_PARAM_SERVICE_SCHARAC_DISCOVERY:
    {
      ret = HID_DiscCharacServ(SCAN_PARAMETER_SERVICE_UUID);
      if (ret != BLE_STATUS_SUCCESS) {
        HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DISCOVERY, ret);
        hidContext.state = HID_HOST_INPUT_REPORT_ENABLE_NOTIFICATION;
        hidContext.hidCharacDiscovered = 0;
        hidContext.infoDiscovered = 0;
      } else {
        hidContext.state = HID_HOST_WAIT_SCAN_PARAM_SERVICE_SCHARAC_DISCOVERY;
      }
    }
    break;
  case HID_HOST_WAIT_SCAN_PARAM_SERVICE_SCHARAC_DISCOVERY:
    /* Waiting the end of Scan Parameter Service Characteristic Discovery */
    break;
  case HID_HOST_SCAN_REFRESH_DESCRIPTOR:
    {
      ret = HID_DiscCharacDesc(SCAN_REFRESH_CHAR_UUID);
      if (ret != BLE_STATUS_SUCCESS) {
        hidContext.hidCharacDiscovered = 0;
        hidContext.infoDiscovered = 0;
	hidContext.state = HID_HOST_INPUT_REPORT_ENABLE_NOTIFICATION;
	HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DESC_DISCOVERY, ret);
      } else {
	hidContext.state = HID_HOST_WAIT_SCAN_REFRESH_DESCRIPTOR;
      }   
    }
    break;
  case HID_HOST_WAIT_SCAN_REFRESH_DESCRIPTOR:
    /* Waiting the end of the Scan Refresh Client Characteristic Config Descriptor discovery procedure */
    break;
  case HID_HOST_SCAN_REFRESH_ENABLE_NOTIFICATION:
    {
      ret = HID_ScanRefreshNotificationStatus(TRUE);
      if (ret != BLE_STATUS_SUCCESS) {
        hidContext.hidCharacDiscovered = 0;
        hidContext.infoDiscovered = 0;
	hidContext.state = HID_HOST_INPUT_REPORT_ENABLE_NOTIFICATION;
	HID_FullConfError_CB(HID_ERROR_IN_ENABLE_NOTIFICATION, ret);        
      } else {
        hidContext.state = HID_HOST_WAIT_SCAN_REFRESH_ENABLE_NOTIFICATION;
      }
    }
    break;
  case HID_HOST_WAIT_SCAN_REFRESH_ENABLE_NOTIFICATION:
    /* Waiting the end of Scan Refresh enable notification procedure */
    break;
 case HID_HOST_INPUT_REPORT_ENABLE_NOTIFICATION:
   {
     ret = HID_SetNotificationStatus(INPUT_REPORT,  TRUE);
     if (ret != BLE_STATUS_SUCCESS) {
       hidContext.hidCharacDiscovered = 0;
       hidContext.infoDiscovered = 0;
       hidContext.state = HID_HOST_BOOT_KEYBOARD_INPUT_ENABLE_NOTIFICATION;
     } else {
       hidContext.state = HID_HOST_WAIT_INPUT_REPORT_ENABLE_NOTIFICATION;
     }
   }
   break;
 case HID_HOST_WAIT_INPUT_REPORT_ENABLE_NOTIFICATION:
   /* Waiting the end of Enable Input Report notification procedure */
   break;
 case HID_HOST_BOOT_KEYBOARD_INPUT_ENABLE_NOTIFICATION:
   {
     ret = HID_SetNotificationStatus(BOOT_KEYBOARD_INPUT_REPORT,  TRUE);
     if (ret != BLE_STATUS_SUCCESS) {
       hidContext.hidCharacDiscovered = 0;
       hidContext.infoDiscovered = 0;
       hidContext.state = HID_HOST_BOOT_MOUSE_INPUT_ENABLE_NOTIFICATION;
     } else {
       hidContext.state = HID_HOST_WAIT_BOOT_KEYBOARD_INPUT_ENABLE_NOTIFICATION;
     } 
   }
   break;
 case HID_HOST_WAIT_BOOT_KEYBOARD_INPUT_ENABLE_NOTIFICATION:
   /* Waiting the end of Enable Keyboard Input Report notification procedure */
   break;
 case HID_HOST_BOOT_MOUSE_INPUT_ENABLE_NOTIFICATION:
   {
     ret = HID_SetNotificationStatus(BOOT_MOUSE_INPUT_REPORT,  TRUE);
     if (ret != BLE_STATUS_SUCCESS) {
       hidContext.hidCharacDiscovered = 0;
       hidContext.infoDiscovered = 0;
       if (hidContext.scanParamPresent) {
         hidContext.state = HID_HOST_WRITE_SCAN_INTERVAL_WINDOW_VALUE;
       } else {
         hidContext.state = HID_HOST_CONNECTED_IDLE;
         hidContext.fullConfEnded = TRUE;
       }
     } else {
       hidContext.state = HID_HOST_WAIT_BOOT_MOUSE_INPUT_ENABLE_NOTIFICATION;
     } 
   }
   break;
 case HID_HOST_WAIT_BOOT_MOUSE_INPUT_ENABLE_NOTIFICATION:
   /* Waiting the end of Enable Mouse Input Report notification procedure */
   break;
  case HID_HOST_WRITE_SCAN_INTERVAL_WINDOW_VALUE:
    {
      ret = HID_WriteScanIntervalWindowParam(hidContext.scanParamService.le_scan_interval, 
                                             hidContext.scanParamService.le_scan_window);
      if (ret != BLE_STATUS_SUCCESS)
        HID_FullConfError_CB(HID_ERROR_IN_WRITE_PROCEDURE, ret);
      hidContext.state = HID_HOST_CONNECTED_IDLE;        
      hidContext.fullConfEnded = TRUE;
    }
    break;
  }
}

/****************** HID Host Callbacks ***************************/
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
    if (hidContext.state != HID_HOST_START_CONNECTION)
      hidContext.state = HID_HOST_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure ended from the application\r\n");
  }

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) {
    hidContext.state = HID_HOST_INITIALIZED;
    CALLBACKS_MESG_DBG(profiledbgfile,"**** Device Discovery Procedure Timeout\r\n");    
  }  

  HID_DeviceDiscovery_CB(*status, *addr_type, addr, *data_length, data, *RSSI);
}

void Master_Connection_CB(uint8_t *connection_evt, uint8_t *status, 
			  uint16_t *connection_handle, connUpdateParamType *param)
{
  switch (*connection_evt) {
  case DISCONNECTION_EVT:
    {
      hidContext.state = HID_HOST_INITIALIZED;
      HID_ConnectionStatus_CB(*connection_evt, *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Disconnection Event Received\r\n");
      if (hidContext.fullConfEnded) {
        hidContext.state = HID_HOST_RECONNECTION;
        if (hidContext.fullConf) {
          hidContext.fullConfReconnection = TRUE;
        } else {
          if (HID_ClearBondedDevices() == BLE_STATUS_SUCCESS) {
            CALLBACKS_MESG_DBG(profiledbgfile, "**** Clear Bonded Device Success\r\n");
          } else {
            CALLBACKS_MESG_DBG(profiledbgfile, "**** Clear Bonded Device Failed\r\n");
          }
          hidContext.fullConfReconnection = FALSE;
          hidContext.fullConf = TRUE;
        }
      }
    }
    break;
  case CONNECTION_ESTABLISHED_EVT:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established with Success\r\n");
        hidContext.hidCharacDiscovered = 0;
	hidContext.connHandle = *connection_handle;
        if (hidContext.fullConf) {
          if (!hidContext.fullConfReconnection) {
            hidContext.state = HID_HOST_CONNECTED;
          } else {
            hidContext.fullConfReconnection = FALSE;
            hidContext.state = HID_HOST_CONNECTED_IDLE;
            if (Master_Start_Pairing_Procedure(hidContext.connHandle, FALSE)) {
              CALLBACKS_MESG_DBG(profiledbgfile, "**** Start Pairing procedure Failed\r\n");
            } else {
              CALLBACKS_MESG_DBG(profiledbgfile, "**** Srart Pairing procedure Success\r\n");
            }
          }
        }
	HID_ConnectionStatus_CB(*connection_evt, *status);
      } else {
        CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Established failed Status 0x%02x\r\n", *status); 
      }
    }
    break;
  case CONNECTION_FAILED_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Failed with Status 0x%02x\r\n", *status);
      hidContext.state = HID_HOST_INITIALIZED;
      if (hidContext.fullConf) {
        if (!hidContext.fullConfReconnection) {
          HID_FullConfError_CB(HID_ERROR_IN_CONNECTION, *status);
        } else {
          hidContext.state = HID_HOST_RECONNECTION;
        }
      } else {
	HID_ConnectionStatus_CB(*connection_evt, *status);
      }
    }
    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Stop Connection Procedure from the application\r\n");
      hidContext.state = HID_HOST_INITIALIZED;
    }
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Request\r\n");
      hidContext.state = HID_HOST_CONN_PARAM_UPDATE_REQ;
      if (Master_ConnectionUpdateParamResponse(hidContext.connHandle, TRUE, param) == BLE_STATUS_SUCCESS) {
        CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Accepted with Success\r\n");
      } else {
        if(Master_ConnectionUpdateParamResponse(hidContext.connHandle, FALSE, param) == BLE_STATUS_SUCCESS) {
          CALLBACKS_MESG_DBG(profiledbgfile, "**** Connection Parameter Update Rejected\r\n");
        } else {
          HID_ConnectionParameterUpdateReq_CB((hidConnUpdateParamType*) param);
        }
      }
    }
    break;
  }
}

void Master_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
  switch(*procedure) {
  case PRIMARY_SERVICE_DISCOVERY:
    {
      uint8_t serviceOk;

      /* Verify if all the mandatory services are present, otherwise disconnect the HID device */
      serviceOk = verifyServices();
      if (hidContext.fullConf) {
	if (serviceOk) {
          hidContext.hidCharacDiscovered = 0;
	  hidContext.state = HID_HOST_GET_INCLUDED_SERVICES;
	} else {
	  hidContext.state = HID_HOST_DISCONNECTION;
	}
      }
      HID_ServicesDiscovery_CB(*status, hidContext.numDeviceServices, 
			       hidContext.deviceServices);
    }
    break;
  case GET_CHARACTERISTICS_OF_A_SERVICE:
    {
      HID_CharacOfService_CB(*status, hidContext.numCharac, hidContext.charac);
      findCharacHandle(hidContext.uuid_searched, 
		       hidContext.numCharac, 
		       hidContext.charac);
      switch (hidContext.uuid_searched) {
      case HUMAN_INTERFACE_DEVICE_SERVICE_UUID:
	if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
	  hidContext.state = HID_HOST_HID_SERVICE_CHARAC_DISCOVERY;
	} else {
	  hidContext.hidCharacDiscovered = 0;
	  hidContext.state = HID_HOST_REPORT_MAP_CHARAC_DESC_DISCOVERY;
	}
	break;
      case DEVICE_INFORMATION_SERVICE_UUID:
	hidContext.hidCharacDiscovered = 0;
	hidContext.state = HID_HOST_READ_REPORT_MAP_DESCRIPTOR;
	break;
      case BATTERY_SERVICE_SERVICE_UUID:
	if (hidContext.hidCharacDiscovered < hidContext.batteryPresent) {
	  hidContext.state = HID_HOST_BATTERY_SERVICE_CHARAC_DISCOVERY;
	} else {
	  hidContext.hidCharacDiscovered = 0;
	  hidContext.state = HID_HOST_BATTERY_LEVEL_CHARAC_DESC_DISCOVERY;
	}
	break;
      case SCAN_PARAMETER_SERVICE_UUID:
        if (hidContext.scanParamService.scanRefreshPresent) {
          hidContext.state = HID_HOST_SCAN_REFRESH_DESCRIPTOR;
        } else {
          hidContext.hidCharacDiscovered = 0;
          hidContext.infoDiscovered = 0;
          hidContext.state = HID_HOST_INPUT_REPORT_ENABLE_NOTIFICATION;        
        }
	break;
      default:
	if (hidContext.otherCharacDiscovered < hidContext.otherServicePresent) {
	  // Set the correct state ???
	} else {
	  // Set the new state ???
	}	
      }
      if (hidContext.fullConf) {
	if (*status != BLE_STATUS_SUCCESS) {
	  hidContext.state = HID_HOST_DISCONNECTION;
	  HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DISCOVERY, *status);
	}
      }
    }
    break;
  case FIND_INCLUDED_SERVICES:
    {
      if (hidContext.fullConf) {
	if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
	  hidContext.state = HID_HOST_GET_INCLUDED_SERVICES;
	} else {
	  hidContext.state = HID_HOST_HID_SERVICE_CHARAC_DISCOVERY;
	  hidContext.hidCharacDiscovered = 0;
          hidContext.reportDataOffset = 0;
	}
	if (*status != BLE_STATUS_SUCCESS) {
	  HID_FullConfError_CB(HID_ERROR_IN_INCLUDE_SERVICE_DISCOVERY, *status); 
	} else {
	  findIncludedHandle();
	}
      }
      HID_IncludedServices_CB(*status, hidContext.numCharac, hidContext.charac);
    }
    break;
  case FIND_CHARAC_DESCRIPTORS:
    {
      findCharacDesc(hidContext.numCharac, hidContext.charac);
      switch (hidContext.uuid_searched) {
      case HUMAN_INTERFACE_DEVICE_SERVICE_UUID:
	{
	  switch (hidContext.characDesc_searched) {
	  case REPORT_MAP_CHAR_UUID:
	    {
	      hidContext. hidCharacDiscovered++;
	      if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
		hidContext.state = HID_HOST_REPORT_MAP_CHARAC_DESC_DISCOVERY;
	      } else {
		hidContext.hidCharacDiscovered = 0;
                hidContext.numReportDiscovered = 0;
		hidContext.state = HID_HOST_REPORT_CHARAC_DESC_DISCOVERY;
	      }
	    }
	    break;
	  case REPORT_CHAR_UUID:
	    {
	      hidServiceType *hid;

	      hid = &hidContext.hidService[hidContext.hidCharacDiscovered];
	      hidContext.numReportDiscovered++;
	      if (hidContext.numReportDiscovered < hid->numReport) {
		hidContext.state = HID_HOST_REPORT_CHARAC_DESC_DISCOVERY;
	      } else {
		hidContext.numReportDiscovered = 0;
		hidContext. hidCharacDiscovered++;
		if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
		  hidContext.state = HID_HOST_REPORT_CHARAC_DESC_DISCOVERY;
		} else {
		  hidContext.hidCharacDiscovered = 0;
		  hidContext.state = HID_HOST_BOOT_KEYBOARD_REPORT_INPUT_CHARAC_DESC_DISCOVERY;
		}
	      }
	    }
	    break;
	  case BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID:
	    {
	      hidContext. hidCharacDiscovered++;
	      if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
		hidContext.state = HID_HOST_BOOT_KEYBOARD_REPORT_INPUT_CHARAC_DESC_DISCOVERY;
	      } else {
		hidContext.hidCharacDiscovered = 0;
		hidContext.state = HID_HOST_BOOT_MOUSE_REPORT_INPUT_CHARAC_DESC_DISCOVERY;
	      }
	    }
	    break;
	  case BOOT_MOUSE_INPUT_REPORT_CHAR_UUID:
	    {
	      hidContext. hidCharacDiscovered++;
	      if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
		hidContext.state = HID_HOST_BOOT_MOUSE_REPORT_INPUT_CHARAC_DESC_DISCOVERY;
	      } else {
		hidContext.hidCharacDiscovered = 0;
		hidContext.state = HID_HOST_BATTERY_SERVICE_CHARAC_DISCOVERY;
	      }
	    }
	    break;
	  }
	}
	break;
      case BATTERY_SERVICE_SERVICE_UUID:
	hidContext. hidCharacDiscovered++;
	if (hidContext.hidCharacDiscovered < hidContext.batteryPresent) {
	  hidContext.state = HID_HOST_BATTERY_LEVEL_CHARAC_DESC_DISCOVERY;
	} else {
	  hidContext. hidCharacDiscovered = 0;
	  hidContext.state = HID_HOST_DEVICE_INFORMATION_SERVICE_CHARAC_DISCOVERY;
	}
	break;
      case SCAN_PARAMETER_SERVICE_UUID:
	hidContext.state = HID_HOST_SCAN_REFRESH_ENABLE_NOTIFICATION;
	break;
      default:
	if (hidContext.otherCharacDiscovered < hidContext.otherServicePresent) {
	  // Set the correct state ???
	} else {
	  // Set the new state ???
	}	
      }

      if (hidContext.fullConf) {
	if (*status != BLE_STATUS_SUCCESS) {
	  hidContext.state = HID_HOST_DISCONNECTION;
	  HID_FullConfError_CB(HID_ERROR_IN_CHARAC_DESC_DISCOVERY, *status);
	}
      } 

      HID_CharacDesc_CB(*status, hidContext.numCharac, hidContext.charac);
    }
    break;
  }
}

void Master_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, 
				uint16_t *connection_handle, dataReceivedType *data)
{
  switch(*procedure) {
  case NOTIFICATION_INDICATION_CHANGE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Indication Change Status (0x%02x)\r\n", *status);
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Indication Change Handle (0x%04x)\r\n", hidContext.startHandle);
    HID_NotificationChageStatus_CB(*status);
    if (hidContext.fullConf) {
      switch(hidContext.state) {
      case HID_HOST_WAIT_SCAN_REFRESH_ENABLE_NOTIFICATION:
        hidContext.hidCharacDiscovered = 0;
        hidContext.infoDiscovered = 0;
        hidContext.state = HID_HOST_INPUT_REPORT_ENABLE_NOTIFICATION;
        break;
      case HID_HOST_WAIT_INPUT_REPORT_ENABLE_NOTIFICATION:
        if (hidContext.hidCharacDiscovered == hidContext.hidPresent) {
          hidContext.hidCharacDiscovered = 0;
          hidContext.infoDiscovered = 0;
          hidContext.state = HID_HOST_BOOT_KEYBOARD_INPUT_ENABLE_NOTIFICATION;
        } else {
          hidContext.state = HID_HOST_INPUT_REPORT_ENABLE_NOTIFICATION;
        }
        break;
      case HID_HOST_WAIT_BOOT_KEYBOARD_INPUT_ENABLE_NOTIFICATION:
        if (hidContext.hidCharacDiscovered == hidContext.hidPresent) {
          hidContext.hidCharacDiscovered = 0;
          hidContext.infoDiscovered = 0;
          hidContext.state = HID_HOST_BOOT_MOUSE_INPUT_ENABLE_NOTIFICATION;
        } else {
          hidContext.state = HID_HOST_BOOT_KEYBOARD_INPUT_ENABLE_NOTIFICATION;
        } 
      case HID_HOST_WAIT_BOOT_MOUSE_INPUT_ENABLE_NOTIFICATION:
        if (hidContext.hidCharacDiscovered == hidContext.hidPresent) {
          hidContext.hidCharacDiscovered = 0;
          hidContext.infoDiscovered = 0;
          if (hidContext.scanParamPresent) {
            hidContext.state = HID_HOST_WRITE_SCAN_INTERVAL_WINDOW_VALUE;
          } else {
            hidContext.state = HID_HOST_CONNECTED_IDLE;
            hidContext.fullConfEnded = TRUE;
          }
        } else {
          hidContext.state = HID_HOST_BOOT_MOUSE_INPUT_ENABLE_NOTIFICATION;
        } 
        break;
      }
    }
    break;
  case READ_VALUE_STATUS:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Value Status (0x%02x)\r\n", *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Value Handle (0x%04x)\r\n", hidContext.startHandle);
      switch (hidContext.state) {
      case HID_HOST_WAIT_READ_PNP_ID_CHARAC:
        pnpIdCharac(*status, hidContext.dataLen, hidContext.data);
        break;
      case HID_HOST_WAIT_READ_REPORT:
        readReport(*status, hidContext.dataLen, hidContext.data);
        break;
      case HID_HOST_WAIT_READ_REPORT_CHARAC_DESC:
        readCharacDesc(*status, hidContext.dataLen, hidContext.data);
        HID_DataValueRead_CB(*status, hidContext.dataLen, hidContext.data);
        break;
      case HID_HOST_WAIT_READ_PROTOCOL_MODE:
        HID_ProtocolMode_CB(*status, hidContext.data[0]);
        if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
          hidContext.state = HID_HOST_READ_PROTOCOL_MODE;
        } else {
          if (hidContext.scanParamPresent && (hidContext.hostMode == REPORT_PROTOCOL_MODE))  {
            hidContext.state = HID_HOST_SCAN_PARAM_SERVICE_SCHARAC_DISCOVERY;
          } else {
            hidContext.hidCharacDiscovered = 0;
            hidContext.infoDiscovered = 0;
            hidContext.state = HID_HOST_INPUT_REPORT_ENABLE_NOTIFICATION;
          }
        }
        break;
      case  HID_HOST_WAIT_READ_REPORT_REFERENCE_VALUE:
        runReadReportReceivedState(hidContext.state, hidContext.dataLen, hidContext.data);        
        HID_DataValueRead_CB(*status, hidContext.dataLen, hidContext.data);
        break;
      case HID_HOST_WAIT_READ_HID_INFORMATION_VALUE:
        runReadHidInformationReceivedState(*status, hidContext.dataLen, hidContext.data);
        break;
      case HID_HOST_WAIT_READ_BATTERY_LEVEL_CHARAC_PRESENTATION_FORMAT:
      case HID_HOST_WAIT_READ_BATTERY_LEVEL_VALUE:
        batteryCharac(hidContext.state, *status, hidContext.dataLen, hidContext.data);
        break;
      default:
        HID_DataValueRead_CB(*status, hidContext.dataLen, hidContext.data);
      } 
    }
    break;
  case READ_LONG_VALUE_STATUS:
    {
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Long Value Status (0x%02x)\r\n", *status);
      CALLBACKS_MESG_DBG(profiledbgfile, "**** Read Long Value Handle (0x%04x)\r\n", hidContext.startHandle);      
      HID_ReadReportDescriptor_CB(*status);
      if (hidContext.fullConf) {
	if (hidContext.state == HID_HOST_WAIT_READ_REPORT_MAP_DESCRIPTOR) {
	  if (*status != BLE_STATUS_SUCCESS) {
	    hidContext.state = HID_HOST_DISCONNECTION;
	    HID_FullConfError_CB(HID_ERROR_IN_READ_REPORT_DESCRIPTOR, *status);
	  } else {
	    hidContext.reportDataOffset += hidContext.reportDescLen[hidContext.hidCharacDiscovered];
	    hidContext. hidCharacDiscovered++;
	    if (hidContext.hidCharacDiscovered < hidContext.hidPresent) {
	      hidContext.state = HID_HOST_READ_REPORT_MAP_DESCRIPTOR;
	    } else {
	      hidContext.hidCharacDiscovered = 0;
              hidContext.infoDiscovered = 0;
              hidContext.state = HID_HOST_READ_REPORT_REFERENCE_VALUE;
	    }
	  }
	}
      }
    }
    break;
  case WRITE_VALUE_STATUS:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Write Value Status (0x%02x)\r\n", *status);
    HID_SetProcedure_CB(*status);
    break;
  case NOTIFICATION_DATA_RECEIVED:
    {
      uint8_t type, id;

      CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Data Received (0x%02x)\r\n", *status);
      if (hidContext.scanParamPresent && hidContext.scanParamService.scanRefreshPresent &&
          (data->attr_handle == hidContext.scanParamService.scanRefresh.start)) {
        if (HID_WriteScanIntervalWindowParam(hidContext.scanParamService.le_scan_interval, hidContext.scanParamService.le_scan_window))
          CALLBACKS_MESG_DBG(profiledbgfile, "**** Write Scan Interval Window Paramters Error\r\n");
      } else {
        if (findNotificationSource(data->attr_handle, &type, &id)) {
          HID_ReportDataReceived_CB(type, id, data->data_length, data->data_value);
        } else {
          CALLBACKS_MESG_DBG(profiledbgfile, "**** Notification Data Received Ignored!!!!!!!!!!!!!!\r\n");
        }
      }
    }
    break;
  case INDICATION_DATA_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Indication Data Received (0x%02x)\r\n", *status);
    break;
  case ATTRIBUTE_MODIFICATION_RECEIVED:
    CALLBACKS_MESG_DBG(profiledbgfile, "**** Attribute Modification Received (0x%02x)\r\n", *status);
    break;
  }
}

void Master_Pairing_CB(uint16_t *conn_handle, uint8_t *status)
{
  CALLBACKS_MESG_DBG(profiledbgfile, "**** Pairing CB status 0x%02x\r\n", *status);
  switch (*status) {
  case PASS_KEY_REQUEST:
    {
      HID_PinCodeRequired_CB();
    }
    break;
  case 0x02:
  case 0x1A:
    {
      hidContext.state = HID_HOST_DISCONNECTION;
      hidContext.fullConf = FALSE;
      HID_PairingFailed_CB();
    }
    break;
  }
}

void Master_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data)
{
  // Nothing to do 
}

/****************** Proximity Monitor Weak Callbacks definition ***************************/

WEAK_FUNCTION(void HID_DeviceDiscovery_CB(uint8_t status, uint8_t addr_type, uint8_t *addr, 
					  uint8_t data_length, uint8_t *data, 
					  uint8_t RSSI))
{
}

WEAK_FUNCTION(void HID_ConnectionStatus_CB(uint8_t connection_evt, uint8_t status))
{
}

WEAK_FUNCTION(void HID_ConnectionParameterUpdateReq_CB(hidConnUpdateParamType *param))
{
}

WEAK_FUNCTION(void HID_ServicesDiscovery_CB(uint8_t status, uint8_t numServices, uint8_t *services))
{
}

WEAK_FUNCTION(void HID_IncludedServices_CB(uint8_t status, uint8_t numIncludedServices, uint8_t *includedServices))
{
}

WEAK_FUNCTION(void HID_CharacOfService_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void HID_CharacDesc_CB(uint8_t status, uint8_t numCharac, uint8_t *charac))
{
}

WEAK_FUNCTION(void HID_ReadReportDescriptor_CB(uint8_t status))
{
}

WEAK_FUNCTION(void HID_DataValueRead_CB(uint8_t status, uint16_t data_len, uint8_t *data))
{
}

WEAK_FUNCTION(void HID_InformationData_CB(uint8_t status, uint16_t version, 
                                          uint8_t countryCode, uint8_t remoteWake, 
                                          uint8_t normallyConnectable))
{
}

WEAK_FUNCTION(void HID_BetteryLevelData_CB(uint8_t status, uint8_t namespace, uint16_t description, uint8_t level))
{
}

WEAK_FUNCTION(void HID_BatteryClientCahracDesc_CB(uint8_t status, uint8_t notification, uint8_t indication))
{
}

WEAK_FUNCTION(void HID_PnPID_CB(uint8_t status, uint8_t vendorIdSource, uint16_t vendorId, uint16_t productId, uint16_t productVersion))
{
}

WEAK_FUNCTION(void HID_BootReportValue_CB(uint8_t status, uint8_t dataLen, uint8_t *data))
{
}

WEAK_FUNCTION(void HID_ReadBootReportClientCharacDesc_CB(uint8_t status, uint8_t notification, uint8_t indication))
{
}

WEAK_FUNCTION(void HID_ProtocolMode_CB(uint8_t status, uint8_t mode))
{
}

WEAK_FUNCTION(void HID_SetProcedure_CB(uint8_t status))
{
}

WEAK_FUNCTION(void HID_NotificationChageStatus_CB(uint8_t status))
{
}

WEAK_FUNCTION(void HID_ReportDataReceived_CB(uint8_t type, uint8_t id, uint8_t data_length, uint8_t *data_value))
{
}

WEAK_FUNCTION(void HID_PinCodeRequired_CB(void))
{
}

WEAK_FUNCTION(void HID_PairingFailed_CB(void))
{
}

#endif /* (BLE_CURRENT_PROFILE_ROLES == HID_HOST) */
