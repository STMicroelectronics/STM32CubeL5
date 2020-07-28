/******************************************************************************/
/*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2014 STMicroelectronics International NV
*
*   FILENAME        -  profile.c
*
*   COMPILER        -  
*
*******************************************************************************
*
*   CHANGE HISTORY
* ---------------------------
*   $Date$:      09/07/2014
*   $Revision$:  Second version 
*   $Author$:    AMS RF Application team
*   Comments:    Redesigned the BLE Main Profile according to unified BlueNRG 
*                DK framework  
*******************************************************************************
*
*  File Description 
*  ---------------------
*  This file will have BLE Main Profile implementation. 
******************************************************************************/

/*******************************************************************************
*******************************************************************************/

/*******************************************************************************
* Include Files
*******************************************************************************/
#include <host_config.h>
#include <hci.h>
#include <hci_le.h>
#include <hci_const.h>
#include <debug.h>
#include <ble_list.h>
#include <ble_events.h>
#include <ble_profile.h>
#include <bluenrg_aci.h>
#include <bluenrg_gap.h>
#include "sm.h"
#include <timer.h>
#include <uuid.h> 
#include <hal.h> 

/*******************************************************************************
* types
*******************************************************************************/
/* gap roles */
#define GAP_PERIPHERAL_ROLE                (0x01)
#define GAP_BROADCASTER_ROLE               (0x02)
#define GAP_CENTRAL_ROLE                   (0x03)
#define GAP_OBSERVER_ROLE                  (0x04)

/**
* device information service context
* contains the handles of all the 
* characteristics added to the device
* information service as specified by bitmask of
* deviceInfoCharToBeAdded field. The value of these
* characteristics cannot be set by the
* application and will have to be set
* at the profile level itself. These values are
* hard coded at the profile level. It is assumed that
* the software, firmware revisions correspond to
* the revision of the profile code
*/ 
typedef struct _tDevInfoServiceContext
{
  /**
  * This flag keeps track of the characteristics
  * that have to be added to the device information service
  */ 
  uint16_t deviceInfoCharToBeAdded;
  
  /**
  * handle of the device information service
  */ 
  uint16_t devInfoServHandle; 
  
  /**
  * handle of the system ID characteristic
  */ 
  uint16_t systemIDCharHandle; 
  
  /**
  * handle of the model number characteristic
  */ 
  uint16_t modelNumCharHandle; 
  
  /**
  * handle of the serial number characteristic
  */ 
  uint16_t serialNumCharHandle; 
  
  /**
  * handle of the firmware revision characteristic
  */ 
  uint16_t firmwareRevisionCharHandle; 
  
  /**
  * handle of the hardware revision characteristic
  */ 
  uint16_t hardwareRevisionCharHandle; 
  
  /**
  * handle of the software revision characteristic
  */
  uint16_t softwareRevisionCharHandle; 
  
  /**
  * handle of the manufacturer name characteristic
  */
  uint16_t manufacturerNameCharHandle; 
  
  /**
  * handle of the IEEE Certification characteristic
  */
  uint16_t ieeeCretificationCharHandle; 
  
  /**
  * handle of the PNP ID characteristic
  */
  uint16_t pnpIDCharHandle; 
}tDevInfoServiceContext;

typedef struct
{
  /**
  * list of all the currently
  * registered profiles
  */ 
  tListNode bleRegstedProfileList;
  
  /**
  * callback to be called to notify the application
  */ 
  BLE_CALLBACK_FUNCTION_TYPE bleProfileApplNotifyFunc;
  
  /**
  * number of profiles currently registered
  * with the main profile
  */ 
  uint8_t bleProfileNumOfRegsProfiles;
  
  /**
  * this is a counter to maintain the status of initialization of all profiles
  * Each time a profile notifies that it has completed its initialization, 
  * this counter will be incremented. The device will be allowed to start 
  * advertising only when all the profiles have completd theur initialization
  */	
  uint8_t bleProfileCanAdvertise;
  
  /**
  * main state of the profile state
  * machine
  */ 
  tProfileState bleProfileMainState;
  
  /**
  * sub state of the profile state
  * machine
  */ 
  tProfileState bleProfileSubState;
  
  /**
  * contains the information related to
  * device information service
  */
  tDevInfoServiceContext devInfoService;
  
}tBLEProfileCtxt;

/*******************************************************************************
* Variable Declarations
*******************************************************************************/
tBLEProfileGlobalContext gBLEProfileContext;
static tBLEProfileCtxt gMainProfileCtxt;
static uint8_t bleProfileIintFlag = 0;
BOOL profileProcess = FALSE;

tCentralDevice centralDevice;

uint8_t bnrg_expansion_board = IDB04A1; /* at startup, suppose the X-NUCLEO-IDB04A1 is used */
/*******************************************************************************
* Imported Variable
*******************************************************************************/
extern void *profiledbgfile;

/*******************************************************************************
* Function Prototypes 
*******************************************************************************/

static tBleStatus BLE_Profile_GATT_Procedure_Complete_EvtHandler(uint16_t procedureCode);

static void SetProfileProcessRequest(BOOL ProfileProcessValue);
static BOOL ReadProfileProcessRequest(void);

static tBleStatus BLE_Profile_Is_Registered_Profile (tProfileInterfaceContext* profileContext);

static tBleStatus BLE_Profile_Notify_To_All_Profiles (void* response);

tBleStatus Add_DIS_Charac(void);

//extern void HCI_Enable_GATT_Cmd(uint16_t opcode);

uint8_t getBlueNRGVersion(uint8_t *hwVersion, uint16_t *fwVersion);

static void SetProfileProcessRequest(BOOL profileProcessValue)
{
  profileProcess = profileProcessValue;
  
  if (profileProcessValue == TRUE)
  {
    Profile_Process_Notification_Request();
  }
  
  return;
}

static BOOL ReadProfileProcessRequest(void)
{
  return(profileProcess);
}

/* Define __weak only for Eclipse based toolchains (e.g Atollic TrueSTUDIO or SW4STM32) */
#ifdef  __ECLIPSE_BASED_IDE__
#define __weak __attribute__((weak))
#endif

__weak void Profile_Process_Notification_Request(void)
{
  return;
}

__weak void Profile_Notify_StateMachine_Update(void)
{
  return;
}

/*******************************************************************************
* SAP
*******************************************************************************/

tBleStatus BLE_Profile_Init(tSecurityParams * securityParameters, 
							BLE_CALLBACK_FUNCTION_TYPE bleCallbackFunc)
{
  PROFILE_MESG_DBG (profiledbgfile, "Initializing BLE Profile \n");
  tBleStatus status = 0;
  
  if (bleProfileIintFlag != 0)
  {
    PROFILE_MESG_DBG (profiledbgfile, "BLE Profile already Initialized !! \n");
    return (BLE_STATUS_NOT_ALLOWED);
  }
  
  if ((securityParameters == NULL) || (bleCallbackFunc == NULL))
  {
    return (BLE_STATUS_NULL_PARAM);
  }
  
  BLUENRG_memset((void *)&gBLEProfileContext,0,sizeof(tBLEProfileGlobalContext));
  BLUENRG_memset((void *)&gMainProfileCtxt,0,sizeof(gMainProfileCtxt));
  
  BLUENRG_memcpy((void *)&(gBLEProfileContext.bleSecurityParam), 
              (void *)securityParameters, 
              sizeof(tSecurityParams));
  
  /* save the application notification function ptr */
  gMainProfileCtxt.bleProfileApplNotifyFunc = bleCallbackFunc;
  
  list_init_head (&gMainProfileCtxt.bleRegstedProfileList);
  
  gBLEProfileContext.advtServUUIDlen = 1;
  gBLEProfileContext.advtServUUID[0] = AD_TYPE_16_BIT_SERV_UUID;
  
  /* initialize the timer */
  Blue_NRG_Timer_Init();
  
  /* initialize ble profile states */
  gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_INIT_BLE;
  gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DETECT_BLE;
  
  /* set the task initialization flag */
  bleProfileIintFlag = 0x01;
  
  return (status);
}

