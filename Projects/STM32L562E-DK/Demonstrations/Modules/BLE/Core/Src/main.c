/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 STMicroelectronics</center></h2>
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

/**
 * @mainpage FP-SNS-ALLMEMS1 Bluetooth Low Energy and Sensors Software adapted to STM32L562E-DK
 *
 * @image html st_logo.png
 *
 * <b>Introduction</b>
 *
 * This firmware package includes Components Device Drivers, Board Support Package
 * and example application for the following STMicroelectronics elements:
 * - STM32L562E-DK board that contains the following components:
 *     - MEMS sensor devices: LSM6DSO
 * - MotionAR software provides real-time activity recognition data using MEMS accelerometer sensor
 * - MotionCP software provides carry Position recognition data using MEMS accelerometer sensor
 * - MotionGR software provides carry Gesture recognition data using MEMS accelerometer sensor
 *
 * <b>Example Application</b>
 *
 * The Example application initializes all the Components and Library creating 3 Custom Bluetooth services:
 * - The first service exposes all the HW and SW characteristics:
 *  - HW characteristics:
 *     - related to MEMS sensor devices: Gyroscope and Accelerometer
 *  - SW characteristics:
 *     - the activity recognized using the MotionAR algorithm
 *     - the carry position recognized using the MotionCP algorithm
 *     - the Gesture recognized using the MotionGR algorithm
 * - The second Service exposes the console services where we have stdin/stdout and stderr capabilities
 * - The last Service is used for configuration purpose
 *
 * For the STEVAL-STLKT01V1, when the Android/iOS device is not connected for more than 20 seconds, the board go on shutdown mode.
 * The shutdown mode can be enabled or disabled by means of the macro ENABLE_SHUT_DOWN_MODE
 * The accelerometer event can be selected and used to wake-up the board to connect it to Android/iOS again and it can be chosen by
 * the constant WakeupSource in the file main.c (The Double Tap event is set as default).
 * Through the define RANGE_TIME_WITHOUT_CONNECTED in main.h file it is possible modified this time value.
 *
 * The example application allows the user to control the initialization phase via UART.
 * Launch a terminal application and set the UART port to 115200 bps, 8 bit, No Parity, 1 stop bit.
 * <br>For having the same UART functionality on SensorTile board, is necessary to recompile the code uncomment the line 72
 * <br>  //#define ALLMEMS1_ENABLE_PRINTF
 * <br>on file:
 * <br>  Config\ALLMEMS1_config.h
 *
 * This example must be used with the related ST BLE Sensor Android/iOS application available on Play/itune store (Version 4.5.0 or higher),
 * in order to read the sent information by Bluetooth Low Energy protocol
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>

#include <limits.h>
#include "TargetFeatures.h"
#include "main.h"
#include "MetaDataManager.h"
#include "sensor_service.h"
#include "bluenrg_utils.h"
#include "HWAdvanceFeatures.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

#define CHECK_CALIBRATION ((uint32_t)0x12345678)

/* Shutdown mode enabled as default */
// PLF #define ENABLE_SHUT_DOWN_MODE 1
#define ENABLE_SHUT_DOWN_MODE 0

/* Imported Variables -------------------------------------------------------------*/
extern uint8_t set_connectable;
extern int connected;

/* Code for MotionAR integration - Start Section */
extern MAR_output_t ActivityCode;
/* Code for MotionAR integration - End Section */

/* Code for MotionCP integration - Start Section */
extern MCP_output_t CarryPositionCode;
/* Code for MotionCP integration - End Section */

/* Code for MotionGR integration - Start Section */
extern MGR_output_t GestureRecognitionCode;
/* Code for MotionGR integration - End Section */

/* Exported Variables -------------------------------------------------------------*/

float sensitivity;

/* Acc sensitivity multiply by FROM_MG_TO_G constant */
float sensitivity_Mul;

uint32_t ConnectionBleStatus  =0;

uint32_t ForceReMagnetoCalibration    =0;
uint32_t FirstConnectionConfig =0;

uint8_t BufferToWrite[256];
int32_t BytesToWrite;

TIM_HandleTypeDef    TimCCHandle;

uint8_t bdaddr[6];

uint32_t uhCCR4_Val = DEFAULT_uhCCR4_Val;

uint32_t MagCalibrationData[30];
uint32_t AccCalibrationData[7];
uint8_t  NodeName[8];

/* Table with All the known Meta Data */
MDM_knownGMD_t known_MetaData[]={
  {GMD_MAG_CALIBRATION,(sizeof(MagCalibrationData))},
  {GMD_ACC_CALIBRATION,(sizeof(AccCalibrationData))},
  {GMD_NODE_NAME,      (sizeof(NodeName))},
  {GMD_END    ,0}/* THIS MUST BE THE LAST ONE */
};

uint16_t PedometerStepCount= 0;

/* Code for MotionPE integration - Start Section */
uint8_t FirstPoseEstimationCode;
/* Code for MotionPE integration - End Section */

/* Private variables ---------------------------------------------------------*/
static volatile int TSInterrupt       = 0;
static volatile int MEMSInterrupt     = 0;
static volatile uint32_t SendEnv      = 0;
static volatile uint32_t SendAccGyro  = 0;

static volatile uint32_t TimeStamp = 0;

volatile uint32_t HCI_ProcessEvent = 0;

