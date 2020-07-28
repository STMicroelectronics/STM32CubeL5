/*******************************************************************************
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
*   FILENAME        -  hid_device.h
*
*   COMPILER        -  
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/06/2012
*   $Revision$:  first version
*   $Author$:    
*   Comments:    
*
********************************************************************************
*
*  File Description 
*  ---------------------
*  
*******************************************************************************/
#ifndef _HID_DEVICE_H_
#define _HID_DEVICE_H_

/*******************************************************************************
* Macros
*******************************************************************************/

/**
 * HID Profile configuration parameters 
 */
/**
 * maximum number of HID service instances
 * supported by the profile
 */ 
#define HID_SERVICE_MAX                         (5)

/**
 * maximum number of battery service instances
 * supported by the profile
 */ 
#define BATTERY_SERVICE_MAX                     (5)

/**
 * maximum number of input reports
 * supported by the profile
 */ 
#define INPUT_REPORT_NUM_MAX                    (5)

/**
 * maximum number of output reports
 * supported by the profile
 */ 
#define OUTPUT_REPORT_NUM_MAX                   (5)

/**
 * maximum number of feature reports
 * supported by the profile
 */ 
#define FEATURE_REPORT_NUM_MAX                  (5)

/**
 * maximum length of the report characteristic
 */ 
#define REPORT_CHAR_SIZE_MAX                    (32)       

/**
 * the maximum size of the buffer to hold
 * the reports
 */ 
#define REPORT_CHAR_VAL_BUFF_SIZE               ((INPUT_REPORT_NUM_MAX + OUTPUT_REPORT_NUM_MAX + FEATURE_REPORT_NUM_MAX)*(HID_SERVICE_MAX)*(REPORT_CHAR_SIZE_MAX))

/**
 * maximum length of the report map
 * characteristic
 */ 
#define REPORT_MAP_CHAR_SIZE_MAX                (256)

/**
 * maximum length of the buffer to hold the 
 * report map characteristic
 */ 
#define REPORT_MAP_CHAR_BUFF_SIZE_MAX           (HID_SERVICE_MAX * REPORT_MAP_CHAR_SIZE_MAX)

/**
 * maximum number of external report reference
 * characteristic descriptors
 */ 
#define EXT_REPORT_REF_CHAR_NUM_MAX             (5)

/**
 * maximum length of the keyboard
 * input report
 */ 
#define BOOT_KBD_IP_REPORT_CHAR_SIZE_MAX        (256)

/**
 * maximum length of the keyboard
 * output report
 */ 
#define BOOT_KBD_OP_REPORT_CHAR_SIZE_MAX        (256)

/**
 * maximum length of boot mouse
 * input report
 */ 
#define BOOT_MOUSE_IP_REPORT_CHAR_SIZE_MAX      (256)

/**
 * maximum length of the buffer to hold
 * the boot device reports
 */ 
#define BOOT_DEVICE_REPORT_CHAR_BUFF_SIZE_MAX   (HID_SERVICE_MAX * 256)

/**
 * maximum length of the HID information
 * characteristic value
 */ 
#define HID_INFO_CHAR_VAL_SIZE_MAX              (256)

/**
 * maximum length of the buffer to hold the
 * HID information characteristic value
 */ 
#define HID_INFO_CHAR_VAL_BUFF_SIZE_MAX         (HID_SERVICE_MAX * HID_INFO_CHAR_VAL_SIZE_MAX)


/**
 * HID Report types
 */ 
#define HID_REPORT_TYPE_INPUT         (0x01)
#define HID_REPORT_TYPE_OUTPUT        (0x02)
#define HID_REPORT_TYPE_FEATURE       (0x03)

/**
 * boot device masks
 */
#define BOOT_DEVICE_KEYBOARD_MASK      (0x01)
#define BOOT_DEVICE_MOUSE_MASK         (0x02) 

/**
 * bit masks  for the scan parameter service and characteristics
 */
