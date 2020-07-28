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
*   FILENAME        -  time_client.c
*
*   COMPILER        -  gnu gcc
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      25/08/2014
*   $Revision$:  second version
*   $Author$:    
*   Comments:    Redesigned the Time Profile's implementation source file for client 
*                role according to unified BlueNRG DK framework.
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* This file have Time Profile's Client role's implementation. 
* 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include <host_config.h>
#include <hci.h>

#include "bluenrg_aci.h"
#include "bluenrg_gatt_server.h"
#include "hci_const.h"
#include "bluenrg_gap.h"
#include "sm.h"

#include <debug.h>
#include <ble_list.h>
#include <timer.h>
#include <ble_profile.h>
#include <ble_events.h>
#include <uuid.h>

#include <time_client.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* advertising intervals in terms of 
 * 625 micro seconds
 */
#define TIP_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define TIP_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define TIP_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define TIP_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */


/* time client profile main states */
#define TIME_CLIENT_STATE_UNINITIALIZED                 (0x00)
#define TIME_CLIENT_STATE_WAITING_BLE_INIT              (0x10)
#define TIME_CLIENT_STATE_INITIALIZED                   (0x30)
#define TIME_CLIENT_STATE_ADVERTIZE                     (0x40)
#define TIME_CLIENT_STATE_CONNECTED                     (0x60)
#define TIME_CLIENT_STATE_INVALID                       (0xFF)

/* Sub State for TIME_CLIENT_STATE_INITIALIZED main state */
#define TIME_CLIENT_STATE_WAITING_FOR_ADVERTISE_STATE                  (0X31)

/* Sub States for TIME_CLIENT_STATE_ADVERTIZE main state */
#define TIME_CLIENT_STATE_ADVERTISABLE_IDLE                            (0x41)
#define TIME_CLIENT_STATE_ADVERTISING_FOR_WHITE_LIST                   (0x42)
#define TIME_CLIENT_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP   (0x43)
#define TIME_CLIENT_STATE_FAST_DISCOVERABLE_DO_START                   (0x44)
#define TIME_CLIENT_STATE_FAST_DISCOVERABLE                            (0x45)
#define TIME_CLIENT_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP            (0x46)
#define TIME_CLIENT_STATE_LOW_POWER_DISCOVERABLE_DO_START              (0x47)
#define TIME_CLIENT_STATE_LOW_POWER_DISCOVERABLE                       (0x48)
#define TIME_CLIENT_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP       (0x49)
#define TIME_CLIENT_STATE_BLE_ADVERTISING                              (0x4A)
#define TIME_CLIENT_STATE_PROCESSING_PAIRING                           (0x4B)

/* Sub States for TIME_CLIENT_STATE_CONNECTED main state */
#define TIME_CLIENT_STATE_CONNECTED_IDLE                        	          (0x61)
#define TIME_CLIENT_STATE_DO_DISCONNECT                         		  (0x62)
#define TIME_CLIENT_STATE_DISCONNECTING                         		  (0x63)
#define TIME_CLIENT_STATE_DISCONNECTED                          		  (0x64)
#define TIME_CLIENT_STATE_EXCHANGING_CONFIG                                       (0x65)
#define TIME_CLIENT_STATE_EXCHG_CONFIG_COMPLETE                                   (0x66)
#define TIME_CLIENT_STATE_DISCOVERING_ALL_PRIMARY_SERVICES                        (0x67)
#define TIME_CLIENT_STATE_DISCOVERED_ALL_PRIMARY_SERVICES                         (0x68)
#define TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_CURRENT_TIME_SERVICE               (0x69)
#define TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_CURRENT_TIME_SERVICE                (0x6A)
#define TIME_CLIENT_STATE_DISCOVERING_ALL_DESCRIPTORS_OF_CURRENT_TIME_CHAR        (0x6B)
#define TIME_CLIENT_STATE_DISCOVERED_ALL_DESCRIPTORS_OF_CURRENT_TIME_CHAR         (0x6C)
#define TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_NEXT_DST_CHANGE_SERVICE            (0x6D)
#define TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_NEXT_DST_CHANGE_SERVICE             (0x6E)
#define TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_REFERENCE_UPDATE_TIME_SERVICE      (0x6F)
#define TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_REFERENCE_UPDATE_TIME_SERVICE       (0x70)
#define TIME_CLIENT_STATE_DISABLING_CURRENT_TIME_UPDATE_NOTIFICATION              (0x71)
#define TIME_CLIENT_STATE_DISABLED_CURRENT_TIME_UPDATE_NOTIFICATION               (0x72)
#define TIME_CLIENT_STATE_READING_CURRENT_TIME_VALUE                              (0x73)
#define TIME_CLIENT_STATE_READING_LOCAL_TIME_INFO                                 (0x74)
#define TIME_CLIENT_STATE_READING_REFERENCE_TIME_INFO                             (0x75)
#define TIME_CLIENT_STATE_READING_NEXT_DST_CHANGE_TIME                            (0x76)
#define TIME_CLIENT_STATE_READING_SERVER_TIME_UPDATE_STATUS                       (0x77)
#define TIME_CLIENT_STATE_WRITING_TIME_UPDATE_CTL_POINT                           (0x78)
#define TIME_CLIENT_STATE_WRITING_CURRENT_TIME_UPDATE_NOTIFICATION                (0x79)

#define UNPACK_2_BYTE_PARAMETER(ptr)  \
                (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
                (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))
/******************************************************************************
* Type definitions
******************************************************************************/
typedef struct _tTimeClientContext
{
  /**
   * main state of the heart rate
   * sensor state machine
   */ 
  tProfileState mainState;
      
  /**
   * sub state of the heart rate
   * sensor state machine
   */ 
  tProfileState subState;
 
  /**
   * connection handle 
   */ 
  uint16_t connHandle;
  
  /**
   * callback to notify the applciation with
   * the profile specific events
   */ 
  BLE_CALLBACK_FUNCTION_TYPE applicationNotifyFunc;
  
  /**
   * bit mask of the services found during the
   * discovery process on the reporter
   */ 
  uint16_t servicesFound;

  /**
   * handle of the current time service
   */ 
  uint16_t CurrentTimeServHandle;
      
  /**
   * End handle of the current time service
   */ 
  uint16_t CurrentTimeServEndHandle;
  
  /**
   * handle of the Next DST change service
   */ 
  uint16_t NextDSTServHandle;
  
  /**
   * End handle of the Next DST change service
   */ 
  uint16_t NextDSTServEndHandle;
  
  /**
   * handle of the Reference Time Update service
   */ 
  uint16_t RefUpdateTimeServHandle;
  
  /**
   * End handle of the Reference Time Update service
   */ 
  uint16_t RefUpdateTimeServEndHandle;
  
  /**
   * handle of the current time characteristic
   */ 
  uint16_t currentTimeCharHandle;
      
  /**
   * handle of the local time information characteristic
   */ 
  uint16_t localTimeInfoCharHandle;
      
  /**
   * handle of the reference time information characteristic
   */ 
  uint16_t refTimeInfoCharHandle;
      
  /**
   * handle of the time with DST characteristic
   */ 
  uint16_t timeWithDSTCharHandle;
      
  /**
   * handle of the time update control point characteristic
   */ 
  uint16_t timeUpdateCtlPointCharHandle;
      
  /**
   * handle of the time update state characteristic
   */ 
  uint16_t timeUpdateStateCharHandle;
      
  /**
   * ID of the timer started by the
   * time client
   */ 
  tTimerID timerID;
      
  /**
   * handle of the client configuration descriptor 
   * of the current time characteristic
   */ 
  uint16_t currentTimeCharConfigDescHandle;

  /**
   * profile interface context to be registered
   * with the main profile
   */ 
  tProfileInterfaceContext TCtoBLEInf;
}tTimeClientContext;