static tBleStatus Configure_Profile(void)
{
  uint8_t  hwVersion;
  uint16_t fwVersion;  
  tBleStatus ret;
  
  PROFILE_MESG_DBG ( profiledbgfile, "Initializing GATT Server \n" );
  ret = aci_gatt_init();
  if(ret != BLE_STATUS_SUCCESS)
    goto INIT_PROFILE_FAIL;
  PROFILE_MESG_DBG (profiledbgfile, "GATT Initialized Successfully \n");
  
  /* get the BlueNRG HW and FW versions */
  getBlueNRGVersion(&hwVersion, &fwVersion);

  if (hwVersion > 0x30) { /* X-NUCLEO-IDB05A1 expansion board is used */
    bnrg_expansion_board = IDB05A1; 
  }
  
  if (bnrg_expansion_board == IDB05A1) {
    ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, 0x07,&gBLEProfileContext.gapServiceHandle, &gBLEProfileContext.devNameCharHandle, &gBLEProfileContext.appearanceCharHandle);
  }
  else {
    ret = aci_gap_init_IDB04A1(GAP_PERIPHERAL_ROLE_IDB04A1, &gBLEProfileContext.gapServiceHandle, &gBLEProfileContext.devNameCharHandle, &gBLEProfileContext.appearanceCharHandle);
  }
  
  if(ret!=BLE_STATUS_SUCCESS)
    goto INIT_PROFILE_FAIL;
  PROFILE_MESG_DBG (profiledbgfile, "GAP Initialized Successfully \n");
  ret = aci_gap_set_io_capability(gBLEProfileContext.bleSecurityParam.ioCapability);
  if(ret!=BLE_STATUS_SUCCESS)
    goto INIT_PROFILE_FAIL;
  PROFILE_MESG_DBG (profiledbgfile, "I/O capability Set Successfully \n");
  ret = aci_gap_set_auth_requirement(gBLEProfileContext.bleSecurityParam.mitm_mode,
                                     gBLEProfileContext.bleSecurityParam.OOB_Data_Present, 
                                     gBLEProfileContext.bleSecurityParam.OOB_Data,
                                     gBLEProfileContext.bleSecurityParam.encryptionKeySizeMin,
                                     gBLEProfileContext.bleSecurityParam.encryptionKeySizeMax,
                                     gBLEProfileContext.bleSecurityParam.Use_Fixed_Pin,
                                     gBLEProfileContext.bleSecurityParam.Fixed_Pin,
                                     gBLEProfileContext.bleSecurityParam.bonding_mode);
  if(ret!=BLE_STATUS_SUCCESS)
    goto INIT_PROFILE_FAIL;
  PROFILE_MESG_DBG (profiledbgfile, "Authentication Req set Successfully \n");
  if(gBLEProfileContext.bleSecurityParam.bonding_mode)
  {
    ret = aci_gap_configure_whitelist();
    if(ret!=BLE_STATUS_SUCCESS)
      goto INIT_PROFILE_FAIL;
    PROFILE_MESG_DBG (profiledbgfile, "White List Configured Successfully \n");
  }
  if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded)
  {
    /* we have to add the device information service */
    uint8_t uuid[2];
    HOST_TO_LE_16(uuid,DEVICE_INFORMATION_SERVICE_UUID);
    
    ret = aci_gatt_add_serv(UUID_TYPE_16, uuid, PRIMARY_SERVICE, MAX_DIS_LEN, &gMainProfileCtxt.devInfoService.devInfoServHandle);
    if(ret!=BLE_STATUS_SUCCESS)
      goto INIT_PROFILE_FAIL;
    PROFILE_MESG_DBG (profiledbgfile, "Device Information Service is added successfully \n");
    
    PROFILE_MESG_DBG ( profiledbgfile, "Device Information Service Handle: %04X\n",
                      gMainProfileCtxt.devInfoService.devInfoServHandle);
    
    ret = Add_DIS_Charac();
    if(ret!=BLE_STATUS_SUCCESS)
      goto INIT_PROFILE_FAIL;
    PROFILE_MESG_DBG (profiledbgfile, "Device Information characteristics added successfully \n");
  }
  
  return BLE_STATUS_SUCCESS;
  
INIT_PROFILE_FAIL:
  PROFILE_MESG_ERR(profiledbgfile, "Error while initializing main profile.\n");
  return ret;
}

int Profile_Process_Q()
{
  uint8_t doNotifyStateChange = 0;
  tBleStatus cmdResult = BLE_STATUS_FAILED;
  
  if (bleProfileIintFlag == 0x00)
  {
    /* the profile is not yet initialized
    * so donot do any processing
    */ 
    return 0x01;
  }
  
  /* process the timer Q */
  Blue_NRG_Timer_Process_Q();
  
  if(ReadProfileProcessRequest() == TRUE)
  {
    PROFILE_MESG_DBG(profiledbgfile,"Profile_Process_Q\n");
    
    PROFILE_MESG_DBG(profiledbgfile, "BLE profile MainState %02X, SubState %02X \n", gMainProfileCtxt.bleProfileMainState, gMainProfileCtxt.bleProfileSubState);
    
    doNotifyStateChange = 0;
    cmdResult = BLE_STATUS_FAILED;
    
    switch (gMainProfileCtxt.bleProfileMainState)
    {
    case BLE_PROFILE_STATE_INIT_BLE:
      {
        /* Steps for controller stack Initialization 
        * 1. GATT Init
        * 2. GAP Init
        * 3. set IO capabilities
        * 4. set authentication requirement
        * 5. set event mask
        * 6. set le event mask
        * 7. set GAP event mask
        * 8. set GATT event mask 
        * 9. configure whitelist 
        */
        if(Configure_Profile() == BLE_STATUS_SUCCESS){
          
          /* notify sub profiles that main profile initialization is complete
          * so that the profile specific initialization can be done
          */ 
          gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_INIT_PROFILE;
          gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_INVALID;
        }
        else {
          /* the profile initialization was completed with an error
          * notify the application
          */
          gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_INIT_BLE;
          gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
        }
        
        doNotifyStateChange = 1;
        
      }
      break; /* BLE_PROFILE_STATE_INIT_BLE: */
    case BLE_PROFILE_STATE_INIT_PROFILE:
      {
        /* the profile task is waiting here for votes from all sub profiles to 
        * go to advertisable state. */
        /* check the advertising vote counter and change state to Advertisable 
        * if all the registered profiles have been voted */
        /* ENTER CRITICAL SECTION */
        if (gMainProfileCtxt.bleProfileCanAdvertise == gMainProfileCtxt.bleProfileNumOfRegsProfiles)
        {
          /* Set the BLE Profile task into connectable state */
          gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTABLE;
          gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTABLE_IDLE;
          
          /* Now how to notify the sub profiles about main profile state change */
          doNotifyStateChange = 1;
          
        }
      }
      break;
    case BLE_PROFILE_STATE_CONNECTING:
      {
        switch (gMainProfileCtxt.bleProfileSubState)
        {
        case BLE_PROFILE_STATE_PAIRING_STARTED:
          {
            PROFILE_MESG_DBG ( profiledbgfile, "pair\n");
            PROFILE_MESG_DBG ( profiledbgfile, "send pairing request\n");
            cmdResult = aci_gap_slave_security_request(gBLEProfileContext.connectionHandle,
                                                       gBLEProfileContext.bleSecurityParam.bonding_mode,
                                                       gBLEProfileContext.bleSecurityParam.mitm_mode);
            if (cmdResult == BLE_STATUS_SUCCESS){
              PROFILE_MESG_DBG ( profiledbgfile, "Slave Security Request has been sent to master \n");
              gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTED;
            }
            else {
              /* security could not be requested */
              gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTED;
            }
          }
          break;
        case BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTED:
          break;
        case BLE_PROFILE_STATE_SLAVE_SECURITY_INITIATED:
          break;
        case BLE_PROFILE_STATE_PASS_KEY_REQUESTED:
          break;
        case BLE_PROFILE_STATE_PAIRING_COMPLETE_WAITING:
          break;
        }
      }
      break; /* BLE_PROFILE_STATE_CONNECTING */
    case BLE_PROFILE_STATE_CONNECTED:
      {
        switch (gMainProfileCtxt.bleProfileSubState)
        {
        case BLE_PROFILE_STATE_CONNECTED_IDLE:
          break;
        case BLE_PROFILE_STATE_EXCHANGE_CONFIG:
          {
            PROFILE_MESG_DBG ( profiledbgfile, "Starting Configuration Exchange Process \n");
            cmdResult = aci_gatt_exchange_configuration(gBLEProfileContext.connectionHandle);
            if (cmdResult == BLE_STATUS_SUCCESS)
            {
              PROFILE_MESG_DBG(profiledbgfile, "BLE profile MainState %02X, SubState %02X,aci_gatt_exchange_configuration() OK \n", gMainProfileCtxt.bleProfileMainState, gMainProfileCtxt.bleProfileSubState);
              /* change profile's sub states */
              gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_EXCHANGING_CONFIG;
            }
            else
              PROFILE_MESG_DBG(profiledbgfile, "BLE profile MainState %02X, SubState %02X,aci_gatt_exchange_configuration() failed %02X \n", gMainProfileCtxt.bleProfileMainState, gMainProfileCtxt.bleProfileSubState,cmdResult);
          }
          break;
        case BLE_PROFILE_STATE_EXCHANGING_CONFIG:
          break;
        case BLE_PROFILE_STATE_DISCONNECTING:
          break;
        }
      }
      break; /* BLE_PROFILE_STATE_CONNECTED */
    }
    
    /* BLE Profile's Notification to registered profiles */
    if (doNotifyStateChange)
    {
      /* when the profiles have been notified, the profile processQ
      * needs to run once again so that it can set its state machine.
      * So donot set the profileProcess flag to false
      */ 
      //PROFILE_MESG_DBG ( profiledbgfile, "Notifying to all registered profiles about main profile state change \n");
      //BLE_Profile_Notify_State_Change();
    }
    else
    {
    	SetProfileProcessRequest(FALSE);
    }
  }
  
  return (ReadProfileProcessRequest());
}

