/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : ancs.c
* Author             : AMG RF Application Team
* Version            : V1.0.0
* Date               : 06-May-2016
* Description        : BlueNRG ANCS demo command
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.2q
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "bluenrg_types.h"
#include "hci.h"
#include "bluenrg_aci.h"
#include "gp_timer.h"
#include "bluenrg_gatt_server.h"
#include "hci_const.h"
#include "bluenrg_gap.h"
#include "sm.h"
#include "bluenrg_utils.h"
#include "ancs.h"

/* Private macro -------------------------------------------------------------*/

//#define DEBUG_ANCS 1
#ifdef DEBUG_ANCS
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define MIN(a,b) ((a) < (b) )? (a) : (b)
#define MAX(a,b) ((a) > (b) )? (a) : (b)

/* Private typedef -----------------------------------------------------------*/
typedef struct ANCS_ContextS {
  uint8_t  state;
  uint8_t  servicePresent;
  uint16_t conn_handle;
  uint16_t servStartHandle;
  uint16_t servEndHandle;
  uint16_t notificationSourceHandle;
  uint8_t  notificationProp;
  uint16_t controlPointHandle;
  uint8_t  controlPointProp;
  uint16_t controlPointExtendedHandle;
  uint16_t controlPointExtendedValue;
  uint16_t dataSourceHandle;
  uint8_t  dataSourceProp;
  uint8_t  error_proc;
  uint16_t attr_bitmask;
  uint8_t  *data_buffer;
  uint16_t max_data_buffer_len;
  uint16_t buffer_len;
  uint8_t  header_length;
  uint8_t  header_index;
  uint8_t  cmd_ongoing;
  uint8_t  parsing_state;
  uint16_t parsing_len;
  uint8_t  performNotificationAction;
} ANCS_Context_type;

typedef union UUID_t_s {
  uint16_t UUID_16 /** 16-bit UUID */;
  uint8_t UUID_128[16] /** 128-bit UUID */;
} UUID_t;

/* Private define ------------------------------------------------------------*/

/* ANCS states */
#define ANCS_STATE_NOT_CONFIGURED 0x01
#define ANCS_STATE_IDLE 0x02
#define ANCS_STATE_DISC_SERVICE 0x03
#define ANCS_STATE_WAITING_DISC_SERVICE 0x04
#define ANCS_STATE_DISC_NOTIFICATION 0x05
#define ANCS_STATE_WAITING_DISC_NOTIFICATION 0x06
#define ANCS_STATE_DISC_CONTROL_POINT 0x07
#define ANCS_STATE_WAITING_DISC_CONTROL_POINT 0x08
#define ANCS_STATE_DISC_DATA_SOURCE 0x09
#define ANCS_STATE_WAITING_DISC_DATA_SOURCE 0x0A
#define ANCS_STATE_DISC_CTRLPOINT_EXTENDED_PROP 0x0B
#define ANCS_STATE_WAITING_DISC_CTRLPOINT_EXTENDED_PROP 0x0C
#define ANCS_STATE_READ_CTRLPOINT_EXTENDED_PROP_VALUE 0x0D
#define ANCS_STATE_WAITING_READ_CTRLPOINT_EXTENDED_PROP_VALUE 0x0E
#define ANCS_STATE_SUBSCRIBE_NOTIFICATION_SOURCE 0x0F
#define ANCS_STATE_WAITING_SUBSCRIBE_NOTIFICATION_SOURCE 0x10
#define ANCS_STATE_SUBSCRIBE_DATA_SOURCE 0x11
#define ANCS_STATE_WAITING_SUBSCRIBE_DATA_SOURCE 0x12
#define ANCS_STATE_CMD_ONGOING 0x13

/* Get Attr Response states */
#define PARSING_STATE_ATTR_ID           0x00
#define PARSING_STATE_LENGTH_1          0x01
#define PARSING_STATE_LENGTH_2          0x02
#define PARSING_STATE_PAYLOAD           0x03
#define PARSING_STATE_CMD_ID            0x04
#define PARSING_STATE_HEADER_ID         0x05
#define PARSING_STATE_REPORT_APPL_EVENT 0x06

/* Implementation define */
#define MIN_DATA_BUFFER_SIZE   20
#define NOTIFY_SOURCE_DATA_LEN 8

/* Private macro -------------------------------------------------------------*/

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
        uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
            uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
                uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