/* Accelerometer event wakeup mode selection */
const uint8_t WakeupSource = ACC_DOUBLE_TAP;

static uint32_t ActivityTimeout_StartTime = 0;
static uint32_t ActivityTimeout_CurrTime  = 0;

/* CRC handler declaration */
static CRC_HandleTypeDef hcrc;

/* Code for MotionAR integration - Start Section */
static volatile uint32_t UpdateMotionAR  =0;
/* Code for MotionAR integration - End Section */

/* Code for MotionCP integration - Start Section */
static volatile uint32_t UpdateMotionCP  =0;
/* Code for MotionCP integration - End Section */

/* Code for MotionGR integration - Start Section */
static volatile uint32_t UpdateMotionGR  =0;
/* Code for MotionGR integration - End Section */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

static void TSCallback(void);

static void Init_BlueNRG_Custom_Services(void);
static void Init_BlueNRG_Stack(void);

static unsigned char ReCallNodeNameFromMemory(void);

static void DeinitTimers(void);

static void MX_CRC_Init(void);

static void InitTimers(void);
static void MEMSCallback(void);
static void SendMotionData(void);

static void MCU_PowerSave(void);

/* Code for MotionAR integration - Start Section */
static void ComputeMotionAR(void);
/* Code for MotionAR integration - End Section */

/* Code for MotionCP integration - Start Section */
static void ComputeMotionCP(void);
/* Code for MotionCP integration - End Section */

/* Code for MotionGR integration - Start Section */
static void ComputeMotionGR(void);
/* Code for MotionGR integration - End Section */

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  HAL_Init();

  /* Enable instruction cache (default 2-ways set associative cache) */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure the System clock */
  SystemClock_Config();

  InitTargetPlatform();

  /* For enabling CRC clock for using motion libraries (for checking if STM32 microprocessor is used)*/
  MX_CRC_Init();

  /* Check the MetaDataManager */
 InitMetaDataManager((void *)&known_MetaData,MDM_DATA_TYPE_GMD,NULL);

  ALLMEMS1_PRINTF("\n\t(HAL %ld.%ld.%ld_%ld)\r\n"
        "\tCompiled %s %s"

#if defined (__IAR_SYSTEMS_ICC__)
        " (IAR)\r\n"
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
        " (KEIL)\r\n"
#elif defined (__GNUC__)
        " (STM32CubeIDE)\r\n"
#endif
         "\tSend Every %4dmS Acc/Gyro\r\n",
         HAL_GetHalVersion() >>24,
          (HAL_GetHalVersion() >>16)&0xFF,
          (HAL_GetHalVersion() >> 8)&0xFF,
           HAL_GetHalVersion()      &0xFF,
         __DATE__,__TIME__,
         ALGO_PERIOD_ACC_GYRO);

#ifdef ALLMEMS1_DEBUG_CONNECTION
  ALLMEMS1_PRINTF("Debug Connection         Enabled\r\n");
#endif /* ALLMEMS1_DEBUG_CONNECTION */

#ifdef ALLMEMS1_DEBUG_NOTIFY_TRAMISSION
  ALLMEMS1_PRINTF("Debug Notify Transmission Enabled\r\n\n");
#endif /* ALLMEMS1_DEBUG_NOTIFY_TRAMISSION */

  /* Set Node Name */
  ReCallNodeNameFromMemory();

  /* Initialize the BlueNRG */
  Init_BlueNRG_Stack();

  /* Initialize the BlueNRG Custom services */
  Init_BlueNRG_Custom_Services();

  /* Set Accelerometer Full Scale to 2G */
  Set2GAccelerometerFullScale();

  /* Read the Acc Sensitivity */
  BSP_MOTION_SENSOR_GetSensitivity(ACCELERO_INSTANCE,MOTION_ACCELERO,&sensitivity);
  sensitivity_Mul = sensitivity * ((float) FROM_MG_TO_G);

  /* initialize timers */
  InitTimers();

  ActivityTimeout_StartTime = HAL_GetTick();

  /* Infinite loop */
  while (1)
  {

      /* Led Blinking when there is not a client connected */
      if(!connected)
      {
        if(!TargetBoardFeatures.LedStatus) {
          if(!(HAL_GetTick()&0x3FF)) {
            LedOnTargetPlatform();
            HAL_Delay(1);
          }
        } else {
          if(!(HAL_GetTick()&0x3FF)) {
            LedOffTargetPlatform();
            HAL_Delay(1);
          }
        }
      }
    if((!connected) && (ENABLE_SHUT_DOWN_MODE))
    {
      ActivityTimeout_CurrTime = HAL_GetTick();
      if( ActivityTimeout_CurrTime - ActivityTimeout_StartTime > RANGE_TIME_WITHOUT_CONNECTED )
      {
        MCU_PowerSave();
      }
    }
    else
    {
      ActivityTimeout_StartTime = HAL_GetTick();
    }

    if(set_connectable){
      /* Code for MotionAR integration - Start Section */
      /* Initialize MotionAR Library */
      if(TargetBoardFeatures.MotionARIsInitalized==0)
        MotionAR_manager_init();
      /* Code for MotionAR integration - End Section */

      /* Code for MotionCP integration - Start Section */
      /* Initialize MotionCP Library */
      if(TargetBoardFeatures.MotionCPIsInitalized==0)
        MotionCP_manager_init();
      /* Code for MotionCP integration - End Section */

      /* Code for MotionGR integration - Start Section */
      /* Initialize MotionGR Library */
      if(TargetBoardFeatures.MotionGRIsInitalized==0)
        MotionGR_manager_init();
      /* Code for MotionGR integration - End Section */

      if(NecessityToSaveMetaDataManager) {
        uint32_t Success = EraseMetaDataManager();
        if(Success) {
          SaveMetaDataManager();
        }
      }

      /* Now update the BLE advertize data and make the Board connectable */
      setConnectable();
      set_connectable = FALSE;
    }

    /* Handle Interrupt from MEMS */
    if(MEMSInterrupt) {
      MEMSCallback();
      MEMSInterrupt=0;

      ActivityTimeout_StartTime = HAL_GetTick();
    }

    /* handle BLE event */
    if(HCI_ProcessEvent) {
      HCI_ProcessEvent=0;
      hci_user_evt_proc();
    }

    /* Handle Interrupt from TouchScreen */
    if(TSInterrupt) {
      TSCallback();
      TSInterrupt=0;
    }

    /* Motion Data */
    if(SendAccGyro) {
      SendAccGyro=0;
      SendMotionData();
    }

    /* Code for MotionAR integration - Start Section */
    if(UpdateMotionAR) {
      UpdateMotionAR=0;
      ComputeMotionAR();
    }
    /* Code for MotionAR integration - End Section */

    /* Code for MotionCP integration - Start Section */
    if(UpdateMotionCP) {
      UpdateMotionCP=0;
      ComputeMotionCP();
    }
    /* Code for MotionCP integration - End Section */

    /* Code for MotionGR integration - Start Section */
    if(UpdateMotionGR) {
      UpdateMotionGR=0;
      ComputeMotionGR();
    }
    /* Code for MotionGR integration - End Section */

    /* Wait for Event */
    __WFI();
  }
}

