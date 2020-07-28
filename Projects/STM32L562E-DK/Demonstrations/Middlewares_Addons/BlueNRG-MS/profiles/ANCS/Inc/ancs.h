/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : ancs.h
* Author             : AMG -RF application team
* Version            : V1.0.0
* Date               : 10-June-2016
* Description        : Header file for Apple Notification Center Service Profile
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef __ANCS_H_
#define __ANCS_H_

/**
 *@addtogroup ANCS ANCS
 *@brief ANCS Profile.
 *@{
 */

/**
 *@defgroup ANCS_defines ANCS defines
 *@brief List of constant used in the ANCS.
 *@{
 */

/**
 * @anchor CategoryIDs
 * @name Category ID values
 * @{
 */
typedef enum {
  CategoryIDOther = 0,
  CategoryIDIncomingCall = 1,
  CategoryIDMissedCall = 2,
  CategoryIDVoicemail = 3,
  CategoryIDSocial = 4,
  CategoryIDSchedule = 5,
  CategoryIDEmail = 6,
  CategoryIDNews = 7,
  CategoryIDHealthAndFitness = 8,
  CategoryIDBusinessAndFinance = 9,
  CategoryIDLocation = 10,
  CategoryIDEntertainment = 11,
} CategoryID;
/**
 * @}
 */

/**
 * @anchor EventIDs
 * @name Event ID values
 * @{
 */
typedef enum {
  EventIDNotificationAdded = 0,
  EventIDNotificationModified = 1,
  EventIDNotificationRemoved = 2,
} EventID;
/**
 * @}
 */

/**
 * @anchor EventFlags
 * @name Event Flag values
 * @{
 */
typedef enum {
  EventFlagSilent = (1 << 0),
  EventFlagImportant = (1 << 1),
  EventFlagPreExisting = (1 << 2),
  EventFlagPositiveAction = (1 << 3),
  EventFlagNegativeAction = (1 << 4),
} EventFlags;
/**
 * @}
 */

/**
 * @anchor CommandIDs
 * @name Command ID values
 * @{
 */
typedef enum {
  CommandIDGetNotificationAttributes = 0,
  CommandIDGetAppAttributes = 1,
  CommandIDPerformNotificationAction = 2,
} CommandID;
/**
 * @}
 */

/**
 * @anchor NotificationAttributeIDs
 * @name Notification Attribute ID values
 * @{
 */
typedef enum {
  NotificationAttributeIDAppIdentifier = 0,
  NotificationAttributeIDTitle = 1, 
  NotificationAttributeIDSubtitle = 2,
  NotificationAttributeIDMessage = 3,
  NotificationAttributeIDMessageSize = 4,
  NotificationAttributeIDDate = 5,
  NotificationAttributeIDPositiveActionLabel = 6,
  NotificationAttributeIDNegativeActionLabel = 7,
} NotificationAttributeID;
/**
 * @}
 */

/**
 * @anchor ActionIDs
 * @name Action ID values
 * @{
 */
typedef enum {
  ActionIDPositive = 0,
  ActionIDNegative = 1,
}ActionID;
/**
 * @}
 */

/**
 * @anchor AppAttributeIDs
 * @name App Attribute ID values
 * @{
 */
typedef enum {
  AppAttributeIDDisplayName = 0,
} AppAttributeID;
/**
 * @}
 */

/**
 * @anchor ANCS_ErrorCode
 * @name ANCS Error Code values
 * @{
 */
#define ERROR_ANCS_NOT_PRESENT                   0x01
#define ERROR_ANCS_CONSUMER_ALREADY_SET          0x02
#define ERROR_MORE_THAN_ONE_ANCS_PRESENT         0x03
#define ERROR_WRONG_CONN_HANDLE                  0x04
#define ERROR_WRONG_CHARAC_PROP                  0x05
#define ERROR_ANCS_DISCOVERY_SERVICE             0x06
#define ERROR_ANCS_DISCOVERY_CHARACTERISTICS     0x07
#define ERROR_ANCS_DISCOVERY_EXTENDED_PROP       0x08
#define ERROR_ANCS_READ_PROC                     0x09
#define ERROR_ANCS_WRITE_PROC                    0x0A
#define ERROR_ANCS_DISCOVERY_CHARACTERISTIC_DESC 0x0B
#define ERROR_ANCS_BUSY                          0x0C
#define ERROR_INSUFFICIENT_MEMORY_SIZE           0x0D
#define ERROR_WRONG_NOTIFICATION_LENGTH          0x0E
/**
 * @}
 */