/******************************************************************************
 * Local Variable Declarations
******************************************************************************/
static tTimeClientContext timeClient;

/**
 * This is a temporary buffer used to hold
 * the gatt response until the procedure
 * completes and it can be given to the 
 * application
 */ 
uint8_t gattRespBuf[100];

/**
 * number of bytes of data in the
 * gattRespBuff
 */ 
uint8_t gattRespLen = 0 ;


uint16_t currentTimeCharDescValue = 0x0000;

/******************************************************************************
 * Global Variable Declarations
******************************************************************************/

/******************************************************************************
 * Imported Variable
******************************************************************************/
extern void *profiledbgfile;

/******************************************************************************
 * Function Declarations
******************************************************************************/
static tBleStatus TimeClient_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax);
static void TimeClient_GATT_Procedure_Complete_EvtHandler(uint8_t procedureCode);
static void TimeClient_Advertise_Period_Timeout_Handler(void);

static void TCProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState TCProfile_Read_MainStateMachine(void);
static void TCProfile_Write_SubStateMachine(tProfileState localsubState);
static tProfileState TCProfile_Read_SubStateMachine(void);

/******************************************************************************
* SAP
******************************************************************************/
tBleStatus TimeClient_Init(BLE_CALLBACK_FUNCTION_TYPE profileCallbackFunc)
{
  tBleStatus retval = BLE_STATUS_FAILED;

  TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Init(), Time Client Profile Init \n" );

  if (Is_Profile_Present(PID_TIME_CLIENT) == BLE_STATUS_SUCCESS)
  {
      return (BLE_STATUS_PROFILE_ALREADY_INITIALIZED);
  }
      
  if (profileCallbackFunc == NULL)
  {
      return (BLE_STATUS_NULL_PARAM);
  }

  /* Initialize Profile's Data Structures */
  BLUENRG_memset ( &timeClient, 0, sizeof(tTimeClientContext) );

  timeClient.TCtoBLEInf.profileID = PID_TIME_CLIENT;
  timeClient.TCtoBLEInf.callback_func = TimeClient_Event_Handler;
  timeClient.TCtoBLEInf.voteForAdvertisableState = 0;
  
  timeClient.applicationNotifyFunc = profileCallbackFunc;
  timeClient.timerID = 0xFF;
  
  retval = BLE_Profile_Register_Profile ( &timeClient.TCtoBLEInf );
  if (retval != BLE_STATUS_SUCCESS)
  {
    /* Profile could not be registered with the BLE main profile */
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Init(), Time Client Profile Init Failed: %02X \n", retval);
    return (retval);
  }
  
  /* Initialize profile's state machine's state variables */
  TCProfile_Write_MainStateMachine(TIME_CLIENT_STATE_WAITING_BLE_INIT);
  TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_WAITING_BLE_INIT);

  TIME_CLIENT_MESG_DBG( profiledbgfile, "TimeClient_Init(), Time Client Profile is Initialized \n");
  return (BLE_STATUS_SUCCESS);
}/* end TimeClient_Init() */

tBleStatus TimeClient_Make_Discoverable(uint8_t useBoundedDeviceList)
{
  tBleStatus retval = BLE_STATUS_FAILED;

  if ((TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_ADVERTIZE)  &&  
     (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_ADVERTISABLE_IDLE))
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Make_Discoverable(),  mode: %d \n", useBoundedDeviceList );
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Make_Discoverable(): MainState %02X, SubState %02X \n", 
                           TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());
    if ((useBoundedDeviceList) && (gBLEProfileContext.bleSecurityParam.bonding_mode))
    { 
      TIME_CLIENT_MESG_DBG ( profiledbgfile, "STARTING Advertising for White List devices \n");

      retval = aci_gap_set_undirected_connectable (PUBLIC_ADDR, WHITE_LIST_FOR_ALL);
      if ( retval == BLE_STATUS_SUCCESS)
      {
        /* start a timer of 10 seconds to try to connnect to white list device */
        Blue_NRG_Timer_Start (10, TimeClient_Advertise_Period_Timeout_Handler,&timeClient.timerID);
      
        /* change profile's sub states */
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_ADVERTISING_FOR_WHITE_LIST);
        /* request for main profile's state change */
        BLE_Profile_Change_Advertise_Sub_State ( &timeClient.TCtoBLEInf, 
                                                 BLE_PROFILE_STATE_ADVERTISING);
      }
      /* New code */
      else
      {
        TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Make_Discoverable(),  Failed: %02X \n", retval);
        /* Which state ? */
      }
    }
    else
    {
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_FAST_DISCOVERABLE_DO_START); 
    }
  }
  else
  {
   TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Make_Discoverable(): Invalid Profile State, MainState %02X, SubState %02X \n", 
                        TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());
  }

  return (retval);
}/* end TimeClient_Make_Discoverable() */

tBleStatus TimeClient_Get_Current_Time(void)
{
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  if ( (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED) && 
       (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_CONNECTED_IDLE) )
  {
      TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Get_Current_Time()\n");
      retval = aci_gatt_read_charac_val(timeClient.connHandle,timeClient.currentTimeCharHandle);
      if (retval == BLE_STATUS_SUCCESS)
      {
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_READING_CURRENT_TIME_VALUE);
      }
      /* Added new code */
      else
      {
        TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Get_Current_Time(), aci_gatt_read_charac_val() Failed: %02X \n", retval);
        /* Which state ? */
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
      }
  }
  else
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Time Client is not in proper state to read current Time value  MainState %02X, SubState %02X \n", 
                          TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
    
}/* end TimeClient_Get_Current_Time() */

/**
 * TimeClient_SetReset_Time_Update_Notification
 * @param[in] onoff : A value of 1 enables notifictions
 *            and 1 value of 0 disables notifications
 * 
 * By default notification of any change in Time on Time server 
 * will remain disabled. If the application on Time client need
 * the time update notification, it need to enable the notification
 * using this function.Once enabled, the notification of time 
 * update on time server will remain enabled. if the application 
 * dont want to receive the notification any more, it need to 
 * disable the notification using this function. 
 */
tBleStatus TimeClient_SetReset_Time_Update_Notification(uint8_t onoff)
{
  /* write characteristic descriptor to write 
   * client characteristic configuration descriptor 
   */
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  currentTimeCharDescValue = onoff ? 0x0001 : 0x0000 ;

  if((TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED) && 
     (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_CONNECTED_IDLE))
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_SetReset_Time_Update_Notification()\n");
    retval = aci_gatt_write_charac_descriptor (timeClient.connHandle,
                                               timeClient.currentTimeCharConfigDescHandle, 
                                               (uint8_t)2, 
                                               (uint8_t *)&currentTimeCharDescValue);
    if (retval == BLE_STATUS_SUCCESS)
    {
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_WRITING_CURRENT_TIME_UPDATE_NOTIFICATION);
    }
    /* Added new code */
    else
    {
      TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_SetReset_Time_Update_Notification(), aci_gatt_write_charac_descriptor() Failed: %02X \n", retval);
      /* Which state ? */
    }
  }
  else
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Time Client is not in proper state to write current time char client config descriptor  MainState %02X, SubState %02X \n", 
                          TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
}/* end TimeClient_SetReset_Time_Update_Notification() */