tProfileState BLE_Profile_Get_Main_State ()
{
  return (gMainProfileCtxt.bleProfileMainState);
}

tProfileState BLE_Profile_Get_Sub_State ()
{
  return (gMainProfileCtxt.bleProfileSubState);
}

tBleStatus Is_Profile_Present(uint8_t profileID)
{
  tListNode * node = NULL;
  tProfileInterfaceContext *temp;
  
  PROFILE_MESG_DBG(profiledbgfile, "Is_Profile_Present\n");
  
  list_get_next_node (&gMainProfileCtxt.bleRegstedProfileList, &node);
  while (node != &gMainProfileCtxt.bleRegstedProfileList)
  {
    temp = (tProfileInterfaceContext*)node;
    if (temp->profileID == profileID)
    {
      PROFILE_MESG_DBG(profiledbgfile, "found profile %x\n",profileID);
      /* profile is a registered one */
      return BLE_STATUS_SUCCESS;
    }
    /* get the next node "registered profile context" in the list */
    list_get_next_node (node, &node);
  }
  PROFILE_MESG_DBG(profiledbgfile, "did not find profile %x\n",profileID);
  return BLE_STATUS_FAILED;
}


tBleStatus BLE_Profile_Register_Profile ( tProfileInterfaceContext * profileContext)
{
  if (profileContext == NULL)
  {
    return (BLE_STATUS_NULL_PARAM);
  }
  
  if (profileContext->callback_func == NULL)
  {
    return (BLE_STATUS_NULL_PARAM);
  }
  
  /* Check whether the profile is already registered */
  if ((BLE_Profile_Is_Registered_Profile (profileContext)) == BLE_STATUS_SUCCESS)
  {
    /* profile is already registered */
    return (BLE_STATUS_FAILED);
  }
  
  /* Check whether at this point the main profile can accept a new proifle */
  if ((gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_INIT_BLE) && 
      (gMainProfileCtxt.bleProfileSubState != BLE_PROFILE_STATE_INITIALIZATION_ERROR))
  {
    /* BLE main profile can accept a new profile */
  }
  else
  {
    /* BLE profile is not in the state to accept a new profile at this time */
    return (BLE_STATUS_FAILED);
  }
  
  PROFILE_MESG_DBG(profiledbgfile,"[b]DISCharRequired %x\n",gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded);
  gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded |= profileContext->DISCharRequired;
  PROFILE_MESG_DBG(profiledbgfile,"[a]DISCharRequired %x\n",gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded);
  
  gMainProfileCtxt.bleProfileNumOfRegsProfiles++;
  
  /* add the new profile interface context to registered profile list */
  list_insert_tail ( (tListNode *)&gMainProfileCtxt.bleRegstedProfileList, (tListNode *)profileContext );
  
  return (BLE_STATUS_SUCCESS);
}


tBleStatus BLE_Profile_Unregister_Profile ( tProfileInterfaceContext * profileContext)
{
  if (profileContext == NULL)
  {
    return (BLE_STATUS_NULL_PARAM);
  }
  
  /* first verify that the requesting profile is actually registered with BLE main profile */
  if ((BLE_Profile_Is_Registered_Profile (profileContext)) != BLE_STATUS_SUCCESS)
  {
    /* profile not registered */
    return (BLE_STATUS_FAILED);
  }
  
  /* here the profile context cant be removed directly from the registered list, because at 
  * the moment the profile call this function, the same list is being used for notification 
  * processing. hence removing current profile context will cause problem. */
  list_remove_node ((tListNode *)profileContext);
  gMainProfileCtxt.bleProfileNumOfRegsProfiles--;
  
  return (BLE_STATUS_SUCCESS);
}

tBleStatus BLE_Profile_Vote_For_Advertisable_State(tProfileInterfaceContext* profileContext)
{
  /* verify that the requesting profile is registered with BLE main profile */
  if ((BLE_Profile_Is_Registered_Profile (profileContext)) != BLE_STATUS_SUCCESS)
  {
    /* profile not registered */
    return (BLE_STATUS_FAILED);
  }
  
  /* check that requesting profile has not voted before for Advertisable state */
  if (profileContext->voteForAdvertisableState != 0)
  {
    /* the profile has alreadt voted for advertisable state */
    return (BLE_STATUS_FAILED);
  }
  
  /* The host can command the controller to go into advertising state only when all
  * the profiles have completed their initialization in the scenario where there are
  * multiple profiles running. So each profile will notify the main profile that
  * it has completed its initialization. The main profile will allow the advertising
  * command only when all registered profiles notify that they have completed their
  * profile specific initialization
  */
  gMainProfileCtxt.bleProfileCanAdvertise++;
  
  profileContext->voteForAdvertisableState = 1;
  
  SetProfileProcessRequest(TRUE);
  
  return (BLE_STATUS_SUCCESS);    
}

void BLE_Profile_Change_Advertise_Sub_State(tProfileInterfaceContext*  profileContext, tProfileState nextState)
{
  /* if the main state of the ble profile is connectable and
  * the profile requesting for a state change is a registered
  * profile, then change the state
  */ 
  if((gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTABLE) &&
     (BLE_Profile_Is_Registered_Profile(profileContext) == BLE_STATUS_SUCCESS))
  {
    gMainProfileCtxt.bleProfileSubState = nextState;
  }
}