#define COPY_ANCS_SERVICE_UUID(uuid_struct)             COPY_UUID_128(uuid_struct, 0x79, 0x05, 0xF4, 0x31, 0xB5, 0xCE, 0x4E, 0x99, 0xA4, 0x0F, 0x4B, 0x1E, 0x12, 0x2D, 0x00, 0xD0)
#define COPY_NOTIFICATION_SOURCE_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct, 0x9F, 0xBF, 0x12, 0x0D, 0x63, 0x01, 0x42, 0xD9, 0x8C, 0x58, 0x25, 0xE6, 0x99, 0xA2, 0x1D, 0xBD)
#define COPY_CONTROL_POINT_CHAR_UUID(uuid_struct)       COPY_UUID_128(uuid_struct, 0x69, 0xD1, 0xD8, 0xF3, 0x45, 0xE1, 0x49, 0xA8, 0x98, 0x21, 0x9B, 0xBD, 0xFD, 0xAA, 0xD9, 0xD9)
#define COPY_DATA_SOURCE_CHAR_UUID(uuid_struct)         COPY_UUID_128(uuid_struct, 0x22, 0xEA, 0xC6, 0xE9, 0x24, 0xD6, 0x4B, 0xB5, 0xBE, 0x44, 0xB3, 0x6A, 0xCE, 0x7C, 0x7B, 0xFB)
   
/* Private variables ---------------------------------------------------------*/
ANCS_Context_type ANCS_Context;

/* Private Function ----------------------------------------------------------*/
static void notificationSource_process_data(uint8_t Attribute_Value_Length,
                                            uint8_t *Attribute_Value)
{
  uint32_t notific_uid;
  uint8_t status = BLE_STATUS_SUCCESS;
  
  if (Attribute_Value_Length != NOTIFY_SOURCE_DATA_LEN) {
    PRINTF("Error in Notification Source data received. Wrong Length %d (exepcted 8 bytes)\r\n", Attribute_Value_Length);
    status = ERROR_WRONG_NOTIFICATION_LENGTH;
  }

  notific_uid = Attribute_Value[4] | (uint32_t)(Attribute_Value[5]<<8) | (uint32_t)(Attribute_Value[6]<<16) | (uint32_t)(Attribute_Value[7]<<24);
  ANCS_Notification_Source_Received_event(status,
                                          (EventID)Attribute_Value[0],
                                          (EventFlags)Attribute_Value[1],
                                          (CategoryID)Attribute_Value[2],
                                          Attribute_Value[3],
                                          notific_uid);
}

static void attr_process_data(uint8_t Attribute_Value_Length, uint8_t *Attribute_Value)
{
  uint8_t index=0;

  if (ANCS_Context.cmd_ongoing) {
    
    if ((ANCS_Context.buffer_len + Attribute_Value_Length - index) > ANCS_Context.max_data_buffer_len) {
      ANCS_GetAttr_event(ERROR_INSUFFICIENT_MEMORY_SIZE,
                         ANCS_Context.data_buffer[0],
                         ANCS_Context.buffer_len-ANCS_Context.header_length,
                         &ANCS_Context.data_buffer[ANCS_Context.header_length]);      
    }
        
    BLUENRG_memcpy(&ANCS_Context.data_buffer[ANCS_Context.buffer_len], Attribute_Value, Attribute_Value_Length);
    ANCS_Context.buffer_len += Attribute_Value_Length;
    while(index < Attribute_Value_Length) {
      switch (ANCS_Context.parsing_state) {
      case PARSING_STATE_CMD_ID:
        ANCS_Context.header_index = 1;
        ANCS_Context.parsing_state = PARSING_STATE_HEADER_ID;
        break;
      case PARSING_STATE_HEADER_ID:
        ANCS_Context.header_index++;
        if (ANCS_Context.header_index == ANCS_Context.header_length) {
          ANCS_Context.parsing_state = PARSING_STATE_ATTR_ID;
        }
        break;
      case PARSING_STATE_ATTR_ID:
        ANCS_Context.attr_bitmask &= (uint16_t)(~(1<<Attribute_Value[index]));
        ANCS_Context.parsing_state = PARSING_STATE_LENGTH_1;
        break;
      case PARSING_STATE_LENGTH_1:
        ANCS_Context.parsing_len = Attribute_Value[index];
        ANCS_Context.parsing_state = PARSING_STATE_LENGTH_2;
        break;
      case PARSING_STATE_LENGTH_2:
        ANCS_Context.parsing_len |= (uint16_t)(Attribute_Value[index] << 8);
        if (ANCS_Context.parsing_len == 0)
          ANCS_Context.parsing_state = PARSING_STATE_REPORT_APPL_EVENT;
        else
          ANCS_Context.parsing_state = PARSING_STATE_PAYLOAD;
        break;  
      case PARSING_STATE_PAYLOAD:
        ANCS_Context.parsing_len--;
        if (ANCS_Context.parsing_len == 0)
          ANCS_Context.parsing_state = PARSING_STATE_REPORT_APPL_EVENT;
        break;
      }
      if ((ANCS_Context.attr_bitmask == 0) &&
          (ANCS_Context.parsing_state == PARSING_STATE_REPORT_APPL_EVENT)) {
        ANCS_GetAttr_event(BLE_STATUS_SUCCESS,
                           ANCS_Context.data_buffer[0],
                           ANCS_Context.buffer_len-ANCS_Context.header_length,
                           &ANCS_Context.data_buffer[ANCS_Context.header_length]);
        ANCS_Context.cmd_ongoing = FALSE;
      }
      if (ANCS_Context.parsing_state == PARSING_STATE_REPORT_APPL_EVENT)
        ANCS_Context.parsing_state = PARSING_STATE_ATTR_ID;
      index++;
    }
  }
}