/**
  * @brief  This function sets the ACC FS to 2g
  * @param  None
  * @retval None
  */
void Set2GAccelerometerFullScale(void)
{
  /* Set Full Scale to +/-2g */
  BSP_MOTION_SENSOR_SetFullScale(ACCELERO_INSTANCE,MOTION_ACCELERO,2.0f);

  /* Read the Acc Sensitivity */
  BSP_MOTION_SENSOR_GetSensitivity(ACCELERO_INSTANCE,MOTION_ACCELERO,&sensitivity);
  sensitivity_Mul = sensitivity * ((float) FROM_MG_TO_G);
}

/**
  * @brief  This function dsets the ACC FS to 4g
  * @param  None
  * @retval None
  */
void Set4GAccelerometerFullScale(void)
{
  /* Set Full Scale to +/-4g */
  BSP_MOTION_SENSOR_SetFullScale(ACCELERO_INSTANCE,MOTION_ACCELERO,4.0f);

  /* Read the Acc Sensitivity */
  BSP_MOTION_SENSOR_GetSensitivity(ACCELERO_INSTANCE,MOTION_ACCELERO,&sensitivity);
  sensitivity_Mul = sensitivity * ((float) FROM_MG_TO_G);
}

/**
  * @brief  Output Compare callback in non blocking mode
  * @param  htim : TIM OC handle
  * @retval None
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  uint32_t uhCapture=0;

  /* Code for MotionCP & MotionGR integration - Start Section */
  /* TIM1_CH2 toggling with frequency = 50Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    uhCapture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(&TimCCHandle, TIM_CHANNEL_2, (uhCapture + DEFAULT_uhCCR2_Val));

    /* Code for MotionCP integration - Start Section */
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CP)) {
      UpdateMotionCP=1;
    }
    /* Code for MotionCP integration - End Section */

    /* Code for MotionGR integration - Start Section */
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_GR)) {
      UpdateMotionGR=1;
    }
    /* Code for MotionGR integration - End Section */
  }
  /* Code for MotionCP & MotionGR integration - End Section */

  /* Code for MotionAR integration - Start Section */
  /* TIM1_CH3 toggling with frequency = 16Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
  {
    uhCapture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(&TimCCHandle, TIM_CHANNEL_3, (uhCapture + DEFAULT_uhCCR3_Val));


    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_AR))
    {
      UpdateMotionAR=1;
      TimeStamp += ALGO_PERIOD_AR_ID_PE;
    }

  }
  /* Code for MotionAR integration - End Section */

  /* TIM1_CH4 toggling with frequency = 20 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
  {
     uhCapture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(&TimCCHandle, TIM_CHANNEL_4, (uhCapture + uhCCR4_Val));
    SendAccGyro=1;
  }
}

/**
  * @brief  Interrupt from TouchScreen
  * @param  None
  * @retval None
  */
static void TSCallback(void)
{
  TS_State_t TsData;

  if (BSP_TS_GetState(0, &TsData) == BSP_ERROR_NONE)
  {
    /* Detect module exit */
    if ((TsData.TouchDetected == 1) &&
        (TsData.TouchX >= 200) && (TsData.TouchY >= 200))
    {
      HAL_NVIC_SystemReset();
    }
  }
}

/**
  * @brief  Send Notification where there is a interrupt from MEMS
  * @param  None
  * @retval None
  */