#define SCAN_PARAMETER_SERVICE_MASK    (0x01)
#define SCAN_REFRESH_CHAR_MASK         (0x02)
															 
/*******************************************************************************
 * type definitions
*******************************************************************************/
/**
 * HID information characteristic
 */
typedef struct _tHidInfoChar
{
  /**
   * 16-bit unsigned integer representing version
   * number of base USB HID Specification 
   * implemented by HID Device
   */ 
  uint16_t bcdHID;
	
  /**
   * 8-bit integer identifying country HID Device 
   * hardware is localized for. Most 
   * hardware is not localized (value 0x00)
   */ 
  uint8_t bCountryCode;
	
  /**
   * Bit 0: RemoteWake - Boolean value indicating whether 
   * HID Device is capable of sending a wake-signal to a HID Host.\n
   * 0bXXXXXXX0 = FALSE\n
   * 0bXXXXXXX1 = TRUE\n
   * Bit 1: NormallyConnectable - Boolean value indicating 
   * whether HID Device will be advertising when bonded but not connected.
   * 0bXXXXXX0X = FALSE\n
   * 0xXXXXXX1X = TRUE
  */ 
  uint8_t flags;
}tHidInfoChar;

/**
 * non HID services to be added that
 * are referenced from the report map
 * descriptor
 */

typedef struct _tnonHIDService
{
  /**
   * service handle
   * for INTERNAL USE ONLY
   */
  uint16_t servhandle;

  /**
   * characteristic handle
   * for INTERNAL USE ONLY
   */
  uint16_t charHandle;
   
  /**
   * service UUID
   */ 
  uint16_t servUUID;
  
  /**
   * characteristic UUID
   */ 
  uint16_t charUUID;
  
  /**
   * maximum length of the characteristic value
   */ 
  uint8_t charValLen;
  
  /**
   * whether the length is variable or constant
   */
  uint8_t isVariable; 
	
}tnonHIDService; 

/**
 * HID service configuration parameters
 * to be provided by the application 
 * at the time of initialization
 */ 
typedef struct _tApplDataForHidServ
{
  /**
   * number of input reports
   */ 
  uint8_t inputReportNum;
      
  /**
   * length of each input report
   */ 
  uint8_t *ipReportLen;
      
  /**
   * report ID of each report. This should have IDs
   * in the same order as the lengths are specified 
   * for the reports in ipReportLenMax
   */ 
  uint8_t *ipReportID;

  /**
   * number of output reports
   */ 
  uint8_t outputReportNum;
      
  /**
   * length of each output report
   */ 
  uint8_t *opReportLen;
      
  /**
   * report ID of each output report
   */ 
  uint8_t *opReportID;

  /**
   * number of feature reports
   */ 
  uint8_t featureReportNum;
      
  /**
   * length of each feature report
   */ 
  uint8_t *ftrReportLen;
      
  /**
   * report ID of each feature report
   */ 
  uint8_t *ftrReportID;

  /**
   * length of the report map characteristic
   */ 
  uint8_t reportMapValueLen;
      
   /**
   * report descriptor
   */ 
  uint8_t *reportDesc;

  /**
   * number of characteristics which
   * are not a part of the HID service
   * that are referenced from the report 
   * map characteristic
   */ 
  uint8_t extReportCharRefNum;
      
  /**
   * list of the non HID services
   * to be added
   */ 
  tnonHIDService *nonHidServInfo;

  /**
   * boot mode protocol type of the profile
   * mouse(0x02) or keyboard(0x01)
   */ 
  uint8_t bootModeDeviceType; 
      
  /**
   * boot input report length
   */ 
  uint8_t bootIpReportLenMax;
      
  /**
   * boot output report length
   * valid only in the case of keyboard
   */ 
  uint8_t bootOpReportLenMax;
      
  /**
   * HID Information characteristic
   * value
   */ 
  tHidInfoChar hidInfoChar;

}tApplDataForHidServ;


/**
 * report characteristic structure
 */ 