static void ancsStateMachine(uint8_t state) {

  uint8_t ret;
  uint8_t uuid[16];
  uint16_t min_char_handle, max_char_handle;
  UUID_t  app;
  uint8_t enable_notif[2]={0x01, 0x00};
  
#ifdef DEBUG_ANCS
  static uint8_t old_state=0;

  if(old_state != state) {
    old_state = state;
    PRINTF("STATE = 0x%02x\r\n", state);
  }
#endif
  
  switch (state) {
  case ANCS_STATE_NOT_CONFIGURED:
    {
      ANCS_SetupComplete_event(ANCS_Context.error_proc);
      ANCS_Context.state =ANCS_STATE_IDLE; 
    }
    break;
  case ANCS_STATE_IDLE:
    {
      /* Nothing to do */
    }
    break;
  case ANCS_STATE_DISC_SERVICE:
    {
      COPY_ANCS_SERVICE_UUID(uuid);
      BLUENRG_memcpy(&app.UUID_128, uuid, 16);
      ret = aci_gatt_disc_prim_service_by_uuid(ANCS_Context.conn_handle, UUID_TYPE_128, (uint8_t*)&app);
      if (ret != BLE_STATUS_SUCCESS) {
        ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
        ANCS_Context.error_proc = ERROR_ANCS_DISCOVERY_SERVICE;
        PRINTF("Error in aci_gatt_disc_primary_service_by_uuid() = 0x%02x\r\n", ret);
      } else {
        ANCS_Context.state = ANCS_STATE_WAITING_DISC_SERVICE;
      }
    }
    break;
  case ANCS_STATE_WAITING_DISC_SERVICE:
      /* Nothing to do */
    break;
  case ANCS_STATE_DISC_NOTIFICATION:
    {      
      COPY_NOTIFICATION_SOURCE_CHAR_UUID(uuid);
      BLUENRG_memcpy(&app.UUID_128, uuid, 16);
      ret = aci_gatt_disc_charac_by_uuid(ANCS_Context.conn_handle,
                                         ANCS_Context.servStartHandle, ANCS_Context.servEndHandle,
                                         UUID_TYPE_128, (uint8_t*)&app);
      if (ret != BLE_STATUS_SUCCESS) {
        ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
        ANCS_Context.error_proc = ERROR_ANCS_DISCOVERY_CHARACTERISTICS;
        PRINTF("Error in aci_gatt_disc_char_by_uuid() = 0x%02x (DISC NOTIFICATION STATE)\r\n", ret);
      } else {
        ANCS_Context.state = ANCS_STATE_WAITING_DISC_NOTIFICATION;
      }
    }
    break;
  case ANCS_STATE_WAITING_DISC_NOTIFICATION:
   /* Nothing to do */
   break;
  case ANCS_STATE_DISC_CONTROL_POINT:
    {
      COPY_CONTROL_POINT_CHAR_UUID(uuid);
      BLUENRG_memcpy(&app.UUID_128, uuid, 16);
      ret = aci_gatt_disc_charac_by_uuid(ANCS_Context.conn_handle,
                                         ANCS_Context.servStartHandle, ANCS_Context.servEndHandle,
                                         UUID_TYPE_128, (uint8_t*)&app);
      if (ret != BLE_STATUS_SUCCESS) {
        ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
        ANCS_Context.error_proc = ERROR_ANCS_DISCOVERY_CHARACTERISTICS;
        PRINTF("Error in aci_gatt_disc_char_by_uuid() = 0x%02x (DISC CONTROL POINT STATE)\r\n", ret);
      } else {
        ANCS_Context.state = ANCS_STATE_WAITING_DISC_CONTROL_POINT;
      }
    }
    break;
 case ANCS_STATE_WAITING_DISC_CONTROL_POINT:
   /* Nothing to do */
   break;
  case ANCS_STATE_DISC_DATA_SOURCE:
    {
      COPY_DATA_SOURCE_CHAR_UUID(uuid);
      BLUENRG_memcpy(&app.UUID_128, uuid, 16);
      ret = aci_gatt_disc_charac_by_uuid(ANCS_Context.conn_handle,
                                         ANCS_Context.servStartHandle, ANCS_Context.servEndHandle,
                                         UUID_TYPE_128, (uint8_t*)&app);
      if (ret != BLE_STATUS_SUCCESS) {
        ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
        ANCS_Context.error_proc = ERROR_ANCS_DISCOVERY_CHARACTERISTICS;
        PRINTF("Error in aci_gatt_disc_char_by_uuid() = 0x%02x (DISC DATA SOURCE STATE)\r\n", ret);
      } else {
        ANCS_Context.state = ANCS_STATE_WAITING_DISC_DATA_SOURCE;
      }
    }
    break;
  case ANCS_STATE_WAITING_DISC_DATA_SOURCE:
    /* Nothing to do */
    break;
 case ANCS_STATE_DISC_CTRLPOINT_EXTENDED_PROP:
   {
     min_char_handle = MIN(ANCS_Context.notificationSourceHandle, ANCS_Context.dataSourceHandle);
     max_char_handle = MAX(ANCS_Context.notificationSourceHandle, ANCS_Context.dataSourceHandle);
     if (ANCS_Context.controlPointExtendedHandle > max_char_handle) {
       ret = aci_gatt_disc_all_charac_descriptors(ANCS_Context.conn_handle,
                                                  ANCS_Context.controlPointExtendedHandle+1,
                                                  ANCS_Context.servEndHandle);
     } else {
       if (ANCS_Context.controlPointExtendedHandle < min_char_handle) {
         ret = aci_gatt_disc_all_charac_descriptors(ANCS_Context.conn_handle,
                                                    ANCS_Context.controlPointExtendedHandle+1,
                                                    min_char_handle-1);
       } else {
         ret = aci_gatt_disc_all_charac_descriptors(ANCS_Context.conn_handle,
                                                    ANCS_Context.controlPointExtendedHandle+1,
                                                    max_char_handle-1);
       }
     }
     if (ret != BLE_STATUS_SUCCESS) {
       ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
       ANCS_Context.error_proc = ERROR_ANCS_DISCOVERY_CHARACTERISTIC_DESC;
       PRINTF("Error in aci_gatt_disc_all_char_desc() = 0x%02x (DISC CTRL EXTENDED PROP STATE)\r\n", ret);
     } else {
       ANCS_Context.state = ANCS_STATE_WAITING_DISC_CTRLPOINT_EXTENDED_PROP;
     }
   }
   break;
  case ANCS_STATE_WAITING_DISC_CTRLPOINT_EXTENDED_PROP:
    /* Nothing to do*/
    break;
  case ANCS_STATE_READ_CTRLPOINT_EXTENDED_PROP_VALUE:
    {
      ret = aci_gatt_read_charac_val(ANCS_Context.conn_handle,
                                     ANCS_Context.controlPointExtendedHandle);
      if (ret != BLE_STATUS_SUCCESS) {
        ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
        ANCS_Context.error_proc = ERROR_ANCS_READ_PROC;
        PRINTF("Error in aci_gatt_disc_char_by_uuid() = 0x%02x (DISC DATA SOURCE EXTENDED PROP STATE)\r\n", ret);
      } else {
        ANCS_Context.state = ANCS_STATE_WAITING_READ_CTRLPOINT_EXTENDED_PROP_VALUE;
      }
    }
    break;
  case ANCS_STATE_WAITING_READ_CTRLPOINT_EXTENDED_PROP_VALUE:
    /* Nothing to do */
    break;
  case ANCS_STATE_SUBSCRIBE_NOTIFICATION_SOURCE:
    {
      ret = BLE_STATUS_SUCCESS;
      if ((ANCS_Context.notificationSourceHandle != 0) &&
          (ANCS_Context.notificationProp & 0x10)) {
            ret = aci_gatt_write_charac_descriptor(ANCS_Context.conn_handle,
                                                   ANCS_Context.notificationSourceHandle+2,
                                                   2,
                                                   enable_notif);
      }
      if (ret != BLE_STATUS_SUCCESS) {
        ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
        ANCS_Context.error_proc = ERROR_ANCS_WRITE_PROC;
        PRINTF("Error in aci_gatt_write_char_desc() = 0x%02x (SUBSCRIBE NOTIF SOURCE STATE)\r\n", ret);
      } else {
        ANCS_Context.state = ANCS_STATE_WAITING_SUBSCRIBE_NOTIFICATION_SOURCE;
      }
    }
    break;
  case  ANCS_STATE_WAITING_SUBSCRIBE_NOTIFICATION_SOURCE:
    /* Nothing to do */
    break;
  case ANCS_STATE_SUBSCRIBE_DATA_SOURCE:
    {
      ret = BLE_STATUS_SUCCESS;
      if ((ANCS_Context.dataSourceHandle != 0) &&
          (ANCS_Context.dataSourceProp & 0x10)) {
        ANCS_Context.state = ANCS_STATE_WAITING_SUBSCRIBE_DATA_SOURCE;
        ret = aci_gatt_write_charac_descriptor(ANCS_Context.conn_handle,
                                               ANCS_Context.dataSourceHandle+2,
                                               2,
                                               enable_notif);
      }
      if (ret != BLE_STATUS_SUCCESS) {
        ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
        ANCS_Context.error_proc = ERROR_ANCS_WRITE_PROC;
        PRINTF("Error in aci_gatt_write_char_desc() = 0x%02x (SUBSCRIBE DATA SOURCE STATE)\r\n", ret);
      }
    }
    break;
  case ANCS_STATE_WAITING_SUBSCRIBE_DATA_SOURCE:
    /* Nothing to do */
    break;
  case ANCS_STATE_CMD_ONGOING:
    /* Nothing to do */
    break;
  }
}