/**
 * TimeClient_Get_Local_Time_Information
 * 
 * Starts a gatt read procedure to read the local time
 * information characteristic on the server. The information read
 * will be passed to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Get_Local_Time_Information()
{
  /* Read Char value of local time information char */
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  if ((TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED) && 
              (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_CONNECTED_IDLE))
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Get_Local_Time_Information()\n");
    retval = aci_gatt_read_charac_val(timeClient.connHandle,timeClient.localTimeInfoCharHandle);
    if (retval == BLE_STATUS_SUCCESS)
    {
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_READING_LOCAL_TIME_INFO);
    }
    /* Added new code */
    else
    {
      TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Get_Local_Time_Information(), aci_gatt_read_charac_val() Failed: %02X \n", retval);
      /* Which state ? */
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
    }
  }
  else
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Time Client is not in proper state to read local time info from time server.  MainState %02X, SubState %02X \n", 
                           TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
}/* end TimeClient_Get_Local_Time_Information() */

/**
 * TimeClient_Get_Time_Accuracy_Info_Of_Server
 * 
 * Starts a gatt read procedure to read the reference time
 * information characteristic on the server. the information read
 * will be passed to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Get_Time_Accuracy_Info_Of_Server()
{
  /* Read Char value of Reference Time Information char */
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  if ( (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED) && 
       (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_CONNECTED_IDLE) )
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Get_Time_Accuracy_Info_Of_Server()\n");
    retval = aci_gatt_read_charac_val(timeClient.connHandle,timeClient.refTimeInfoCharHandle);
    if (retval == BLE_STATUS_SUCCESS)
    {
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_READING_REFERENCE_TIME_INFO);
    }
    /* Added new code */
    else
    {
      TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Get_Time_Accuracy_Info_Of_Server(), aci_gatt_read_charac_val() Failed: %02X \n", retval);
      /* Which state ? */
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
    }
  }
  else
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Time Client is not in proper state to read reference time info from time server.  MainState %02X, SubState %02X \n", 
                           TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
}/* end TimeClient_Get_Time_Accuracy_Info_Of_Server() */

/**
 * TimeClient_Get_Next_DST_Change_Time
 * 
 * Starts a gatt read procedure to read the time with DST
 * characteristic on the server. The information read
 * will be passed to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Get_Next_DST_Change_Time()
{
  /* Read Char value of Time With DST char */
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  
  if ( (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED) && 
       (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_CONNECTED_IDLE) )
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Get_Next_DST_Change_Time()\n");
    retval = aci_gatt_read_charac_val(timeClient.connHandle,timeClient.timeWithDSTCharHandle);
    if (retval == BLE_STATUS_SUCCESS)
    {
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_READING_NEXT_DST_CHANGE_TIME);
    }
    else
    {
      TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Get_Next_DST_Change_Time(), aci_gatt_read_charac_val() Failed: %02X \n", retval);
      /* Which state ? */
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
    }
  }
  else
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Time Client is not in proper state to read Next DST change time. MainState %02X, SubState %02X \n", 
                           TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
}/* end TimeClient_Get_Next_DST_Change_Time() */

/**
 * TimeClient_Get_Server_Time_Update_State
 * 
 * Starts a gatt read procedure to read the time update state
 * characteristic on the server. The information read
 * will be passed to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Get_Server_Time_Update_State()
{
  /* Read Char value of Time Update State Char */
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
  
  if ( (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED) && 
       (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_CONNECTED_IDLE) )
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Get_Server_Time_Update_State()\n");
    retval = aci_gatt_read_charac_val (timeClient.connHandle,timeClient.timeUpdateStateCharHandle);
    if (retval == BLE_STATUS_SUCCESS)
    {
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_READING_SERVER_TIME_UPDATE_STATUS);
    }
    else
    {
      TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Get_Server_Time_Update_State(), aci_gatt_read_charac_val() Failed: %02X \n", retval);
      /* Which state ? */
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
    }
  }
  else
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Time Client is not in proper state to read time update status.  MainState %02X, SubState %02X \n", 
                           TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
}/* end TimeClient_Get_Server_Time_Update_State() */

/**
 * TimeClient_Update_Reference_Time_On_Server
 * 
 * @param[in] ctlValue: the value passed starts
 * a procedure to cancel or start a update process\n
 * 
 * Starts a gatt write without response procedure to 
 * write the time update control point characteristic  
 * on the server. The information read will be passed 
 * to the application in the form of an event
 * 
 */ 
tBleStatus TimeClient_Update_Reference_Time_On_Server(uint8_t ctlValue)
{
  /* write without response for Time Update Control Point Char*/
  tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

  if ((ctlValue != GET_REFERENCE_UPDATE) && (ctlValue != CANCEL_REFERENCE_UPDATE))
  {
    return (retval);
  }

  if ( (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED) && 
       (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_CONNECTED_IDLE) )
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Update_Reference_Time_On_Server()\n");
    retval = aci_gatt_write_without_response(timeClient.connHandle,
                                             timeClient.timeUpdateCtlPointCharHandle, 
                                             (uint8_t)1, /* control point value is of 1 byte */
                                             &ctlValue);
    /* A command complete event is generated when this command is processed */
    if (retval == BLE_STATUS_SUCCESS)
    {
      if (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED)
      {
        /* time update control instruction has been sent to time server */
        TIME_CLIENT_MESG_DBG( profiledbgfile, "Time Client has wrote time update control point value to server \n");
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
      }
    }
    else
    {
      TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Update_Reference_Time_On_Server(), aci_gatt_write_without_response() Failed: %02X \n", retval);
      /* Which state ? */
    }
  }
  else
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Time Client is not in proper state to write time update control order to time server.  MainState %02X, SubState %02X \n", 
                           TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());
    retval = BLE_STATUS_FAILED;
  }
  return (retval);
}/* end TimeClient_Update_Reference_Time_On_Server() */


/* TO Be DONE ???? */



/******************************************************************************
* Local Functions
******************************************************************************/

