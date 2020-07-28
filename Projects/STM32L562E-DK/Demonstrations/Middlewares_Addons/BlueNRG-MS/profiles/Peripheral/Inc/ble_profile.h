/*******************************************************************************/
/*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  ble_profile.h
*
*   COMPILER        -  EWARM (gnu gcc)
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/03/2012
*   $Revision$:  first version
*   $Author$:    
*   Comments:    BLE Main Profile Header File
*
********************************************************************************
*
*  File Description 
*  ---------------------
*  
*
*******************************************************************************/
#ifndef _BLE_PROFILE_H_
#define _BLE_PROFILE_H_

#include "bluenrg_def.h"
#include "ble_list.h"

/*******************************************************************************
 * Macro
*******************************************************************************/
/* profile IDs */
#define PID_BLE_PROFILE					       (0x01)
#define PID_HEART_RATE_SENSOR				   (0x02)
#define PID_PROXIMITY_MONITOR				   (0x03)
#define PID_PROXIMITY_REPORTER				   (0x04)
#define PID_TIME_SERVER				   		   (0x05)
#define PID_TIME_CLIENT			    		   (0x06)
#define PID_HID_DEVICE						   (0x07)
#define PID_FIND_ME_TARGET				   	   (0x08)
#define PID_FIND_ME_LOCATOR				   	   (0x09)
#define PID_PHONE_ALERT_CLIENT				   (0x0A)
#define PID_BLOOD_PRESSURE_SENSOR			   (0x0B)
#define PID_HEALTH_THERMOMETER			       (0x0C)
#define PID_ALERT_NOTIFICATION_CLIENT		   (0x0D)
#define PID_ALERT_NOTIFICATION_SERVER		   (0x0E)

/* Added PID for Glucose sensor profile role */
#define PID_GLUCOSE_SENSOR		   (0x0F)

#define NO_SECURITY_REQUIRED			(0x00)
#define SLAVE_INITIATE_SECURITY			(0x01)
#define WAIT_FOR_PAIRING_CMPLT			(0x02)

/* AD types */
#define AD_TYPE_16_BIT_SERV_UUID				(0x02)

#define CHAR_EXTENDED_PROPERTIES_DESCRIPTOR_LENGTH         (2)
#define CLIENT_CHAR_CONFIG_DESCRIPTOR_LENGTH               (2)
#define SERVER_CHAR_CONFIG_DESCRIPTOR_LENGTH               (2)
#define CHAR_PRESENTATION_FORMAT_DESCRIPTOR_LENGTH         (7)
#define CHAR_AGGREGATE_FORMAT_DESCRIPTOR_LENGTH            (2)
#define REPORT_REFERENCE_DESCRIPTOR_LENGTH                 (2)
#define ATTRIBUTE_OPCODE_HANDLE_VALUE_NOTIFICATION         (0x1B)


/* Status values for write permission response */
#define GATT_WRITE_RESP_STATUS_NO_ERROR            (0x00)
#define GATT_WRITE_RESP_STATUS_VALUE_ERROR         (0x01)

/* advertising timeout is 30 seconds */
#define BLE_FAST_CONN_ADV_TIMEOUT	   (30)

/**
 * bit masks for the characteristics of the device information
 * service
 */ 
#define SYSTEM_ID_CHAR_MASK                                 (0x01)
#define MODEL_NUMBER_STRING_CHAR_MASK                       (0x02)
#define SERIAL_NUMBER_STRING_CHAR_MASK                      (0x04)
#define FIRMWARE_REVISION_STRING_CHAR_MASK                  (0x08)
#define HARDWARE_REVISION_STRING_CHAR_MASK                  (0x10)
#define SOFTWARE_REVISION_STRING_CHAR_MASK                  (0x20)
#define MANUFACTURER_NAME_STRING_CHAR_MASK                  (0x40)
#define IEEE_CERTIFICATION_CHAR_MASK                        (0x80)
#define PNP_ID_CHAR_MASK                                    (0x100)

#define MAX_DIS_LEN			(9)

/* device information service characteristic lengths */
#define SYSTEM_ID_LEN_MAX                                 (8)
#define MODEL_NUMBER_STRING_LEN_MAX                       (32)
#define SERIAL_NUMBER_STRING_LEN_MAX                      (32)
#define FIRMWARE_REVISION_STRING_LEN_MAX                  (32)
#define HARDWARE_REVISION_STRING_LEN_MAX                  (32)
#define SOFTWARE_REVISION_STRING_LEN_MAX                  (32)
#define MANUFACTURER_NAME_STRING_LEN_MAX                  (32)
#define IEEE_CERTIFICATION_LEN_MAX                        (32)
#define PNP_ID_LEN_MAX                                    (7)

/** 
 * error codes for device
 * information service
 */