/* Public Function -----------------------------------------------------------*/
uint8_t ANCS_ConsumerSetup(uint16_t conn_handle, uint16_t dataBuffer_len, uint8_t *dataBuffer)
{
  /* Reset the Context Variable */
  BLUENRG_memset(&ANCS_Context, 0, sizeof(ANCS_Context));

  if (dataBuffer_len < MIN_DATA_BUFFER_SIZE)
    return ERROR_INSUFFICIENT_MEMORY_SIZE;
  
  ANCS_Context.conn_handle = conn_handle;
  ANCS_Context.max_data_buffer_len = dataBuffer_len;
  ANCS_Context.data_buffer = dataBuffer;
  ANCS_Context.state = ANCS_STATE_DISC_SERVICE;

  return BLE_STATUS_SUCCESS;
}
 
void ANCS_Tick(void)
{
  ancsStateMachine(ANCS_Context.state);
}

void ANCS_ServiceDiscovery_Callback(uint16_t conn_handle, 
                                    uint8_t Num_of_Handle_Pair,
                                    Attribute_Group_Handle_Pair_t Attribute_Group_Handle_Pair[])
{
  if (ANCS_Context.servicePresent)
    return;

  if ((conn_handle != ANCS_Context.conn_handle) ||
      (Num_of_Handle_Pair != 1)) {
    ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
    ANCS_Context.error_proc = ERROR_MORE_THAN_ONE_ANCS_PRESENT;
    return;
  }

  ANCS_Context.servStartHandle = Attribute_Group_Handle_Pair[0].Found_Attribute_Handle;
  ANCS_Context.servEndHandle = Attribute_Group_Handle_Pair[0].Group_End_Handle;
  ANCS_Context.servicePresent = TRUE;
}