static void MEMSCallback(void)
{
  BSP_MOTION_SENSOR_Event_Status_t status;

  BSP_MOTION_SENSOR_Get_Event_Status(ACCELERO_INSTANCE,&status);

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_PEDOMETER)) ||
	  (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) )
  {
    /* Check if the interrupt is due to Pedometer */
    if(status.StepStatus != 0) {
      PedometerStepCount = GetStepHWPedometer();
       if(W2ST_CHECK_HW_FEATURE(W2ST_HWF_PEDOMETER))
         AccEvent_Notify(PedometerStepCount, 2);
    }
  }

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_FREE_FALL)) ||
      (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) )
  {
    /* Check if the interrupt is due to Free Fall */
    if(status.FreeFallStatus != 0) {
      AccEvent_Notify(ACC_FREE_FALL, 2);
    }
  }

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_SINGLE_TAP)) ||
      (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) )
  {
    /* Check if the interrupt is due to Single Tap */
    if(status.TapStatus != 0) {
      AccEvent_Notify(ACC_SINGLE_TAP, 2);
    }
  }

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_DOUBLE_TAP)) ||
      (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) )
  {
    /* Check if the interrupt is due to Double Tap */
    if(status.DoubleTapStatus != 0) {
      AccEvent_Notify(ACC_DOUBLE_TAP, 2);
    }
  }

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_TILT)) ||
      (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) )
  {
    /* Check if the interrupt is due to Tilt */
    if(status.TiltStatus != 0) {
      AccEvent_Notify(ACC_TILT, 2);
    }
  }

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_6DORIENTATION)) ||
      (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) )
  {
    /* Check if the interrupt is due to 6D Orientation */
    if(status.D6DOrientationStatus != 0) {
      AccEventType Orientation = GetHWOrientation6D();
      AccEvent_Notify(Orientation, 2);
    }
  }

  if(W2ST_CHECK_HW_FEATURE(W2ST_HWF_WAKE_UP)) {
    /* Check if the interrupt is due to Wake Up */
    if(status.WakeUpStatus != 0) {
      AccEvent_Notify(ACC_WAKE_UP, 2);
    }
  }

  if(W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS))
  {
    AccEvent_Notify(PedometerStepCount, 3);
  }
}

/**
  * @brief  Send Motion Data Acc/Mag/Gyro to BLE
  * @param  None
  * @retval None
  */
static void SendMotionData(void)
{
  BSP_MOTION_SENSOR_Axes_t ACC_Value;
  BSP_MOTION_SENSOR_Axes_t GYR_Value;
  BSP_MOTION_SENSOR_Axes_t MAG_Value;

  /* Read the Acc values */
  BSP_MOTION_SENSOR_GetAxes(ACCELERO_INSTANCE,MOTION_ACCELERO,&ACC_Value);

  /* Read the Gyro values */
  BSP_MOTION_SENSOR_GetAxes(GYRO_INSTANCE,MOTION_GYRO, &GYR_Value);

  AccGyroMag_Update(&ACC_Value,&GYR_Value,&MAG_Value);
}

/* Code for MotionAR integration - Start Section */
/**
  * @brief  MotionAR Working function
  * @param  None
  * @retval None
  */
static void ComputeMotionAR(void)
{
  static MAR_output_t ActivityCodeStored = MAR_NOACTIVITY;
  BSP_MOTION_SENSOR_AxesRaw_t ACC_Value_Raw;

  /* Read the Acc RAW values */
  BSP_MOTION_SENSOR_GetAxesRaw(ACCELERO_INSTANCE,MOTION_ACCELERO,&ACC_Value_Raw);

  MotionAR_manager_run(ACC_Value_Raw, TimeStamp);

  if(ActivityCodeStored!=ActivityCode){
    ActivityCodeStored = ActivityCode;

    ActivityRec_Update(ActivityCode);

    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
       BytesToWrite = sprintf((char *)BufferToWrite,"Sending: AR=%d\r\n",ActivityCode);
       Term_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Sending: AR=%d\r\n",ActivityCode);
    }
  }
}
/* Code for MotionAR integration - End Section */

/* Code for MotionCP integration - Start Section */
/**
  * @brief  MotionCP Working function
  * @param  None
  * @retval None
  */
static void ComputeMotionCP(void)
{
  static MCP_output_t CarryPositionCodeStored = MCP_UNKNOWN;
  BSP_MOTION_SENSOR_AxesRaw_t ACC_Value_Raw;

  /* Read the Acc RAW values */
  BSP_MOTION_SENSOR_GetAxesRaw(ACCELERO_INSTANCE,MOTION_ACCELERO,&ACC_Value_Raw);
  MotionCP_manager_run(ACC_Value_Raw);

  if(CarryPositionCodeStored!=CarryPositionCode){
    CarryPositionCodeStored = CarryPositionCode;
    CarryPosRec_Update(CarryPositionCode);

    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
       BytesToWrite = sprintf((char *)BufferToWrite,"Sending: CP=%d\r\n",CarryPositionCode);
       Term_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Sending: CP=%d\r\n",CarryPositionCode);
    }
  }
}
/* Code for MotionCP integration - End Section */

/* Code for MotionGR integration - Start Section */
/**
  * @brief  MotionGR Working function
  * @param  None
  * @retval None
  */
