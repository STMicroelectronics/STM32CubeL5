/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2014 STMicroelectronics International NV
*
*   FILENAME        -  alertNotification_Server.c
*
*   COMPILER        -  EWARM
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      31/07/2014
*   $Revision$:  Second version
*   $Author$:    
*   Comments:    Redesigned the Alert Notification Profile  according 
*                to unified BlueNRG DK framework
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  source code for the server role of alert notification profile
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
#include <alertNotification_Server.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* profile states */
#define ANS_UNINITIALIZED				(0x00)
#define ANS_ADD_ALERT_NOTIFICATION_SERVICE	 	(0x01)
#define ANS_ADD_SUPPORTED_NEW_ALERT_CATEGORY_CHAR	(0x02)
#define ANS_ADD_NEW_ALERT_CHAR 		 		(0x03)
#define ANS_ADD_UNREAD_STATUS_CATEGORY_CHAR 		(0x04)
#define ANS_ADD_UNREAD_ALERT_STATUS_CHAR 		(0x05)
#define ANS_ADD_CONTROL_POINT_CHAR 			(0x06)
#define ANS_UPDATE_NEW_ALERT_CATEGORY_CHAR 		(0x07)
#define ANS_UPDATE_UNREAD_STATUS_CATEGORY_CHAR 		(0x08)
#define ANS_IDLE					(0x09)
#define ANS_INITIALIZED					(0x0A)
#define ANS_UNDIRECTED_ADV_MODE				(0x0B)
#define ANS_ADV_IN_FAST_DISC_MODE			(0x0C)
#define ANS_START_ADV_IN_LOW_PWR_MODE                   (0x0D)
#define ANS_ADV_IN_LOW_PWR_MODE				(0x0E)
#define ANS_CONNECTED					(0x0F)
#define ANS_NOTIFY_NEW_ALERT_IMMEDIATELY 		(0x10)
#define ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY 	(0x11)

/* advertising intervals in terms of 625 micro seconds */
#define ANS_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define ANS_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define ANS_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define ANS_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */

/* alert notification profile specific error code */
#define COMMAND_NOT_SUPPORTED 		(0xA0)
/******************************************************************************
* type definitions
******************************************************************************/
typedef struct _tANSContext
{
  /**
   * state of the blood pressure
   * sensor state machine
   */ 
  tProfileState state;
	
  /**
   * connection handle 
   */ 
  uint16_t connHandle;
	
  /**
   * callback to notify the applciation with
   * the profile specific events
   */ 
  BLE_CALLBACK_FUNCTION_TYPE applcb;
	
  /**
   * handle of the alert notification service
   */ 
  uint16_t alertServiceHandle;
	
  /**
   * handle of the supported new alert category
   * characteristic
   */ 
  uint16_t newAlertCategoryHandle;
	
  /**
   * handle of the new alert characteristic
   */ 
  uint16_t newAlertHandle;
	
  /**
   * handle of the supported unread alert
   * category characteristic
   */ 
  uint16_t UnreadAlertCategoryHandle;
	
  /**
   * handle of the unread alert status
   * characteristic
   */ 
  uint16_t unreadAlertStatusHandle;
	
  /**
   * handle of the alert notification
   * control point characteristic
   */ 
  uint16_t ctrlPointCharHandle;
    
  /**
   * bitmask of the alert categories 
   * supported for the new alert
   */ 
  uint8_t alertCategory[2];
	
  /**
   * bitmask of the alert categories 
   * supported for the unread alert status
   */ 
  uint8_t unreadAlertCategory[2];
	
  /**
   * this is set to 1 when the control point
   * is written by the peer to enable new
   * alert notifications
   */ 
  uint8_t notifyNewAlert[10];
	
  /**
   * this is set to 1 when the control point
   * is written by the peer to enable unread
   * alert status notifications
   */ 
  uint8_t notifyUnreadAlert[10];
	
  /**
   * will contain the timer ID of the 
   * timer started by the alert notification profile
   */ 
  tTimerID timerID;
	
  /**
   * profile interface context. This is
   * registered with the main profile
   */ 
  tProfileInterfaceContext ANStoBLEInf;
}tANSContext;

/******************************************************************************
* Variable Declarations
******************************************************************************/
static tANSContext alertNotificationServer;

/* stores the alerts sent by the appication
 * these will be sent to server only when the
 * client enables notification by writing to the
 * control point
 */ 
uint8_t new_alert_status[10]=
{
	0x00, /*CATEGORY_ID_SIMPLE_ALERT*/
	0x00, /*CATEGORY_ID_EMAIL*/
	0x00, /*CATEGORY_ID_NEWS*/
	0x00, /*CATEGORY_ID_CALL*/
	0x00, /*CATEGORY_ID_MISSED_CALL*/
	0x00, /*CATEGORY_ID_SMS_MMS*/
	0x00, /*CATEGORY_ID_VOICE_MAIL*/
	0x00, /*CATEGORY_ID_SCHEDULE*/
	0x00, /*CATEGORY_ID_HIGH_PRIORITIZED_ALERT*/
	0x00  /*CATEGORY_ID_INSTANT_MESSAGE*/
};

uint8_t unread_alert_status[10]=
{
	0x00, /*CATEGORY_ID_SIMPLE_ALERT*/
	0x00, /*CATEGORY_ID_EMAIL*/
	0x00, /*CATEGORY_ID_NEWS*/
	0x00, /*CATEGORY_ID_CALL*/
	0x00, /*CATEGORY_ID_MISSED_CALL*/
	0x00, /*CATEGORY_ID_SMS_MMS*/
	0x00, /*CATEGORY_ID_VOICE_MAIL*/
	0x00, /*CATEGORY_ID_SCHEDULE*/
	0x00, /*CATEGORY_ID_HIGH_PRIORITIZED_ALERT*/
	0x00  /*CATEGORY_ID_INSTANT_MESSAGE*/
};