void ANCS_CharacDiscovery_Callback(uint16_t conn_handle,
                                   uint16_t Attribute_Handle,
                                   uint8_t Attribute_Value_Length,
                                   uint8_t Attribute_Value[])
{
  if (conn_handle != ANCS_Context.conn_handle) {
    ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
    ANCS_Context.error_proc = ERROR_WRONG_CONN_HANDLE;
    return;
  }

  /* Discovery Notification Source */
  if (ANCS_Context.state == ANCS_STATE_WAITING_DISC_NOTIFICATION) {
    if ((Attribute_Value[0]&0x10) == 0) {
      ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
      ANCS_Context.error_proc = ERROR_WRONG_CHARAC_PROP;
      return;
    }
    ANCS_Context.notificationSourceHandle = Attribute_Handle;
    ANCS_Context.notificationProp = Attribute_Value[0];
  }

  /* Discovery Control Point */
  if (ANCS_Context.state == ANCS_STATE_WAITING_DISC_CONTROL_POINT) {
    if ((Attribute_Value[0]&0x08) == 0) {
      ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
      ANCS_Context.error_proc = ERROR_WRONG_CHARAC_PROP;
      return;
    }
    ANCS_Context.controlPointHandle = Attribute_Handle;
    ANCS_Context.controlPointProp = Attribute_Value[0];    
  }

  /* Discovery Data Source */
  if (ANCS_Context.state == ANCS_STATE_WAITING_DISC_DATA_SOURCE) {
    if ((Attribute_Value[0]&0x10) == 0) {
      ANCS_Context.state = ANCS_STATE_NOT_CONFIGURED;
      ANCS_Context.error_proc = ERROR_WRONG_CHARAC_PROP;
      return;
    }
    ANCS_Context.dataSourceHandle = Attribute_Handle;
    ANCS_Context.dataSourceProp = Attribute_Value[0];    
   }
}