/**
* BLE_Profile_Set_State
* 
* @param[in] state : state to be set
* 
* sets the substate of the BLE profile
*/ 
void BLE_Profile_Set_State(tProfileState state)
{
  gMainProfileCtxt.bleProfileSubState = state;
}

tBleStatus BLE_Profile_Disconnect()
{
  PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Disconnect\n");
  
  if((gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED) ||
     (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING))
  {
    PROFILE_MESG_DBG(profiledbgfile,"Sending disconnect command\n");
    
    if(aci_gap_terminate(gBLEProfileContext.connectionHandle, ERR_RMT_USR_TERM_CONN) == BLE_STATUS_SUCCESS)
    {  
      PROFILE_MESG_DBG ( profiledbgfile, "Disconnection Processing !! \n");
      gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DISCONNECTING;
      
      return BLE_STATUS_SUCCESS;
    }
    else
    {
      PROFILE_MESG_DBG ( profiledbgfile, "Disconnection Error !! \n");
    }
  }
  return BLE_STATUS_FAILED;
}

tBleStatus BLE_Profile_Send_Pass_Key(uint32_t passkey)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Send_Pass_Key\n");
  if(((gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) ||
      (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED)) && 
     (gMainProfileCtxt.bleProfileSubState == BLE_PROFILE_STATE_PASS_KEY_REQUESTED))
  {
    retval = aci_gap_pass_key_response(gBLEProfileContext.connectionHandle, passkey);
    
    if (retval == BLE_STATUS_SUCCESS)
    {
      PROFILE_MESG_DBG ( profiledbgfile, "Pass Key Response Accepted \n");
      
      /* as pass key has been send to master as authentication parameter, the only
      * thing remained to the pairing complete event. hence wait for pairing complete. 
      */
      gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PAIRING_COMPLETE_WAITING;
    }
    else
    {
      PROFILE_MESG_DBG ( profiledbgfile, "Pass Key Response Declined \n");
      gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PAIRING_STARTED;
    }
  }
  
  return (retval);
}

tBleStatus BLE_Profile_Start_Configuration_Exchange(void)
{
  PROFILE_MESG_DBG ( profiledbgfile, "BLE_Profile_Start_Configuration_Exchange: %02X, %02X \n", gMainProfileCtxt.bleProfileMainState, gMainProfileCtxt.bleProfileSubState);
  
  if ( (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED) && 
      (gMainProfileCtxt.bleProfileSubState == BLE_PROFILE_STATE_CONNECTED_IDLE) )
  {
    gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_EXCHANGE_CONFIG;
  }
  
  return (BLE_STATUS_SUCCESS);
}

void BLE_Profile_Add_Advertisment_Service_UUID(uint16_t servUUID)
{
  uint8_t indx = gBLEProfileContext.advtServUUIDlen;
  
  gBLEProfileContext.advtServUUID[indx] = (uint8_t)(servUUID & 0xFF);
  indx++;
  gBLEProfileContext.advtServUUID[indx] = (uint8_t)(servUUID >> 8) & 0xFF;
  indx++;
  gBLEProfileContext.advtServUUIDlen = indx;
  PROFILE_MESG_DBG(profiledbgfile,"[add UUID]%x ,%d\n",servUUID,gBLEProfileContext.advtServUUIDlen);
}

tBleStatus BLE_Profile_Remove_Advertisment_Service_UUID(uint16_t servUUID)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  uint8_t indx = 1;
  uint16_t uuid;
  while (indx < gBLEProfileContext.advtServUUIDlen)
  {
    uuid = (uint16_t)(gBLEProfileContext.advtServUUID[indx] >> 8);
    indx++;
    uuid |= (uint16_t)(gBLEProfileContext.advtServUUID[indx]);
    indx++;
    if (uuid == servUUID)
    {
      /* this is the UUID we need to remove from list */
      indx -= 2;
      for (; indx < gBLEProfileContext.advtServUUIDlen ; indx++ )
      {
        gBLEProfileContext.advtServUUID[indx] = gBLEProfileContext.advtServUUID[indx + 1];
      }
      gBLEProfileContext.advtServUUIDlen -= 2;
      retval = BLE_STATUS_SUCCESS;
      break;
    }
  }
  
  return (retval);
}