typedef struct _tReportCharContext
{
  /**
   * handle of the report characteristic
   */ 
  uint16_t reportCharHandle;
      
  /**
   * state of the characteristic
   */ 
  uint8_t charState;
      
  /**
   * report ID
   */ 
  uint8_t reportID;
      
  /**
   * type of the report
   * (input, output or feature)
   */ 
  uint8_t reportType; 
      
  /**
   * length of the report
   */ 
  uint8_t reportValueLen; 
      
  /**
   * pointer to the report value
   */ 
  uint8_t * pReportValue;
    
}tReportCharContext;

/**
 * HID service context
 */ 
typedef struct _tHidServiceContext
{
  /**
   * handle of HID service
   */ 
  uint16_t hidServiceHandle;

  /**
   * protocol mode of the HID
   * BOOT_PROTOCOL(0x00)
   * REPORT_PROTOCOL (0x01)
   */ 
  uint8_t  protocolMode; 
      
  /**
   * handle of the protocol mode
   * characteristic
   */ 
  uint16_t protocolModeCharHandle;

  /**
   * number of input reports
   */ 
  uint8_t  inputReportCharNum;
      
  /**
   * input report currently under process
   */ 
  uint8_t  currIpReportCharIndx; 
      
  /**
   * input report
   */ 
  tReportCharContext inputReport[INPUT_REPORT_NUM_MAX];

  /**
   * number of output reports
   */ 
  uint8_t  outputreportCharNum;
      
  /**
   * output report currently under processing
   */ 
  uint8_t  currOpReportCharIndx; 
      
  /**
   * output report
   */ 
  tReportCharContext outputReport[OUTPUT_REPORT_NUM_MAX];

  /**
   * number of feature reports
   */ 
  uint8_t  featureReportCharNum; 
      
  /**
   * feature report currently under processing
   */ 
  uint8_t  currFtrReportCharIndx; 
      
  /**
   * feature report
   */ 
  tReportCharContext featureReport[FEATURE_REPORT_NUM_MAX];

  /**
   * report map characteristic handle
   */ 
  uint16_t reportMapCharHandle; 
      
  /**
   * length of the report descriptor
   */ 
  uint8_t  reportMapCharValueLen; 
      
  /**
   * report map characteristic value.
   * It maps to the report descriptor
   * of USB HID specification
   */ 
  uint8_t * pReportMapCharValue; 

  /**
   * number of characteristics which
   * are not a part of the HID service
   * that are referenced from the report 
   * map characteristic
   */ 
  uint8_t extReportRefCharDescNum; 
      
  /**
   * the external characteristic currently under
   * processing
   */ 
  uint8_t currExtReportRefIndx;
      
  /**
   * list of the non HID services referenced from the
   * reportmap characteristic. It contains
   * the service UUID, characteristic UUID, attribute length
   * and whether the attribute value is constant or variable
   */ 
  tnonHIDService nonHidServInfo[EXT_REPORT_REF_CHAR_NUM_MAX];
       
  /**
   * the type of device in boot mode
   * keyboard(0x01), mouse(0x02)
   */ 
  uint8_t  bootDeviceMode; 
      
  /**
   * boot keyboard input report
   */ 
  tReportCharContext  bootKbdInput;
      
  /**
   * boot keyboard output report
   */ 
  tReportCharContext  bootKbdOutput;

  /**
   * boot mouse input report
   */ 
  tReportCharContext  bootMouseInput;

  /**
   * HID information characteristic handle
   */ 
  uint16_t hidInfoCharHandle; 

  /**
   * HID Information characteristic
   * value
   */ 
  tHidInfoChar hidInfoChar;

  /**
   * HID control point characteristic handle
   */ 
  uint16_t hidCtlPointCharHandle; 
}tHidServiceContext;


/**
 * battery service
 */ 