/**
 * @}
 */

/**
 *@defgroup ANCS_NotifType ANCS Notification Attribute Types
 *@brief ANCS notification attributes types.
 *@{
 */

/**
 * @anchor notificationAttrS
 * @name Notification Attributes typedef
 * @{
 */
typedef struct notificationAttrS {
  /** A 32-bit numerical value that is the unique identifier (UID) for the iOS notification. This
   *  value can be used as a handle in commands sent to the Control Point characteristic to interact with the
   *  iOS notification.
   */
  uint32_t UID;
  /** App Identifer flag, if TRUE this attribute is requested from the Get Notification Attributes command */
  uint8_t  appID_flag;
  /** Title flag, if TRUE this attribute is requested from the Get Notification Attributes command */
  uint8_t  title_flag;
  /** Max title length parameter */
  uint16_t title_max_size;
  /** Subtitle flag, if TRUE this attribute is requested from the Get Notification Attributes command */
  uint8_t  subtitle_flag;
  /** Max Subtitle length paramter */
  uint16_t subtitle_max_size;
  /** Message flag, if TRUE this attribute is requested from the Get Notification Attributes command */
  uint8_t  message_flag;
  /** Max Message length parameter */
  uint16_t message_max_size;
  /** Message size flag, if TRUE this attribute is requested from the Get Notification Attributes command */
  uint8_t  messageSize_flag;
  /** Data flag, if TRUE this attribute is requested from the Get Notification Attributes command */
  uint8_t  date_flag;
  /** Positive Action flag, if TRUE this attribute is requested from the Get Notification Attributes command */
  uint8_t  positiveAction_flag;
  /** Negative Action flag, if TRUE this attribute is requested from the Get Notification Attributes command */
  uint8_t  negativeAction_flag;
} notificationAttr_type;
/**
 * @}
 */

typedef struct Attribute_Group_Handle_Pair_t_s {
  uint16_t Found_Attribute_Handle /** Found Attribute handle */;
  uint16_t Group_End_Handle /** Group End handle */;
} Attribute_Group_Handle_Pair_t;
   
/**
 *@defgroup ANCS_Functions ANCS functions
 *@brief API for ANCS profile.
 *@{
 */

/**
 * @brief Configures the Notification Consumer.
 * @param conn_handle Connection handle used from the notification consumer to exchange commands.
 * @param dataBuffer_len Max length of the data buffer
 * @param dataBuffer Buffer used from the ANCS profile to provide the data recieved from the Notification Provider.
 * @return Value indicating success or error code.
 */
uint8_t ANCS_ConsumerSetup(uint16_t conn_handle, uint16_t dataBuffer_len, uint8_t *dataBuffer);

/**
 * @brief The Get Notification Attributes command allows an Notification Consumer to retrieve the attributes of a specific iOS notification.
 * @param attr Notification Attribute Type
 * @return Value indicating success or error code.
 */
uint8_t ANCS_GetNotificationAttr(notificationAttr_type attr);

/**
 * @brief The Get App Attributes command allows a notification consumer to retrieve attributes of a specific app installed on the notification provider.
 * @param appID_len App Attribute ID len buffer (see next parameter)
 * @param appID The string identifier of the app the client wants information about. This string must be NULL-terminated
 * @return Value indicating success or error code.
 */
uint8_t ANCS_GetAppAttr(uint8_t appID_len, uint8_t *appID);

/**
 * @brief The Perform Notification Action command allows an notification consumer to perform a predetermined action on a specific iOS notification.
 * @param notificationUID A 32-bit numerical value representing the UID of the iOS notification on which the client wants to perform an action
 * @param id The desired action the notification consumer wants to be performed on the iOS notification. (see @ref ActionIDs)
 * @return Value indicating success or error code.
 */
uint8_t ANCS_PerformNotificationAction(uint32_t notificationUID, ActionID id);

/**
 * @brief Runs the ANCS profile state machine.
 * @return None
 */
void ANCS_Tick(void);

/**
 * @brief This callback function returns the service discovery information. Call this function inside the aci_att_find_by_type_value_resp_event()
 * @param conn_handle Connection handle
 * @param Num_of_Handle_Pair Number of attribute, group handle pairs 
 * @param Attribute_Group_Handle_Pair Attribute group handle pairs data
 * @return None
 */