static void ComputeMotionGR(void)
{
  static MGR_output_t GestureRecognitionCodeStored = MGR_NOGESTURE;
  BSP_MOTION_SENSOR_AxesRaw_t ACC_Value_Raw;

  /* Read the Acc RAW values */
  BSP_MOTION_SENSOR_GetAxesRaw(ACCELERO_INSTANCE,MOTION_ACCELERO,&ACC_Value_Raw);
  MotionGR_manager_run(ACC_Value_Raw);

  if(GestureRecognitionCodeStored!=GestureRecognitionCode){
    GestureRecognitionCodeStored = GestureRecognitionCode;
    GestureRec_Update(GestureRecognitionCode);

    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
       BytesToWrite = sprintf((char *)BufferToWrite,"Sending: GR=%d\r\n",GestureRecognitionCode);
       Term_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Sending: GR=%d\r\n",GestureRecognitionCode);
    }
  }
}
/* Code for MotionGR integration - End Section */

/**
 * @brief  Collect accelerometer data
 * @param  cal_data Pointer to 2D array of calibration data cal_data[num_records][3]
 * @param  num_records Number of records to be taken (3 axes per record)
 * @retval 0  Ok
 * @retval 1  Accelerometer error
 */
uint8_t CollectData(float cal_data[][3], uint32_t num_records)
{
  uint32_t i = 0;
  uint8_t status = 0;

  BSP_MOTION_SENSOR_Axes_t AccValue;

  /* Clean DRDY */
  (void)BSP_MOTION_SENSOR_GetAxes(ACCELERO_INSTANCE, MOTION_ACCELERO, &AccValue);

  while (i < num_records)
  {
    if (BSP_MOTION_SENSOR_Get_DRDY_Status(ACCELERO_INSTANCE, MOTION_ACCELERO, &status) != BSP_ERROR_NONE)
    {
      return 1;
    }

    if (status == 1)
    {
      if (BSP_MOTION_SENSOR_GetAxes(ACCELERO_INSTANCE, MOTION_ACCELERO, &AccValue) != BSP_ERROR_NONE)
      {
        return 1;
      }

      cal_data[i][0] = (float)AccValue.x / 1000.0f;
      cal_data[i][1] = (float)AccValue.y / 1000.0f;
      cal_data[i][2] = (float)AccValue.z / 1000.0f;
      i++;
    }
  }

  return 0;
}

/**
 * @brief  Get estimated measurement time
 * @param  time_s Pointer to time in [s]
 * @param  num_records Number of records taken
 * @retval None
 */
void GetEstimatedMeasTime(float *time_s, uint32_t num_records)
{
  float odr = 0.0f;

  (void)BSP_MOTION_SENSOR_Enable(ACCELERO_INSTANCE, MOTION_ACCELERO);


  (void)BSP_MOTION_SENSOR_GetOutputDataRate(ACCELERO_INSTANCE, MOTION_ACCELERO, &odr);

  if (odr > 0.001f)
  {
    *time_s = (float)num_records / odr;
  }
}
/* Code for MotionTL integration - End Section */

/**
  * @brief  CRC init function.
  * @param  None
  * @retval None
  */
static void MX_CRC_Init(void)
{
  hcrc.Instance = CRC;

  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
}
/**
* @brief  Function for initializing timers for sending the information to BLE:
 *  - 1 for sending MotionAR/CP and Acc/Gyro
 * @param  None
 * @retval None
 */