static void TCProfile_Write_MainStateMachine(tProfileState localmainState)
{
  timeClient.mainState = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState TCProfile_Read_MainStateMachine(void)
{
  return(timeClient.mainState);
}

static void TCProfile_Write_SubStateMachine(tProfileState localsubState)
{
  timeClient.subState = localsubState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState TCProfile_Read_SubStateMachine(void)
{
  return(timeClient.subState);
}

/* TimeClient_Set_Discoverable() */
tBleStatus TimeClient_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  const char localName[] = {0x08,'C','l','i','e','n','t'};
  
  retval = aci_gap_set_limited_discoverable(ADV_IND,
                                            AdvIntervMin, 
                                            AdvIntervMax,
                                            PUBLIC_ADDR, 
                                            NO_WHITE_LIST_USE, 
                                            0x07, 
                                            localName, 
                                            gBLEProfileContext.advtServUUIDlen, 
                                            gBLEProfileContext.advtServUUID,    
                                            0, 
                                            0);          
  return retval;
}/* end TimeClient_Set_Discoverable() */


/**
 * TimeClient_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 * 
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB().
 */ 
void TimeClient_Event_Handler(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

  TIME_CLIENT_MESG_DBG ( profiledbgfile, "Time Client profile MainState %02X, SubState %02X \n", 
                         TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());

  switch(event_pckt->evt)
  {
    case EVT_DISCONN_COMPLETE:
    {
      TIME_CLIENT_MESG_DBG( profiledbgfile, "TimeClient_Event_Handler(), Received event EVT_DISCONN_COMPLETE \n");
      if ( ( (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_ADVERTIZE) &&
            (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_PROCESSING_PAIRING)) || 
           (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED) )
     {
        evt_disconn_complete *cc = (void *)event_pckt->data;
        
        TIME_CLIENT_MESG_DBG( profiledbgfile,"TimeClient_Event_Handler(), Received an EVT_DISCONN_COMPLETE %02X \n",cc->status);
        if (cc->status == BLE_STATUS_SUCCESS)
        {
          /* Profile's State Change */
          TCProfile_Write_MainStateMachine(TIME_CLIENT_STATE_ADVERTIZE);
          TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_ADVERTISABLE_IDLE);
          
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Event_Handler(): MainState %02X, SubState %02X \n", 
                         TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());
        }
      }
    }
    break;
    
    case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      TIME_CLIENT_MESG_DBG( profiledbgfile,"TimeClient_Event_Handler(), Received an EVT_LE_META_EVENT %04X \n",evt->subevent);
      
      switch(evt->subevent)
      {
        case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
        
          TIME_CLIENT_MESG_DBG(profiledbgfile,"TimeClient_Event_Handler(), Received event EVT_LE_CONN_COMPLETE \n");
         
          /* check event status */
          if (cc->status == BLE_STATUS_SUCCESS)
          {
            timeClient.connHandle = cc->handle;
            if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTED)
            {
              TCProfile_Write_MainStateMachine(TIME_CLIENT_STATE_CONNECTED);
              TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_EXCHANGING_CONFIG);

              /* the MTU size has to be negotiated first */
              BLE_Profile_Start_Configuration_Exchange();
            }
            else
            {
              /* connection req is accepted and pairing process started */
              TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_PROCESSING_PAIRING);
            }
          }
          else
          {
            /* connection was not successful, now we need to wait for
             * the application to put the device in discoverable mode
             */
            TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_ADVERTISABLE_IDLE);
          }
          /* Here there is no need to do anything extra if this profile is not
           * controlling the advertising process. Above if-else statement does
           * the needful. */
          /* Stop if any timer is running */
          
        }
        break; /* EVT_LE_CONN_COMPLETE */
      } /* switch(response->leEvent.subEventCode) */
    }
    break; /* EVT_LE_META_EVENT */
    
    case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      
      TIME_CLIENT_MESG_DBG( profiledbgfile, "TimeClient_Event_Handler(), Received an VENDOR_SPECIFIC EVENT %04X \n", blue_evt->ecode);

      switch (blue_evt->ecode)
      {
        case EVT_BLUE_GAP_LIMITED_DISCOVERABLE:
        {
          TIME_CLIENT_MESG_DBG( profiledbgfile, "TimeClient_Event_Handler(), Received EVT_BLUE_GAP_LIMITED_DISCOVERABLE \n");

          if ((TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_ADVERTIZE) && 
              ((TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_FAST_DISCOVERABLE) ||
               (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_LOW_POWER_DISCOVERABLE)))
          {
            /* Put the main profile in connectable idle state */
            BLE_Profile_Change_Advertise_Sub_State ( &timeClient.TCtoBLEInf, BLE_PROFILE_STATE_CONNECTABLE_IDLE);
            TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Event_Handler(), Limited Discoverable mode has timed out \n" );
            if(TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_LOW_POWER_DISCOVERABLE) 
            {
              /* Limited Discoverable mode has timed out */
              TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_ADVERTISABLE_IDLE); 
              
              /* notify the application */
              timeClient.applicationNotifyFunc(EVT_MP_ADVERTISING_TIMEOUT,0x00,NULL);
            }
          }
        }
        break;
        
        case EVT_BLUE_GAP_PAIRING_CMPLT:
        {
          evt_gap_pairing_cmplt *pr = (void*)blue_evt->data;
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Event_Handler(), Received EVT_BLUE_GAP_PAIRING_CMPLT \n");

          if ((TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_ADVERTIZE) && 
              (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_PROCESSING_PAIRING))
          {
            if (pr->status == BLE_STATUS_SUCCESS)
            {
              TCProfile_Write_MainStateMachine(TIME_CLIENT_STATE_CONNECTED);
              TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_EXCHANGING_CONFIG);
            
              /* the MTU size has to be negotiated first */
              BLE_Profile_Start_Configuration_Exchange();
            }
          }
        }
        break; /* EVT_BLUE_GAP_PAIRING_CMPLT */
        
        case EVT_BLUE_ATT_FIND_INFORMATION_RESP:
        {
          evt_att_find_information_resp *pr = (void*)blue_evt->data;
	  uint8_t numDesc, idx, i;
	  uint16_t uuid;
          /*
	   * event data will be of the format
	   * 2 bytes handle
	   * 2 bytes UUID
	   */ 
          
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Event_Handler(),Received EVT_BLUE_ATT_FIND_INFORMATION_RESP \n");

          if((TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED) &&
             (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_DISCOVERING_ALL_DESCRIPTORS_OF_CURRENT_TIME_CHAR))
          {
            numDesc = (pr->event_data_length - 1) / 4;
	    /* we are interested only in 16 bit Handle & UUIDs */
	    idx = 2;
	    if (pr->format == UUID_TYPE_16) 
	    {
	      for (i=0; i<numDesc; i++)
	      {
                uuid = UNPACK_2_BYTE_PARAMETER(&pr->handle_uuid_pair[idx]);
                
		if (uuid == CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID)
		{
		  TIME_CLIENT_MESG_DBG(profiledbgfile, "TimeClient_Event_Handler(), Client configuration UUID 0x%04x\n", uuid);
                  
                  if (!timeClient.currentTimeCharConfigDescHandle)
		  {
		    timeClient.currentTimeCharConfigDescHandle= UNPACK_2_BYTE_PARAMETER(&pr->handle_uuid_pair[idx-2]);
		    TIME_CLIENT_MESG_DBG(profiledbgfile, "TimeClient_Event_Handler(), Client configuration Char Desc Handle 0x%04x\n", 
                                         timeClient.currentTimeCharConfigDescHandle);
		  }
                }
                idx += 4;
              }
            }
          }
        }
        break; /* EVT_BLUE_ATT_FIND_INFORMATION_RESP */
        
        case EVT_BLUE_ATT_READ_BY_TYPE_RESP:
        {
          evt_att_read_by_type_resp *pr = (void*)blue_evt->data;
	  uint8_t idx;
	  uint16_t uuid;
          
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Event_Handler(), EVT_BLUE_ATT_READ_BY_TYPE_RESP \n");
          if ( (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED) && 
               ( (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_CURRENT_TIME_SERVICE) || 
                 (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_NEXT_DST_CHANGE_SERVICE) || 
                 (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_REFERENCE_UPDATE_TIME_SERVICE) ) )
          {
            idx = 5;
	    /* we are interested in only 16 bit UUIDs */
	    if (pr->handle_value_pair_length == 7)
	    {
	      pr->event_data_length -= 1;
	      while(pr->event_data_length > 0)
	      {
		uuid = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx]);
                
                if (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_CURRENT_TIME_SERVICE)
                {
                  if (uuid == CURRENT_TIME_CHAR_UUID)
                  {
                    timeClient.servicesFound |= CURRENT_TIME_CHAR_MASK;
                    timeClient.currentTimeCharHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]);
                    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Discovered Current Time Char, uuid: 0x%04x, handle: 0x%04x\n", uuid,timeClient.currentTimeCharHandle);
                  }
                  else if (uuid == LOCAL_TIME_INFORMATION_CHAR_UUID)
                  {
                    timeClient.servicesFound |= LOCAL_TIME_INFORMATION_CHAR_MASK;
                    timeClient.localTimeInfoCharHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]);
                    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Discovered Local Time Info Char, uuid: 0x%04x, handle: 0x%04x\n", uuid,timeClient.localTimeInfoCharHandle);
                  }
                  else if (uuid == REFERENCE_TIME_INFORMATION_CHAR_UUID)
                  {
                    timeClient.servicesFound |= REFERENCE_TIME_INFORMATION_CHAR_MASK;
                    timeClient.refTimeInfoCharHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]);
                    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Discovered Reference Time Info Char, uuid: 0x%04x, handle: 0x%04x\n", uuid,timeClient.refTimeInfoCharHandle);
                  }
                }
                else if (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_NEXT_DST_CHANGE_SERVICE)
                {
                  if (uuid == TIME_WITH_DST_CHAR_UUID)
                  {
                    timeClient.servicesFound |= TIME_WITH_DST_CHAR_MASK;
                    timeClient.timeWithDSTCharHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]);
                    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Discovered Time With DST Char, uuid: 0x%04x, handle: 0x%04x\n", uuid,timeClient.timeWithDSTCharHandle);
                  }
                }
                else if (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_REFERENCE_UPDATE_TIME_SERVICE)
                {
                  if (uuid == TIME_UPDATE_CONTROL_POINT_CHAR_UUID)
                  {
                    timeClient.servicesFound |= TIME_UPDATE_CONTROL_POINT_CHAR_MASK;
                    timeClient.timeUpdateCtlPointCharHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]);
                    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Discovered Time Update Control Point Char, uuid: 0x%04x, handle: 0x%04x\n", uuid,timeClient.timeUpdateCtlPointCharHandle);
                  }
                  else if (uuid == TIME_UPDATE_STATE_CHAR_UUID)
                  {
                    timeClient.servicesFound |= TIME_UPDATE_STATE_CHAR_MASK;
                    timeClient.timeUpdateStateCharHandle = UNPACK_2_BYTE_PARAMETER(&pr->handle_value_pair[idx-2]);
                    TIME_CLIENT_MESG_DBG ( profiledbgfile, "Discovered Time Update Status Char, uuid: 0x%04x, handle: 0x%04x\n", uuid,timeClient.timeUpdateStateCharHandle);
                  }
                }
                pr->event_data_length -= 7;
		idx += 7;
              }
            }
          }
        }
        break; /* EVT_BLUE_ATT_READ_BY_TYPE_RESP */
        
        case EVT_BLUE_ATT_READ_RESP : 
        {
          evt_att_read_resp *pr = (void*)blue_evt->data;
          uint8_t i;
          
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "----- TimeClient_Event_Handler(), EVT_BLUE_ATT_READ_RESP, Value: \n");
          if (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED)
          { 
            for (i=0; i<pr->event_data_length; i++)
                TIME_CLIENT_MESG_DBG(profiledbgfile," 0x%02x", pr->attribute_value[i]);
  
            if (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_READING_CURRENT_TIME_VALUE)
            {
              if (pr->event_data_length == 10)
              {
                gattRespLen = pr->event_data_length;
                BLUENRG_memcpy(gattRespBuf,&pr->attribute_value[0],gattRespLen);
                TIME_CLIENT_MESG_DBG ( profiledbgfile, " length: 10\n");
              }
            }
            else if (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_READING_LOCAL_TIME_INFO)
            {
              if (pr->event_data_length == 2)
              {
                gattRespLen = pr->event_data_length;
                BLUENRG_memcpy(gattRespBuf,&pr->attribute_value[0],gattRespLen);
                TIME_CLIENT_MESG_DBG ( profiledbgfile, " length: 2 (READING_LOCAL_TIME_INFO)\n");
              }
            }
            else if (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_READING_REFERENCE_TIME_INFO)
            {
              if (pr->event_data_length == 4)
              {
                gattRespLen = pr->event_data_length;
                BLUENRG_memcpy(gattRespBuf,&pr->attribute_value[0],gattRespLen);
                TIME_CLIENT_MESG_DBG ( profiledbgfile, " length: 4, value: 0x%04x\n",pr->attribute_value[0]);
              }
            }
            else if (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_READING_NEXT_DST_CHANGE_TIME)
            {
              if (pr->event_data_length == 8)
              {
                gattRespLen = pr->event_data_length;
                BLUENRG_memcpy(gattRespBuf,&pr->attribute_value[0],gattRespLen);
                TIME_CLIENT_MESG_DBG ( profiledbgfile, " length: 8\n");
              }
            }
            else if (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_READING_SERVER_TIME_UPDATE_STATUS)
            {
              if (pr->event_data_length == 2)
              {
                gattRespLen = pr->event_data_length;
                BLUENRG_memcpy(gattRespBuf,&pr->attribute_value[0],gattRespLen);
                TIME_CLIENT_MESG_DBG ( profiledbgfile, " length: 2 (READING_SERVER_TIME_UPDATE_STATUS)\n");
              }
            }
          }
        }
        break; /* EVT_BLUE_ATT_READ_RESP */
        
        case  EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
        {
          evt_att_read_by_group_resp *pr = (void*)blue_evt->data;
	  uint8_t numServ, i, idx;
	  uint16_t uuid;
          
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Event_Handler(), EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP \n");
          if ((TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED) && 
              (TCProfile_Read_SubStateMachine() == TIME_CLIENT_STATE_DISCOVERING_ALL_PRIMARY_SERVICES))
          {
            numServ = (pr->event_data_length  - 1) / pr->attribute_data_length;    
            /**
             * Resp event group data fromat:
             * uint16_t Start_Handle,
             * uint16_t End_Handle, 
             * uint16_t UUID
             * We are interested only if the UUID is 16 bit.
             * So check if the data length is 6.
             */
            if (pr->attribute_data_length == 6)
	    {
	      idx = 4;
	      for (i=0; i<numServ; i++)
	      {
		uuid = UNPACK_2_BYTE_PARAMETER(&pr->attribute_data_list[idx]); 
                if (uuid == CURRENT_TIME_SERVICE_UUID)
                {
                  timeClient.servicesFound |= CURRENT_TIME_SERVICE_MASK;
                  timeClient.CurrentTimeServHandle = UNPACK_2_BYTE_PARAMETER(&pr->attribute_data_list[idx-4]);
                  timeClient.CurrentTimeServEndHandle = UNPACK_2_BYTE_PARAMETER (&pr->attribute_data_list[idx-2]);
                  TIME_CLIENT_MESG_DBG(profiledbgfile,"found the current time service : start handle 0x%02x, end handle 0x%02x\n",timeClient.CurrentTimeServHandle,timeClient.CurrentTimeServEndHandle);
                }
                else if (uuid == NEXT_DST_CHANGE_SERVICE_UUID)
                {
                  timeClient.servicesFound |= NEXT_DST_CHANGE_SERVICE_MASK;
                  timeClient.NextDSTServHandle = UNPACK_2_BYTE_PARAMETER(&pr->attribute_data_list[idx-4]);
                  timeClient.NextDSTServEndHandle = UNPACK_2_BYTE_PARAMETER (&pr->attribute_data_list[idx-2]);
                  TIME_CLIENT_MESG_DBG(profiledbgfile,"found the next DST change service : start handle 0x%02x, end handle 0x%02x\n",timeClient.NextDSTServHandle,timeClient.NextDSTServEndHandle);
                }
                else if (uuid == REFERENCE_UPDATE_TIME_SERVICE_UUID)
                {
                  timeClient.servicesFound |= REFERENCE_UPDATE_TIME_SERVICE_MASK;
                  timeClient.RefUpdateTimeServHandle = UNPACK_2_BYTE_PARAMETER(&pr->attribute_data_list[idx-4]);
                  timeClient.RefUpdateTimeServEndHandle = UNPACK_2_BYTE_PARAMETER (&pr->attribute_data_list[idx-2]);
                  TIME_CLIENT_MESG_DBG(profiledbgfile,"found the reference update time service : start handle 0x%02x, end handle 0x%02x\n",timeClient.RefUpdateTimeServHandle,timeClient.RefUpdateTimeServEndHandle);
                }
                idx += 6;
              }
            }
          }
        }
        break;/* EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP */
        case EVT_BLUE_GATT_NOTIFICATION:
        {
          evt_gatt_attr_notification *pr = (void*)blue_evt->data;
          uint8_t i;
          
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Event_Handler(), Received EVT_BLUE_GATT_NOTIFICATION \n");
          if (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_CONNECTED)
          {
            if ( (pr->attr_handle == timeClient.currentTimeCharHandle) && 
                 (pr->event_data_length == (2+10)) ) 
            {
               TIME_CLIENT_MESG_DBG(profiledbgfile,"---- Received notification,  Handle 0x%04x, Lenght 0x%02x\n",
                                    pr->attr_handle, pr->event_data_length);
              
              TIME_CLIENT_MESG_DBG(profiledbgfile,"\n Current Time Value: ");
              for (i=0; i<pr->event_data_length-2; i++)
                TIME_CLIENT_MESG_DBG(profiledbgfile," 0x%02x", pr->attr_value[i]);
              
              timeClient.applicationNotifyFunc(EVT_TC_CUR_TIME_VAL_RECEIVED,pr->event_data_length-2,&pr->attr_value[0]);
            }
            /* here we need some type of protection for received current time value */
          }
        }
        break;/* end EVT_BLUE_GATT_NOTIFICATION */
        case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
        {
          evt_gatt_procedure_complete *pr = (void*)blue_evt->data;
          
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "Received EVT_BLUE_GATT_PROCEDURE_COMPLETE with Error Code 0x%02x\n", pr->error_code);
          /* we know the procedure has completed, so move the state machine to
           * the next state
           */ 
           TimeClient_GATT_Procedure_Complete_EvtHandler(pr->error_code); //TBR
        }
        break;/* EVT_BLUE_GATT_PROCEDURE_COMPLETE */
        case EVT_BLUE_GATT_ERROR_RESP:
        {
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Event_Handler(), Received EVT_BLUE_GATT_ERROR_RESP \n");
        }
        break;
      }
    }
    break; /* HCI_EVT_VENDOR_SPECIFIC */
  }
}/* end TimeClient_Event_Handler() */