/*******************************************************************************
* Local Functions
*******************************************************************************/
/**
* Parses the events received from HCI according
* to the profile's state machine and notifies the
* other profiles 
* 
* @param[in] pckt : pointer to the event data
*
*/ 
void HCI_Event_CB(void *pckt)
{  
  
  uint16_t gattProcedureOpcode = 0x00;
  uint8_t notifyApp = 0x00;
  uint8_t appNotifEvt;
  uint8_t appNotifStatus = BLE_STATUS_SUCCESS;
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  PROFILE_MESG_DBG ( profiledbgfile, "BLE_PROFILE_RX_EVENT_HANDLER: ENTER >>>> \n");
  PROFILE_MESG_DBG ( profiledbgfile, "BLE profile MainState %02X, SubState %02X \n", 
                    gMainProfileCtxt.bleProfileMainState, gMainProfileCtxt.bleProfileSubState);
  PROFILE_MESG_DBG ( profiledbgfile, "BLE_Profile_Rx_Event_Handler: %02X \n", event_pckt->evt);
  
  SetProfileProcessRequest(TRUE);
  
  if(hci_pckt->type != HCI_EVENT_PKT)
    return;
  
  switch(event_pckt->evt){
    
  case EVT_DISCONN_COMPLETE:
    {
      evt_disconn_complete *evt = (void *)event_pckt->data;
      
      PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_DISCONN_COMPLETE \n");
      
      if ( (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) || 
          (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED) )
      {
        /* change state */
        if (evt->handle == gBLEProfileContext.connectionHandle)
        {
          if (evt->status == BLE_STATUS_SUCCESS)
          {
            gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTABLE;
            gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTABLE_IDLE;
          }
          notifyApp = 0x01;
          appNotifEvt = EVT_MP_DISCONNECTION_COMPLETE;
          appNotifStatus = evt->status;
        }
      }
    }
    break;
    
  case EVT_ENCRYPT_CHANGE:
    {
      PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_ENCRYPT_CHANGE \n");
    }
    break;
    
  case EVT_READ_REMOTE_VERSION_COMPLETE:
    {
      PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_READ_REMOTE_VERSION_COMPLETE \n");
    }
    break;
    
  case EVT_HARDWARE_ERROR:
    {
      PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_HARDWARE_ERROR \n");
    }
    break;
    
  case EVT_ENCRYPTION_KEY_REFRESH_COMPLETE:
    {
      PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_ENCRYPTION_KEY_REFRESH_COMPLETE \n");
    }
    break;
    
  case EVT_LE_META_EVENT:
    {
      PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_LE_META_EVENT \n");
      
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      switch(evt->subevent){
        
      case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
          
          PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_LE_CONN_COMPLETE %x\n",
                            cc->status);
          /* check task states */
          if(gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTABLE) 
          {
            PROFILE_MESG_DBG ( profiledbgfile, "state\n");
            /* check comand status */
            if (cc->status == BLE_STATUS_SUCCESS)
            {
              PROFILE_MESG_DBG ( profiledbgfile, "BLE_STATUS_SUCCESS\n");
              /* save connection handle */
              gBLEProfileContext.connectionHandle = cc->handle;
              
              if(gBLEProfileContext.bleSecurityParam.initiateSecurity == SLAVE_INITIATE_SECURITY)
              {
                gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTING;
                gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PAIRING_STARTED;
              }
              else if(gBLEProfileContext.bleSecurityParam.initiateSecurity == WAIT_FOR_PAIRING_CMPLT)
              {
                gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTING;
                gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_WAIT_FOR_PAIRING;
              }
              else
              {
                gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTED;
                gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTED_IDLE;
              }

              //HID: Store the address of the central device
              centralDevice.peer_bdaddr_type = cc->peer_bdaddr_type;
              BLUENRG_memcpy(centralDevice.peer_bdaddr, cc->peer_bdaddr, sizeof(cc->peer_bdaddr));
            }
            else
            {
              /* connection was not successful, then we need to wait for
              * the application to put the BLE in discoverable mode again
              */
              gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTABLE;
              gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTABLE_IDLE;
            }
            notifyApp = 0x01;
            appNotifEvt = EVT_MP_CONNECTION_COMPLETE;
            appNotifStatus = cc->status;
            
            /* Stop if any timer is running */
          }
        }
        break; /* HCI_EVT_LE_CONN_COMPLETE */
        
      case EVT_LE_CONN_UPDATE_COMPLETE:
        break;
      }
    }
    break; /* HCI_EVT_LE_META_EVENT */
    
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      
      PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_VENDOR_SPECIFIC \n");
      
      switch (blue_evt->ecode)
      {
      case EVT_BLUE_INITIALIZED:
        {
          evt_blue_initialized * evt = (evt_blue_initialized*)blue_evt->data;
          PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_INITIALIZED\n");
          /* if the BlueNRG is initialized in the
          * normal mode, we need to proceed with
          * further processing
          */ 
          if(evt->reason_code == 0x01)
          {
            PROFILE_MESG_DBG ( profiledbgfile, "BlueNRG initialized\n");
            
            if(gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_INIT_BLE)
            {
              gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZING_GATT;
            }
          }
          else
          {
            PROFILE_MESG_DBG ( profiledbgfile, "BlueNRG initialization error\n");
          }
          
          notifyApp = 0x01;
          appNotifStatus = evt->reason_code-1;
          appNotifEvt = EVT_MP_BLUE_INITIALIZED;
        }
        break;
        
      case EVT_BLUE_GAP_LIMITED_DISCOVERABLE:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "Received event EVT_BLUE_GAP_LIMITED_DISCOVERABLE \n");
        }
        break;
        
      case EVT_BLUE_GAP_PAIRING_CMPLT:
        {
          evt_gap_pairing_cmplt * evt = (evt_gap_pairing_cmplt*)blue_evt->data;
          
          PROFILE_MESG_DBG ( profiledbgfile, "Received event EVT_BLUE_GAP_PAIRING_CMPLT \n");
          
          if ( (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) ||
              (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED) )
          {
            if (evt->status == BLE_STATUS_SUCCESS)
            {
              /* Pairing successful. Now the device is paired with another BT device. */
              gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTED;
              gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTED_IDLE;
            }
            else
            {
              if(gBLEProfileContext.bleSecurityParam.initiateSecurity == SLAVE_INITIATE_SECURITY)
              {
                gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTING;
                gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PAIRING_STARTED;
              }
              else if(gBLEProfileContext.bleSecurityParam.initiateSecurity == WAIT_FOR_PAIRING_CMPLT)
              {
                gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTING;
                gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_WAIT_FOR_PAIRING;
              }
              else
              {
                gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTED;
                gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTED_IDLE;
              }
            }
            
            notifyApp = 0x01;
            appNotifStatus = evt->status;
            appNotifEvt = EVT_MP_PAIRING_COMPLETE;
          }
        }        
        break;
        
      case EVT_BLUE_GAP_PASS_KEY_REQUEST:
        {
          //evt_gap_pass_key_req * evt = (evt_gap_pass_key_req*) blue_evt->data;
          PROFILE_MESG_DBG ( profiledbgfile, "Received event EVT_BLUE_GAP_PASS_KEY_REQUEST \n");
          /* check state */
          if ( (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) ||
              (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED) )
          {
            PROFILE_MESG_DBG ( profiledbgfile, "Requesting Authentication key from application \n");
            gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PASS_KEY_REQUESTED;
            /* pass event to application for user provided pass key 
            * if using fixed pin then we would not have
            * got this event at all. So no need to check for
            * fixed pin option here
            */
            gMainProfileCtxt.bleProfileApplNotifyFunc(EVT_MP_PASSKEY_REQUEST,0,NULL);
          }
        }
        break;
        
      case EVT_BLUE_GAP_AUTHORIZATION_REQUEST:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_GAP_AUTHORIZATION_REQUEST \n");
          /* send the authorization response.
          * currently we donot have a profile 
          * which requires authorization
          */ 
          aci_gap_authorization_response(gBLEProfileContext.connectionHandle, CONNECTION_AUTHORIZED);
        }
        break;
        
      case EVT_BLUE_GAP_SLAVE_SECURITY_INITIATED:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_GAP_SLAVE_SECURITY_INITIATED \n");
          
          if ( (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) && 
              (gMainProfileCtxt.bleProfileSubState == BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTED) )
          {
            if (gBLEProfileContext.bleSecurityParam.mitm_mode)
            {
              /* man in the middle is required to exchange pass key for authentication process.
              * hence wait in the same state for pass key request from master side */
              PROFILE_MESG_DBG ( profiledbgfile, "Man in the middle is required for authentication \n");
              gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SLAVE_SECURITY_INITIATED;
            }
            else
            {
              /* if mitm is not required, we are only left for pairing complete event */
              PROFILE_MESG_DBG ( profiledbgfile, "Man in the middle is not required for authentication \n");
              gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PAIRING_COMPLETE_WAITING;
            }
          }
        }
        break;
        
      case EVT_BLUE_L2CAP_CONN_UPD_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_L2CAP_CONN_UPD_RESP \n");
        }
        break;
        
      case EVT_BLUE_L2CAP_PROCEDURE_TIMEOUT:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_L2CAP_PROCEDURE_TIMEOUT \n");
        }
        break;
        
      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_ATTRIBUTE_MODIFIED \n");
        }
        break;
        
      case EVT_BLUE_GATT_PROCEDURE_TIMEOUT:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_PROCEDURE_TIMEOUT \n");
          /* when a gatt procedure timeout occurs no more gatt pdus can be exchanged
          * since profiles are hugely dependant on GATT we can as well start a 
          * disconnection here
          */ 
          BLE_Profile_Disconnect();
        }
        break;
        
      case EVT_BLUE_ATT_EXCHANGE_MTU_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_EXCHANGE_MTU_RESP \n");
        }
        break;
        
      case EVT_BLUE_ATT_FIND_INFORMATION_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_FIND_INFORMATION_RESP \n");
        }
        break;
        
      case EVT_BLUE_ATT_FIND_BY_TYPE_VAL_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_FIND_BY_TYPE_VAL_RESP \n");
        }
        break;
        
      case EVT_BLUE_ATT_READ_BY_TYPE_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_READ_BY_TYPE_RESP \n");
        }
        break;
        
      case EVT_BLUE_ATT_READ_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_READ_RESP \n");
        }
        break;
      case EVT_BLUE_ATT_READ_BLOB_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_READ_BLOB_RESP \n");
        }
        break;
        
      case EVT_BLUE_ATT_READ_MULTIPLE_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_READ_MULTIPLE_RESP \n");
        }
        break;
        
      case EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP \n");
        }
        break;
        
      case EVT_BLUE_ATT_PREPARE_WRITE_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_PREPARE_WRITE_RESP \n");
        }
        break;
        
      case EVT_BLUE_ATT_EXEC_WRITE_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_ATT_EXEC_WRITE_RESP \n");
        }
        break;
        
      case EVT_BLUE_GATT_INDICATION:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_INDICATION \n");
        }
        break;
        
      case EVT_BLUE_GATT_NOTIFICATION:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "Received EVT_BLUE_GATT_NOTIFICATION \n");
        }
        break;
        
      case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
        {
          //evt_gatt_procedure_complete* evt = (evt_gatt_procedure_complete*) blue_evt->data;
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_PROCEDURE_COMPLETE \n");
          gattProcedureOpcode = BLE_Profile_Get_Current_GATT_Procedure_Opcode();
          BLE_Profile_GATT_Procedure_Complete_EvtHandler (gattProcedureOpcode);
        }
        break;
        
      case EVT_BLUE_GATT_ERROR_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_ERROR_RESP \n");
        }
        break;
        
      case EVT_BLUE_GATT_DISC_READ_CHAR_BY_UUID_RESP:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_DISC_READ_CHAR_BY_UUID_RESP \n");
        }
        break;
        
      case EVT_BLUE_GATT_WRITE_PERMIT_REQ:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_WRITE_PERMIT_REQ \n");
        }
        break;
        
      case EVT_BLUE_GATT_READ_PERMIT_REQ:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_READ_PERMIT_REQ \n");
        }
        break;
        
      case EVT_BLUE_GATT_READ_MULTI_PERMIT_REQ:
        {
          PROFILE_MESG_DBG ( profiledbgfile, "EVT_BLUE_GATT_READ_MULTI_PERMIT_REQ \n");
        }
        break;
      }
    }
    break; /* HCI_EVT_VENDOR_SPECIFIC */
    
  default:
    {
      PROFILE_MESG_DBG ( profiledbgfile, "Received an Invalid Event !! \n");
    }
    break;
  }  
  
  /* Pass the event to all the profiles */
  PROFILE_MESG_DBG(profiledbgfile, "notifying all profiles\n");
  BLE_Profile_Notify_To_All_Profiles(pckt);
  
  if(notifyApp)
  {
    PROFILE_MESG_DBG(profiledbgfile, "notifying application\n");
    gMainProfileCtxt.bleProfileApplNotifyFunc(appNotifEvt,1,&appNotifStatus);
  }
  
  if(gattProcedureOpcode)
  {
    PROFILE_MESG_DBG(profiledbgfile, "gattProcedureOpcode\n");
    BLE_Profile_Reset_GATT_Procedure_Status ();
    //HCI_Enable_GATT_Cmd(gattProcedureOpcode); TODO: Add procedure commands in a queue?
  }
  PROFILE_MESG_DBG ( profiledbgfile, "BLE_PROFILE_RX_EVENT_HANDLER: EXIT >>>> \n");
  
  return;
}

