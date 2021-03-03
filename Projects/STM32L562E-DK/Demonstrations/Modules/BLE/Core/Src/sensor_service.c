/**
  ******************************************************************************
  * @file    sensor_service.c
  * @author  System Research & Applications Team - Catania Lab.
  * @version V4.0.0
  * @date    20-Sep-2019
  * @brief   Add 4 bluetooth services using vendor specific profiles.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#include <stdio.h>
#include "TargetFeatures.h"
#include "main.h"
#include "MetaDataManager.h"
#include "sensor_service.h"
#include "HWAdvanceFeatures.h"
#include "bluenrg_utils.h"
#include "bluenrg_l2cap_aci.h"
#include "uuid_ble_service.h"

/* Exported variables ---------------------------------------------------------*/
int connected = FALSE;
uint8_t set_connectable = TRUE;

/* SD card logging status (stop=0, start=1) */
uint8_t  SD_Card_Status= 0x00;
/* Feature mask that identify the data mens selected for recording*/
uint32_t SD_Card_FeaturesMask= 0x00000000;
/* Time range for data recording in second */
uint32_t SD_Card_StepTime= 0x00000000;

volatile uint32_t FeatureMask;

/* Imported Variables -------------------------------------------------------------*/
extern uint32_t ConnectionBleStatus;

/* Code for MotionAR integration - Start Section */
extern MAR_output_t ActivityCode;
/* Code for MotionAR integration - End Section */

/* Code for MotionCP integration - Start Section */
extern MCP_output_t CarryPositionCode;
/* Code for MotionCP integration - End Section */

/* Code for MotionGR integration - Start Section */
extern MGR_output_t GestureRecognitionCode;
/* Code for MotionGR integration - End Section */

extern uint32_t ForceReMagnetoCalibration;

extern uint32_t FirstConnectionConfig;

extern TIM_HandleTypeDef    TimCCHandle;

extern volatile float RMS_Ch[];
extern float DBNOISE_Value_Old_Ch[];

extern uint8_t bdaddr[6];
extern uint8_t NodeName[8];

extern uint32_t uhCCR4_Val;

/* Private variables ------------------------------------------------------------*/
static uint32_t FirstCommandSent= 1;

static uint16_t HWServW2STHandle;
static uint16_t AccGyroMagCharHandle;
static uint16_t AccEventCharHandle;

/* Code for MotionAR integration - Start Section */
static uint16_t ActivityRecCharHandle;
/* Code for MotionAR integration - End Section */

/* Code for MotionCP integration - Start Section */
static uint16_t CarryPosRecCharHandle;
/* Code for MotionCP integration - End Section */

/* Code for MotionGR integration - Start Section */
static uint16_t GestureRecCharHandle;
/* Code for MotionGR integration - End Section */

static uint16_t ConfigServW2STHandle;
static uint16_t ConfigCharHandle;

static uint16_t ConsoleW2STHandle;
static uint16_t TermCharHandle;
static uint16_t StdErrCharHandle;

static uint8_t LastStderrBuffer[W2ST_CONSOLE_MAX_CHAR_LEN];
static uint8_t LastStderrLen;
static uint8_t LastTermBuffer[W2ST_CONSOLE_MAX_CHAR_LEN];
static uint8_t LastTermLen;

static uint16_t connection_handle = 0;

/* Private functions ------------------------------------------------------------*/
static void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle);
static void GAP_DisconnectionComplete_CB(void);
static uint32_t DebugConsoleCommandParsing(uint8_t * att_data, uint8_t data_length);
static uint32_t ConfigCommandParsing(uint8_t * att_data, uint8_t data_length);
static void Force_UUID_Rescan(void);

/************************************/
/* Hardware Characteristics Service */
/************************************/
static void AccGyroMag_AttributeModified_CB(uint8_t *att_data);
static void AccelerometerEvents_AttributeModified_CB(uint8_t *att_data);

/***********************************
* Software Characteristics Service *
*        MotionXX Section          *
************************************/

/* Code for MotionAR integration - Start Section */
static void ActivityRecognition_AttributeModified_CB(uint8_t *att_data);
/* Code for MotionAR integration - End Section */

/* Code for MotionCP integration - Start Section */
static void CarryPosition_AttributeModified_CB(uint8_t *att_data);
/* Code for MotionCP integration - End Section */

/* Code for MotionGR integration - Start Section */
static void GestureRecognition_AttributeModified_CB(uint8_t *att_data);
/* Code for MotionGR integration - End Section */

/* Private define ------------------------------------------------------------*/

#ifdef ACC_BLUENRG_CONGESTION
#define ACI_GATT_UPDATE_CHAR_VALUE safe_aci_gatt_update_char_value
static int32_t breath;


/* @brief  Update the value of a characteristic avoiding (for a short time) to
 *         send the next updates if an error in the previous sending has
 *         occurred.
 * @param  servHandle The handle of the service
 * @param  charHandle The handle of the characteristic
 * @param  charValOffset The offset of the characteristic
 * @param  charValueLen The length of the characteristic
 * @param  charValue The pointer to the characteristic
 * @retval tBleStatus Status
 */
tBleStatus safe_aci_gatt_update_char_value(uint16_t servHandle,
				      uint16_t charHandle,
				      uint8_t charValOffset,
				      uint8_t charValueLen,
				      const uint8_t *charValue)
{
  tBleStatus ret = BLE_STATUS_INSUFFICIENT_RESOURCES;

  if (breath > 0) {
    breath--;
  } else {
    ret = aci_gatt_update_char_value(servHandle,charHandle,charValOffset,charValueLen,charValue);

    if (ret != BLE_STATUS_SUCCESS){
      breath = ACC_BLUENRG_CONGESTION_SKIP;
    }
  }

  return (ret);
}

#else /* ACC_BLUENRG_CONGESTION */
#define ACI_GATT_UPDATE_CHAR_VALUE aci_gatt_update_char_value
#endif /* ACC_BLUENRG_CONGESTION */


/**
 * @brief  Add the Config service using a vendor specific profile
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Add_ConfigW2ST_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];

  COPY_CONFIG_SERVICE_UUID(uuid);
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 1+3,&ConfigServW2STHandle);

  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  COPY_CONFIG_W2ST_CHAR_UUID(uuid);
  ret =  aci_gatt_add_char(ConfigServW2STHandle, UUID_TYPE_128, uuid, 20 /* Max Dimension */,
                           CHAR_PROP_NOTIFY| CHAR_PROP_WRITE_WITHOUT_RESP,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &ConfigCharHandle);

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  return BLE_STATUS_SUCCESS;

fail:
  //ALLMEMS1_PRINTF("Error while adding Configuration service.\n");
  return BLE_STATUS_ERROR;
}