static void InitTimers(void)
{

  uint32_t uwPrescalerValue;

  /* Timer Output Compare Configuration Structure declaration */
  TIM_OC_InitTypeDef sConfig;

  /* Compute the prescaler value to have TIM1 counter clock equal to 10 KHz */
  uwPrescalerValue = (uint32_t) ((SystemCoreClock / 10000) - 1);

  /* Set TIM1 instance ( Motion ) */
  TimCCHandle.Instance = TIM1;
  TimCCHandle.Init.Period        = 65535;
  TimCCHandle.Init.Prescaler     = uwPrescalerValue;
  TimCCHandle.Init.ClockDivision = 0;
  TimCCHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;
  if(HAL_TIM_OC_Init(&TimCCHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

 /* Configure the Output Compare channels */
 /* Common configuration for all channels */
  sConfig.OCMode     = TIM_OCMODE_TOGGLE;
  sConfig.OCPolarity = TIM_OCPOLARITY_LOW;

  /* Code for MotionCP & MotionGR integration - Start Section */
  /* Output Compare Toggle Mode configuration: Channel2 */
  sConfig.Pulse = DEFAULT_uhCCR2_Val;
  if(HAL_TIM_OC_ConfigChannel(&TimCCHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }
  /* Code for MotionCP & MotionGR integration - End Section */

  /* Code for MotionAR integration - Start Section */
  /* Output Compare Toggle Mode configuration: Channel3 */
  sConfig.Pulse = DEFAULT_uhCCR3_Val;
  if(HAL_TIM_OC_ConfigChannel(&TimCCHandle, &sConfig, TIM_CHANNEL_3) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }
  /* Code for MotionAR integration - End Section */

  /* Output Compare Toggle Mode configuration: Channel4 */
  sConfig.Pulse = DEFAULT_uhCCR4_Val;
  if(HAL_TIM_OC_ConfigChannel(&TimCCHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }

}

/**
* @brief  Function for De-initializing timers:
 *  - 1 for sending MotionAR/CP and Acc/Gyro
 * @param  None
 * @retval None
 */
static void DeinitTimers(void)
{
  /* Set TIM1 instance (Motion)*/
  TimCCHandle.Instance = TIM1;
  if(HAL_TIM_Base_DeInit(&TimCCHandle) != HAL_OK)
  {
    /* Deinitialization Error */
    Error_Handler();
  }

}

/** @brief Initialize the BlueNRG Stack
 * @param None
 * @retval None
 */
static void Init_BlueNRG_Stack(void)
{
  //const char BoardName[8] = {NAME_BLUEMS,0};
  char BoardName[8];
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  int ret;
  uint8_t  hwVersion;
  uint16_t fwVersion;

    for(int i=0; i<7; i++)
      BoardName[i]= NodeName[i+1];

    BoardName[7]= 0;

#ifdef MAC_ALLMEMS1
  {
    uint8_t tmp_bdaddr[6]= {MAC_ALLMEMS1};
    int32_t i;
    for(i=0;i<6;i++)
      bdaddr[i] = tmp_bdaddr[i];
  }
#endif /* MAC_ALLMEMS1 */

  /* Initialize the BlueNRG SPI driver */
  //BSP_SPI1_Init();

  /* Initialize the BlueNRG SPI driver */
  hci_init(HCI_Event_CB, NULL);

  /* get the BlueNRG HW and FW versions */
  getBlueNRGVersion(&hwVersion, &fwVersion);

  /*
   * Reset BlueNRG again otherwise we won't
   * be able to change its MAC address.
   * aci_hal_write_config_data() must be the first
   * command after reset otherwise it will fail.
   */
  hci_reset();

#ifndef MAC_ALLMEMS1
  #ifdef MAC_STM32UID_ALLMEMS1
  /* Create a Unique BLE MAC Related to STM32 UID */
  {
    bdaddr[0] = (STM32_UUID[1]>>24)&0xFF;
    bdaddr[1] = (STM32_UUID[0]    )&0xFF;
    bdaddr[2] = (STM32_UUID[2] >>8)&0xFF;
    bdaddr[3] = (STM32_UUID[0]>>16)&0xFF;
    bdaddr[4] = (((ALLMEMS1_VERSION_MAJOR-48)*10) + (ALLMEMS1_VERSION_MINOR-48)+100)&0xFF;
    bdaddr[5] = 0xC0; /* for a Legal BLE Random MAC */
  }
  #else /* MAC_STM32UID_ALLMEMS1 */
  {
    /* we will let the BLE chip to use its Random MAC address */
    uint8_t data_len_out;
    ret = aci_hal_read_config_data(CONFIG_DATA_RANDOM_ADDRESS, 6, &data_len_out, bdaddr);

    if(ret){
      ALLMEMS1_PRINTF("\r\nReading  Random BD_ADDR failed\r\n");
      goto fail;
    }
  }
  #endif /* MAC_STM32UID_ALLMEMS1 */
#else /* MAC_ALLMEMS1 */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                                  CONFIG_DATA_PUBADDR_LEN,
                                  bdaddr);

  if(ret){
     ALLMEMS1_PRINTF("\r\nSetting Pubblic BD_ADDR failed\r\n");
     goto fail;
  }
#endif /* MAC_ALLMEMS1 */

  ret = aci_gatt_init();
  if(ret){
     ALLMEMS1_PRINTF("\r\nGATT_Init failed\r\n");
     goto fail;
  }

  ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);

  if(ret != BLE_STATUS_SUCCESS){
     ALLMEMS1_PRINTF("\r\nGAP_Init failed\r\n");
     goto fail;
  }

#ifndef  MAC_ALLMEMS1
  #ifdef MAC_STM32UID_ALLMEMS1
    ret = hci_le_set_random_address(bdaddr);

    if(ret){
       ALLMEMS1_PRINTF("\r\nSetting the Static Random BD_ADDR failed\r\n");
       goto fail;
    }
  #endif /* MAC_STM32UID_ALLMEMS1 */
#endif /* MAC_ALLMEMS1 */

  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
                                   7/*strlen(BoardName)*/, (uint8_t *)BoardName);

  if(ret){
     ALLMEMS1_PRINTF("\r\naci_gatt_update_char_value failed\r\n");
    while(1);
  }

  ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED,
                                     OOB_AUTH_DATA_ABSENT,
                                     NULL, 7, 16,
                                     USE_FIXED_PIN_FOR_PAIRING, 123456,
                                     BONDING);
  if (ret != BLE_STATUS_SUCCESS) {
     ALLMEMS1_PRINTF("\r\nGAP setting Authentication failed\r\n");
     goto fail;
  }

  ALLMEMS1_PRINTF("SERVER: BLE Stack Initialized \r\n"
         "\t\tHWver= %d\r\n"
         "\t\tFWver= %d.%d.%c\r\n"
         "\t\tBoardName= %s\r\n"
         "\t\tBoardMAC = %x:%x:%x:%x:%x:%x\r\n\n",
         hwVersion,
         fwVersion>>8,
         (fwVersion>>4)&0xF,
         (hwVersion > 0x30) ? ('a'+(fwVersion&0xF)-1) : 'a',
         BoardName,
         bdaddr[5],bdaddr[4],bdaddr[3],bdaddr[2],bdaddr[1],bdaddr[0]);

  /* Set output power level */
  aci_hal_set_tx_power_level(1,4);

  return;

fail:
  return;
}

/** @brief Initialize all the Custom BlueNRG services
 * @param None
 * @retval None
 */