tBleStatus Add_DIS_Charac(void)
{
  tBleStatus ret;
  uint8_t uuid[2];
  
  if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & SYSTEM_ID_CHAR_MASK)
  {  
    HOST_TO_LE_16(uuid,SYSTEM_ID_UUID);
    
    ret = aci_gatt_add_char(gMainProfileCtxt.devInfoService.devInfoServHandle,
                            UUID_TYPE_16,
                            uuid,
                            SYSTEM_ID_LEN_MAX,
                            CHAR_PROP_READ,  
                            ATTR_PERMISSION_NONE,
                            GATT_DONT_NOTIFY_EVENTS,
                            10,
                            CHAR_VALUE_LEN_CONSTANT,
                            &gMainProfileCtxt.devInfoService.systemIDCharHandle);
    if(ret != BLE_STATUS_SUCCESS)
      goto ADD_DIS_CHARAC_FAIL;
    PROFILE_MESG_DBG ( profiledbgfile, "[DIS] system ID Handle: %04X !!\n",
                      gMainProfileCtxt.devInfoService.systemIDCharHandle);
  }
  if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & MODEL_NUMBER_STRING_CHAR_MASK)
  {
    HOST_TO_LE_16(uuid,MODEL_NUMBER_UUID);
    
    ret = aci_gatt_add_char(gMainProfileCtxt.devInfoService.devInfoServHandle,
                            UUID_TYPE_16,
                            uuid,
                            MODEL_NUMBER_STRING_LEN_MAX,
                            CHAR_PROP_READ,  
                            ATTR_PERMISSION_NONE,
                            GATT_DONT_NOTIFY_EVENTS,
                            10,
                            CHAR_VALUE_LEN_VARIABLE,
                            &gMainProfileCtxt.devInfoService.modelNumCharHandle);
    if(ret != BLE_STATUS_SUCCESS)
      goto ADD_DIS_CHARAC_FAIL;
    
    PROFILE_MESG_DBG ( profiledbgfile, "[DIS] model number Handle: %04X !!\n",
                      gMainProfileCtxt.devInfoService.modelNumCharHandle);
    
  }
  if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & SERIAL_NUMBER_STRING_CHAR_MASK)
  {
    HOST_TO_LE_16(uuid,SERIAL_NUMBER_UUID);
    
    ret = aci_gatt_add_char(gMainProfileCtxt.devInfoService.devInfoServHandle,
                            UUID_TYPE_16,
                            uuid,
                            SERIAL_NUMBER_STRING_LEN_MAX,
                            CHAR_PROP_READ,  
                            ATTR_PERMISSION_NONE,
                            GATT_DONT_NOTIFY_EVENTS,
                            10,
                            CHAR_VALUE_LEN_VARIABLE,
                            &gMainProfileCtxt.devInfoService.serialNumCharHandle);
    if(ret != BLE_STATUS_SUCCESS)
      goto ADD_DIS_CHARAC_FAIL;
    
	PROFILE_MESG_DBG(profiledbgfile, "[DIS] serial number Handle: %04X !!\n",
                     gMainProfileCtxt.devInfoService.serialNumCharHandle);
  }
  if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & FIRMWARE_REVISION_STRING_CHAR_MASK)
  {
    HOST_TO_LE_16(uuid,FIRMWARE_REVISION_UUID);
    
    ret = aci_gatt_add_char(gMainProfileCtxt.devInfoService.devInfoServHandle,
                            UUID_TYPE_16,
                            uuid,
                            FIRMWARE_REVISION_STRING_LEN_MAX,
                            CHAR_PROP_READ,  
                            ATTR_PERMISSION_NONE,
                            GATT_DONT_NOTIFY_EVENTS,
                            10,
                            CHAR_VALUE_LEN_VARIABLE,
                            &gMainProfileCtxt.devInfoService.firmwareRevisionCharHandle);
    if(ret != BLE_STATUS_SUCCESS)
      goto ADD_DIS_CHARAC_FAIL;
    
	PROFILE_MESG_DBG(profiledbgfile, "[DIS] firmware revision Handle: %04X !!\n",
                     gMainProfileCtxt.devInfoService.firmwareRevisionCharHandle);
  }
  if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & HARDWARE_REVISION_STRING_CHAR_MASK)
  {
    HOST_TO_LE_16(uuid,HARDWARE_REVISION_UUID);
    
    ret = aci_gatt_add_char(gMainProfileCtxt.devInfoService.devInfoServHandle,
                            UUID_TYPE_16,
                            uuid,
                            HARDWARE_REVISION_STRING_LEN_MAX,
                            CHAR_PROP_READ,  
                            ATTR_PERMISSION_NONE,
                            GATT_DONT_NOTIFY_EVENTS,
                            10,
                            CHAR_VALUE_LEN_VARIABLE,
                            &gMainProfileCtxt.devInfoService.hardwareRevisionCharHandle);
    if(ret != BLE_STATUS_SUCCESS)
      goto ADD_DIS_CHARAC_FAIL;
    
	PROFILE_MESG_DBG(profiledbgfile, "[DIS] hardware revision Handle: %04X !!\n",
                     gMainProfileCtxt.devInfoService.hardwareRevisionCharHandle);
  }
  if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & SOFTWARE_REVISION_STRING_CHAR_MASK)
  {
    HOST_TO_LE_16(uuid,SOFTWARE_REVISION_UUID);
    
    ret = aci_gatt_add_char(gMainProfileCtxt.devInfoService.devInfoServHandle,
                            UUID_TYPE_16,
                            uuid,
                            SOFTWARE_REVISION_STRING_LEN_MAX,
                            CHAR_PROP_READ,  
                            ATTR_PERMISSION_NONE,
                            GATT_DONT_NOTIFY_EVENTS,
                            10,
                            CHAR_VALUE_LEN_VARIABLE,
                            &gMainProfileCtxt.devInfoService.softwareRevisionCharHandle);
    if(ret != BLE_STATUS_SUCCESS)
      goto ADD_DIS_CHARAC_FAIL;
    
	PROFILE_MESG_DBG(profiledbgfile, "[DIS] software revision Handle: %04X !!\n",
                     gMainProfileCtxt.devInfoService.softwareRevisionCharHandle);
  }
  if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & MANUFACTURER_NAME_STRING_CHAR_MASK)
  {
    HOST_TO_LE_16(uuid,MANUFACTURER_NAME_UUID);
    
    ret = aci_gatt_add_char(gMainProfileCtxt.devInfoService.devInfoServHandle,
                            UUID_TYPE_16,
                            uuid,
                            MANUFACTURER_NAME_STRING_LEN_MAX,
                            CHAR_PROP_READ,  
                            ATTR_PERMISSION_NONE,
                            GATT_DONT_NOTIFY_EVENTS,
                            10,
                            CHAR_VALUE_LEN_VARIABLE,
                            &gMainProfileCtxt.devInfoService.manufacturerNameCharHandle);
    if(ret != BLE_STATUS_SUCCESS)
      goto ADD_DIS_CHARAC_FAIL;
    
	PROFILE_MESG_DBG(profiledbgfile, "[DIS] manufacturer name Handle: %04X !!\n",
                     gMainProfileCtxt.devInfoService.manufacturerNameCharHandle);
  }
  if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & IEEE_CERTIFICATION_CHAR_MASK)
  {
    HOST_TO_LE_16(uuid,IEEE_CERTIFICATION_UUID);
    
    ret = aci_gatt_add_char(gMainProfileCtxt.devInfoService.devInfoServHandle,
                            UUID_TYPE_16,
                            uuid,
                            IEEE_CERTIFICATION_LEN_MAX,
                            CHAR_PROP_READ,  
                            ATTR_PERMISSION_NONE,
                            GATT_DONT_NOTIFY_EVENTS,
                            10,
                            CHAR_VALUE_LEN_VARIABLE,
                            &gMainProfileCtxt.devInfoService.ieeeCretificationCharHandle);
    if(ret != BLE_STATUS_SUCCESS)
      goto ADD_DIS_CHARAC_FAIL;
    
	PROFILE_MESG_DBG(profiledbgfile, "[DIS] IEE certification Handle: %04X !!\n",
                     gMainProfileCtxt.devInfoService.ieeeCretificationCharHandle);
  }
  if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & PNP_ID_CHAR_MASK)
  {
    HOST_TO_LE_16(uuid,PNP_ID_UUID);
    
    ret = aci_gatt_add_char(gMainProfileCtxt.devInfoService.devInfoServHandle,
                            UUID_TYPE_16,
                            uuid,
                            PNP_ID_LEN_MAX,
                            CHAR_PROP_READ,  
                            ATTR_PERMISSION_NONE,
                            GATT_DONT_NOTIFY_EVENTS,
                            10,
                            CHAR_VALUE_LEN_CONSTANT,
                            & gMainProfileCtxt.devInfoService.pnpIDCharHandle);
    if(ret != BLE_STATUS_SUCCESS)
      goto ADD_DIS_CHARAC_FAIL;
    
	PROFILE_MESG_DBG(profiledbgfile, "[DIS] PNP ID Handle: %04X !!\n",
                     gMainProfileCtxt.devInfoService.pnpIDCharHandle);
  }
  
  return BLE_STATUS_SUCCESS;
  