/**
 * @brief  Add the Console service using a vendor specific profile
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Add_ConsoleW2ST_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];

  COPY_CONSOLE_SERVICE_UUID(uuid);
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 1+3*2,&ConsoleW2STHandle);

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  COPY_TERM_CHAR_UUID(uuid);
  ret =  aci_gatt_add_char(ConsoleW2STHandle, UUID_TYPE_128, uuid, W2ST_CONSOLE_MAX_CHAR_LEN,
                           CHAR_PROP_NOTIFY| CHAR_PROP_WRITE_WITHOUT_RESP | CHAR_PROP_WRITE | CHAR_PROP_READ ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &TermCharHandle);

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  COPY_STDERR_CHAR_UUID(uuid);
  ret =  aci_gatt_add_char(ConsoleW2STHandle, UUID_TYPE_128, uuid, W2ST_CONSOLE_MAX_CHAR_LEN,
                           CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &StdErrCharHandle);

  if (ret != BLE_STATUS_SUCCESS) {
     goto fail;
  }

  return BLE_STATUS_SUCCESS;

fail:
  //ALLMEMS1_PRINTF("Error while adding Console service.\n");
  return BLE_STATUS_ERROR;
}

/**
 * @brief  Update Stderr characteristic value
 * @param  uint8_t *data string to write
 * @param  uint8_t length lengt of string to write
 * @retval tBleStatus      Status
 */
tBleStatus Stderr_Update(uint8_t *data,uint8_t length)
{
  tBleStatus ret;
  uint8_t Offset;
  uint8_t DataToSend;

  /* Split the code in packages*/
  for(Offset =0; Offset<length; Offset +=W2ST_CONSOLE_MAX_CHAR_LEN){
    DataToSend = (length-Offset);
    DataToSend = (DataToSend>W2ST_CONSOLE_MAX_CHAR_LEN) ?  W2ST_CONSOLE_MAX_CHAR_LEN : DataToSend;

    /* keep a copy */
    memcpy(LastStderrBuffer,data+Offset,DataToSend);
    LastStderrLen = DataToSend;

    ret = aci_gatt_update_char_value(ConsoleW2STHandle, StdErrCharHandle, 0, DataToSend , data+Offset);
    if (ret != BLE_STATUS_SUCCESS) {
      return BLE_STATUS_ERROR;
    }
    HAL_Delay(10);
  }

  return BLE_STATUS_SUCCESS;
}

/**
 * @brief  Update Terminal characteristic value
 * @param  uint8_t *data string to write
 * @param  uint8_t length lengt of string to write
 * @retval tBleStatus      Status
 */
tBleStatus Term_Update(uint8_t *data,uint8_t length)
{
  tBleStatus ret;
  uint8_t Offset;
  uint8_t DataToSend;

  /* Split the code in packages */
  for(Offset =0; Offset<length; Offset +=W2ST_CONSOLE_MAX_CHAR_LEN){
    DataToSend = (length-Offset);
    DataToSend = (DataToSend>W2ST_CONSOLE_MAX_CHAR_LEN) ?  W2ST_CONSOLE_MAX_CHAR_LEN : DataToSend;

    /* keep a copy */
    memcpy(LastTermBuffer,data+Offset,DataToSend);
    LastTermLen = DataToSend;

    ret = aci_gatt_update_char_value(ConsoleW2STHandle, TermCharHandle, 0, DataToSend , data+Offset);
    if (ret != BLE_STATUS_SUCCESS) {
        ALLMEMS1_PRINTF("Error Updating Stdout Char\r\n");
      return BLE_STATUS_ERROR;
    }
    HAL_Delay(20);
  }

  return BLE_STATUS_SUCCESS;
}

/**
 * @brief  Update Stderr characteristic value after a read request
 * @param None
 * @retval tBleStatus      Status
 */
static tBleStatus Stderr_Update_AfterRead(void)
{
  tBleStatus ret;

  ret = aci_gatt_update_char_value(ConsoleW2STHandle, StdErrCharHandle, 0, LastStderrLen , LastStderrBuffer);
  if (ret != BLE_STATUS_SUCCESS) {
    return BLE_STATUS_ERROR;
  }

  return BLE_STATUS_SUCCESS;
}

/**
 * @brief  Update Terminal characteristic value after a read request
 * @param None
 * @retval tBleStatus      Status
 */
static tBleStatus Term_Update_AfterRead(void)
{
  tBleStatus ret;

  ret = aci_gatt_update_char_value(ConsoleW2STHandle, TermCharHandle, 0, LastTermLen , LastTermBuffer);
  if (ret != BLE_STATUS_SUCCESS) {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating Stdout Char\r\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Error Updating Stdout Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }

  return BLE_STATUS_SUCCESS;
}

/* Code for MotionAR integration - Start Section */
/**
 * @brief  Update Activity Recognition value
 * @param  MAR_output_t ActivityCode Activity Recognized
 * @retval tBleStatus      Status
 */
tBleStatus ActivityRec_Update(MAR_output_t ActivityCode)
{
  tBleStatus ret;

  uint8_t buff[2+ 1];

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  buff[2] = ActivityCode;

  ret = aci_gatt_update_char_value(HWServW2STHandle, ActivityRecCharHandle, 0, 2+1, buff);

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating ActivityRec Char\r\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Error Updating ActivityRec Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}
/* Code for MotionAR integration - End Section */

/* Code for MotionCP integration - Start Section */
/**
 * @brief  Update Carry Position Recognition value
 * @param  MCP_output_t CarryPositionCode Carry Position Recognized
 * @retval tBleStatus      Status
 */
tBleStatus CarryPosRec_Update(MCP_output_t CarryPositionCode)
{
  tBleStatus ret;

  uint8_t buff[2+ 1];

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  buff[2] = CarryPositionCode;

  ret = aci_gatt_update_char_value(HWServW2STHandle, CarryPosRecCharHandle, 0, 2+1, buff);

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating CarryPosRec Char\r\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Error Updating CarryPosRec Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}
/* Code for MotionCP integration - End Section */

/* Code for MotionGR integration - Start Section */
/**
 * @brief  Update Gesture Recognition value
 * @param  MGR_output_t GestureCode Gesture Recognized
 * @retval tBleStatus      Status
 */
tBleStatus GestureRec_Update(MGR_output_t GestureCode)
{
  tBleStatus ret;

  uint8_t buff[2+ 1];

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  buff[2] = GestureCode;

  ret = aci_gatt_update_char_value(HWServW2STHandle, GestureRecCharHandle, 0, 2+1, buff);

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating Gesture Rec Char\r\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Error Updating Gesture Rec Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}
/* Code for MotionGR integration - End Section */

/* @brief  Send a notification for answering to a configuration command for Accelerometer events
 * @param  uint32_t Feature Feature calibrated
 * @param  uint8_t Command Replay to this Command
 * @param  uint8_t data result to send back
 * @retval tBleStatus Status
 */