const uint8_t Category_ID_To_Bitmask[10]=
{
	CATEGORY_SIMPLE_ALERT,
	CATEGORY_EMAIL,
	CATEGORY_NEWS,
	CATEGORY_CALL,
	CATEGORY_MISSED_CALL,
	CATEGORY_SMS_MMS,
	CATEGORY_VOICE_MAIL,
	CATEGORY_SCHEDULE,
	CATEGORY_HIGH_PRIORITIZED_ALERT,
	CATEGORY_INSTANT_MESSAGE
};

/******************************************************************************
* Imported Variable
******************************************************************************/
extern void *profiledbgfile;
extern uint8_t bnrg_expansion_board;

/******************************************************************************
 * Function Prototypes
******************************************************************************/
static void ANS_Event_Handler(void *pckt);
static void ANS_AdvTimeout_Handler(void);
static void ANS_Exit(uint8_t errCode);
static void ANC_Handle_ControlPoint_Write(uint8_t *attVal);
static void Update_Alert(void);

static void ANSProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState ANSProfile_Read_MainStateMachine(void);

/*******************************************************************************
* SAP
*******************************************************************************/
tBleStatus ANS_Init(uint8_t* alertCategory,
		    uint8_t* unreadAlertCategory,
		    BLE_CALLBACK_FUNCTION_TYPE applcb)
{
  tBleStatus retval = BLE_STATUS_FAILED;
	
  ANS_DBG_MSG(profiledbgfile, "ANS_Init\n");

  if(Is_Profile_Present(PID_ALERT_NOTIFICATION_SERVER) == BLE_STATUS_SUCCESS)
  {
    return BLE_STATUS_PROFILE_ALREADY_INITIALIZED;
  }

  if(applcb == NULL)
  {
    return BLE_STATUS_NULL_PARAM;
  }
	
  if(((!alertCategory[0]) && (!alertCategory[1])) ||
     ((!unreadAlertCategory[0]) && (!unreadAlertCategory[1])) ||
     (alertCategory[1] == 0xFF) || (unreadAlertCategory[1] == 0xFF))
  {
    return BLE_STATUS_INVALID_PARAMS;
  }
	
  /* Initialize Data Structures */
  BLUENRG_memset(&alertNotificationServer,0,sizeof(alertNotificationServer));
    
  /* register the sensor with BLE main Profile */
  BLUENRG_memset(&alertNotificationServer.ANStoBLEInf,0,sizeof(alertNotificationServer.ANStoBLEInf));
  alertNotificationServer.ANStoBLEInf.profileID = PID_ALERT_NOTIFICATION_SERVER;
  alertNotificationServer.ANStoBLEInf.callback_func = ANS_Event_Handler;
  alertNotificationServer.ANStoBLEInf.voteForAdvertisableState = 0;
	
  retval = BLE_Profile_Register_Profile(&alertNotificationServer.ANStoBLEInf);
  if (retval != BLE_STATUS_SUCCESS)
  {
    /* Profile could not be registered with the BLE main profile */
    return (retval);
  }
	
  alertNotificationServer.applcb = applcb;
  alertNotificationServer.timerID = 0xFF;
  ANSProfile_Write_MainStateMachine(ANS_UNINITIALIZED);
  alertNotificationServer.unreadAlertCategory[0] = unreadAlertCategory[0];
  alertNotificationServer.unreadAlertCategory[1] = unreadAlertCategory[1];
  alertNotificationServer.alertCategory[0] = alertCategory[0];
  alertNotificationServer.alertCategory[1] = alertCategory[1];
	
  ANS_DBG_MSG(profiledbgfile,"Alert Notification Profile is Initialized in Server role\n");
	
  return (BLE_STATUS_SUCCESS);
}

tBleStatus ANS_Advertize(uint8_t useWhitelist)
{
  tBleStatus status = BLE_STATUS_FAILED;
  const char name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'A','l','e','r','t','N','o','t','i','f','i','c','a','t','i','o','n'};
	
  ANS_DBG_MSG(profiledbgfile,"ANS_Advertize %x\n",ANSProfile_Read_MainStateMachine());
	
  if(ANSProfile_Read_MainStateMachine() == ANS_INITIALIZED)
  {
    if(useWhitelist)
    {
      /* start the undirected connectable mode */
      status = aci_gap_set_undirected_connectable(PUBLIC_ADDR, WHITE_LIST_FOR_ALL);
      if(status == BLE_STATUS_SUCCESS)
      {
	ANSProfile_Write_MainStateMachine(ANS_UNDIRECTED_ADV_MODE);
	ANS_DBG_MSG(profiledbgfile,"Enabled advertising fast connection\n");
	/* start a timer for 120 seconds */
	Blue_NRG_Timer_Start(120,ANS_AdvTimeout_Handler,&alertNotificationServer.timerID);
					
	/* request for main profile's state change */
	BLE_Profile_Change_Advertise_Sub_State(&alertNotificationServer.ANStoBLEInf, 
					       BLE_PROFILE_STATE_ADVERTISING);
      }
      else
      {
	/* inform the application that the advertizing was not
	 * enabled
	 */
	ANSProfile_Write_MainStateMachine(ANS_IDLE);
	alertNotificationServer.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
      }
    }
    else
    {
      status = aci_gap_set_limited_discoverable(ADV_IND, 
						ANS_FAST_CONN_ADV_INTERVAL_MIN, 
						ANS_FAST_CONN_ADV_INTERVAL_MAX,
						PUBLIC_ADDR, 
						NO_WHITE_LIST_USE, 
						sizeof(name),
						name, 
						0, 
						NULL,
						0, 
						0);

      if(status == BLE_STATUS_SUCCESS)
      {
	ANSProfile_Write_MainStateMachine(ANS_ADV_IN_FAST_DISC_MODE);
	ANS_DBG_MSG(profiledbgfile,"Enabled advertising fast connection\n");
	/* start a timer for 30 seconds */
	Blue_NRG_Timer_Start(30,ANS_AdvTimeout_Handler,&alertNotificationServer.timerID);
	/* request for main profile's state change */
	BLE_Profile_Change_Advertise_Sub_State(&alertNotificationServer.ANStoBLEInf, BLE_PROFILE_STATE_ADVERTISING);
      }
      else
      {
	/* inform the application that the advertizing was not
	 * enabled
	 */
	ANSProfile_Write_MainStateMachine(ANS_IDLE);
	alertNotificationServer.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
      }
    }
  }

  return status;
}