void ANCS_FindInfoExtendedProp_Callback(uint16_t Connection_Handle,
                                        uint8_t Format,
                                        uint8_t Event_Data_Length,
                                        uint8_t Handle_UUID_Pair[])
{
  uint8_t i;
  uint16_t app_handle, app_uuid;

  if ((Format != 1) || (ANCS_Context.conn_handle != Connection_Handle))
    return;

  for (i=0; i<Event_Data_Length; i+=4) {
    app_handle = Handle_UUID_Pair[i] | (uint16_t)(Handle_UUID_Pair[i+1]<<8);
    app_uuid = Handle_UUID_Pair[i+2] | (uint16_t)(Handle_UUID_Pair[i+3]<<8);
    if (app_uuid == CHAR_EXTENDED_PROP_DESC_UUID) {
      ANCS_Context.controlPointExtendedHandle = app_handle;
      break;
    }
  }
}

void ANCS_DiscoveryComplete_Callback(uint16_t conn_handle,
                                     uint8_t Error_Code)
{
  if (ANCS_Context.conn_handle != conn_handle)
    return;
  
  switch (ANCS_Context.state) {
  case ANCS_STATE_WAITING_DISC_SERVICE:
    ANCS_Context.state = ANCS_STATE_DISC_NOTIFICATION;
    break;
  case ANCS_STATE_WAITING_DISC_NOTIFICATION:
    ANCS_Context.state = ANCS_STATE_DISC_CONTROL_POINT;
    break;
  case ANCS_STATE_WAITING_DISC_CONTROL_POINT:
    ANCS_Context.state = ANCS_STATE_DISC_DATA_SOURCE;
    break;
  case ANCS_STATE_WAITING_DISC_DATA_SOURCE:
    if (ANCS_Context.controlPointProp & 0x80)
      ANCS_Context.state = ANCS_STATE_DISC_CTRLPOINT_EXTENDED_PROP;
    else
      ANCS_Context.state = ANCS_STATE_SUBSCRIBE_NOTIFICATION_SOURCE;
    break;
  case ANCS_STATE_WAITING_DISC_CTRLPOINT_EXTENDED_PROP:
    ANCS_Context.state = ANCS_STATE_READ_CTRLPOINT_EXTENDED_PROP_VALUE;
    break;
  case ANCS_STATE_WAITING_READ_CTRLPOINT_EXTENDED_PROP_VALUE:
    ANCS_Context.state = ANCS_STATE_SUBSCRIBE_NOTIFICATION_SOURCE;
    break;
  case ANCS_STATE_WAITING_SUBSCRIBE_NOTIFICATION_SOURCE:
    ANCS_Context.state = ANCS_STATE_SUBSCRIBE_DATA_SOURCE;
    break;
  case ANCS_STATE_WAITING_SUBSCRIBE_DATA_SOURCE:
    ANCS_Context.state = ANCS_STATE_IDLE;
    ANCS_SetupComplete_event(Error_Code);
    break;
  case ANCS_STATE_CMD_ONGOING:
    if (Error_Code != BLE_STATUS_SUCCESS)
      ANCS_GetAttr_event(Error_Code, 0, 0, 0);
    ANCS_Context.state = ANCS_STATE_IDLE;
    if ((Error_Code != BLE_STATUS_SUCCESS) || ANCS_Context.performNotificationAction) {
      ANCS_Context.cmd_ongoing = FALSE;
      ANCS_Context.performNotificationAction = FALSE;
    }
    break;
  default:
    /* Nothing to do */
    PRINTF("??? State Not expected 0x%02x\r\n",ANCS_Context.state);
  }
}