tBleStatus Config_Notify(uint32_t Feature,uint8_t Command,uint8_t data)
{
  tBleStatus ret;
  uint8_t buff[2+4+1+1];

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  STORE_BE_32(buff+2,Feature);
  buff[6] = Command;
  buff[7] = data;

  ret = aci_gatt_update_char_value (ConfigServW2STHandle, ConfigCharHandle, 0, 8,buff);
  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating Configuration Char\r\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Error Updating Configuration Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

/**
 * @brief  Send a notification When the DS3 detects one Acceleration event
 * @param  Command to Send
 * @retval tBleStatus Status
 */
tBleStatus AccEvent_Notify(uint16_t Command, uint8_t dimByte)
{
  tBleStatus ret= BLE_STATUS_SUCCESS;
  uint8_t buff_2[2+2];
  uint8_t buff_3[2+3];

  switch(dimByte)
  {
  case 2:
    STORE_LE_16(buff_2  ,(HAL_GetTick()>>3));
    STORE_LE_16(buff_2+2,Command);
    ret = aci_gatt_update_char_value(HWServW2STHandle, AccEventCharHandle, 0, 2+2,buff_2);
    break;
  case 3:
    STORE_LE_16(buff_3  ,(HAL_GetTick()>>3));
    buff_3[2]= 0;
    STORE_LE_16(buff_3+3,Command);
    ret = aci_gatt_update_char_value(HWServW2STHandle, AccEventCharHandle, 0, 2+3,buff_3);
    break;
  }

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating AccEvent_Notify Char\r\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Error Updating AccEvent_Notify Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

/**
 * @brief  Add the HW Features service using a vendor specific profile
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Add_HW_SW_ServW2ST_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];

  uint8_t max_attr_records=2;  /* Acc/Gyro */

  /* Code for MotionAR integration - Start Section */
  max_attr_records++;
  /* Code for MotionAR integration - End Section */

  /* Code for MotionCP integration - Start Section */
  max_attr_records++;
  /* Code for MotionCP integration - End Section */

  /* Code for MotionGR integration - Start Section */
  max_attr_records++;
  /* Code for MotionGR integration - End Section */

  COPY_HW_SENS_W2ST_SERVICE_UUID(uuid);
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE,
                          1+3*max_attr_records,
                          &HWServW2STHandle);

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  COPY_ACC_GYRO_MAG_W2ST_CHAR_UUID(uuid);
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, 2+3*3*2,
                           CHAR_PROP_NOTIFY,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &AccGyroMagCharHandle);

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  COPY_ACC_EVENT_W2ST_CHAR_UUID(uuid);
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, 2+3, //2+2,
                           CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &AccEventCharHandle);

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  /* Code for MotionAR integration - Start Section */
  COPY_ACTIVITY_REC_W2ST_CHAR_UUID(uuid);
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, 2+1,
                           CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &ActivityRecCharHandle);

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }
  /* Code for MotionAR integration - End Section */

  /* Code for MotionCP integration - Start Section */
  COPY_CARRY_POSITION_REC_W2ST_CHAR_UUID(uuid);
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, 2+1,
                           CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &CarryPosRecCharHandle);

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }
  /* Code for MotionCP integration - End Section */

  /* Code for MotionGR integration - Start Section */
  COPY_GESTURE_REC_W2ST_CHAR_UUID(uuid);
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, 2+1,
                           CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &GestureRecCharHandle);

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }
  /* Code for MotionGR integration - End Section */

  return BLE_STATUS_SUCCESS;

fail:
  //ALLMEMS1_PRINTF("Error while adding HW's Characteristcs service.\n");
  return BLE_STATUS_ERROR;
}

/**
 * @brief  Update acceleration/Gryoscope and Magneto characteristics value
 * @param  BSP_MOTION_SENSOR_Axes_t Acc Structure containing acceleration value in mg
 * @param  BSP_MOTION_SENSOR_Axes_t Gyro Structure containing Gyroscope value
 * @param  BSP_MOTION_SENSOR_Axes_t Mag Structure containing magneto value
 * @retval tBleStatus      Status
 */