ADD_DIS_CHARAC_FAIL:
  PROFILE_MESG_ERR(profiledbgfile, "[DIS] Error while adding characteristics.\n");
  return ret;
}

tBleStatus BLE_Profile_Update_DIS_SystemID(uint8_t len,uint8_t *SystemId)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_SystemID %d\n",len);
  if(len <= SYSTEM_ID_LEN_MAX)
  {	
    ret = aci_gatt_update_char_value(gMainProfileCtxt.devInfoService.devInfoServHandle, 
                                     gMainProfileCtxt.devInfoService.systemIDCharHandle,
                                     0,
                                     len,
                                     SystemId);
    
    if(ret == BLE_STATUS_SUCCESS)
    {
      gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= SYSTEM_ID_CHAR_MASK;
    }
  }
  return ret;
}

tBleStatus BLE_Profile_Update_DIS_ModelNum(uint8_t len,uint8_t *modelNum)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_ModelNum %d\n",len);
  if(len <= MODEL_NUMBER_STRING_LEN_MAX)
  {	
    ret = aci_gatt_update_char_value(gMainProfileCtxt.devInfoService.devInfoServHandle, 
                                     gMainProfileCtxt.devInfoService.modelNumCharHandle, 
                                     0,
                                     len, 
                                     modelNum);
    if(ret == BLE_STATUS_SUCCESS)
    {
      gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= MODEL_NUMBER_STRING_CHAR_MASK;
    }
  }
  return ret;
}

tBleStatus BLE_Profile_Update_DIS_SerialNum(uint8_t len,uint8_t *serialNum)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_SerialNum %d\n",len);
  if(len <= SERIAL_NUMBER_STRING_LEN_MAX)
  {	
    ret = aci_gatt_update_char_value(gMainProfileCtxt.devInfoService.devInfoServHandle, 
                                     gMainProfileCtxt.devInfoService.serialNumCharHandle, 
                                     0,
                                     len, 
                                     serialNum);
    if(ret == BLE_STATUS_SUCCESS)
    {
      gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= SERIAL_NUMBER_STRING_CHAR_MASK;
    }
  }
  return ret;
}

tBleStatus BLE_Profile_Update_DIS_FirmwareRev(uint8_t len,uint8_t *firmwareRev)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_FirmwareRev %d\n",len);
  if(len <= FIRMWARE_REVISION_STRING_LEN_MAX)
  {	
    ret = aci_gatt_update_char_value(gMainProfileCtxt.devInfoService.devInfoServHandle, 
                                     gMainProfileCtxt.devInfoService.firmwareRevisionCharHandle, 
                                     0,
                                     len, 
                                     firmwareRev);
    if(ret == BLE_STATUS_SUCCESS)
    {
      gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= FIRMWARE_REVISION_STRING_CHAR_MASK;
    }
  }
  return ret;
}

tBleStatus BLE_Profile_Update_DIS_HardwareRev(uint8_t len,uint8_t *hardwareRev)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_HardwareRev %d\n",len);
  if(len <= HARDWARE_REVISION_STRING_LEN_MAX)
  {	
    ret = aci_gatt_update_char_value(gMainProfileCtxt.devInfoService.devInfoServHandle, 
                                     gMainProfileCtxt.devInfoService.hardwareRevisionCharHandle, 
                                     0,
                                     len, 
                                     hardwareRev);
    if(ret == BLE_STATUS_SUCCESS)
    {
      gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= HARDWARE_REVISION_STRING_CHAR_MASK;
    }
  }
  return ret;
}

tBleStatus BLE_Profile_Update_DIS_SoftwareRev(uint8_t len,uint8_t *softwareRev)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_SoftwareRev %d\n",len);
  if(len <= SOFTWARE_REVISION_STRING_LEN_MAX)
  {	
    ret = aci_gatt_update_char_value(gMainProfileCtxt.devInfoService.devInfoServHandle, 
                                     gMainProfileCtxt.devInfoService.softwareRevisionCharHandle, 
                                     0,
                                     len, 
                                     softwareRev);
    if(ret == BLE_STATUS_SUCCESS)
    {
      gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= SOFTWARE_REVISION_STRING_CHAR_MASK;
    }
  }
  return ret;
}