#define DIS_COULD_NOT_BE_ADDED					(0x01)
#define DIS_ADD_MODEL_NUM_CHAR_FAILED			(0x02)
#define DIS_ADD_SERIAL_NUM_CHAR_FAILED			(0x03)
#define DIS_ADD_FIRMWARE_REV_CHAR_FAILED		(0x04)
#define DIS_ADD_HARDWARE_REV_CHAR_FAILED		(0x05)
#define DIS_ADD_SOFTWARE_REV_CHAR_FAILED		(0x06)
#define DIS_ADD_MANUFACTURER_NAME_CHAR_FAILED	(0x07)
#define DIS_ADD_IEEE_CERTIFICATION_CHAR_FAILED	(0x08)
#define DIS_ADD_PNP_ID_CHAR_FAILED				(0x09)

/*******************************************************************************
* BLE PROFILE STATES
*******************************************************************************/ 

/* BLE Profile task main states */
#define BLE_PROFILE_STATE_UNINIT               (0x00)
#define BLE_PROFILE_STATE_INIT_BLE             (0x10)
#define BLE_PROFILE_STATE_INIT_PROFILE         (0x20)
#define BLE_PROFILE_STATE_CONNECTABLE          (0x30)
#define BLE_PROFILE_STATE_CONNECTING           (0x40)
#define BLE_PROFILE_STATE_CONNECTED            (0x50)
#define BLE_PROFILE_INVALID                    (0xFF)

/* ble profile sub states under BLE_PROFILE_STATE_INIT_BLE main state */
#define BLE_PROFILE_STATE_DETECT_BLE                (0x11)
#define BLE_PROFILE_STATE_INITIALIZING_GATT         (0x12)
#define BLE_PROFILE_STATE_INITIALIZATION_ERROR      (0x1F)

/* ble profile sub states under BLE_PROFILE_STATE_CONNECTABLE main state */
#define BLE_PROFILE_STATE_CONNECTABLE_IDLE					     (0x31)
#define BLE_PROFILE_STATE_ADVERTISING                            (0x33)


/* ble profile sub states under BLE_PROFILE_STATE_CONNECTING main state */
#define BLE_PROFILE_STATE_WAIT_FOR_PAIRING						 (0x39)
#define BLE_PROFILE_STATE_PAIRING_STARTED                        (0x40)
#define BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTED               (0x42)
#define BLE_PROFILE_STATE_SLAVE_SECURITY_INITIATED               (0x43)
#define BLE_PROFILE_STATE_PASS_KEY_REQUESTED                     (0x44)
#define BLE_PROFILE_STATE_PAIRING_COMPLETE_WAITING               (0x47)
#define BLE_PROFILE_STATE_PAIRING_COMPLETE                       (0x48)


/* ble profile sub states under BLE_PROFILE_STATE_CONNECTED main state */
#define BLE_PROFILE_STATE_CONNECTED_IDLE                         (0x51)
#define BLE_PROFILE_STATE_DISCONNECTING                          (0x55)

#define BLE_PROFILE_STATE_EXCHANGE_CONFIG                        (0x56)
#define BLE_PROFILE_STATE_EXCHANGING_CONFIG                      (0x57)

#define IDB04A1 0
#define IDB05A1 1

/*******************************************************************************
 * type definitions 
*******************************************************************************/
/**
 * security parameters structure
 */ 
typedef struct _tSecurityParams
{
  /**
  * IO capability of the device
  */ 
  uint8_t ioCapability;
  
  /**
  * Authentication requirement of the device
  * Man In the Middle protection required?
  */ 
  uint8_t mitm_mode;
  
  /**
  * bonding mode of the device
  */ 
  uint8_t bonding_mode;
  
  /**
  * Flag to tell whether OOB data has 
  * to be used during the pairing process
  */ 
  uint8_t OOB_Data_Present; 
  
  /**
  * OOB data to be used in the pairing process if 
  * OOB_Data_Present is set to TRUE
  */ 
  uint8_t OOB_Data[16]; 
  
  /**
  * this variable indicates whether to use a fixed pin 
  * during the pairing process or a passkey has to be 
  * requested to the application during the pairing process
  * 0 implies use fixed pin and 1 implies request for passkey
  */ 
  uint8_t Use_Fixed_Pin; 
  
  /**
  * minimum encryption key size requirement
  */ 
  uint8_t encryptionKeySizeMin;
  
  /**
  * maximum encryption key size requirement
  */ 
  uint8_t encryptionKeySizeMax;
  
  /**
  * fixed pin to be used in the pairing process if
  * Use_Fixed_Pin is set to 1
  */ 
  uint32_t Fixed_Pin;
  
  /**
  * this flag indicates whether the host has to initiate
  * the security, wait for pairing or does not have any security
  * requirements.\n
  * 0x00 : no security required
  * 0x01 : host should initiate security by sending the slave security
  *        request command
  * 0x02 : host need not send the clave security request but it
  * has to wait for paiirng to complete before doing any other
  * processing
  */ 
  uint8_t initiateSecurity;
}tSecurityParams;