tBleStatus AccGyroMag_Update(BSP_MOTION_SENSOR_Axes_t *Acc,BSP_MOTION_SENSOR_Axes_t *Gyro,BSP_MOTION_SENSOR_Axes_t *Mag)
{
  tBleStatus ret;
  int32_t x;
  int32_t y;
  int32_t z;

  uint8_t buff[2+3*3*2];

  STORE_LE_16(buff   ,(HAL_GetTick()>>3));

  STORE_LE_16(buff+2 ,Acc->x);
  STORE_LE_16(buff+4 ,Acc->y);
  STORE_LE_16(buff+6 ,Acc->z);

  Gyro->x/=100;
  Gyro->y/=100;
  Gyro->z/=100;

  STORE_LE_16(buff+8 ,Gyro->x);
  STORE_LE_16(buff+10,Gyro->y);
  STORE_LE_16(buff+12,Gyro->z);

  /* Apply Magneto calibration */
  x = Mag->x;
  y = Mag->y;
  z = Mag->z;

  STORE_LE_16(buff+14,x);
  STORE_LE_16(buff+16,y);
  STORE_LE_16(buff+18,z);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(HWServW2STHandle, AccGyroMagCharHandle, 0, 2+3*3*2, buff);

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating Acc/Gyro/Mag Char\r\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Error Updating Acc/Gyro/Mag Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

/**
 * @brief  Puts the device in connectable mode.
 * @param  None
 * @retval None
 */
void setConnectable(void)
{
  //char local_name[8] = {AD_TYPE_COMPLETE_LOCAL_NAME,NAME_BLUEMS};
  char local_name[8] = {AD_TYPE_COMPLETE_LOCAL_NAME,NodeName[1],NodeName[2],NodeName[3],NodeName[4],NodeName[5],NodeName[6],NodeName[7]};
  uint8_t manuf_data[26] = {
    2,0x0A,0x00 /* 0 dBm */, // Transmission Power
    //8,0x09,NAME_BLUEMS, // Complete Name
    8,0x09,NodeName[1],NodeName[2],NodeName[3],NodeName[4],NodeName[5],NodeName[6],NodeName[7], // Complete Name
    13,0xFF,0x01/*SKD version */,
    0x80,
    0x00, /*  */
    0xD0, /* ACC (0x80) + Gyro (0x40) */
    0x00, /*  Hardware Events */
    0x00, /*  */
    0x00, /* BLE MAC start */
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, /* BLE MAC stop */
  };

  /* BLE MAC */
  manuf_data[20] = bdaddr[5];
  manuf_data[21] = bdaddr[4];
  manuf_data[22] = bdaddr[3];
  manuf_data[23] = bdaddr[2];
  manuf_data[24] = bdaddr[1];
  manuf_data[25] = bdaddr[0];

  /* Accelerometer Events */
  manuf_data[18] |=0x04;

  /* Code for MotionAR integration - Start Section */
  if(TargetBoardFeatures.MotionARIsInitalized) {
    manuf_data[19] |= 0x10;
  }
  /* Code for MotionAR integration - End Section */

  /* Code for MotionCP integration - Start Section */
  if(TargetBoardFeatures.MotionCPIsInitalized) {
    manuf_data[19] |= 0x08;
  }
  /* Code for MotionCP integration - End Section */

  /* Code for MotionGR integration - Start Section */
  if(TargetBoardFeatures.MotionGRIsInitalized) {
    manuf_data[19] |= 0x02;
  }
  /* Code for MotionGR integration - End Section */

  /* disable scan response */
  hci_le_set_scan_resp_data(0,NULL);
  aci_gap_set_discoverable(ADV_IND, 0, 0,
#ifndef MAC_ALLMEMS1
  #ifdef MAC_STM32UID_ALLMEMS1
                           STATIC_RANDOM_ADDR,
  #else /* MAC_STM32UID_ALLMEMS1 */
                           RANDOM_ADDR,
  #endif /* MAC_STM32UID_ALLMEMS1 */
#else /* MAC_ALLMEMS1 */
                           PUBLIC_ADDR,
#endif /* MAC_ALLMEMS1 */
                           NO_WHITE_LIST_USE,
                           sizeof(local_name), local_name, 0, NULL, 0, 0);

  /* Send Advertising data */
  aci_gap_update_adv_data(26, manuf_data);
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  uint8_t addr[6] Address of peer device
 * @param  uint16_t handle Connection handle
 * @retval None
 */
static void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle)
{
  connected = TRUE;
  connection_handle = handle;

#ifdef ALLMEMS1_DEBUG_CONNECTION
  ALLMEMS1_PRINTF("\r\n>>>>>>CONNECTED %x:%x:%x:%x:%x:%x\r\n\r\n",addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
#endif /* ALLMEMS1_DEBUG_CONNECTION */

  ConnectionBleStatus=0;

  DisableHWFeatures();

  ForceReMagnetoCalibration     =0;
  FirstConnectionConfig  =0;

  FirstCommandSent       =1;

}

/**
 * @brief  This function is called when the peer device get disconnected.
 * @param  None
 * @retval None
 */
static void GAP_DisconnectionComplete_CB(void)
{
  connected = FALSE;

#ifdef ALLMEMS1_DEBUG_CONNECTION
  ALLMEMS1_PRINTF("<<<<<<DISCONNECTED\r\n");
#endif /* ALLMEMS1_DEBUG_CONNECTION */

  /* Make the device connectable again. */
  set_connectable = TRUE;

  ConnectionBleStatus=0;

  DisableHWFeatures();

//  /* Enable Accelerometer WakeUp */
//  EnableHWWakeUp();
//
//  /* The MCU has to be woken up by the LSMDS3 which generates an interrupt on INT2 (connected to GPIOA pin 2) */
//  /* Enable MCU WakeUp on PA2 */
//  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_HIGH);

  ForceReMagnetoCalibration     =0;
  FirstConnectionConfig  =0;

  FirstCommandSent       =1;

  /************************/
  /* Stops all the Timers */
  /************************/

  /* Code for MotionCP & MotionGR integration - Start Section */
  /* Stop Timer For MotionCP and MotionGR */
  if(HAL_TIM_OC_Stop_IT(&TimCCHandle, TIM_CHANNEL_2) != HAL_OK){
    /* Stopping Error */
    Error_Handler();
  }
  /* Code for MotionCP & MotionGR integration - End Section */

  /* Code for MotionAR integration - Start Section */
  /* Stop Timer For MotionAR */
  if(HAL_TIM_OC_Stop_IT(&TimCCHandle, TIM_CHANNEL_3) != HAL_OK){
    /* Stopping Error */
    Error_Handler();
  }
  /* Code for MotionAR integration - End Section */

  /* Stop Timer For Acc/Gyro */
  if(HAL_TIM_OC_Stop_IT(&TimCCHandle, TIM_CHANNEL_4) != HAL_OK){
    /* Stopping Error */
    Error_Handler();
  }

}

/**
 * @brief  This function is called when there is a Bluetooth Read request
 * @param  uint16_t handle Handle of the attribute
 * @retval None
 */
void Read_Request_CB(uint16_t handle)
{
  if(handle == AccEventCharHandle +1) {
    /* Read Request for Acc Pedometer */
    uint16_t StepCount;
    if(W2ST_CHECK_HW_FEATURE(W2ST_HWF_PEDOMETER)) {
      StepCount = GetStepHWPedometer();
    } else {
      StepCount = 0;
    }
    AccEvent_Notify(StepCount, 2);
  } else if (handle == StdErrCharHandle + 1) {
    /* Send again the last packet for StdError */
    Stderr_Update_AfterRead();
  } else if (handle == TermCharHandle + 1) {
    /* Send again the last packet for Terminal */
    Term_Update_AfterRead();

  /* Code for MotionAR integration - Start Section */
  } else if(handle == ActivityRecCharHandle + 1){
     ActivityRec_Update(ActivityCode);
  /* Code for MotionAR integration - End Section */

  /* Code for MotionCP integration - Start Section */
  } else if(handle == CarryPosRecCharHandle + 1){
    CarryPosRec_Update(CarryPositionCode);
  /* Code for MotionCP integration - End Section */

  /* Code for MotionGR integration - Start Section */
  } else if(handle == GestureRecCharHandle + 1){
    GestureRec_Update(GestureRecognitionCode);
  /* Code for MotionGR integration - End Section */

  }

  //EXIT:
  if(connection_handle != 0)
    aci_gatt_allow_read(connection_handle);
}

/**
 * @brief  This function is called when there is a change on the gatt attribute
 * With this function it's possible to understand if one application
 * is subscribed or not to the one service
 * @param uint16_t att_handle Handle of the attribute
 * @param uint8_t *att_data attribute data
 * @param uint8_t data_length length of the data
 * @retval None
 */
void Attribute_Modified_CB(uint16_t attr_handle, uint8_t * att_data, uint8_t data_length)
{
    if(attr_handle == AccGyroMagCharHandle + 2) {
      AccGyroMag_AttributeModified_CB(att_data);
    } else if(attr_handle == AccEventCharHandle + 2) {
      AccelerometerEvents_AttributeModified_CB(att_data);
    }
    /* Code for MotionAR integration - Start Section */
    else if(attr_handle == ActivityRecCharHandle + 2){
      ActivityRecognition_AttributeModified_CB(att_data);
    }
    /* Code for MotionAR integration - End Section */
    /* Code for MotionCP integration - Start Section */
    else if(attr_handle == CarryPosRecCharHandle + 2){
       CarryPosition_AttributeModified_CB(att_data);
    }
    /* Code for MotionCP integration - End Section */
    /* Code for MotionGR integration - Start Section */
    else if(attr_handle == GestureRecCharHandle + 2){
       GestureRecognition_AttributeModified_CB(att_data);
    }
    /* Code for MotionGR integration - End Section */
    else if(attr_handle == ConfigCharHandle + 2){
      if (att_data[0] == 01) {
        FirstConnectionConfig=1;
      } else if (att_data[0] == 0){
        FirstConnectionConfig=0;
      }
  #ifdef ALLMEMS1_DEBUG_CONNECTION
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
        BytesToWrite =sprintf((char *)BufferToWrite,"--->Calib=%s\r\n\n", FirstConnectionConfig ? "ON" : "OFF");
       Term_Update(BufferToWrite,BytesToWrite);
      } else
        ALLMEMS1_PRINTF("--->Calib=%s\r\n\n", FirstConnectionConfig ? "ON" : "OFF");
  #endif /* ALLMEMS1_DEBUG_CONNECTION */
    } else if (attr_handle == ConfigCharHandle + 1) {
      /* Received one write command from Client on Configuration characteristc */
      ConfigCommandParsing(att_data, data_length);
    } else if(attr_handle == StdErrCharHandle + 2){
      if (att_data[0] == 01) {
        W2ST_ON_CONNECTION(W2ST_CONNECT_STD_ERR);
      } else if (att_data[0] == 0){
        W2ST_OFF_CONNECTION(W2ST_CONNECT_STD_ERR);
      }
    } else if(attr_handle == TermCharHandle + 2){
      if (att_data[0] == 01) {
        W2ST_ON_CONNECTION(W2ST_CONNECT_STD_TERM);
      } else if (att_data[0] == 0){
        W2ST_OFF_CONNECTION(W2ST_CONNECT_STD_TERM);
      }
    } else if (attr_handle == TermCharHandle + 1){
      uint32_t SendBackData =1; /* By default Answer with the same message received */

      {
        /* Received one write from Client on Terminal characteristc */
        SendBackData = DebugConsoleCommandParsing(att_data,data_length);
      }

      /* Send it back for testing */
      if(SendBackData) {
        Term_Update(att_data,data_length);
      }
    } else if (attr_handle==(0x0002+2)){
      /* Force one UUID rescan */
      Force_UUID_Rescan();
    } else {
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
        BytesToWrite =sprintf((char *)BufferToWrite, "Notification UNKNOWN handle\r\n");
        Stderr_Update(BufferToWrite,BytesToWrite);
      } else {
        ALLMEMS1_PRINTF("Notification UNKNOWN handle\r\n");
      }
    }
}