static void Init_BlueNRG_Custom_Services(void)
{
  int ret;

  ret = Add_HW_SW_ServW2ST_Service();
  if(ret == BLE_STATUS_SUCCESS)
  {
     ALLMEMS1_PRINTF("HW & SW Service W2ST added successfully\r\n");
  }
  else
  {
     ALLMEMS1_PRINTF("\r\nError while adding HW & SW Service W2ST\r\n");
  }

  ret = Add_ConsoleW2ST_Service();
  if(ret == BLE_STATUS_SUCCESS)
  {
     ALLMEMS1_PRINTF("Console Service W2ST added successfully\r\n");
  }
  else
  {
     ALLMEMS1_PRINTF("\r\nError while adding Console Service W2ST\r\n");
  }

  ret = Add_ConfigW2ST_Service();
  if(ret == BLE_STATUS_SUCCESS)
  {
     ALLMEMS1_PRINTF("Config  Service W2ST added successfully\r\n");
  }
  else
  {
     ALLMEMS1_PRINTF("\r\nError while adding Config Service W2ST\r\n");
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 6
  *            PLL_N                          = 40
  *            PLL_R                          = 4
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_PWR_EnableBkUpAccess();

  /* Enable the LSE Oscilator */
  /* Activate PLL with MSI as source (MSI is on at reset at 4Mhz) */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState            = RCC_LSE_ON_RTC_ONLY;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM            = 1;
  RCC_OscInitStruct.PLL.PLLN            = 55;
  RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function put the MCU in ShutDown mode
  * @param  None
  * @retval None
  */
void MCU_PowerSave(void)
{
  ALLMEMS1_PRINTF("\r\n");

  /* Enable Accelerometer Event wakeup mode*/
  switch (WakeupSource)
  {
  /* Enable Accelerometer WakeUp */
  case ACC_WAKE_UP:
    EnableHWWakeUp();
    break;
  /* Enable Accelerometer Double Tap */
  case ACC_DOUBLE_TAP:
    EnableHWDoubleTap();
    break;
  default:
    break;
  }

  /*  Disable sensors */
  if(BSP_MOTION_SENSOR_Disable(GYRO_INSTANCE, MOTION_GYRO)==BSP_ERROR_NONE)
    ALLMEMS1_PRINTF("\nDisable Gyroscope Sensor\n\r");

  ALLMEMS1_PRINTF("\r\nMCU Shut Down Mode (Wake up with Double Tap)\r\n");

  GPIO_InitTypeDef GPIO_InitStruct;
  aci_gap_set_non_discoverable();
  HAL_Delay(2000);

  //Shutdown BlueNRG radio
  //Comment this for Fixed bug for the power consump in shot down mode
  //HAL_GPIO_WritePin(BNRG_SPI_RESET_PORT, BNRG_SPI_RESET_PIN, GPIO_PIN_RESET);

  /* Enable GPIOA Clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*** Just for DEBUG and Emulate Stop Mode ***/
//  HAL_DBGMCU_EnableDBGStopMode();
  /*** Just for DEBUG and Emulate Stop Mode ***/

  /* Disable BlueNRG SPI peripheral & Interrupt */
  //Disable_SPI_IRQ();
  HCI_TL_SPI_DeInit();
  __SPI1_CLK_DISABLE();
  //HAL_SPI_DeInit(&SpiHandle);
  //HAL_SPI_DeInit(&SPI_SD_Handle);
  //HAL_I2C_DeInit(&I2C_SENSORTILE_Handle);
//        USBD_DeInit(&USBD_Device);
  /* Disable DFSDM clock */
  __HAL_RCC_DFSDM_CLK_DISABLE();

  DeinitTimers();

  /* Disable all GPIO ports but GPIOA (WakeUp pin is connected to PA2) */
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  //HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
  //__HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  //__HAL_RCC_GPIOH_CLK_DISABLE();
  HAL_SuspendTick();

  __HAL_RCC_RTC_DISABLE();
  __HAL_RCC_DISABLE_IT(RCC_IT_LSIRDY|RCC_IT_LSERDY|RCC_IT_MSIRDY|RCC_IT_HSIRDY|RCC_IT_HSERDY|
                       RCC_IT_PLLRDY|RCC_IT_PLLSAI1RDY|RCC_IT_PLLSAI2RDY|RCC_IT_HSI48RDY);

  /* Mask all interrupt except for PA2 */
  uint32_t temp = 0x00;
  /* Clear EXTI line configuration except for the Wake-up Pin (PA2) */
  temp = EXTI->IMR1;
  temp &= ~((uint32_t)0xFFFFFFFB);
  EXTI->IMR1 = temp;
  temp = EXTI->IMR2;
  temp &= ~((uint32_t)0xFFFFFFFF);
  EXTI->IMR2 = temp;
  temp = EXTI->RTSR1;
  temp &= ~((uint32_t)0xFFFFFFFB);
  EXTI->RTSR1 = temp;

  /* Disable all used wakeup sources: WKUP pin */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  /* Clear wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF1);
  /* Disable all used wakeup sources: WKUP pin */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);
  /* Clear wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
  /* Disable all used wakeup sources: WKUP pin */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN3);
  /* Clear wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF3);
  /* Disable all used wakeup sources: WKUP pin */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN4);
  /* Clear wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF4);
  /* Disable all used wakeup sources: WKUP pin */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN5);
  /* Clear wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF5);

  /* Enable MCU WakeUp on PA2 */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_HIGH);

   /* Clear wake up Flag */
  //__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF4);

  /* Ensure that MSI is wake-up system clock */
  //__HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);
  /* Enter Stop Mode (MCU current = 1.1uA) */
  //HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);


  /* Set RTC back-up register RTC_BKP31R to indicate
     later on that system has entered shutdown mode  */
  WRITE_REG( TAMP->BKP31R, 0x1 );

  /* Enter ShutDown Mode (MCU current = 30nA) */
  //while(1);
  HAL_PWREx_EnterSHUTDOWNMode();
  /* Waiting for SensorTile Movement to Wake Up the MCU */

  /* Reset the MCU after WakeUp from StopMode2 */
  //NVIC_SystemReset();
}