typedef void (* PROFILE_CALLBACK_FUNC_TYPE) ( void* response);

typedef uint8_t tStateAgreementVote;

/**
 * profile context structure which
 * every profile has to regiater with the main
 * profile
 */ 
typedef struct _tProfileInterfaceContext
{
	/**
	 * list node
	 * for internal usage
	 */ 
    tListNode currentNode;

    /* each profile that is supported by the BlueNRG is 
	 * assigned a unique ID. The IDs are listed in ble_profile.h
	 * When a profile is registered with the main profile, it has
	 * to supply a valid ID. This will be used in all other
	 * operations after the profile is registered to identify the
	 * actions of the profile
	 */ 
    uint8_t profileID;

   /**
    * callback function to be called by the main profile
	* to notify the profile about the events received at HCI
	*/ 
    PROFILE_CALLBACK_FUNC_TYPE callback_func;

    /**
	 * this variable is set when the profile has completed
	 * its initialization and is ready to start advertsing when 
	 * commanded by the user
	 */ 
    tStateAgreementVote voteForAdvertisableState;
	
	/**
	 * bitmask of the device information service
	 * characteristics required\n
	 */ 
	uint16_t DISCharRequired;
}tProfileInterfaceContext;

/**
 * global profile context
 * contains the variables common to all 
 * profiles
 */ 
typedef struct _tBLEProfileGlobalContext{
	
	/**
	 * security requirements of the host
	 */ 
    tSecurityParams bleSecurityParam;
	
    /**
     * gap service handle
     */
	uint16_t gapServiceHandle;
	
	/**
	 * device name characteristic handle
	 */ 
	uint16_t devNameCharHandle;
	
	/**
	 * appearance characteristic handle
	 */ 
	uint16_t appearanceCharHandle;
	
	/**
	 * connection handle of the current active connection
	 * When not in connection, the handle is set to 0xFFFF
	 */ 
    uint16_t connectionHandle;

    /**
	 * length of the UUID list to be used while advertising
	 */ 
    uint8_t advtServUUIDlen;
	
	/**
	 * the UUID list to be used while advertising
	 */ 
    uint8_t advtServUUID[100];

}tBLEProfileGlobalContext;


/**
 * profile state
 */ 
typedef uint8_t tProfileState;

/**
 * notofication event
 */
typedef uint8_t tNotificationEvent;

/**
 * notification callback to the application
 */ 
typedef void (* BLE_CALLBACK_FUNCTION_TYPE)(tNotificationEvent event,uint8_t evtLen,uint8_t* evtData);

/**
 * DIS callback
 */ 
typedef void (* DIS_CALLBACK)(uint8_t status);

/**
 * HID: we need to know the address of the connected Central
 */
typedef struct _tCentralDevice {
  uint8_t  peer_bdaddr_type;
  uint8_t peer_bdaddr[6];
} tCentralDevice;

extern tBLEProfileGlobalContext gBLEProfileContext;
/*******************************************************************************
 * Function Prototypes
*******************************************************************************/

/**
 * BLE_Profile_Init
 * 
 * @param[in] tSecurityParams : Security requirements of the host
 * @param[in] BLE_CALLBACK_FUNCTION_TYPE : callback to be called
 *            by the profile to notify the application
 * 
 * Initializes the data structures required to run any of the BLE profiles.
 * Also initializes the HCI
 */ 
tBleStatus BLE_Profile_Init (tSecurityParams * securityParameters, 
							 BLE_CALLBACK_FUNCTION_TYPE bleCallbackFunc );

/**
 * BLE_Profile_Get_Main_State
 * 
 * @return main state of the profile state machine
 */ 
tProfileState BLE_Profile_Get_Main_State (void);

/**
 * BLE_Profile_Get_Sub_State
 * 
 * @return sub state of the profile state machine
 */ 
tProfileState BLE_Profile_Get_Sub_State (void);

/**
 * BLE_Profile_Register_Profile
 * 
 * @param[in] profileContext : profile context of the profile 
 *            to be registered
 * 
 * Registers the profile context with the main profile. The profile ID
 * will be used to monitor the commands sent by the profile at the HCI.
 * Notifications of the events received via HCI will be sent only to
 * registered profiles via the callback registered by the profile
 * with the main profile
 * 
 * @return BLE_STATUS_SUCCESS if the profile is successfully registered
 *         BLE_STATUS_FAILED if the profile registration failed
 */ 
tBleStatus BLE_Profile_Register_Profile ( tProfileInterfaceContext * profileContext);