/************************************/
/* Hardware Characteristics Service */
/************************************/

/**
 * @brief  This function is called when there is a change on the gatt attribute for Acc,Gyro and Mag
 * With this function it's possible to understand if one application
 * is subscribed or not to the Acc,Gyro and Mag service
 * @param uint8_t *att_data attribute data
 * @retval None
 */
static void AccGyroMag_AttributeModified_CB(uint8_t *att_data)
{
  if (att_data[0] == 01) {
    W2ST_ON_CONNECTION(W2ST_CONNECT_ACC_GYRO_MAG);

    /* Start the TIM Base generation in interrupt mode */
    if(HAL_TIM_OC_Start_IT(&TimCCHandle, TIM_CHANNEL_4) != HAL_OK){
      /* Starting Error */
      Error_Handler();
    }

    /* Set the new Capture compare value */
    {
      uint32_t uhCapture = __HAL_TIM_GET_COUNTER(&TimCCHandle);
      /* Set the Capture Compare Register value */
      __HAL_TIM_SET_COMPARE(&TimCCHandle, TIM_CHANNEL_4, (uhCapture + uhCCR4_Val));
    }
  } else if (att_data[0] == 0) {
    W2ST_OFF_CONNECTION(W2ST_CONNECT_ACC_GYRO_MAG);

    /* Stop the TIM Base generation in interrupt mode */
    if(HAL_TIM_OC_Stop_IT(&TimCCHandle, TIM_CHANNEL_4) != HAL_OK){
      /* Stopping Error */
      Error_Handler();
    }
  }
#ifdef ALLMEMS1_DEBUG_CONNECTION
  if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
    BytesToWrite =sprintf((char *)BufferToWrite,"--->Acc/Gyro/Mag=%s", W2ST_CHECK_CONNECTION(W2ST_CONNECT_ACC_GYRO_MAG) ? " ON\r\n" : " OFF\r\n\n");
    Term_Update(BufferToWrite,BytesToWrite);
  } else
    ALLMEMS1_PRINTF("--->Acc/Gyro/Mag=%s", W2ST_CHECK_CONNECTION(W2ST_CONNECT_ACC_GYRO_MAG) ? " ON\r\n" : " OFF\r\n\n");
#endif /* ALLMEMS1_DEBUG_CONNECTION */
}

/**
 * @brief  This function is called when there is a change on the gatt attribute for Accelerometer Events
 * With this function it's possible to understand if one application
 * is subscribed or not to the Accelerometer Events service
 * @param uint8_t *att_data attribute data
 * @retval None
 */
static void AccelerometerEvents_AttributeModified_CB(uint8_t *att_data)
{
  if (att_data[0] == 01) {
    W2ST_ON_CONNECTION(W2ST_CONNECT_ACC_EVENT);
#if 0
    EnableHWMultipleEvents();
    ResetHWPedometer();
#endif
    Config_Notify(FEATURE_MASK_ACC_EVENTS,'m',1);
  } else if (att_data[0] == 0) {
    W2ST_OFF_CONNECTION(W2ST_CONNECT_ACC_EVENT);
    DisableHWMultipleEvents();
    }
#ifdef ALLMEMS1_DEBUG_CONNECTION
  if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
    BytesToWrite =sprintf((char *)BufferToWrite,"--->AccEvent=%s", W2ST_CHECK_CONNECTION(W2ST_CONNECT_ACC_EVENT) ? " ON\r\n" : " OFF\r\n\n");
    Term_Update(BufferToWrite,BytesToWrite);
  } else
    ALLMEMS1_PRINTF("--->AccEvent=%s", W2ST_CHECK_CONNECTION(W2ST_CONNECT_ACC_EVENT) ? " ON\r\n" : " OFF\r\n\n");
#endif /* ALLMEMS1_DEBUG_CONNECTION */
}

/***********************************
* Software Characteristics Service *
*        MotionXX Section          *
************************************/

/* Code for MotionAR integration - Start Section */
/**
 * @brief  This function is called when there is a change on the gatt attribute for Activity Recognition
 * With this function it's possible to understand if one application
 * is subscribed or not to the Activity Recognition service
 * @param uint8_t *att_data attribute data
 * @retval None
 */