/**
 * TimeClient_GATT_Procedure_Complete_EvtHandler
 * 
 * @param[in] procedureCode : opcode of the gatt procedure that
 *            has completed
 * 
 * Changes the profile substate according to the result of the procedure
 * and also starts the next action that needs to be taken 
 */ 
void TimeClient_GATT_Procedure_Complete_EvtHandler(uint8_t procedureCode/* tHciCommandOpcode procedureCode*/)
{
  tBleStatus retval = BLE_STATUS_FAILED;

  TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_GATT_Procedure_Complete_EvtHandler %04X\n", procedureCode );
  TIME_CLIENT_MESG_DBG ( profiledbgfile, "Time Client profile MainState %02X, SubState %02X \n", 
                         TCProfile_Read_MainStateMachine() , TCProfile_Read_SubStateMachine());
  
  if (TCProfile_Read_MainStateMachine() != TIME_CLIENT_STATE_CONNECTED)
  {
    TIME_CLIENT_MESG_DBG ( profiledbgfile, "The Device is not in connected state, no GATT Client procedure can run.  \n");
    return;
  }

  switch(TCProfile_Read_SubStateMachine())
  {
    case TIME_CLIENT_STATE_EXCHANGING_CONFIG:
    {
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_EXCHG_CONFIG_COMPLETE);
    }
    case TIME_CLIENT_STATE_EXCHG_CONFIG_COMPLETE:
    {
      
      retval = aci_gatt_disc_all_prim_services(timeClient.connHandle);
      if (retval == BLE_STATUS_SUCCESS)
      {
        TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_GATT_Procedure_Complete_EvtHandler(), aci_gatt_disc_all_prim_services() start OK \n"); 
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERING_ALL_PRIMARY_SERVICES); 
      }
      else
      {
        /* Service discovery failed, retry */ 
        TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_GATT_Procedure_Complete_EvtHandler(), aci_gatt_disc_all_prim_services() failed %02X \n",retval); 
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_EXCHG_CONFIG_COMPLETE);
      }
    }
    break;
    case TIME_CLIENT_STATE_DISCOVERING_ALL_PRIMARY_SERVICES:
    {
      if(timeClient.servicesFound & CURRENT_TIME_SERVICE_MASK)
      {
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERED_ALL_PRIMARY_SERVICES); 
      }
      else
      {
        /* we have not found the mandatory current time service for time client, 
         * hence exit the time client state machine 
         */ 
        TCProfile_Write_MainStateMachine(TIME_CLIENT_STATE_UNINITIALIZED);
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_UNINITIALIZED);

        retval = CT_SERVICE_NOT_FOUND;
        timeClient.applicationNotifyFunc(EVT_TC_DISCOVERY_CMPLT,1,&retval);

        /* Do profile exit process */
        /* Unregister the profile from main profile registered list */
        BLE_Profile_Unregister_Profile ( &timeClient.TCtoBLEInf );
        break;
      }
    }
    case TIME_CLIENT_STATE_DISCOVERED_ALL_PRIMARY_SERVICES:
    {
      retval = aci_gatt_disc_all_charac_of_serv(timeClient.connHandle,
                                                timeClient.CurrentTimeServHandle, 
                                                timeClient.CurrentTimeServEndHandle);
      if (retval == BLE_STATUS_SUCCESS)
      {
          TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_CURRENT_TIME_SERVICE);
      }
      else
      {
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERED_ALL_PRIMARY_SERVICES);
      }
    }
    break;
    case TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_CURRENT_TIME_SERVICE:
    {
      if (timeClient.servicesFound & CURRENT_TIME_CHAR_MASK)
      {
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_CURRENT_TIME_SERVICE);
      }
      else
      {
        /* we have not found the mandatory current time characteristic of current time service, 
         * hence exit the time client state machine 
         */ 
        TCProfile_Write_MainStateMachine(TIME_CLIENT_STATE_UNINITIALIZED);
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_UNINITIALIZED);

                            retval = CT_CHARACTERISTIC_NOT_FOUND;
        timeClient.applicationNotifyFunc(EVT_TC_DISCOVERY_CMPLT,1,&retval);

        /* Do profile exit process */
        /* Unregister the profile from main profile registered list */
        BLE_Profile_Unregister_Profile ( &timeClient.TCtoBLEInf );
        break;
      }
    }
    case TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_CURRENT_TIME_SERVICE:
    {
      retval = aci_gatt_disc_all_charac_descriptors(timeClient.connHandle,
                                                    timeClient.currentTimeCharHandle, 
                                                    timeClient.CurrentTimeServEndHandle);
      if (retval == BLE_STATUS_SUCCESS)
      {
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERING_ALL_DESCRIPTORS_OF_CURRENT_TIME_CHAR);
      }
      else
      {
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_CURRENT_TIME_SERVICE);
      }
    }
    break;
    case TIME_CLIENT_STATE_DISCOVERING_ALL_DESCRIPTORS_OF_CURRENT_TIME_CHAR:
    {
      /* discover all descriptors of current time char is complete */
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERED_ALL_DESCRIPTORS_OF_CURRENT_TIME_CHAR);
    }
    case TIME_CLIENT_STATE_DISCOVERED_ALL_DESCRIPTORS_OF_CURRENT_TIME_CHAR:
    {
      /* check whether the Next DST Change service is supported at time server or not ? */ 
      if (timeClient.servicesFound & NEXT_DST_CHANGE_SERVICE_MASK)
      {
        retval = aci_gatt_disc_all_charac_of_serv(timeClient.connHandle, 
                                                  timeClient.NextDSTServHandle, 
                                                  timeClient.NextDSTServEndHandle);
        if (retval == BLE_STATUS_SUCCESS)
        {
           TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_NEXT_DST_CHANGE_SERVICE);
        }
        else
        {
          TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERED_ALL_DESCRIPTORS_OF_CURRENT_TIME_CHAR);
        }
      }
      else
      {
        /* skip all processing related to next dst change service */
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_NEXT_DST_CHANGE_SERVICE);
        TimeClient_GATT_Procedure_Complete_EvtHandler (0);
      }
    }
    break;
    case TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_NEXT_DST_CHANGE_SERVICE:
    {
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_NEXT_DST_CHANGE_SERVICE);

      if (timeClient.servicesFound & TIME_WITH_DST_CHAR_MASK)
      {
        /* mandatory char of Next DST change service is not found, hence the service also
         * can not be supported */
        /* reset service and char mask bit */
        timeClient.servicesFound &= ~(NEXT_DST_CHANGE_SERVICE_MASK);
      }
    }
    case TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_NEXT_DST_CHANGE_SERVICE:
    {
      if (timeClient.servicesFound & REFERENCE_UPDATE_TIME_SERVICE_MASK)
      {
        retval = aci_gatt_disc_all_charac_of_serv (timeClient.connHandle,
                                                   timeClient.RefUpdateTimeServHandle, 
                                                   timeClient.RefUpdateTimeServEndHandle );
        if (retval == BLE_STATUS_SUCCESS)
        {
            TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_REFERENCE_UPDATE_TIME_SERVICE);
        }
        else
        {
          TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_NEXT_DST_CHANGE_SERVICE);
        }
      }
      else
      {
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_REFERENCE_UPDATE_TIME_SERVICE);
        TimeClient_GATT_Procedure_Complete_EvtHandler (0);
      }
    }
    break;
    case TIME_CLIENT_STATE_DISCOVERING_CHARS_OF_REFERENCE_UPDATE_TIME_SERVICE:
    {
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_REFERENCE_UPDATE_TIME_SERVICE);

      if ( (timeClient.servicesFound & TIME_UPDATE_CONTROL_POINT_CHAR_MASK) &&
           (timeClient.servicesFound & TIME_UPDATE_STATE_CHAR_MASK) )
      {
          /* mandatory char of Reference Time Update Service is not found, hence the service also
           * can not be supported */
          /* reset service and char mask bit */
          timeClient.servicesFound &= ~(REFERENCE_UPDATE_TIME_SERVICE_MASK | 
                                        TIME_UPDATE_CONTROL_POINT_CHAR_MASK | 
                                        TIME_UPDATE_STATE_CHAR_MASK);
      }
    }
    
    case TIME_CLIENT_STATE_DISCOVERED_CHARS_OF_REFERENCE_UPDATE_TIME_SERVICE:
    {
        TIME_CLIENT_MESG_DBG ( profiledbgfile, "Time Client tries to disable current time char notification \n");
        currentTimeCharDescValue = 0x0000;
        retval = aci_gatt_write_charac_descriptor (timeClient.connHandle,
                                                   timeClient.currentTimeCharConfigDescHandle, 
                                                   (uint8_t)2, 
                                                   (uint8_t *)&currentTimeCharDescValue);
       
        if (retval == BLE_STATUS_SUCCESS)
        {
            TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DISABLING_CURRENT_TIME_UPDATE_NOTIFICATION);
        }
         else
        {
          /* what to do next as disabling time update notification has failed */
          TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
        }
    }
    break;
    case TIME_CLIENT_STATE_DISABLING_CURRENT_TIME_UPDATE_NOTIFICATION:
    {
      /* Notify to application that time server is ready for use */
      /* set the time client at connected idle state */
      TCProfile_Write_MainStateMachine(TIME_CLIENT_STATE_CONNECTED);
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
          
      retval = BLE_STATUS_SUCCESS;
      timeClient.applicationNotifyFunc(EVT_TC_DISCOVERY_CMPLT,1,&retval);
    }
    break;
    case TIME_CLIENT_STATE_READING_CURRENT_TIME_VALUE:
    {
      /* current time value has been read completely from time server, 
       * notify the application also */
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
      timeClient.applicationNotifyFunc(EVT_TC_READ_CUR_TIME_CHAR,gattRespLen,gattRespBuf);
    }
    break;
    case TIME_CLIENT_STATE_READING_LOCAL_TIME_INFO:
    {
      /* local time info has been read completely from time server, 
       * notify the application also */
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
      timeClient.applicationNotifyFunc(EVT_TC_READ_LOCAL_TIME_INFO_CHAR,gattRespLen,gattRespBuf);
    }
    break;
    case TIME_CLIENT_STATE_READING_REFERENCE_TIME_INFO:
    {
      /* server's time reference info has been read completely from time server, 
       * notify the application also */
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
      timeClient.applicationNotifyFunc(EVT_TC_READ_REF_TIME_INFO_CHAR,gattRespLen,gattRespBuf);
      TIME_CLIENT_MESG_DBG ( profiledbgfile, "sending the event for read %d\n",gattRespLen);
    }
    break;
    case TIME_CLIENT_STATE_READING_NEXT_DST_CHANGE_TIME:
    {
      /* next DST change time has been read completely from time server, 
       * notify the application also */
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
      timeClient.applicationNotifyFunc(EVT_TC_READ_TIME_WITH_DST_CHAR,gattRespLen,gattRespBuf);
    }
    break;
    case TIME_CLIENT_STATE_READING_SERVER_TIME_UPDATE_STATUS:
    {
      /* time update state has been read completely from time server, 
       * notify the application also */
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
      timeClient.applicationNotifyFunc(EVT_TC_READ_TIME_UPDATE_STATE_CHAR,gattRespLen,gattRespBuf);
    }
    break;
    case TIME_CLIENT_STATE_WRITING_CURRENT_TIME_UPDATE_NOTIFICATION:
    {
      TIME_CLIENT_MESG_DBG( profiledbgfile, "Current time char client config descriptor wrote with %d \n", currentTimeCharDescValue);
      TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_CONNECTED_IDLE);
    }
    break;
  } /* switch(TCProfile_Read_SubStateMachine()) */

  return;
}/* end TimeClient_GATT_Procedure_Complete_EvtHandler() */