void ANCS_ReadResp_Callback(uint16_t Connection_Handle,
                            uint8_t Event_Data_Length,
                            uint8_t Attribute_Value[])
{
  if (ANCS_Context.conn_handle != Connection_Handle)
    return;

  switch (ANCS_Context.state) {
  case ANCS_STATE_WAITING_READ_CTRLPOINT_EXTENDED_PROP_VALUE:
    ANCS_Context.controlPointExtendedValue = Attribute_Value[0] | (uint16_t)(Attribute_Value[1]<<8);
    break;
  }
}

void ANCS_Data_Received(uint16_t Connection_Handle,
                        uint16_t Attribute_Handle,
                        uint8_t Attribute_Value_Length,
                        uint8_t Attribute_Value[])
{
  if ((Connection_Handle != ANCS_Context.conn_handle) ||
      ((Attribute_Handle != (ANCS_Context.notificationSourceHandle+1)) &&
       (Attribute_Handle != (ANCS_Context.dataSourceHandle+1))))
    return;

  if (Attribute_Handle == (ANCS_Context.notificationSourceHandle+1))
    notificationSource_process_data(Attribute_Value_Length, Attribute_Value);

  if (Attribute_Handle == (ANCS_Context.dataSourceHandle+1))
    attr_process_data(Attribute_Value_Length, Attribute_Value);
}

uint8_t ANCS_GetNotificationAttr(notificationAttr_type attr)
{
  uint8_t ret, index;

  if (ANCS_Context.state == ANCS_STATE_CMD_ONGOING)
    return ERROR_ANCS_BUSY;
  
  index = 0;
  ANCS_Context.buffer_len = 0;
  ANCS_Context.attr_bitmask = 0;
  ANCS_Context.header_length = 5;
  ANCS_Context.parsing_state = PARSING_STATE_CMD_ID;

  ANCS_Context.data_buffer[index] = CommandIDGetNotificationAttributes;
  index++;
  BLUENRG_memcpy(&ANCS_Context.data_buffer[index], &attr.UID, 4);
  index += 4;
  if (attr.appID_flag) {
    ANCS_Context.attr_bitmask |= 1<< NotificationAttributeIDAppIdentifier;
    ANCS_Context.data_buffer[index] = NotificationAttributeIDAppIdentifier;
    index++;
  }
  if (attr.title_flag) {
    ANCS_Context.attr_bitmask |= 1<< NotificationAttributeIDTitle;
    ANCS_Context.data_buffer[index] = NotificationAttributeIDTitle;
    index++;
    if (attr.title_max_size > 0) {
      BLUENRG_memcpy(&ANCS_Context.data_buffer[index], &attr.title_max_size, 2);
      index += 2;
    }
  }
  if (attr.subtitle_flag) {
    ANCS_Context.attr_bitmask |= 1<< NotificationAttributeIDSubtitle;
    ANCS_Context.data_buffer[index] = NotificationAttributeIDSubtitle;
    index++;
    if (attr.subtitle_max_size > 0) {
      BLUENRG_memcpy(&ANCS_Context.data_buffer[index], &attr.subtitle_max_size, 2);
      index += 2;
    }
  }
  if (attr.message_flag) {
    ANCS_Context.attr_bitmask |= 1<< NotificationAttributeIDMessage;
    ANCS_Context.data_buffer[index] = NotificationAttributeIDMessage;
    index++;
    if (attr.message_max_size > 0) {
      BLUENRG_memcpy(&ANCS_Context.data_buffer[index], &attr.message_max_size, 2);
      index += 2;
    }
  }
  if (attr.messageSize_flag) {
    ANCS_Context.attr_bitmask |= 1<< NotificationAttributeIDMessageSize;
    ANCS_Context.data_buffer[index] = NotificationAttributeIDMessageSize;
    index++;
  }
  if (attr.date_flag) {
    ANCS_Context.attr_bitmask |= 1<< NotificationAttributeIDDate;
    ANCS_Context.data_buffer[index] = NotificationAttributeIDDate;
    index++;    
  }
  if (attr.positiveAction_flag) {
    ANCS_Context.attr_bitmask |= 1<< NotificationAttributeIDPositiveActionLabel;
    ANCS_Context.data_buffer[index] = NotificationAttributeIDPositiveActionLabel;
    index++;        
  }
  if (attr.negativeAction_flag) {
    ANCS_Context.attr_bitmask |= 1<< NotificationAttributeIDNegativeActionLabel;
    ANCS_Context.data_buffer[index] = NotificationAttributeIDNegativeActionLabel;
    index++;            
  }

  ret = aci_gatt_write_charac_value(ANCS_Context.conn_handle,
                                    ANCS_Context.controlPointHandle+1,
                                    index,
                                    ANCS_Context.data_buffer);
  if (ret == BLE_STATUS_SUCCESS) {
    ANCS_Context.state = ANCS_STATE_CMD_ONGOING;
    ANCS_Context.cmd_ongoing = TRUE;
  }
    
  return ret;
}