tBleStatus BLE_Profile_Update_DIS_manufacturerName(uint8_t len,uint8_t *name)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_manufacturerName %d\n",len);
  if(len <= MANUFACTURER_NAME_STRING_LEN_MAX)
  {	
    ret = aci_gatt_update_char_value(gMainProfileCtxt.devInfoService.devInfoServHandle, 
                                     gMainProfileCtxt.devInfoService.manufacturerNameCharHandle, 
                                     0,
                                     len, 
                                     name);
    if(ret == BLE_STATUS_SUCCESS)
    {
      gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= MANUFACTURER_NAME_STRING_CHAR_MASK;
    }
  }
  return ret;
}

tBleStatus BLE_Profile_Update_DIS_IEEECertification(uint8_t len,uint8_t *ieeeCert)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_manufacturerName %d\n",len);
  if(len <= IEEE_CERTIFICATION_LEN_MAX)
  {	
    ret = aci_gatt_update_char_value(gMainProfileCtxt.devInfoService.devInfoServHandle, 
                                     gMainProfileCtxt.devInfoService.ieeeCretificationCharHandle, 
                                     0,
                                     len, 
                                     ieeeCert);
    if(ret == BLE_STATUS_SUCCESS)
    {
      gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= IEEE_CERTIFICATION_CHAR_MASK;
    }
  }
  return ret;
}

tBleStatus BLE_Profile_Update_DIS_pnpId(uint8_t len,uint8_t *pnpId)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_pnpId %d\n",len);
  if(len <= PNP_ID_LEN_MAX)
  {	
    ret = aci_gatt_update_char_value(gMainProfileCtxt.devInfoService.devInfoServHandle, 
                                     gMainProfileCtxt.devInfoService.pnpIDCharHandle, 
                                     0,
                                     len, 
                                     pnpId);
    if(ret == BLE_STATUS_SUCCESS)
    {
      gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= PNP_ID_CHAR_MASK;
    }
  }
  return ret;
}

/**
* BLE_Profile_GATT_Procedure_Complete_EvtHandler
* 
* @param[in] procedureCode : opcode of the gatt procedure that
*           has been completed
*/ 
tBleStatus BLE_Profile_GATT_Procedure_Complete_EvtHandler(uint16_t procedureCode)
{
  PROFILE_MESG_DBG ( profiledbgfile, "BLE_Profile_GATT_Procedure_Complete_EvtHandler: opcode %04X \n", procedureCode);
  switch (gMainProfileCtxt.bleProfileMainState)
  {
  case BLE_PROFILE_STATE_CONNECTED:
    {
      switch (gMainProfileCtxt.bleProfileSubState)
      {
      case BLE_PROFILE_STATE_EXCHANGING_CONFIG:
        {
          if (procedureCode == OCF_GATT_EXCHANGE_CONFIG)
          {
            gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTED_IDLE;
          }
        }
        break;
      }
    }
    break;
  }
  return (BLE_STATUS_SUCCESS);
}

/**
* BLE_Profile_Is_Registered_Profile
* 
* @param[in] profileContext : pointer to the profile interface context
*            which has to be checked for its registration with the main profile
* 
* searches through the list of registered profiles to find the requested
* profile
* 
* @return returns BLE_STATUS_SUCCESS if the desired profile is found else
* returns BLE_STATUS_FAILED
*/ 
tBleStatus BLE_Profile_Is_Registered_Profile(tProfileInterfaceContext *profileContext)
{
  tListNode * node = NULL;
  
  list_get_next_node (&gMainProfileCtxt.bleRegstedProfileList, &node);
  while (node != &gMainProfileCtxt.bleRegstedProfileList)
  {
    if (node == (tListNode *)profileContext)
    {
      /* profile is a registered one */
      return BLE_STATUS_SUCCESS;
    }
    
    /* get the next node "registered profile context" in the list */
    list_get_next_node (node, &node);
  }
  
  return BLE_STATUS_FAILED;
}

/**
* BLE_Profile_Notify_To_All_Profiles
* 
* @param[in] response : pointer to the event data
* 
* notifies all the profiles registered with the main profile
* about the event
*/ 
tBleStatus BLE_Profile_Notify_To_All_Profiles(void* response)
{
  tListNode * node = NULL;
  tProfileInterfaceContext * profileContext = NULL;
  
  if (response == NULL)
  {
    return (BLE_STATUS_NULL_PARAM);
  }
  
  
  /* traverse through the registered profile list */
  list_get_next_node (&gMainProfileCtxt.bleRegstedProfileList, &node);
  while (node != &gMainProfileCtxt.bleRegstedProfileList)
  {
    profileContext = (tProfileInterfaceContext *)node;
    if ( (profileContext->callback_func) != NULL )
    {
      profileContext->callback_func(response);
    }
    /* get the next node "registered profile context" in the list */
    list_get_next_node (node, &node);
  }
  
  return (BLE_STATUS_SUCCESS);
}

static uint16_t runningGATTProcedureOpcode = 0 ;

/*******************************************************************************
* Functions to handle ATT sequential protocol
*******************************************************************************/

void BLE_Profile_Set_GATT_Procedure_Status(uint16_t opcode)
{
  runningGATTProcedureOpcode = opcode;
}

void BLE_Profile_Reset_GATT_Procedure_Status(void)
{
  runningGATTProcedureOpcode = 0;
}

uint16_t BLE_Profile_Get_Current_GATT_Procedure_Opcode ( void )
{
  return (runningGATTProcedureOpcode);
}

/* New function added for directly calling the aci_gap_slave_security_request()
   from GAP peripheral device. It used for some specific PTS tests. */
void  BLE_Profile_Send_Security_Request() 
{ 
  tBleStatus cmdResult = BLE_STATUS_INVALID_PARAMS;
  
  PROFILE_MESG_DBG ( profiledbgfile, "BLE_Profile_Send_Security_Request(): state %02X, substate %02X\n", gMainProfileCtxt.bleProfileMainState,gMainProfileCtxt.bleProfileSubState);
  
  PROFILE_MESG_DBG ( profiledbgfile, "BLE_Profile_Send_Security_Request(): send slave security request\n");
  cmdResult = aci_gap_slave_security_request(gBLEProfileContext.connectionHandle,
                                             gBLEProfileContext.bleSecurityParam.bonding_mode,
                                             gBLEProfileContext.bleSecurityParam.mitm_mode);
  if (cmdResult == BLE_STATUS_SUCCESS){
    PROFILE_MESG_DBG ( profiledbgfile, "BLE_Profile_Send_Security_Request(): Slave Security Request has been sent to master \n");
    gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTED;
  }
  else {
    /* security could not be requested */
    PROFILE_MESG_DBG ( profiledbgfile, "BLE_Profile_Send_Security_Request: aci_gap_slave_security_request() failure %02X  \n", cmdResult);
    gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTED;
  }
  
}/* end BLE_Profile_Send_Security_Request() */

uint8_t getBlueNRGVersion(uint8_t *hwVersion, uint16_t *fwVersion)
{
  uint8_t status;
  uint8_t hci_version, lmp_pal_version;
  uint16_t hci_revision, manufacturer_name, lmp_pal_subversion;

  status = hci_le_read_local_version(&hci_version, &hci_revision, &lmp_pal_version, 
				     &manufacturer_name, &lmp_pal_subversion);

  if (status == BLE_STATUS_SUCCESS) {
    *hwVersion = hci_revision >> 8;
    *fwVersion = (hci_revision & 0xFF) << 8;              // Major Version Number
    *fwVersion |= ((lmp_pal_subversion >> 4) & 0xF) << 4; // Minor Version Number
    *fwVersion |= lmp_pal_subversion & 0xF;               // Patch Version Number
  }

  //HCI_Process(); // To receive the BlueNRG EVT

  return status;
}