static void ActivityRecognition_AttributeModified_CB(uint8_t *att_data)
{
  if (att_data[0] == 01) {
    Set4GAccelerometerFullScale();

    W2ST_ON_CONNECTION(W2ST_CONNECT_AR);

    /* Start the TIM Base generation in interrupt mode */
    if(HAL_TIM_OC_Start_IT(&TimCCHandle, TIM_CHANNEL_3) != HAL_OK){
      /* Starting Error */
      Error_Handler();
    }
    /* Set the new Capture compare value */
    {
      uint32_t uhCapture = __HAL_TIM_GET_COUNTER(&TimCCHandle);
      /* Set the Capture Compare Register value */
      __HAL_TIM_SET_COMPARE(&TimCCHandle, TIM_CHANNEL_3, (uhCapture + DEFAULT_uhCCR3_Val));
    }
  } else if (att_data[0] == 0){

    Set2GAccelerometerFullScale();

    W2ST_OFF_CONNECTION(W2ST_CONNECT_AR);

    /* Stop the TIM Base generation in interrupt mode */
    if(HAL_TIM_OC_Stop_IT(&TimCCHandle, TIM_CHANNEL_3) != HAL_OK){
      /* Stopping Error */
      Error_Handler();
    }
  }
#ifdef ALLMEMS1_DEBUG_CONNECTION
  if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
    BytesToWrite =sprintf((char *)BufferToWrite,"--->ActRec=%s",  W2ST_CHECK_CONNECTION(W2ST_CONNECT_AR) ? " ON\r\n" : " OFF\r\n\n");
    Term_Update(BufferToWrite,BytesToWrite);
  } else
    ALLMEMS1_PRINTF("--->ActRec=%s",  W2ST_CHECK_CONNECTION(W2ST_CONNECT_AR) ? " ON\r\n" : " OFF\r\n\n");
#endif /* ALLMEMS1_DEBUG_CONNECTION */
}
/* Code for MotionAR integration - End Section */

/* Code for MotionCP integration - Start Section */
/**
 * @brief  This function is called when there is a change on the gatt attribute for Carry Position
 * With this function it's possible to understand if one application
 * is subscribed or not to the Carry Position service
 * @param uint8_t *att_data attribute data
 * @retval None
 */
static void CarryPosition_AttributeModified_CB(uint8_t *att_data)
{
  if (att_data[0] == 01) {
    Set4GAccelerometerFullScale();

    W2ST_ON_CONNECTION(W2ST_CONNECT_CP);

    /* Start the TIM Base generation in interrupt mode */
    if(HAL_TIM_OC_Start_IT(&TimCCHandle, TIM_CHANNEL_2) != HAL_OK){
      /* Starting Error */
      Error_Handler();
    }
    /* Set the new Capture compare value */
    {
    uint32_t uhCapture = __HAL_TIM_GET_COUNTER(&TimCCHandle);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(&TimCCHandle, TIM_CHANNEL_2, (uhCapture + DEFAULT_uhCCR2_Val));
    }
  } else if (att_data[0] == 0){

    Set2GAccelerometerFullScale();

    W2ST_OFF_CONNECTION(W2ST_CONNECT_CP);

    /* Stop the TIM Base generation in interrupt mode */
    if(HAL_TIM_OC_Stop_IT(&TimCCHandle, TIM_CHANNEL_2) != HAL_OK){
      /* Stopping Error */
      Error_Handler();
    }
  }
#ifdef ALLMEMS1_DEBUG_CONNECTION
  if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
    BytesToWrite =sprintf((char *)BufferToWrite,"--->CarryPosRec=%s", W2ST_CHECK_CONNECTION(W2ST_CONNECT_CP) ? " ON\r\n" : " OFF\r\n\n");
    Term_Update(BufferToWrite,BytesToWrite);
  } else
    ALLMEMS1_PRINTF("--->CarryPosRec=%s", W2ST_CHECK_CONNECTION(W2ST_CONNECT_CP) ? " ON\r\n" : " OFF\r\n\n");
#endif /* ALLMEMS1_DEBUG_CONNECTION */
}
/* Code for MotionCP integration - End Section */

/* Code for MotionGR integration - Start Section */
/**
 * @brief  This function is called when there is a change on the gatt attribute for Gesture Recognition
 * With this function it's possible to understand if one application
 * is subscribed or not to the Gesture Recognition service
 * @param uint8_t *att_data attribute data
 * @retval None
 */
static void GestureRecognition_AttributeModified_CB(uint8_t *att_data)
{
  if (att_data[0] == 01) {

    Set4GAccelerometerFullScale();

    W2ST_ON_CONNECTION(W2ST_CONNECT_GR);

    /* Start the TIM Base generation in interrupt mode */
    if(HAL_TIM_OC_Start_IT(&TimCCHandle, TIM_CHANNEL_2) != HAL_OK){
      /* Starting Error */
      Error_Handler();
    }
    /* Set the new Capture compare value */
    {
    uint32_t uhCapture = __HAL_TIM_GET_COUNTER(&TimCCHandle);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(&TimCCHandle, TIM_CHANNEL_2, (uhCapture + DEFAULT_uhCCR2_Val));
    }
  } else if (att_data[0] == 0){

    Set2GAccelerometerFullScale();

    W2ST_OFF_CONNECTION(W2ST_CONNECT_GR);

    /* Stop the TIM Base generation in interrupt mode */
    if(HAL_TIM_OC_Stop_IT(&TimCCHandle, TIM_CHANNEL_2) != HAL_OK){
      /* Stopping Error */
      Error_Handler();
    }
  }
#ifdef ALLMEMS1_DEBUG_CONNECTION
  if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
    BytesToWrite =sprintf((char *)BufferToWrite,"--->GestureRec=%s", W2ST_CHECK_CONNECTION(W2ST_CONNECT_GR) ? " ON\r\n" : " OFF\r\n\n");
    Term_Update(BufferToWrite,BytesToWrite);
  } else
    ALLMEMS1_PRINTF("--->GestureRec=%s", W2ST_CHECK_CONNECTION(W2ST_CONNECT_GR) ? " ON\r\n" : " OFF\r\n\n");
#endif /* ALLMEMS1_DEBUG_CONNECTION */
}
/* Code for MotionGR integration - End Section */

/**
 * @brief  This function makes the parsing of the Debug Console Commands
 * @param uint8_t *att_data attribute data
 * @param uint8_t data_length length of the data
 * @retval uint32_t SendItBack true/false
 */