/**
 * BLE_Profile_Unregister_Profile
 * 
 * @param[in] profileContext : profile context of the profile 
 *            to be unregistered
 * 
 * Unregisters the profile context with the main profile. The profile ID
 * will be used to monitor the commands sent by the profile at the HCI.
 * Notifications of the events received via HCI will be sent only to
 * registered profiles via the callback registered by the profile
 * with the main profile
 * 
 * @return BLE_STATUS_SUCCESS if the profile is successfully registered
 *         BLE_STATUS_FAILED if the profile registration failed
 */ 
tBleStatus BLE_Profile_Unregister_Profile(tProfileInterfaceContext* profileContext);

/**
 * BLE_Profile_Vote_For_Advertisable_State
 * 
 * @param[in] profileContext : context of the profile which is voting
 * 
 * The host can command the controller to go into advertising state only when all
 * the profiles have completed their initialization in the scenario where there are
 * multiple profiles running. So each profile will notify the main profile that
 * it has completed its initialization. The main profile will allow the advertising
 * command only when all registered profiles notify that they have completed their
 * profile specific initialization
 * 
 * @return BLE_STATUS_SUCCESS if the voting was accepted else BLE_STATUS_FAILED
 */ 
tBleStatus BLE_Profile_Vote_For_Advertisable_State(tProfileInterfaceContext* profileContext);

/**
 * BLE_Profile_Disconnect
 * 
 * issues the disconnect command to the controller.
 * This command can be given by any of the profiles
 * or the application at any point in time when in
 * connection
 */ 
tBleStatus BLE_Profile_Disconnect(void);

/**
 * BLE_Profile_Change_Advertise_Sub_State
 * 
 * @param[in] profileContext : pointer to the profile context that is requesting
 *            the state change
 * @param[in] nextState : the state to which the main profile's sub state has to be changed
 * 
 * If the profile requesting the state change is a registered profile and 
 * the main profile is in a state to accept the change, the sub state is changed.
 * This function has to be called by the upper profiles to notify the main profile
 * about teh advertising status. This function has to be called with the nextState 
 * parameter set to BLE_PROFILE_STATE_CONNECTABLE_IDLE when advertising is disabled
 * and BLE_PROFILE_STATE_ADVERTISING when the advertising has been successfully enabled
 */ 
void BLE_Profile_Change_Advertise_Sub_State(tProfileInterfaceContext* profileContext, tProfileState nextState);

/**
 * BLE_Profile_Send_Pass_Key
 * 
 * @param[in] passkey : passkey to be sent to the controller
 * 
 * sends the passkey supplied by the user to the controller
 */ 
tBleStatus BLE_Profile_Send_Pass_Key(uint32_t passkey);

tBleStatus BLE_Profile_Start_Configuration_Exchange(void);

/**
 * BLE_Profile_Add_Advertisment_Service_UUID
 * 
 * @param[in] servUUID : UUID of the service to be added in the
 *            advertisement data
 * 
 * adds the UUID specified in the advertisement data list
 */ 
void BLE_Profile_Add_Advertisment_Service_UUID(uint16_t servUUID);

/**
 * BLE_Profile_Remove_Advertisment_Service_UUID
 * 
 * @param[in] servUUID : UUID of the service to be removed from the
 *            advertisement data
 * 
 * removes the UUID specified in the advertisement data list
 */ 
tBleStatus BLE_Profile_Remove_Advertisment_Service_UUID(uint16_t servUUID);

/**
 * Is_Profile_Present
 * 
 * @param[in] profileID: profile ID of the profile which needs to be
 *            checked for its registration with the main profile
 * 
 * searched through the list of registered profiles to see if
 * there is a profile of the ID specified is present
 * 
 * @return BLE_STATUS_SUCCESS if the profile specified by profileID
 *         is registered with the main profile else BLE_STATUS_FAILED
 */ 
tBleStatus Is_Profile_Present(uint8_t profileID);

/**
 * Profile_Notify_StateMachine_Update
 *
 * @param[in] None
 *
 * Profile's state machine update notification.
 * The application shall call in the main loop Profile_StateMachine() upon that
 * notification.
 */
void Profile_Notify_StateMachine_Update(void);

/**
 * Profile_Process_Q
 * 
 * processes the main profile state machine
 * and also the timer Q
 * 
 */ 
int Profile_Process_Q(void);


void BLE_Profile_Set_GATT_Procedure_Status(uint16_t opcode);

void BLE_Profile_Reset_GATT_Procedure_Status(void);

uint16_t BLE_Profile_Get_Current_GATT_Procedure_Opcode ( void );

void  BLE_Profile_Send_Security_Request(void);

void Profile_Process_Notification_Request(void);

#endif /* BLE_PROFILE_H_ */