typedef struct _tBatteryServiceContext
{
  /**
   * handle of the battery service
   */ 
  uint16_t batteryServHandle;
      
  /**
   * handle of the battery level
   * characteristic
   */ 
  uint16_t batteryLevlCharHandle; 
      
  /**
   * flag to keep track of the status of the
   * battery level characteristic
   */
   uint8_t battLvlAllowRead; 
}tBatteryServiceContext;


/**
 * scan parameters service parameters
 */ 
typedef struct _tScanParamService
{
  /**
   * characteristics supported by the
   * scan parameter service
   */ 
  uint8_t  serviceSupports;
      
  /**
   * scan parameter service handle
   */ 
  uint16_t scanParamServHandle; 
      
  /**
   * scan interval window characteristic
   */ 
  uint16_t scanIntervalWinCharHandle; 
      
  /**
   * scan refresh characteristic handle
   */ 
  uint16_t scanRefreshCharHandle;
}tScanParamService;


/******************************************************************************
 * function prototypes
******************************************************************************/
/**
 * HidDevice_Init
 * 
 * @param[in] numOfHIDServices : number of HID services to be exposed in the profile
 * @param[in] hidServiceData : pointer to the hid service data
 * @param[in] numOfBatteryServices : number of battery services to be exposed in the profile
 * @param[in] deviceInfoCharSuppoerts
 */ 
tBleStatus HidDevice_Init(uint8_t numOfHIDServices, 
                          tApplDataForHidServ * hidServiceData,
                          uint8_t numOfBatteryServices, 
                          uint16_t deviceInfoCharSupports, 
                          uint8_t scanParamServiceSupport, 
                          uint8_t scanRefreshCharSupport, 
                          BLE_CALLBACK_FUNCTION_TYPE hidDeviceCallbackFunc);


/**
 * HidDevice_Make_Discoverable
 * 
 * @param[in] useBoundedDeviceList: advertize with whitelist which contains previously
 *            bonded devices
 * 
 * puts the device into discoverable mode
 *
 * @return BLE_STATUS_SUCCESS if the command to put the device into
 * discoverable mode was issued successfully
 */
tBleStatus HidDevice_Make_Discoverable(uint8_t useBoundedDeviceList);

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
                                         uint8_t * ipReportValue);

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
                                           uint8_t *ftrReportValue);

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
                                                       uint8_t *bootKbdIpReportValue);

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
                                                    uint8_t * bootMouseIpReportValue);

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
                                          uint8_t batteryLevel);

/**
 * HidDevice_Update_Scan_Refresh_Char
 * 
 * @param[in] scanRefresh : value of the scan refresh characteristic
 * 
 * starts the update for the scan refresh characteristic
 * 
 * @return BLE_STATUS_SUCCESS if the update was successfully started
 */ 
tBleStatus HidDevice_Update_Scan_Refresh_Char(uint8_t scanRefresh);		

/**
 * Allow_BatteryLevel_Char_Read
 * 
 * @param[in] batteryServiceIndex : The battery servcie index
 * for whose characteristic the read has to be allowed
 * 
 * sends the allow read command to the controller
 */ 
tBleStatus Allow_BatteryLevel_Char_Read(uint8_t batteryServiceIndex);		

/**
 * HIDProfile_StateMachine
 * 
 * @param[in] None
 * 
   * HID profile's state machine: to be called on application main loop. 
 */ 
tBleStatus HIDProfile_StateMachine(void);

/**
 * HIDProfile_Add_Services_Characteristics
 * 
 * @param[in]: None
 *            
 * It adds HID service & related characteristics
*/ 
tBleStatus HIDProfile_Add_Services_Characteristics(void);

/**
* HIDProfile_Rx_Event_Handler
* 
* @param[in] pckt : Pointer to the ACI packet
* 
* It parses the events received from ACI according to the profile's state machine.
* Inside this function each event must be identified and correctly parsed.
* NOTE: It is the event handler to be called inside HCI_Event_CB() 
*/ 
void HIDProfile_Rx_Event_Handler(void *pckt);

#endif  /* _HID_DEVICE_H_ */