uint8_t ANCS_GetAppAttr(uint8_t appID_len,
                        uint8_t *appID)
{
  uint8_t ret, index;

  if (ANCS_Context.state == ANCS_STATE_CMD_ONGOING)
    return ERROR_ANCS_BUSY;
  
  index = 0;
  ANCS_Context.buffer_len = 0;
  ANCS_Context.attr_bitmask = 0;
  ANCS_Context.header_length = appID_len+1;
  ANCS_Context.parsing_state = PARSING_STATE_CMD_ID;

  ANCS_Context.data_buffer[index] = CommandIDGetAppAttributes;
  index++;
  BLUENRG_memcpy(&ANCS_Context.data_buffer[index], appID, appID_len);
  index += appID_len;
  ANCS_Context.attr_bitmask |= 1<< AppAttributeIDDisplayName;
  ANCS_Context.data_buffer[index] = AppAttributeIDDisplayName;
  index++;

  if (index > (ATT_MTU-3)) {
    ret = aci_gatt_write_long_charac_val(ANCS_Context.conn_handle,
                                         ANCS_Context.controlPointHandle+1,
                                         0,
                                         index,
                                         ANCS_Context.data_buffer);
  } else {
    ret = aci_gatt_write_charac_value(ANCS_Context.conn_handle,
                                      ANCS_Context.controlPointHandle+1,
                                      index,
                                      ANCS_Context.data_buffer);
  }

  if (ret == BLE_STATUS_SUCCESS) {
    ANCS_Context.state = ANCS_STATE_CMD_ONGOING;
    ANCS_Context.cmd_ongoing = TRUE;
  }
  
  return ret;
}

uint8_t ANCS_PerformNotificationAction(uint32_t notificationUID, ActionID id)
{
  uint8_t ret, index;

  if (ANCS_Context.state == ANCS_STATE_CMD_ONGOING)
    return ERROR_ANCS_BUSY;

  index = 0;
  ANCS_Context.data_buffer[index] = CommandIDPerformNotificationAction;
  index++;
  BLUENRG_memcpy(&ANCS_Context.data_buffer[index], &notificationUID, 4);
  index += 4;
  ANCS_Context.data_buffer[index] = id;
  index++;

  ret = aci_gatt_write_charac_value(ANCS_Context.conn_handle,
                                    ANCS_Context.controlPointHandle+1,
                                    index,
                                    ANCS_Context.data_buffer);
  if (ret == BLE_STATUS_SUCCESS) {
    ANCS_Context.state = ANCS_STATE_CMD_ONGOING;
    ANCS_Context.performNotificationAction = TRUE;
    ANCS_Context.cmd_ongoing = TRUE;
  }

  return ret;
}