static uint32_t DebugConsoleCommandParsing(uint8_t * att_data, uint8_t data_length)
{
  uint32_t SendBackData = 1;

    /* Help Command */
    if(!strncmp("help",(char *)(att_data),4)) {
      /* Print Legend */
      SendBackData=0;

      BytesToWrite =sprintf((char *)BufferToWrite,"Command:\r\n"
         "info -> System Info\r\n"
         "powerstatus-> Battery Status [%% mV mA]\r\n"
         "versionFw  -> FW Version\r\n"
         "versionBle -> Ble Version\r\n"
           );
      Term_Update(BufferToWrite,BytesToWrite);

      BytesToWrite =sprintf((char *)BufferToWrite,
         "TLcalibstart  -> Start MotionTL Calibration\r\n"
           );
      Term_Update(BufferToWrite,BytesToWrite);

      BytesToWrite =sprintf((char *)BufferToWrite,
         "TLcalibstop  -> Stop MotionTL Calibration\r\n"
           );
      Term_Update(BufferToWrite,BytesToWrite);

      BytesToWrite =sprintf((char *)BufferToWrite,
         "setName xxxxxxx     -> Set the node name (Max 7 characters)\r\n"
           );
      Term_Update(BufferToWrite,BytesToWrite);

    } else if(!strncmp("versionFw",(char *)(att_data),9)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"%s_%s_%c.%c.%c\r\n",
#ifdef STM32F401xE
                            "F401"
#elif STM32F446xx
                            "F446"
#elif STM32L476xx
                            "L476"
#elif STM32L562xx
                            "L562"
#else
#error "Undefined STM32 processor type"
#endif
                            ,ALLMEMS1_PACKAGENAME,
                            ALLMEMS1_VERSION_MAJOR,
                            ALLMEMS1_VERSION_MINOR,
                            ALLMEMS1_VERSION_PATCH);
      if(FirstCommandSent)
      {
        FirstCommandSent= 0;
        Term_Update(BufferToWrite,BytesToWrite);
        SendBackData=0;
      }
      else
        SendBackData=1;
    } else if(!strncmp("info",(char *)(att_data),4)) {
      SendBackData=0;

      BytesToWrite =sprintf((char *)BufferToWrite,"\r\nSTMicroelectronics %s:\r\n"
         "\tVersion %c.%c.%c\r\n"
         "\tSTM32L562E-DK board"
         "\r\n",
         ALLMEMS1_PACKAGENAME,
         ALLMEMS1_VERSION_MAJOR,ALLMEMS1_VERSION_MINOR,ALLMEMS1_VERSION_PATCH);
      Term_Update(BufferToWrite,BytesToWrite);

      BytesToWrite =sprintf((char *)BufferToWrite,"\t(HAL %ld.%ld.%ld_%ld)\r\n"
        "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
        " (IAR)\r\n",
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
        " (KEIL)\r\n",
#elif defined (__GNUC__)
        " (STM32CubeIDE)\r\n",
#endif
          HAL_GetHalVersion() >>24,
          (HAL_GetHalVersion() >>16)&0xFF,
          (HAL_GetHalVersion() >> 8)&0xFF,
           HAL_GetHalVersion()      &0xFF,
           __DATE__,__TIME__);
      Term_Update(BufferToWrite,BytesToWrite);

    }  if(!strncmp("versionBle",(char *)(att_data),10)) {
      uint8_t  hwVersion;
      uint16_t fwVersion;
      /* get the BlueNRG HW and FW versions */
      getBlueNRGVersion(&hwVersion, &fwVersion);
      BytesToWrite =sprintf((char *)BufferToWrite,"%s_%d.%d.%c\r\n",
                            (hwVersion > 0x30) ? "BleMS" : "Ble",
                            fwVersion>>8,
                            (fwVersion>>4)&0xF,
                            (hwVersion > 0x30) ? ('a'+(fwVersion&0xF)-1) : 'a');
      Term_Update(BufferToWrite,BytesToWrite);
      SendBackData=0;
    } else if((att_data[0]=='u') & (att_data[1]=='i') & (att_data[2]=='d')) {
      /* Write back the STM32 UID */
      uint8_t *uid = (uint8_t *)STM32_UUID;
      uint32_t MCU_ID = STM32_MCU_ID[0]&0xFFF;
      BytesToWrite =sprintf((char *)BufferToWrite,"%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.3lX\r\n",
                            uid[ 3],uid[ 2],uid[ 1],uid[ 0],
                            uid[ 7],uid[ 6],uid[ 5],uid[ 4],
                            uid[11],uid[ 10],uid[9],uid[8],
                            MCU_ID);
      Term_Update(BufferToWrite,BytesToWrite);
      SendBackData=0;
    } else if(!strncmp("setName ",(char *)(att_data),8)) {

      //int NameLength= strcspn((const char *)att_data,"\n");
      int NameLength= data_length -1;

      if(NameLength > 8)
      {
        for(int i=1;i<8;i++)
          NodeName[i]= atoi(" ");

        if((NameLength - 8) > 7)
          NameLength= 7;
        else NameLength= NameLength - 8;

        for(int i=1;i<NameLength+1;i++)
          NodeName[i]= att_data[i+7];

        MDM_SaveGMD(GMD_NODE_NAME,(void *)&NodeName);
        NecessityToSaveMetaDataManager=1;

        BytesToWrite =sprintf((char *)BufferToWrite,"\nThe node nome has been updated\r\n");
        Term_Update(BufferToWrite,BytesToWrite);
        BytesToWrite =sprintf((char *)BufferToWrite,"Disconnecting and riconnecting to see the new node name\r\n");
        Term_Update(BufferToWrite,BytesToWrite);
      }
      else
      {
        BytesToWrite =sprintf((char *)BufferToWrite,"\nInsert the node name\r\n");
        Term_Update(BufferToWrite,BytesToWrite);
        BytesToWrite =sprintf((char *)BufferToWrite,"Use command: setName 'xxxxxxx'\r\n");
        Term_Update(BufferToWrite,BytesToWrite);
      }

      SendBackData=0;
    }

#if 1

  /* If it's something not yet recognized... only for testing.. This must be removed*/
   if(SendBackData) {
    if(att_data[0]=='@') {

      if(att_data[1]=='T') {
        uint8_t loc_att_data[6];
        uint8_t loc_data_length=6;

        loc_att_data[0] = (FEATURE_MASK_TEMP1>>24)&0xFF;
        loc_att_data[1] = (FEATURE_MASK_TEMP1>>16)&0xFF;
        loc_att_data[2] = (FEATURE_MASK_TEMP1>>8 )&0xFF;
        loc_att_data[3] = (FEATURE_MASK_TEMP1    )&0xFF;
        loc_att_data[4] = 255;

        switch(att_data[2]) {
          case 'L':
            loc_att_data[5] = 50; /* @5S */
          break;
          case 'M':
            loc_att_data[5] = 10; /* @1S */
          break;
          case 'H':
            loc_att_data[5] = 1; /* @100mS */
          break;
          case 'D':
            loc_att_data[5] = 0; /* Default */
          break;
        }
        SendBackData = ConfigCommandParsing(loc_att_data,loc_data_length);
      } else if(att_data[1]=='A') {
        uint8_t loc_att_data[6];
        uint8_t loc_data_length=6;

        loc_att_data[0] = (FEATURE_MASK_ACC>>24)&0xFF;
        loc_att_data[1] = (FEATURE_MASK_ACC>>16)&0xFF;
        loc_att_data[2] = (FEATURE_MASK_ACC>>8 )&0xFF;
        loc_att_data[3] = (FEATURE_MASK_ACC    )&0xFF;
        loc_att_data[4] = 255;

        switch(att_data[2]) {
          case 'L':
            loc_att_data[5] = 50; /* @5S */
          break;
          case 'M':
            loc_att_data[5] = 10; /* @1S */
          break;
          case 'H':
            loc_att_data[5] = 1; /* @100mS */
          break;
          case 'D':
            loc_att_data[5] = 0; /* Default */
          break;
        }
        SendBackData = ConfigCommandParsing(loc_att_data,loc_data_length);
      } else if(att_data[1]=='M') {
        uint8_t loc_att_data[6];
        uint8_t loc_data_length=6;

        loc_att_data[0] = (FEATURE_MASK_MIC>>24)&0xFF;
        loc_att_data[1] = (FEATURE_MASK_MIC>>16)&0xFF;
        loc_att_data[2] = (FEATURE_MASK_MIC>>8 )&0xFF;
        loc_att_data[3] = (FEATURE_MASK_MIC    )&0xFF;
        loc_att_data[4] = 255;

        switch(att_data[2]) {
          case 'L':
            loc_att_data[5] = 50; /* @5S */
          break;
          case 'M':
            loc_att_data[5] = 10; /* @1S */
          break;
          case 'H':
            loc_att_data[5] = 1; /* @100mS */
          break;
          case 'D':
            loc_att_data[5] = 0; /* Default */
          break;
        }
        SendBackData = ConfigCommandParsing(loc_att_data,loc_data_length);
      }
    }
  }