tBleStatus ANS_Update_New_Alert_Category(uint8_t len,uint8_t* category)
{
  tBleStatus retval = ERR_COMMAND_DISALLOWED;
	
  /* the value of this characteristic should not be changed
   * while in connection. So donot allow an update during connection
   */ 
  if((ANSProfile_Read_MainStateMachine() != ANS_CONNECTED) &&
     (ANSProfile_Read_MainStateMachine() >= ANS_IDLE))
  {
    if(((len != 1) || (len != 2)) ||
       ((len == 2) && (category[0] == 0) && 
	(!(category[1] & (CATEGORY_HIGH_PRIORITIZED_ALERT|CATEGORY_INSTANT_MESSAGE)))) ||
       ((len == 1) && (category[0] == 0)))
    {
      retval = BLE_STATUS_INVALID_PARAMS;
    }

    retval = aci_gatt_update_char_value(alertNotificationServer.alertServiceHandle,
					alertNotificationServer.newAlertCategoryHandle,
					0,
					len,
					category);
    if(retval == BLE_STATUS_SUCCESS)
    {
      alertNotificationServer.alertCategory[0] = 0x00;
      alertNotificationServer.alertCategory[1] = 0x00;
      BLUENRG_memcpy(alertNotificationServer.alertCategory,category,len);
      ANS_DBG_MSG(profiledbgfile,"Updated new alert category\n");
    }
    else
    {
      ANS_DBG_MSG(profiledbgfile,"FAILED to update new alert category %x\n",retval);
    }
  }
	
  return retval;
}

tBleStatus ANS_Update_Unread_Alert_Category(uint8_t len,uint8_t* category)
{
  tBleStatus retval = ERR_COMMAND_DISALLOWED;
	
  /* the value of this characteristic should not be changed
   * while in connection. So donot allow an update during connection
   */ 
  if((ANSProfile_Read_MainStateMachine() != ANS_CONNECTED) &&
     (ANSProfile_Read_MainStateMachine() >= ANS_IDLE))
  {
    if(((len != 1) || (len != 2)) ||
       ((len == 2) && (category[0] == 0) && 
	(!(category[1] & (CATEGORY_HIGH_PRIORITIZED_ALERT|CATEGORY_INSTANT_MESSAGE)))) ||
       ((len == 1) && (category[0] == 0)))
    {
      retval = BLE_STATUS_INVALID_PARAMS;
    }

    retval = aci_gatt_update_char_value(alertNotificationServer.alertServiceHandle,
					alertNotificationServer.UnreadAlertCategoryHandle,
					0,
					len,
					category);
    if(retval == BLE_STATUS_SUCCESS)
    {
      alertNotificationServer.unreadAlertCategory[0] = 0x00;
      alertNotificationServer.unreadAlertCategory[1] = 0x00;
      BLUENRG_memcpy(alertNotificationServer.unreadAlertCategory,category,len);
      ANS_DBG_MSG(profiledbgfile,"Updated unread alert category\n");
    }
    else
    {
      ANS_DBG_MSG(profiledbgfile,"FAILED to update unread alert category %x\n",retval);
    }
  }
	
  return retval;
}

tBleStatus ANS_Update_New_Alert(tCategoryID categoryID,uint8_t alertCount,tTextInfo textInfo)
{
  uint8_t charVal[ALERT_CHAR_MAX_TEXT_FIELD_LEN+2];
  tBleStatus retval;

  if((categoryID > CATEGORY_ID_INSTANT_MESSAGE) ||
     (textInfo.len > ALERT_CHAR_MAX_TEXT_FIELD_LEN))
  {
    return BLE_STATUS_INVALID_PARAMS;
  }
	
  ANS_DBG_MSG(profiledbgfile,"ANS_Update_New_Alert\n");
  new_alert_status[categoryID] = alertCount;
	
  /* if the notification for that particular category
   * is enabled, then update
   */ 
  if(alertNotificationServer.notifyNewAlert[categoryID])
  {
    ANS_DBG_MSG(profiledbgfile,"New alert notify\n");
    charVal[0] = categoryID;
    charVal[1] = alertCount;
    BLUENRG_memcpy(&charVal[2],textInfo.str,textInfo.len);
    retval = aci_gatt_update_char_value(alertNotificationServer.alertServiceHandle,
					alertNotificationServer.newAlertHandle,
					0,
					textInfo.len+2,
					charVal);
    if (retval != BLE_STATUS_SUCCESS)
    {
      ANS_DBG_MSG(profiledbgfile,"New alert notify failed\n");
    }
  }
	
  return retval;
}