/**
  * @brief This function provides accurate delay (in milliseconds) based
  *        on variable incremented.
  * @note This is a user implementation using WFI state
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay(__IO uint32_t Delay)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay){
    __WFI();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1){
  }
}

/**
 * @brief  EXTI line detection callback.
 * @param  uint16_t GPIO_Pin Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin){
  case HCI_TL_SPI_EXTI_PIN:
      hci_tl_lowlevel_isr();
      HCI_ProcessEvent=1;
    break;

  case GPIO_PIN_3:
    MEMSInterrupt=1;
    break;
  }
}

/**
  * @brief  TS Callback.
  * @param  Instance TS Instance.
  * @retval None.
  */
void BSP_TS_Callback(uint32_t Instance)
{
  if (Instance == 0)
  {
    TSInterrupt=1;
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: ALLMEMS1_PRINTF("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1){
  }
}
#endif

/**
 * @brief  Check if there are a valid Accellerometer Calibration Values in Memory and read them
 * @param  uint16_t dataSize
 * @param  uint32_t *data
 * @retval uint32_t Success/Not Success
 */
unsigned char ReCallAccellerometerCalibrationFromMemory(uint16_t dataSize, uint32_t *data)
{
  /* ReLoad the Accellerometer Calibration Values from RAM */
  uint32_t Success=1;

  int i;

  /* Recall the accellerometer calibration Credential saved */
  if(MDM_ReCallGMD(GMD_ACC_CALIBRATION,(void *)&AccCalibrationData))
  {
    for(i=0; i<dataSize; i++)
    {
      data[i]= AccCalibrationData[i+1];
    }

    Success= 0;
  }

  return Success;
}

/**
 * @brief  Save the Accellerometer Calibration Values to Memory
 * @param  uint16_t dataSize
 * @param  uint32_t *data
 * @retval uint32_t Success/Not Success
 */
unsigned char SaveAccellerometerCalibrationToMemory(uint16_t dataSize, uint32_t *data)
{
  uint32_t Success=1;

  int i;

  /* Store in RAM */
  AccCalibrationData[0] = CHECK_CALIBRATION;

  for(i=0; i<dataSize; i++)
  {
    AccCalibrationData[i+1]= data[i];
  }

  if(MDM_SaveGMD(GMD_ACC_CALIBRATION,(void *)&AccCalibrationData))
  {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
     BytesToWrite = sprintf((char *)BufferToWrite, "Accellerometer Calibration will be saved in FLASH\r\n");
     Term_Update(BufferToWrite,BytesToWrite);
    } else {
      ALLMEMS1_PRINTF("Accellerometer Calibration will be saved in FLASH\r\n");
    }

    NecessityToSaveMetaDataManager=1;

    Success= 0;
  }

  return Success;
}

/**
 * @brief  Reset the Accellerometer Calibration Values in Memory
 * @param  None
 * @retval unsigned char Success/Not Success
 */
unsigned char ResetAccellerometerCalibrationInMemory(void)
{
  /* Reset Calibration Values in RAM */
  unsigned char Success=1;
  int32_t Counter;

  for(Counter=0;Counter<7;Counter++)
    AccCalibrationData[Counter]=0x0;

  if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
     BytesToWrite = sprintf((char *)BufferToWrite, "Accellerometer Calibration will be eresed in FLASH\r\n");
     Term_Update(BufferToWrite,BytesToWrite);
  } else {
    ALLMEMS1_PRINTF("Accellerometer Calibration will be eresed in FLASH\r\n");
  }

  MDM_SaveGMD(GMD_ACC_CALIBRATION,(void *)&AccCalibrationData);

  NecessityToSaveMetaDataManager=1;
  return Success;
}

/**
 * @brief  Check if there are a valid Node Name Values in Memory and read them
 * @param  None
 * @retval unsigned char Success/Not Success
 */
static unsigned char ReCallNodeNameFromMemory(void)
{
  const char DefaultBoardName[7] = {NAME_BLUEMS};

  /* ReLoad the Node Name Values from RAM */
  unsigned char Success=0;

  /* Recall the node name Credential saved */
  MDM_ReCallGMD(GMD_NODE_NAME,(void *)&NodeName);

  if(NodeName[0] != 0x12)
  {
    NodeName[0]= 0x12;

    for(int i=0; i<7; i++)
      NodeName[i+1]= DefaultBoardName[i];

    MDM_SaveGMD(GMD_NODE_NAME,(void *)&NodeName);
    NecessityToSaveMetaDataManager=1;
  }

  return Success;
}



/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