void ANCS_ServiceDiscovery_Callback(uint16_t conn_handle, 
                                    uint8_t Num_of_Handle_Pair,
                                    Attribute_Group_Handle_Pair_t Attribute_Group_Handle_Pair[]);

/**
 * @brief This callback function returns the characteristic discovery information. Call this function inside the aci_gatt_disc_read_char_by_uuid_resp_event()
 * @param conn_handle Connection handle
 * @param Num_of_Handle_Pair Number of attribute, group handle pairs
 * @param Attribute_Group_Handle_Pair Attribute group handle pairs data
 * @retval None
 */
void ANCS_CharacDiscovery_Callback(uint16_t conn_handle,
                                   uint16_t Attribute_Handle,
                                   uint8_t Attribute_Value_Length,
                                   uint8_t Attribute_Value[]);

/**
 * @brief This callback function returns the extended properties information. Call this function inside the aci_att_find_info_resp_event()
 * @param Connection_Handle Connection handle related to the response
 * @param Format Format of the hanndle-uuid pairs
 * @param Event_Data_Length Length of Handle_UUID_Pair in octets
 * @param Handle_UUID_Pair A sequence of handle-uuid pairs. if format=1, each pair is:[2 octets for handle, 2 octets for UUIDs], if format=2, each pair is:[2 octets for handle, 16 octets for UUIDs]
 * @retval None
 */
void ANCS_FindInfoExtendedProp_Callback(uint16_t Connection_Handle,
                                        uint8_t Format,
                                        uint8_t Event_Data_Length,
                                        uint8_t Handle_UUID_Pair[]);

/**
 * @brief This callback function is called when a GATT procedure is ended. Call this function inside the aci_gatt_proc_complete_event()
 * @param conn_handle Connection handle related to the response
 * @param Error_Code Indicates whether the procedure completed with an error or was successful
 * @retval None
 */
void ANCS_DiscoveryComplete_Callback(uint16_t conn_handle,
                                     uint8_t Error_Code);

/**
 * @brief This callback function is called when a read response event is received. Call this function inside the aci_att_read_resp_event()
 * @param Connection_Handle Connection handle related to the response
 * @param Event_Data_Length Length of following data
 * @param Attribute_Value The value of the attribute.
 * @retval None
 */
void ANCS_ReadResp_Callback(uint16_t Connection_Handle,
                            uint8_t Event_Data_Length,
                            uint8_t Attribute_Value[]);

/**
 * @brief This function is called when a notification is received. Call this function inside the aci_gatt_notification_event()
 * @param Connection_Handle Connection handle related to the response
 * @param Attribute_Handle The handle of the attribute
 * @param Attribute_Value_Length Length of Attribute_Value in octets
 * @param Attribute_Value The current value of the attribute
 * @retval None
 */
void ANCS_Data_Received(uint16_t Connection_Handle,
                        uint16_t Attribute_Handle,
                        uint8_t Attribute_Value_Length,
                        uint8_t Attribute_Value[]);

/**
 * @brief This event signals the application that the ANCS profile setup is completed.
 * @param status Status of the ANCS setup.
 * @retval None
 */
extern void ANCS_SetupComplete_event(uint8_t status);

/**
 * @brief This event signals the application that a notification is received from the Notification Provider.
 * @param status Status of the notification received.
 * @param evID Event ID value (see @ref EventIDs)
 * @param evFlag Event Flag value (see @ref EventFlags)
 * @param catID Category ID value (see @ref CategoryIDs)
 * @param catCount Number of notification pending
 * @param notifUID Notification UID
 * @retval None
 */
extern void ANCS_Notification_Source_Received_event(uint8_t status,
                                                    EventID evID,
                                                    EventFlags evFlag,
                                                    CategoryID catID,
                                                    uint8_t catCount,
                                                    uint32_t notifUID);

/**
 * @brief This event signals the application that a get notification attribute response or get App attribute response is received from the Notification Provider.
 * @param status Status of the notification received.
 * @param CommandID Command response received, possible value are: CommandIDGetNotificationAttributes, CommandIDGetAppAttributes
 * @param attrLen Attribute Length
 * @param attrList Attribute List (see ANCS spec for the List format details)
 * @retval None
 */
extern void ANCS_GetAttr_event(uint8_t  status,
                               uint8_t  CommandID,
                               uint16_t attrLen,
                               uint8_t  *attrList);

/**
 * @}
 */


/**
 * @}
 */
  
#endif /* __ANCS_H_ */