tBleStatus ANS_Update_Unread_Alert_Status(tCategoryID categoryID,uint8_t alertCount)
{
  uint8_t charVal[2];
  tBleStatus retval;
  
  ANS_DBG_MSG(profiledbgfile,"ANS_Update_Unread_Alert_Status\n");
  if(categoryID > CATEGORY_ID_INSTANT_MESSAGE)
  {
    return BLE_STATUS_INVALID_PARAMS;
  }
	
  unread_alert_status[categoryID] = alertCount;
	
  /* if the notification for that particular category
   * is enabled, then update
   */ 
  if(alertNotificationServer.notifyUnreadAlert[categoryID])
  {
    charVal[0] = categoryID;
    charVal[1] = alertCount;
    ANS_DBG_MSG(profiledbgfile,"Unread alert status notify\n");
    retval = aci_gatt_update_char_value(alertNotificationServer.alertServiceHandle,
					alertNotificationServer.unreadAlertStatusHandle,
					0,
					2,
					charVal);
    if (retval != BLE_STATUS_SUCCESS)
    {
      ANS_DBG_MSG(profiledbgfile,"Unread Alert status notify failed\n");
    }
   }
	
  return retval;
}

/*******************************************************************************
* Local Functions
*******************************************************************************/
static void ANSProfile_Write_MainStateMachine(tProfileState localmainState)
{
  alertNotificationServer.state = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState ANSProfile_Read_MainStateMachine(void)
{
  return(alertNotificationServer.state);
}

/*
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
static void ANSProfile_Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{  
  ANS_DBG_MSG(profiledbgfile,"EVT_BLUE_GATT_ATTRIBUTE_MODIFIED %x\n",handle);
  if(handle == (alertNotificationServer.ctrlPointCharHandle + 1))
  {
    ANC_Handle_ControlPoint_Write(att_data);
  }
}

/**
 * ANS_Event_Handler
 * 
 * @param[in] pckt : Pointer to the ACI packet
 * 
 * It parses the events received from ACI according to the profile's state machine.
 * Inside this function each event must be identified and correctly parsed.
 * NOTE: It is the event handler to be called inside HCI_Event_CB() 
 */ 
void ANS_Event_Handler(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
	
  switch(event_pckt->evt)
  {
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;

      switch(evt->subevent)
      {
      case EVT_LE_CONN_COMPLETE:
	{
	  evt_le_connection_complete *cc = (void *)evt->data;
	  
	  if (cc->status == BLE_STATUS_SUCCESS)
	  {
	    alertNotificationServer.connHandle = cc->handle;
	    ANS_DBG_MSG(profiledbgfile,"Connected\n");
	    ANSProfile_Write_MainStateMachine(ANS_CONNECTED);
	  }
	}
	break;
      }
    }
    break;
  case EVT_DISCONN_COMPLETE:
    {
      ANS_DBG_MSG(profiledbgfile,"Disconnected\n");
      ANSProfile_Write_MainStateMachine(ANS_INITIALIZED);
      /* all notifications have to be disabled */
      BLUENRG_memset(alertNotificationServer.notifyNewAlert,0,10);
      BLUENRG_memset(alertNotificationServer.notifyUnreadAlert,0,10);
    }
    break;
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      
      switch(blue_evt->ecode)
      {
      case EVT_BLUE_GAP_LIMITED_DISCOVERABLE:
	{
	  if((ANSProfile_Read_MainStateMachine() == ANS_ADV_IN_FAST_DISC_MODE) || (ANSProfile_Read_MainStateMachine() == ANS_ADV_IN_LOW_PWR_MODE))
	  {
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State(&alertNotificationServer.ANStoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
	    
	    if(ANSProfile_Read_MainStateMachine() == ANS_ADV_IN_LOW_PWR_MODE)
	    {
	      ANSProfile_Write_MainStateMachine(ANS_IDLE);
	      /* notify the application */
	      alertNotificationServer.applcb(EVT_MP_ADVERTISING_TIMEOUT,0,NULL);
	    }
	  }
	}
	break;
      case EVT_BLUE_GATT_WRITE_PERMIT_REQ:
	{ 
	  evt_gatt_write_permit_req *wp = (void*)blue_evt->data;

	  ANS_DBG_MSG(profiledbgfile,"EVT_BLUE_GATT_WRITE_PERMIT_REQ \n");
	  if(wp->attr_handle == (alertNotificationServer.ctrlPointCharHandle + 1)) 
	  {
	    /* this is the highest command value possible
	     * any other value higher than this is invalid
	     */ 
	    if(wp->data[0] > NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY)
	    {
	      /* invalid command */
	      ANS_DBG_MSG(profiledbgfile,"Invalid command\n");
	      if (aci_gatt_write_response(alertNotificationServer.connHandle,
					  wp->attr_handle,
					  0x01, /* write_status = 1 (error))*/ // BLE_STATUS_FAILED
					  COMMAND_NOT_SUPPORTED,
					  wp->data_length,
					  wp->data) != BLE_STATUS_SUCCESS)
	      {
		ANS_DBG_MSG(profiledbgfile,"Error in command aci_gatt_write_response()\n");
	      }
	    }
	    else
	    {
	      /* valid command */
	      ANS_DBG_MSG(profiledbgfile,"Valid command\n");
	      if (aci_gatt_write_response(alertNotificationServer.connHandle,
					  wp->attr_handle,
					  BLE_STATUS_SUCCESS,
					  0,
					  wp->data_length,
					  wp->data) != BLE_STATUS_SUCCESS)
	      {
		ANS_DBG_MSG(profiledbgfile,"Error in command aci_gatt_write_response()\n");
	      }
	    }
	  }
	}
	break;
      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
	{
          if (bnrg_expansion_board == IDB05A1) {
            evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
            ANSProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
          else {
            evt_gatt_attr_modified_IDB04A1 *evt = (evt_gatt_attr_modified_IDB04A1*)blue_evt->data;
            ANSProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
	}
	break;
      }
    }
    break;
  }
}

void EnDis_All_Supp_Categories(uint8_t* category,uint8_t* notify,uint8_t enable)
{
  uint8_t idx;
  uint8_t index;
	
  ANS_DBG_MSG(profiledbgfile,"EnDis_All_Supp_Categories %x %x\n",category[0],category[1]);
	
  for(idx=1,index=0; idx<=0x08; idx=idx<<1,index++)
  {
    if(category[0] & idx)
    {
      notify[index] = enable;
    }
  }
  if(category[1] & 0x01)
  {
    notify[8] = enable;
  }
  if(category[1] & 0x02)
  {
    notify[9] = enable;
  }
	
  ANS_DBG_DATA(profiledbgfile,notify,10);
}

void ANC_Handle_ControlPoint_Write(uint8_t *attVal)
{
  uint8_t idx;
	
  ANS_DBG_MSG(profiledbgfile,"Control point value 0x%02x 0x%02x\n",attVal[0],attVal[1]);
	
  if((attVal[1] != 0xFF) && (attVal[1] > CATEGORY_ID_INSTANT_MESSAGE))
  {
    /* invalid category */
    ANS_DBG_MSG(profiledbgfile,"Invalid category\n");
    return;
  }
	
  switch(attVal[0])
  {
  case ENABLE_NEW_ALERT_NOTIFICATION:
    {
      ANS_DBG_MSG(profiledbgfile,"ENABLE_NEW_ALERT_NOTIFICATION 0x%02x\n",attVal[1]/8);
      if(attVal[1] == 0xFF)
      {
	/* enable notifications for all supported
	 * categories
	 */
	EnDis_All_Supp_Categories(alertNotificationServer.alertCategory,
				  alertNotificationServer.notifyNewAlert,
				  0x01);
      }
      else if(alertNotificationServer.alertCategory[attVal[1]/8] & Category_ID_To_Bitmask[attVal[1]])
      {
	/* if the enabling of notification is for a category supported,
	 * then enable the notifications
	 */ 
	alertNotificationServer.notifyNewAlert[attVal[1]] = 0x01;
	ANS_DBG_MSG(profiledbgfile,"Set\n");
      }
    }
    break;
  case ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION:
    {
      ANS_DBG_MSG(profiledbgfile,"ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION 0x%02x\n",attVal[1]/8);
      if(attVal[1] == 0xFF)
      {
	/* enable notifications for all supported
	 * categories
	 */
	EnDis_All_Supp_Categories(alertNotificationServer.unreadAlertCategory,
				  alertNotificationServer.notifyUnreadAlert,
				  0x01);
      }
      else if(alertNotificationServer.unreadAlertCategory[attVal[1]/8] & Category_ID_To_Bitmask[attVal[1]])
      {
	/* if the enabling of notification is for a category supported,
	 * then enable the notifications
	 */ 
	alertNotificationServer.notifyUnreadAlert[attVal[1]] = 0x01;
	ANS_DBG_MSG(profiledbgfile,"Set\n");
      }
    }
    break;
  case DISABLE_NEW_ALERT_NOTIFICATION:
    {
      if(attVal[1] == 0xFF)
      {
	/* disable notifications for all supported
	 * categories
	 */
	EnDis_All_Supp_Categories(alertNotificationServer.alertCategory,
				  alertNotificationServer.notifyNewAlert,
				  0x00);
      }
      else if(alertNotificationServer.alertCategory[attVal[1]/8] & Category_ID_To_Bitmask[attVal[1]])
      {
	/* if the disabling of notification is for a category supported,
	 * then disable the notifications
	 */ 
	alertNotificationServer.notifyNewAlert[attVal[1]] = 0x00;
      }
    }
    break;
  case DISABLE_UNREAD_ALERT_STATUS_NOTIFICATION:
    {
      if(attVal[1] == 0xFF)
      {
	/* disable notifications for all supported
	 * categories
	 */
	EnDis_All_Supp_Categories(alertNotificationServer.unreadAlertCategory,
				  alertNotificationServer.notifyUnreadAlert,
				  0x00);
      }
      else if(alertNotificationServer.unreadAlertCategory[attVal[1]/8] & Category_ID_To_Bitmask[attVal[1]])
      {
	/* if the disabling of notification is for a category supported,
	 * then disable the notifications
	 */ 
	alertNotificationServer.notifyUnreadAlert[attVal[1]] = 0x00;
      }
    }
    break;
  case NOTIFY_NEW_ALERT_IMMEDIATELY:
    {
      ANS_DBG_MSG(profiledbgfile,"NOTIFY_NEW_ALERT_IMMEDIATELY 0x%02x\n",attVal[1]);
      if(attVal[1] == 0xFF)
      {
	/* notify all supported categories for which
	 * the notification has been previously enabled
	 */
	for(idx=0;idx<=CATEGORY_ID_INSTANT_MESSAGE;idx++)
	{
	  if(alertNotificationServer.notifyNewAlert[idx] == 0x01)
	  {
	    alertNotificationServer.notifyNewAlert[idx] = 0x02;
	  }
	}
	
	ANS_DBG_DATA(profiledbgfile,alertNotificationServer.notifyNewAlert,10);
      }
      else if((alertNotificationServer.alertCategory[attVal[1]/8] & Category_ID_To_Bitmask[attVal[1]]) &&
	      (alertNotificationServer.notifyNewAlert[attVal[1]] == 0x01))
      {
	/* if the enabling of notification is for a category supported,
	 * and that category has been previously enabled,
	 * then enable the notifications
	 */ 
	alertNotificationServer.notifyNewAlert[attVal[1]] = 0x02;
      }
      else
      {
	break;
      }
			
      if(ANSProfile_Read_MainStateMachine() != ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY)
      {
	ANSProfile_Write_MainStateMachine(ANS_NOTIFY_NEW_ALERT_IMMEDIATELY);
	/* update the characteristic so that it will be
	 * notified
	 */ 
	Update_Alert();
	ANS_DBG_DATA(profiledbgfile,alertNotificationServer.notifyNewAlert,10);
      }
    }
    break;
  case NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY:
    {
      ANS_DBG_MSG(profiledbgfile,"NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY 0x%02x\n",attVal[1]);
      if(attVal[1] == 0xFF)
      {
	/* notify all supported categories for which
	 * the notification has been previously enabled
	 */
	for(idx=0;idx<=CATEGORY_ID_INSTANT_MESSAGE;idx++)
	{
	  if(alertNotificationServer.notifyUnreadAlert[idx] == 0x01)
	  {
	    alertNotificationServer.notifyUnreadAlert[idx] = 0x02;
	  }
	}
	ANS_DBG_DATA(profiledbgfile,alertNotificationServer.notifyUnreadAlert,10);
      }
      else if(alertNotificationServer.unreadAlertCategory[attVal[1]/8] & Category_ID_To_Bitmask[attVal[1]])
      {
	/* if the enabling of notification is for a category supported,
	 * then enable the notifications
	 */ 
	alertNotificationServer.notifyUnreadAlert[attVal[1]] = 0x02;
      }
      else
      {
	break;
      }
      
      if(ANSProfile_Read_MainStateMachine() != ANS_NOTIFY_NEW_ALERT_IMMEDIATELY)
      {
	ANSProfile_Write_MainStateMachine(ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY);
	
	/* update the characteristic so that it will be
	 * notified
	 */ 
	Update_Alert();
      }
    }
    break;
  }
}

/**
 * starts an update procedure to
 * update the new alert or unread
 * alert status characteristic
 */ 
void Update_Alert(void)
{
  uint8_t idx;
  uint8_t charVal[2];
	
  ANS_DBG_MSG(profiledbgfile,"Update_Alert\n");
  if(ANSProfile_Read_MainStateMachine() == ANS_NOTIFY_NEW_ALERT_IMMEDIATELY)
  {
    ANS_DBG_DATA(profiledbgfile,alertNotificationServer.notifyNewAlert,10);
    for(idx=0;idx<10;idx++)
    {
      if(alertNotificationServer.notifyNewAlert[idx] == 0x02)
      {
	charVal[0] = idx;
	charVal[1] = new_alert_status[idx];
	if (aci_gatt_update_char_value(alertNotificationServer.alertServiceHandle,
				       alertNotificationServer.newAlertHandle,
				       0,
				       2,   
				       charVal) != BLE_STATUS_SUCCESS)
	{
	  ANS_DBG_MSG(profiledbgfile,"Error in Update char value for new alert characteristic\n");
	} 
	else 
	{
	  alertNotificationServer.notifyNewAlert[idx] = 0x01;
	}
	return;
      }
    }
		
    /* need not notify any alerts. check for
     * unread alert status
     */ 
    for(idx=0;idx<10;idx++)
    {
      if(alertNotificationServer.notifyUnreadAlert[idx] == 0x02)
      {
	charVal[0] = idx;
	charVal[1] = unread_alert_status[idx];
	if (aci_gatt_update_char_value(alertNotificationServer.alertServiceHandle,
				       alertNotificationServer.unreadAlertStatusHandle,
				       0,
				       2,
				       charVal) != BLE_STATUS_SUCCESS)
	{
	  ANS_DBG_MSG(profiledbgfile,"Error in Update char value for unread alert status characteristic\n");
	}
	else
	{
	  alertNotificationServer.notifyUnreadAlert[idx] = 0x01;
	  ANSProfile_Write_MainStateMachine(ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY);
	}
	return;
      }
    }
		
    /* no new alerts or unread alerts, so change the
     * state
     */ 
    ANSProfile_Write_MainStateMachine(ANS_CONNECTED);
  }
  else if(ANSProfile_Read_MainStateMachine() == ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY)
  {
    for(idx=0;idx<10;idx++)
    {
      if(alertNotificationServer.notifyUnreadAlert[idx] == 0x02)
      {
	charVal[0] = idx;
	charVal[1] = unread_alert_status[idx];
	if (aci_gatt_update_char_value(alertNotificationServer.alertServiceHandle,
				       alertNotificationServer.unreadAlertStatusHandle,
				       0,
				       2,
				       charVal) != BLE_STATUS_SUCCESS)
	{
	  ANS_DBG_MSG(profiledbgfile,"Error in Update char value for unread alert status characteristic!!!!!\n");
	}
	else
	{
	  alertNotificationServer.notifyUnreadAlert[idx] = 0x01;
	}
	return;
      }
    }
    
    /* need not notify any unread alerts. Check for
     * new alert notification
     */ 
    for(idx=0;idx<10;idx++)
    {
      if(alertNotificationServer.notifyNewAlert[idx] == 0x02)
      {
	charVal[0] = idx;
	charVal[1] = new_alert_status[idx];
	if (aci_gatt_update_char_value(alertNotificationServer.alertServiceHandle,
				       alertNotificationServer.newAlertHandle,
				       0,
				       2,
				       charVal) != BLE_STATUS_SUCCESS)
	{
	}
	else
	{
	  alertNotificationServer.notifyNewAlert[idx] = 0x01;
	  ANSProfile_Write_MainStateMachine(ANS_NOTIFY_NEW_ALERT_IMMEDIATELY);
	}
	return;
      }
    }
		
    /* no new alerts or unread alerts, so change the
     * state
     */ 
    ANSProfile_Write_MainStateMachine(ANS_CONNECTED);
  }
}

/**
 * ANSProfile_StateMachine
 * 
 * @param[in] None
 * 
 * ANS profile's state machine: to be called on application main loop. 
 */ 
tBleStatus ANSProfile_StateMachine(void)
{
  uint16_t uuid;
  uint8_t status;
  const char name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'A','l','e','r','t','N','o','t','i','f','i','c','a','t','i','o','n'};
  static uint8_t mainState=ANS_UNINITIALIZED;
	
  if (mainState != ANSProfile_Read_MainStateMachine()) 
  {
    ANS_DBG_MSG(profiledbgfile,"[cmplt]%x\n",ANSProfile_Read_MainStateMachine());
    mainState = ANSProfile_Read_MainStateMachine();
  }
	
  switch(ANSProfile_Read_MainStateMachine())
  {
  case ANS_UNINITIALIZED:
    {
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
      {
	ANS_DBG_MSG(profiledbgfile,"Main profile initialized, now do the ANS initialization\n");
	  
	uuid = ALERT_NOTIFICATION_SERVICE_UUID;
	
	status = aci_gatt_add_serv(UUID_TYPE_16,
				   (uint8_t *)&uuid,
				   PRIMARY_SERVICE,
				   13,
				   &alertNotificationServer.alertServiceHandle);
	if (status == BLE_STATUS_SUCCESS)
	{
	  ANSProfile_Write_MainStateMachine(ANS_ADD_ALERT_NOTIFICATION_SERVICE);
	  ANS_DBG_MSG(profiledbgfile,"Alert notification service handle 0x%04x\n",alertNotificationServer.alertServiceHandle);
	}
	else
	{
	  ANS_DBG_MSG(profiledbgfile,"Error in aci_gatt_add_serv() call\n");
	  /* initialization error inform the application and exit */
	  ANS_Exit(status);
	}
      }
    }
    break;
  case ANS_ADD_ALERT_NOTIFICATION_SERVICE:
    {
      uuid = SUPPORTED_NEW_ALERT_CATEGORY_CHAR_UUID;
      status = aci_gatt_add_char(alertNotificationServer.alertServiceHandle,
				 UUID_TYPE_16,
				 (uint8_t *)&uuid ,
				 2,
				 CHAR_PROP_READ,
				 ATTR_PERMISSION_NONE,
				 GATT_DONT_NOTIFY_EVENTS,
				 MIN_ENCRY_KEY_SIZE,
				 CHAR_VALUE_LEN_VARIABLE,
				 &alertNotificationServer.newAlertCategoryHandle);
      if (status == BLE_STATUS_SUCCESS)
      {
	ANS_DBG_MSG(profiledbgfile,"New alert category char handle 0x%04x\n",alertNotificationServer.newAlertCategoryHandle);
	ANSProfile_Write_MainStateMachine(ANS_ADD_SUPPORTED_NEW_ALERT_CATEGORY_CHAR);
      }
      else
      {
	/* initialization error inform the application and exit */
	ANS_Exit(status);
      }
    }
    break;
  case ANS_ADD_SUPPORTED_NEW_ALERT_CATEGORY_CHAR:
    {
      uuid = NEW_ALERT_CHAR_UUID;
      status = aci_gatt_add_char(alertNotificationServer.alertServiceHandle,
				 UUID_TYPE_16,
				 (uint8_t *)&uuid ,
				 20,
				 CHAR_PROP_NOTIFY,
				 ATTR_PERMISSION_NONE,
				 GATT_DONT_NOTIFY_EVENTS,
				 MIN_ENCRY_KEY_SIZE,
				 CHAR_VALUE_LEN_VARIABLE,
				 &alertNotificationServer.newAlertHandle);
      if (status == BLE_STATUS_SUCCESS)
      {
	ANS_DBG_MSG(profiledbgfile,"New alert char handle 0x%04x\n",alertNotificationServer.newAlertHandle);
	ANSProfile_Write_MainStateMachine(ANS_ADD_NEW_ALERT_CHAR);
      }
      else
      {
	/* initialization error inform the application and exit */
	ANS_Exit(status);
      }
    }
    break;
  case ANS_ADD_NEW_ALERT_CHAR:
    {
      uuid = SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_UUID;
      status = aci_gatt_add_char(alertNotificationServer.alertServiceHandle,
				 UUID_TYPE_16,
				 (uint8_t *)&uuid ,
				 2,
				 CHAR_PROP_READ,
				 ATTR_PERMISSION_NONE,
				 GATT_DONT_NOTIFY_EVENTS,
				 MIN_ENCRY_KEY_SIZE,
				 CHAR_VALUE_LEN_VARIABLE,
				 &alertNotificationServer.UnreadAlertCategoryHandle);
      if (status == BLE_STATUS_SUCCESS)
      {
	ANS_DBG_MSG(profiledbgfile,"Unread alert category char handle 0x%04x\n",alertNotificationServer.UnreadAlertCategoryHandle);
	ANSProfile_Write_MainStateMachine(ANS_ADD_UNREAD_STATUS_CATEGORY_CHAR);
      }
      else
      {
	/* initialization error inform the application and exit */
	ANS_Exit(status);
      }
    }
    break;
  case ANS_ADD_UNREAD_STATUS_CATEGORY_CHAR:
    {
      uuid = UNREAD_ALERT_STATUS_CHAR_UUID;
      status = aci_gatt_add_char(alertNotificationServer.alertServiceHandle,
				 UUID_TYPE_16,
				 (uint8_t *)&uuid ,
				 2,
				 CHAR_PROP_NOTIFY,
				 ATTR_PERMISSION_NONE,
				 GATT_DONT_NOTIFY_EVENTS,
				 MIN_ENCRY_KEY_SIZE,
				 CHAR_VALUE_LEN_CONSTANT,
				 &alertNotificationServer.unreadAlertStatusHandle);
      if (status == BLE_STATUS_SUCCESS)
      {
	ANS_DBG_MSG(profiledbgfile,"Unread alert status char handle 0x%04x\n",alertNotificationServer.unreadAlertStatusHandle);
	ANSProfile_Write_MainStateMachine(ANS_ADD_UNREAD_ALERT_STATUS_CHAR);
      }
      else
      {
	/* initialization error inform the application and exit */
	ANS_Exit(status);
      }
    }
    break;
  case ANS_ADD_UNREAD_ALERT_STATUS_CHAR:
    {
      uuid = ALERT_NOTIFICATION_CONTROL_POINT_CHAR_UUID;
      status = aci_gatt_add_char(alertNotificationServer.alertServiceHandle,
				 UUID_TYPE_16,
				 (uint8_t *)&uuid ,
				 2,
				 CHAR_PROP_WRITE,
				 ATTR_PERMISSION_NONE,
				 GATT_NOTIFY_ATTRIBUTE_WRITE|GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
				 MIN_ENCRY_KEY_SIZE,
				 CHAR_VALUE_LEN_CONSTANT,
				 &alertNotificationServer.ctrlPointCharHandle);
      if (status == BLE_STATUS_SUCCESS)
      {
	ANS_DBG_MSG(profiledbgfile,"Control point char handle 0x%04x\n",alertNotificationServer.ctrlPointCharHandle);
	ANSProfile_Write_MainStateMachine(ANS_ADD_CONTROL_POINT_CHAR);
      }
      else
      {
	/* initialization error inform the application and exit */
	ANS_Exit(status);
      }
    }
    break;
  case ANS_ADD_CONTROL_POINT_CHAR:
    {
      status = aci_gatt_update_char_value(alertNotificationServer.alertServiceHandle,
					  alertNotificationServer.newAlertCategoryHandle,
					  0,
					  2,
					  alertNotificationServer.alertCategory);
      if (status == BLE_STATUS_SUCCESS)
      {	
	ANSProfile_Write_MainStateMachine(ANS_UPDATE_NEW_ALERT_CATEGORY_CHAR);
      }
      else
      {
	/* initialization error inform the application and exit */
	ANS_Exit(status);
      }
    }
    break;
  case ANS_UPDATE_NEW_ALERT_CATEGORY_CHAR:
    {
      status = aci_gatt_update_char_value(alertNotificationServer.alertServiceHandle,
					  alertNotificationServer.UnreadAlertCategoryHandle,
					  0,
					  2,
					  alertNotificationServer.unreadAlertCategory);
      if (status == BLE_STATUS_SUCCESS)
      {
	ANSProfile_Write_MainStateMachine(ANS_UPDATE_UNREAD_STATUS_CATEGORY_CHAR);
      }
      else
      {
	/* initialization error inform the application and exit */
	ANS_Exit(status);
      }
    }
    break;
  case ANS_UPDATE_UNREAD_STATUS_CATEGORY_CHAR:
    {
      ANS_DBG_MSG(profiledbgfile,"ANS initialized\n");
      BLE_Profile_Vote_For_Advertisable_State(&alertNotificationServer.ANStoBLEInf);
      ANSProfile_Write_MainStateMachine(ANS_IDLE);
    }
    break;
  case ANS_IDLE:
    {
      ANS_DBG_MSG(profiledbgfile,"ANS_IDLE\n");
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE)
      {
	ANS_DBG_MSG(profiledbgfile,"Notify the application that initialization is complete\n");
	status = BLE_STATUS_SUCCESS;
	ANSProfile_Write_MainStateMachine(ANS_INITIALIZED);
	alertNotificationServer.applcb(EVT_ANS_INITIALIZED,1,&status);
      }
    }
    break;
  case ANS_START_ADV_IN_LOW_PWR_MODE:
    {
      status = aci_gap_set_limited_discoverable(ADV_IND,
						ANS_LOW_PWR_ADV_INTERVAL_MIN,
						ANS_LOW_PWR_ADV_INTERVAL_MAX,
						PUBLIC_ADDR,
						NO_WHITE_LIST_USE,
						sizeof(name),
						name,
						0,
						NULL,
						0x0,
						0x0);
				
      if(status == BLE_STATUS_SUCCESS)
      {											 
	ANSProfile_Write_MainStateMachine(ANS_ADV_IN_LOW_PWR_MODE);
      }
      else
      {
	ANSProfile_Write_MainStateMachine(ANS_INITIALIZED);
	alertNotificationServer.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
      }
    }
    break;
  case ANS_CONNECTED:
    {
      
    }
    break;
  case ANS_NOTIFY_NEW_ALERT_IMMEDIATELY:
  case ANS_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY: 
    {
      ANS_DBG_MSG(profiledbgfile,"ANS_CONNECTED\n");
      Update_Alert();
    }
    break;
  }
  return status;
}