#endif

  return SendBackData;
}

/**
 * @brief  This function makes the parsing of the Configuration Commands
 * @param uint8_t *att_data attribute data
 * @param uint8_t data_length length of the data
 * @retval uint32_t SendItBack true/false
 */
static uint32_t ConfigCommandParsing(uint8_t * att_data, uint8_t data_length)
{
  FeatureMask = (att_data[3]) | (att_data[2]<<8) | (att_data[1]<<16) | (att_data[0]<<24);
  uint8_t Command = att_data[4];
  uint8_t Data    = att_data[5];
  uint32_t SendItBack = 1;

  switch (FeatureMask) {
  case FEATURE_MASK_ACC_EVENTS:
    /* Acc events */
#ifdef ALLMEMS1_DEBUG_CONNECTION
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"Conf Sig F=%lx C=%c D=%x\r\n",FeatureMask,Command,Data);
      Term_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Conf Sig F=%lx C=%c D=%x\r\n",FeatureMask,Command,Data);
    }
#endif /* ALLMEMS1_DEBUG_CONNECTION */
    switch(Command) {
      case 'm':
        /* Multiple Events */
        switch(Data) {
          case 1:
            EnableHWMultipleEvents();
            ResetHWPedometer();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
          case 0:
            DisableHWMultipleEvents();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
         }
        break;
      case 'f':
        /* FreeFall */
        switch(Data) {
          case 1:
            EnableHWFreeFall();
             Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
          case 0:
            DisableHWFreeFall();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
         }
      break;
      case 'd':
        /* Double Tap */
        switch(Data) {
          case 1:
            EnableHWDoubleTap();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
          case 0:
            DisableHWDoubleTap();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
        }
      break;
      case 's':
        /* Single Tap */
        switch(Data) {
          case 1:
            EnableHWSingleTap();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
          case 0:
            DisableHWSingleTap();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
        }
      break;
      case 'p':
        /* Pedometer */
        switch(Data) {
          case 1:
            EnableHWPedometer();
            ResetHWPedometer();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
          case 0:
            DisableHWPedometer();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
        }
       break;
      case 'w':
        /* Wake UP */
        switch(Data) {
          case 1:
            EnableHWWakeUp();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
          case 0:
            DisableHWWakeUp();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
        }
       break;
       case 't':
         /* Tilt */
        switch(Data) {
          case 1:
            EnableHWTilt();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
          case 0:
            DisableHWTilt();
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            break;
        }
      break;
      case 'o' :
        /* Tilt */
        switch(Data) {
        case 1:
          EnableHWOrientation6D();
          Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
          break;
        case 0:
          DisableHWOrientation6D();
          Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
          break;
        }
      break;
    }
    break;
    /* Inertial features */
    case FEATURE_MASK_ACC:
    case FEATURE_MASK_GRYO:
    case FEATURE_MASK_MAG:
      switch(Command) {
        case 255:
          /* Change the Sending interval */
          if(Data!=0) {
            /* Multiple of 100mS */
            uhCCR4_Val  = 1000*Data;
          } else {
            /* Default Value */
            uhCCR4_Val  = DEFAULT_uhCCR4_Val;
          }
#ifdef ALLMEMS1_DEBUG_CONNECTION
          if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
            BytesToWrite = sprintf((char *)BufferToWrite,"Conf Sig F=%lx C=%2x Data=%2x\n\r",FeatureMask,Command,Data);
            Term_Update(BufferToWrite,BytesToWrite);
          } else {
            ALLMEMS1_PRINTF("Conf Sig F=%lx C=%2x Data=%2x\n\r",FeatureMask,Command,Data);
          }
#endif /* ALLMEMS1_DEBUG_CONNECTION */
          SendItBack = 0;
        break;
      }
    break;
  }
  return SendItBack;
}

/**
 * @brief  Force one UUID rescan
 * @param  None
 * @retval None
 */
static void Force_UUID_Rescan(void)
{
  tBleStatus ret;

  uint8_t buff[4];

  /* Delete all the Handles from 0x0001 to 0xFFFF */
  STORE_LE_16(buff  ,0x0001);
  STORE_LE_16(buff+2,0xFFFF);

  ret = aci_gatt_update_char_value(0x0001,0x0002,0,4,buff);

  if (ret == BLE_STATUS_SUCCESS){
    ALLMEMS1_PRINTF("UUID Rescan Forced\r\n\r\n");
  } else {
    ALLMEMS1_PRINTF("Problem forcing UUID Rescan\r\n\r\n");
  }
}

/**
 * @brief  This function is called whenever there is an ACI event to be processed.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  void *pckt Pointer to the ACI packet
 * @retval None
 */
void HCI_Event_CB(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

  if(hci_pckt->type != HCI_EVENT_PKT) {
    return;
  }

  switch(event_pckt->evt){

  case EVT_DISCONN_COMPLETE:
    {
      GAP_DisconnectionComplete_CB();
    }
    break;
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;

      switch(evt->subevent){
      case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
          GAP_ConnectionComplete_CB(cc->peer_bdaddr, cc->handle);
        }
        break;
      }
    }
    break;
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      switch(blue_evt->ecode){
      case EVT_BLUE_GATT_READ_PERMIT_REQ:
        {
          evt_gatt_read_permit_req *pr = (void*)blue_evt->data;
          Read_Request_CB(pr->attr_handle);
        }
        break;
      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
        {
          evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
          Attribute_Modified_CB(evt->attr_handle, evt->att_data,evt->data_length);
        }
        break;
      }
    }
    break;
  }
}

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