/**
 * TimeClient_StateMachine
 * 
 * @param[in] None
 * 
 * TimeClient profile's state machine: to be called on application main loop. 
 */ 
tBleStatus TimeClient_StateMachine(void)
{
  tBleStatus hciCmdResult = BLE_STATUS_FAILED;

  /* timeClient.mainState, timeClient.subState */
  switch (TCProfile_Read_MainStateMachine() )
  {
    case TIME_CLIENT_STATE_UNINITIALIZED:
    break;
    case TIME_CLIENT_STATE_WAITING_BLE_INIT:
    {
      /* chech whether the main profile has done with ble init or not */
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
      {
        TIME_CLIENT_MESG_DBG( profiledbgfile, "Time Client is in initialized State \n");
        
        /* there is no specific initialization required for the
                         * time client profile. So go to initialized state once
                         * the main profile has been initialized
                         */ 
        TCProfile_Write_MainStateMachine(TIME_CLIENT_STATE_INITIALIZED);
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_INVALID);

        /* vote to the main profile to go to advertise state */
        (void) BLE_Profile_Vote_For_Advertisable_State (&timeClient.TCtoBLEInf);
      }
    }
    break; /* TIME_CLIENT_STATE_WAITING_BLE_INIT */
    case TIME_CLIENT_STATE_INITIALIZED:
    {
      /* Time Client has completed its initialization process and voted the 
       * main profile for Advertisable state. Now its waiting for the main profile
       * to set to Advertisable state. When the main profile notifies of its state
                   * change, change the profiles state machine also
                   */
      if ((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) && 
          (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_CONNECTABLE_IDLE))
      {
        TCProfile_Write_MainStateMachine(TIME_CLIENT_STATE_ADVERTIZE);
        TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_ADVERTISABLE_IDLE);
        hciCmdResult = BLE_STATUS_SUCCESS;
        timeClient.applicationNotifyFunc(EVT_TC_INITIALIZED,1,&hciCmdResult);
      }
    }
    break; /* TIME_CLIENT_STATE_INITIALIZED */
    case TIME_CLIENT_STATE_ADVERTIZE:
    {
      switch (TCProfile_Read_SubStateMachine())
      {
        case TIME_CLIENT_STATE_ADVERTISABLE_IDLE:
        {
          //TBR
        }
        break;
                
        case TIME_CLIENT_STATE_ADVERTISING_FOR_WHITE_LIST:
        break;
        
        case TIME_CLIENT_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP:
        {
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "STOPPING Advertising to White List devices \n");
          
          hciCmdResult = aci_gap_set_non_discoverable();
          if ( hciCmdResult == BLE_STATUS_SUCCESS)
          {
            /* change profile's sub states */
            TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_FAST_DISCOVERABLE_DO_START);
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State (&timeClient.TCtoBLEInf, 
                                                    BLE_PROFILE_STATE_CONNECTABLE_IDLE);
          }
          else
          {
            break;
          }
        }
        case TIME_CLIENT_STATE_FAST_DISCOVERABLE_DO_START:
        {
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "STARTING FAST Limited Discoverable Mode \n");

          hciCmdResult = TimeClient_Set_Discoverable ((uint16_t)TIP_FAST_CONN_ADV_INTERVAL_MIN,
                                                      (uint16_t)TIP_FAST_CONN_ADV_INTERVAL_MAX);
          if ( hciCmdResult == BLE_STATUS_SUCCESS)
          {
            /* change profile's sub states */
            TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_FAST_DISCOVERABLE);

            /* start a timer of 30 seconds for first phase of discoverable mode (fast discoverable mode) */
            Blue_NRG_Timer_Start (30, TimeClient_Advertise_Period_Timeout_Handler,&timeClient.timerID);
            
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State(&timeClient.TCtoBLEInf, 
                                                   BLE_PROFILE_STATE_ADVERTISING);
          }
          else
          {
            /* Request to put the device in limited discoverable mode is 
             * unsuccess, put the device back in advrtisable idle mode */
            TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_ADVERTISABLE_IDLE);
          }
        }
        break;
       
        case TIME_CLIENT_STATE_FAST_DISCOVERABLE:
        break;
        case TIME_CLIENT_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP:
        {
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "STOPPING FAST Limited Discoverable Mode \n");
          
          hciCmdResult = aci_gap_set_non_discoverable();
          if ( hciCmdResult == BLE_STATUS_SUCCESS)
          {
            /* change profile's sub states */
            TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_LOW_POWER_DISCOVERABLE_DO_START);
            /* request for main profile's state change */
            BLE_Profile_Change_Advertise_Sub_State ( &timeClient.TCtoBLEInf, 
                                                     BLE_PROFILE_STATE_CONNECTABLE_IDLE);
            
            TIME_CLIENT_MESG_DBG ( profiledbgfile, "STARTING Low Power General Discoverable Mode \n");
            
            hciCmdResult = TimeClient_Set_Discoverable((uint16_t)TIP_LOW_PWR_ADV_INTERVAL_MIN,
                                                        (uint16_t)TIP_LOW_PWR_ADV_INTERVAL_MAX);
                                                      
            if ( hciCmdResult == BLE_STATUS_SUCCESS)
            {
              /* change profile's sub states */
              TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_LOW_POWER_DISCOVERABLE);

              /* request for main profile's state change */
              BLE_Profile_Change_Advertise_Sub_State(&timeClient.TCtoBLEInf, 
                                                     BLE_PROFILE_STATE_ADVERTISING);
            }
            else
            {
              TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Set_Discoverable() FAILED: %02X \n", hciCmdResult ); 
              /* Request to put the device in limited discoverable mode is 
                * unsuccess, put the device back in advertisable idle mode */
              TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_ADVERTISABLE_IDLE);
              break;
            }
          }
          else
          {
            TIME_CLIENT_MESG_DBG ( profiledbgfile, "aci_gap_set_non_discoverable() FAILED: %02X \n", hciCmdResult );
            break;
          }
        }
        break;
        
        case TIME_CLIENT_STATE_LOW_POWER_DISCOVERABLE:
        break;
        case TIME_CLIENT_STATE_BLE_ADVERTISING:
        {
          //TBR
        }
        break;
        case TIME_CLIENT_STATE_PROCESSING_PAIRING:
        {
          if (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_DISCONNECTING)
          {
            TCProfile_Write_MainStateMachine(TIME_CLIENT_STATE_CONNECTED);
            TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DO_DISCONNECT);
          }
        }
        break;
      }
    }
    break; /* TIME_CLIENT_STATE_ADVERTISE */
    
    case TIME_CLIENT_STATE_CONNECTED:
    {
      switch (TCProfile_Read_SubStateMachine())
      {
        case TIME_CLIENT_STATE_CONNECTED_IDLE: 
        {
          if (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_DISCONNECTING)
          {
              TCProfile_Write_SubStateMachine(TIME_CLIENT_STATE_DO_DISCONNECT);
          }
        }
        break;
      }
    }
    break; /* TIME_CLIENT_STATE_CONNECTED */
  }
  return (hciCmdResult);
}/* end TimeClient_StateMachine() */