/**
 * ANS_AdvTimeout_Handler
 * 
 * advertising timeout handler
 */ 
void ANS_AdvTimeout_Handler(void)
{
  uint8_t status;

  /* stop the running timer */
  Blue_NRG_Timer_Stop(alertNotificationServer.timerID);
  alertNotificationServer.timerID = 0xFF;
  if((ANSProfile_Read_MainStateMachine() == ANS_UNDIRECTED_ADV_MODE) ||
     (ANSProfile_Read_MainStateMachine() == ANS_ADV_IN_FAST_DISC_MODE)) 
  {
    /* stop advertizing */
    status = aci_gap_set_non_discoverable();
    if (status == BLE_STATUS_SUCCESS)
    {
      ANSProfile_Write_MainStateMachine(ANS_START_ADV_IN_LOW_PWR_MODE);
    }
    else
    {
      ANC_DBG_MSG(profiledbgfile,"Set Non Discoverable failed\n");
      ANSProfile_Write_MainStateMachine(ANS_INITIALIZED);
      /* request for main profile's state change */
      BLE_Profile_Change_Advertise_Sub_State(&alertNotificationServer.ANStoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
    }
  }
}

/**
 * ANS_Exit
 * 
 * @param[in] errCode: reason for exit
 * 
 * informs the application for the reason
 * the initialization failed and de-registers 
 * the profile
 */ 
void ANS_Exit(uint8_t errCode)
{
  ANSProfile_Write_MainStateMachine(ANS_UNINITIALIZED);
  alertNotificationServer.applcb(EVT_ANS_INITIALIZED,1,&errCode);
  BLE_Profile_Unregister_Profile(&alertNotificationServer.ANStoBLEInf);
}