/**
 * TimeClient_Advertise_Period_Timeout_Handler
 * 
 * this function is called when the timer started
 * by the heart rate profile for advertising times out
 */ 
void TimeClient_Advertise_Period_Timeout_Handler(void)
{
  tProfileState nextState = TCProfile_Read_SubStateMachine();
      
  TIME_CLIENT_MESG_DBG ( profiledbgfile, "TimeClient_Advertise_Period_Timeout_Handler(), Advertising Period Timedout \n");
   
  Blue_NRG_Timer_Stop(timeClient.timerID);
  timeClient.timerID = 0xFF;
  if (TCProfile_Read_MainStateMachine() == TIME_CLIENT_STATE_ADVERTIZE)
  {
    switch (TCProfile_Read_SubStateMachine())
    {
      case TIME_CLIENT_STATE_ADVERTISING_FOR_WHITE_LIST:
          nextState = TIME_CLIENT_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP;
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "White list Advertising Timedout \n");
      break;
      case TIME_CLIENT_STATE_FAST_DISCOVERABLE:
          nextState = TIME_CLIENT_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP;
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "Fast Advertising Timedout \n");
      break;
      case TIME_CLIENT_STATE_LOW_POWER_DISCOVERABLE:
          nextState = TIME_CLIENT_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP;
          TIME_CLIENT_MESG_DBG ( profiledbgfile, "Low Power Advertising Timedout \n");
      break;
    }

    if (nextState != TCProfile_Read_SubStateMachine())
    {
      TCProfile_Write_SubStateMachine(nextState);
    }
  }
}/* end TimeClient_Advertise_Period_Timeout_Handler() */
