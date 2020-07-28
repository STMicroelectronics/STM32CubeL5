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
*   FILENAME        -  glucose_sensor.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/07/2014
*   $Revision$:  first version
*   $Author$:    
*   Comments:    glucose sensor
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  source code for the sensor role of glucose profile
* 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/

#include <glucose_service.h>
#include <glucose_sensor.h>
#include <glucose_racp.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* profile states */
#define GL_UNINITIALIZED		        (0x00)
#define GL_WAITING_BLE_INIT                     (0x01)
#define GL_IDLE					(0x02)
#define GL_INITIALIZED				(0x03)
#define GL_UNDIRECTED_ADV_MODE			(0x04)
#define GL_ADV_IN_FAST_DISC_MODE	 	(0x05)
#define GL_ADV_IN_LOW_PWR_MODE			(0x06)
#define GL_CONNECTED				(0x07)
#define GL_CONNECTED_DO_NOTIFY			(0x08)
#define GL_CONNECTED_STOP_NOTIFY		(0x09)

#define GL_ADV_IN_FAST_DISC_MODE_STOP           (0x0A)
#define GL_UNDIRECTED_ADV_MODE_STOP             (0X0B)


/* advertising intervals in terms of 625 micro seconds */
#define GL_FAST_CONN_ADV_INTERVAL_MIN	(0x20) /* 20ms */
#define GL_FAST_CONN_ADV_INTERVAL_MAX	(0x30) /* 30ms */
#define GL_LOW_PWR_ADV_INTERVAL_MIN	(0x640) /* 1 second */
#define GL_LOW_PWR_ADV_INTERVAL_MAX	(0xFA0) /* 2.5 seconds */

/* Glucose Profiles timer periods */

#define GL_TIME_FACTOR  1 /* 1 for seconds */
#define GL_FAST_CONNECTION_TIMER_DURATION (30 * GL_TIME_FACTOR) /* 30 seconds */
#define GL_UNDIRECT_CONNECTABLE_MODE_TIMER_DURATION (120 * GL_TIME_FACTOR) /* 120 seconds: it is defined by glucose sensor */
#define GL_IDLE_CONNECTION_TIMER_DURATION (5 * GL_TIME_FACTOR)  /* 5 seconds */

/******************************************************************************
* type definitions
******************************************************************************/


/******************************************************************************
* Variable Declarations
******************************************************************************/
tGlucoseContext glucose;

static uint8_t charval[GLUCOSE_MEASUREMENT_MAX_LEN]; 
static uint8_t charval_context[GLUCOSE_MEASUREMENT_CONTEXT_MAX_LEN];
/******************************************************************************
* Imported Variable
******************************************************************************/
extern void *profiledbgfile;
extern uint8_t bnrg_expansion_board;

/******************************************************************************
 * Function Prototypes
******************************************************************************/
static tBleStatus GL_Add_Services_Characteristics(void);
static tBleStatus GL_Sensor_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax);
static void GL_AdvTimeout_Handler(void);
static void GL_Exit(uint8_t errCode);
static void GL_IdleConnection_Handler(void);

static void GLProfile_Write_MainStateMachine(tProfileState localmainState);
static tProfileState GLProfile_Read_MainStateMachine(void);
static void GLProfile_Write_SubStateMachine(tProfileState localsubState);
static tProfileState GLProfile_Read_SubStateMachine(void);

/*******************************************************************************
* SAP
*******************************************************************************/
/**
  * @brief  Reset the initialization flags for glucose sensor 
  * @param sequenceNumber: last stored sequence number on database
  * @retval None
  */
void GL_ResetFlags(uint16_t sequenceNumber)
{
    /* Reset timer value */
    glucose.timerID = 0xFF;

    /* unset flag for next measurement record to be sent */
    glucose.RACP_SendNextMeasurementRecord_Flag = UNSET_FLAG;
    /* unset flag for next measurement context to be sent */
    glucose.RACP_SendNextMeasurementRecord_Flag = UNSET_FLAG;
    /* reset index for next measurement record to be sent */
    glucose.RACP_NextMeasurementRecord_Index = 0; 
    /* unset flag for received RACP abort operation request */
    glucose.RACP_Abort_Flag = UNSET_FLAG;
    /* unset flag for RACP operation (different from abort) already in progress */
    glucose.RACP_In_Progress_Flag = UNSET_FLAG; 
    /* initial sequence number value = number of stored records on measurement database */
    glucose.sequenceNumber = sequenceNumber;
    /* reset received RACP attribute value */
    BLUENRG_memset(&(glucose.attValue),0,RACP_COMMAND_MAX_LEN);
}/* end GL_ResetFlags() */

/**
  * @brief Init Glucose Sensor 
  * @param sequenceNumber : last stored sequence number on database
  * @param gl_measurement_db_records : pointer to user glucose measurement database
  * @param gl_measurement_context_db_records : pointer to user glucose measurement context database
  * @param applcb :    glucose profile callback 
  * @retval tBleStatus :    init status
  */
tBleStatus GL_Init(uint16_t sequenceNumber, 
                   tGlucoseMeasurementVal * gl_measurement_db_records, 
                   tGlucoseMeasurementContextVal * gl_measurement_context_db_records,
                   BLE_CALLBACK_FUNCTION_TYPE applcb)
{
    tBleStatus retval = BLE_STATUS_FAILED;
          
    GL_DBG_MSG(profiledbgfile, "GL_Init\n");
    
    if(Is_Profile_Present(PID_GLUCOSE_SENSOR) == BLE_STATUS_SUCCESS)
    {
      return BLE_STATUS_PROFILE_ALREADY_INITIALIZED;
    }
    
    if(applcb == NULL)
    {
      return BLE_STATUS_NULL_PARAM;
    }
          
    /* Initialize Data Structures */
    BLUENRG_memset(&glucose,0,sizeof(glucose));
    glucose.applcb = applcb;
    /* reset all initialization flags */
    GL_ResetFlags(sequenceNumber); 
    GLProfile_Write_MainStateMachine(GL_UNINITIALIZED);
    GLProfile_Write_SubStateMachine(GL_UNINITIALIZED); 
    glucose.descriptors_configuration = 0;
    glucose.max_number_of_used_records = sequenceNumber;
    
    /* Pointer to the User Glucose measurement database: it's a simulated database with some default values */
    glucose.glucoseDatabase_MeasurementRecords_ptr = gl_measurement_db_records;
    /* Pointer to User Glucose measurement context database: it's a simulated database with some default values */
    glucose.glucoseDataBase_MeasurementContextRecords_ptr = gl_measurement_context_db_records;
          
    /* register the glucose sensor with BLE main Profile */
    BLUENRG_memset(&glucose.GLtoBLEInf,0,sizeof(glucose.GLtoBLEInf));
    glucose.GLtoBLEInf.profileID = PID_GLUCOSE_SENSOR;
    glucose.GLtoBLEInf.callback_func = GL_Event_Handler;
    glucose.GLtoBLEInf.voteForAdvertisableState = 0;
    /* manufacturer name, model number and system ID characteristics
     * have to be added to the device information service
     */ 
    glucose.GLtoBLEInf.DISCharRequired = (MANUFACTURER_NAME_STRING_CHAR_MASK|MODEL_NUMBER_STRING_CHAR_MASK|SYSTEM_ID_CHAR_MASK);
    
    retval = BLE_Profile_Register_Profile(&glucose.GLtoBLEInf);
    if (retval != BLE_STATUS_SUCCESS)
    {
      GL_DBG_MSG(profiledbgfile,"GL_Init(), BLE_Profile_Register_Profile() failed\n");
      /* Profile could not be registered with the BLE main profile */
      return (retval);
    }

    GLProfile_Write_MainStateMachine(GL_WAITING_BLE_INIT);
    
    GL_DBG_MSG(profiledbgfile,"Glucose Sensor Profile is Initialized\n");
    
    return (retval);
 
}/* end GL_Init() */

/**
  * @brief  Handle Glucose Sensor Advertising
  * @param  useWhitelist: it defines which advertising mode to be used 
  * @retval tBleStatus:   function status
  */
tBleStatus GL_Advertize(uint8_t useWhitelist)
{
    tBleStatus status = BLE_STATUS_FAILED;
          
    GL_DBG_MSG(profiledbgfile,"GL_Advertize(),glucose.state = %x\n",GLProfile_Read_MainStateMachine());
          
    if(GLProfile_Read_MainStateMachine() == GL_INITIALIZED)
    {
      if(useWhitelist)
      {
        /* start the undirected connectable mode */
        status = aci_gap_set_undirected_connectable(PUBLIC_ADDR, WHITE_LIST_FOR_ALL);   
        if(status == BLE_STATUS_SUCCESS)
        {
          GLProfile_Write_MainStateMachine(GL_UNDIRECTED_ADV_MODE); 
          
          GL_DBG_MSG(profiledbgfile,"GL_Advertize(), enabled undirected connectable mode\n");
          
          /* start a timer for GL_UNDIRECT_CONNECTABLE_MODE_TIMER_DURATION seconds */
	  Blue_NRG_Timer_Start(GL_UNDIRECT_CONNECTABLE_MODE_TIMER_DURATION,GL_AdvTimeout_Handler,&glucose.timerID);
					
	  /* request for main profile's state change */
	  BLE_Profile_Change_Advertise_Sub_State(&glucose.GLtoBLEInf, 
						 BLE_PROFILE_STATE_ADVERTISING);
          
        }
        else
        {
           GL_DBG_MSG (profiledbgfile,"GL_Advertize(), FAILED to start undirected connectable mode, Error: %02X !!\n", status);
           
           /* inform the application that the advertizing was not enabled */
           GLProfile_Write_MainStateMachine(GL_IDLE);
           glucose.applcb(EVT_MP_ADVERTISE_ERROR,1,&status); 
        }
      }
      else
      {
        status = GL_Sensor_Set_Discoverable(GL_FAST_CONN_ADV_INTERVAL_MIN,
                                            GL_FAST_CONN_ADV_INTERVAL_MAX);
        if(status == BLE_STATUS_SUCCESS)
        {
          GLProfile_Write_MainStateMachine(GL_ADV_IN_FAST_DISC_MODE);
          
          GL_DBG_MSG(profiledbgfile,"GL_Advertize(),enabled advertising fast connection\n");
          
          /* start a timer for GL_FAST_CONNECTION_TIMER_DURATION seconds */
          Blue_NRG_Timer_Start(GL_FAST_CONNECTION_TIMER_DURATION,GL_AdvTimeout_Handler,&glucose.timerID);
	  
	  /* request for main profile's state change */
	  BLE_Profile_Change_Advertise_Sub_State(&glucose.GLtoBLEInf, 
						 BLE_PROFILE_STATE_ADVERTISING);
          
        }
        else
        {
          /* inform the application that the advertizing was not
	   * enabled
	   */
          GL_DBG_MSG(profiledbgfile,"GL_Advertize(),FAILED to start limited discoverable mode %x\n",status); 
	  GLProfile_Write_MainStateMachine(GL_IDLE);
	  glucose.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
          
        }
      }
    }
    else
    {
        GL_DBG_MSG(profiledbgfile,"GL_Advertize(), FAILED to start advertize, since glucose.state = %x\n",GLProfile_Read_MainStateMachine()); 
    }
    return (status);
}/* end GL_Advertize() */

/**
  * @brief  Send a Glucose Measurement
  * @param  glucoseMeasurementVal: glucose measurement to be sent (from database)
  * @retval tBleStatus:            function status
  */
tBleStatus GL_Send_Glucose_Measurement(tGlucoseMeasurementVal glucoseMeasurementVal)
{
    uint8_t idx = 0;
    tBleStatus retval = BLE_STATUS_FAILED; 

    if(GLProfile_Read_MainStateMachine() == GL_CONNECTED)
    {
      BLUENRG_memset(charval,0x00,GLUCOSE_MEASUREMENT_MAX_LEN);
          
      charval[idx++] = glucoseMeasurementVal.flags;
      STORE_LE_16(&charval[idx],glucoseMeasurementVal.sequenceNumber);
      
      idx += 2;

      /* The Base Time field is defined to use the same format as the Date Time characteristic.
         However, a value of 0 for the year, month or day fields shall not be used for this service */
      if((glucoseMeasurementVal.baseTime.year == 0) ||
         (glucoseMeasurementVal.baseTime.month == 0) ||
         (glucoseMeasurementVal.baseTime.day == 0))
      {
          return BLE_STATUS_INVALID_PARAMS;
      }			
      STORE_LE_16(&charval[idx],glucoseMeasurementVal.baseTime.year);
      
      idx += 2;
      charval[idx++] = glucoseMeasurementVal.baseTime.month;
      charval[idx++] = glucoseMeasurementVal.baseTime.day;
      charval[idx++] = glucoseMeasurementVal.baseTime.hours;
      charval[idx++] = glucoseMeasurementVal.baseTime.minutes;
      charval[idx++] = glucoseMeasurementVal.baseTime.seconds;

      if(glucoseMeasurementVal.flags & GLUCOSE_MEASUREMENT_FLAGS_TIME_OFFSET_IS_PRESENT)
      {
        STORE_LE_16(&charval[idx],glucoseMeasurementVal.timeOffset);
        idx += 2;
      }	

      if(glucoseMeasurementVal.flags & GLUCOSE_MEASUREMENT_FLAGS_CONCENTRATION_IS_PRESENT)
      {
        STORE_LE_16(&charval[idx],glucoseMeasurementVal.glucoseConcentration);
        idx += 2;
        /* if glucose Concentration field is present, Type-Sample Location field shall also be present */
        charval[idx++] = glucoseMeasurementVal.typeSampleLocation;
      }
       
      if(glucoseMeasurementVal.flags & GLUCOSE_MEASUREMENT_FLAGS_STATUS_ANNUNCIATION_IS_PRESENT)
      {

        //glucoseMeasurementVal.sensorStatusAnnunciation &= glucose.featureValue;//TBR
        STORE_LE_16(&charval[idx],glucoseMeasurementVal.sensorStatusAnnunciation);
        idx += 2;
      }	

      /* TBR: DO we need to check if (glucoseMeasurementVal.state == GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED) before doing
         next operations ? */
          
      /* Check if measurement context has to be sent for current selected measurement record */
      if(glucoseMeasurementVal.flags & GLUCOSE_MEASUREMENT_FLAGS_CONTEXT_INFO_IS_PRESENT)
      {
        /* Set flag for sending measurement context record */
        glucose.RACP_SendNextMeasurementContextRecord_Flag = SET_FLAG;
      }

      /* TBR */
      retval = BLE_STATUS_INSUFFICIENT_RESOURCES;
      while (retval == BLE_STATUS_INSUFFICIENT_RESOURCES)
      {
      /* Notify the glucose measurement */
      retval =  aci_gatt_update_char_value(glucose.glServiceHandle, 
                                           glucose.MeasurementCharHandle,
                                           0, /* charValOffset */
                                           idx, /* charValueLen */
                                           (uint8_t *) charval);  
      }
      
      if(retval == BLE_STATUS_SUCCESS)
      {
        GL_DBG_MSG(profiledbgfile,"GL_Send_Glucose_Measurement(), glucose measurement sent successfully\n");
                          
        /* stop the idle timer */
        Blue_NRG_Timer_Stop(glucose.timerID);
        glucose.timerID = 0xFF;
        GLProfile_Write_SubStateMachine(GL_CONNECTED_DO_NOTIFY); 
      }
      else
      {
        GL_DBG_MSG(profiledbgfile,"GL_Send_Glucose_Measurement(),FAILED to send glucose measurement %02X \n", retval);
        /* Set flag for sending measurement context record */
        glucose.RACP_SendNextMeasurementContextRecord_Flag = UNSET_FLAG; 
      }
    }

    return (retval);
}/* GL_Send_Glucose_Measurement() */

/**
  * @brief  Send a Glucose Measurement Context (it follows a Glucose Measurement)
  * @param  glucoseMeasurementVal: glucose measurement context to be sent
  * @retval tBleStatus:            function status
  */
tBleStatus GL_Send_Glucose_Measurement_Context(tGlucoseMeasurementContextVal glucoseMeasurementContextVal)
{
    uint8_t idx = 0;
    tBleStatus retval = BLE_STATUS_FAILED; 
          
    if (GLProfile_Read_MainStateMachine() == GL_CONNECTED)
    {
      BLUENRG_memset(charval_context,0x00,GLUCOSE_MEASUREMENT_CONTEXT_MAX_LEN);
                  
      charval_context[idx++] = glucoseMeasurementContextVal.flags;
      STORE_LE_16(&charval_context[idx],glucoseMeasurementContextVal.sequenceNumber);
      idx += 2;

      if(glucoseMeasurementContextVal.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_EXTENDED_IS_PRESENT)
      {
        charval_context[idx++] = glucoseMeasurementContextVal.extendedFlags;
      }

      if(glucoseMeasurementContextVal.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_CARBOHYDRATE_IS_PRESENT)
      {
        charval_context[idx++] = glucoseMeasurementContextVal.carbohydrateId;
        STORE_LE_16(&charval_context[idx],glucoseMeasurementContextVal.carbohydrateUnits);
        idx += 2;
      }
      
      if(glucoseMeasurementContextVal.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEAL_IS_PRESENT)
      {
        charval_context[idx++] = glucoseMeasurementContextVal.meal;
      }

      if(glucoseMeasurementContextVal.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_TESTER_HEALTH_IS_PRESENT)
      {
        charval_context[idx++] = glucoseMeasurementContextVal.testerHealth;
      }

      if(glucoseMeasurementContextVal.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_EXERCISE_DURATION_IS_PRESENT)
      {
        STORE_LE_16(&charval_context[idx],glucoseMeasurementContextVal.exerciseDuration);
        idx += 2;
        charval_context[idx++] = glucoseMeasurementContextVal.exerciseIntensity;
      }
     
      if(glucoseMeasurementContextVal.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_ID_IS_PRESENT)
      {
        charval_context[idx++] = glucoseMeasurementContextVal.medicationId;
        STORE_LE_16(&charval_context[idx],glucoseMeasurementContextVal.medicationUnits);
        idx += 2;
      }

      if(glucoseMeasurementContextVal.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_HB1A1C_IS_PRESENT)
      {
        STORE_LE_16(&charval_context[idx],glucoseMeasurementContextVal.HbA1c);
        idx += 2;
      }
      /* TBR: DO we need to check if (glucoseMeasurementVal.state == GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED) before doing
         next operations ? */
      /* Notify the glucose measurement context */
      
      /* TBR */
      retval = BLE_STATUS_INSUFFICIENT_RESOURCES;
      while (retval == BLE_STATUS_INSUFFICIENT_RESOURCES)
      {
      retval =  aci_gatt_update_char_value(glucose.glServiceHandle, 
                                           glucose.MeasurementContextCharHandle,
                                           0, /* charValOffset */
                                           idx, /* charValueLen */
                                           (uint8_t *)charval_context); 
      }
      if(retval == BLE_STATUS_SUCCESS)
      {
        GL_DBG_MSG(profiledbgfile,"GL_Send_Glucose_Measurement_Context(), glucose measurement context sent successfully\n");
        Blue_NRG_Timer_Stop(glucose.timerID);
        glucose.timerID = 0xFF; 
        GLProfile_Write_SubStateMachine(GL_CONNECTED_DO_NOTIFY); 
      }
      else
      {
         GL_DBG_MSG(profiledbgfile,"GL_Send_Glucose_Measurement_Context(), FAILED to send glucose measurement context %02X \n", retval);
      }
    }
    return (retval);
}/* end GL_Send_Glucose_Measurement_Context() */

/**
  * @brief  Set the glucose feature characteristic value
  * @param  value: glucose feature value
  * @retval tBleStatus: function status
  */
tBleStatus GL_Set_Glucose_Feature_Value(uint16_t value)
{
    tBleStatus retval = BLE_STATUS_FAILED; 
          
    /* The glucose feature characteristic cannot be updated during connection.
     * Currently it is updated at init time with lifetime values 
     */ 
    glucose.featureValue = value;
    if (GLProfile_Read_MainStateMachine() != GL_CONNECTED)
    {
        retval =  aci_gatt_update_char_value(glucose.glServiceHandle, 
                                             glucose.FeatureCharHandle,
                                             0, /* charValOffset */
                                             GLUCOSE_FEATURE_MAX_LEN, /* charValueLen */
                                             (const uint8_t *) &(glucose.featureValue));    
      
        if(retval == BLE_STATUS_SUCCESS)
        {
            GL_DBG_MSG(profiledbgfile,"GL_Set_Glucose_Feature_Value(), updated glucose feature\n");
        }
        else
        {
          GL_DBG_MSG(profiledbgfile,"GL_Set_Glucose_Feature_Value(), FAILED to update glucose feature %02X \n", retval);
        }
    }
    return (retval);
}/* end GL_Set_Glucose_Feature_Value() */

/*******************************************************************************
* Local Functions
*******************************************************************************/

static void GLProfile_Write_MainStateMachine(tProfileState localmainState)
{
  glucose.state = localmainState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState GLProfile_Read_MainStateMachine(void)
{
  return(glucose.state);
}

static void GLProfile_Write_SubStateMachine(tProfileState localsubState)
{
  glucose.substate = localsubState;
  Profile_Notify_StateMachine_Update();
  
  return;
}

static tProfileState GLProfile_Read_SubStateMachine(void)
{
  return(glucose.substate);
}

/*
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
static void GLProfile_Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  uint8_t attValLen;
  uint16_t attrHandle;
  
  attrHandle = handle;
  
  GL_DBG_MSG(profiledbgfile,"GL_Event_Handler(), Recevied event EVT_BLUE_GATT_ATTRIBUTE_MODIFIED %x\n",attrHandle);
  /* TBR: Store  glucose characteristics client descriptors */
  if (attrHandle == (glucose.RACPCharHandle + 2)) /* Indicate value: 0x02 */
  { 
    if (att_data[0] == 0x02)
      glucose.descriptors_configuration |=  RACP_DESCRIPTOR_PROPERLY_CONFIGURED;
    else 
      glucose.descriptors_configuration &= ~RACP_DESCRIPTOR_PROPERLY_CONFIGURED;
  }
  else if (attrHandle == (glucose.MeasurementCharHandle + 2)) 
  {
    if (att_data[0] == 0x01) /* : Notify value:0x01 */
      glucose.descriptors_configuration |= MEASUREMENT_DESCRIPTOR_PROPERLY_CONFIGURED;
    else 
      glucose.descriptors_configuration &= ~MEASUREMENT_DESCRIPTOR_PROPERLY_CONFIGURED;
  }        
  else if (attrHandle == (glucose.MeasurementContextCharHandle + 2))
  {
    if (att_data[0] == 0x01) /* : Notify value:0x01 */
      glucose.descriptors_configuration |= MEASUREMENT_CONTEXT_DESCRIPTOR_PROPERLY_CONFIGURED;
    else 
      glucose.descriptors_configuration &= ~MEASUREMENT_CONTEXT_DESCRIPTOR_PROPERLY_CONFIGURED;
  }
  if(attrHandle == (glucose.RACPCharHandle + 1))
  {
    /* this event is received only when the client writes to the RACP attribute
    * value. 
    */ 
    attValLen = data_length;
    /* reset received RACP attribute value */
    BLUENRG_memset(&(glucose.attValue),0,RACP_COMMAND_MAX_LEN);
    /* copy the received RACP characteristic command */ 
    BLUENRG_memcpy((void *) &(glucose.attValue), 
                (void *) att_data,
                attValLen); 
    //GL_DBG_MSG(profiledbgfile,"attValue %x\n",attValue);
    GL_DBG_MSG(profiledbgfile,"------> Len:  %x, aV[0]: %x, aV[1]: %x, aV[2]: %x, aV[3]: %x, aV[4]: %x\n",attValLen,glucose.attValue[0],glucose.attValue[1],glucose.attValue[2],glucose.attValue[3],glucose.attValue[4]); 
    
    /* A RACP request has been received and sensor can start processing it */
    /* TBR:is it correct to set it here or in previous HCI_EVT_BLUE_GATT_WRITE_PERMIT_REQ ?) */
    glucose.RACP_In_Progress_Flag = SET_FLAG; 
    
    /* Call the RACP request handler */
    glucoseRACP_RequestHandler(attValLen);
    
    /* notify the application TBR */	
    //glucose.applcb(EVT_GL_GLUCOSE_RACP_REQUEST,attValLen,(uint8_t*)&(glucose.attValue));												
  }
}

/* GL_Sensor_Set_Discoverable() */
tBleStatus GL_Sensor_Set_Discoverable(uint16_t AdvIntervMin, uint16_t AdvIntervMax)
{
    tBleStatus retval = BLE_STATUS_SUCCESS;
    
    const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'G','l','u','c','o','s','e','S','e','n','s','o','r'};
    
    retval = aci_gap_set_limited_discoverable(ADV_IND,
                                              AdvIntervMin, 
                                              AdvIntervMax,
                                              PUBLIC_ADDR, 
                                              NO_WHITE_LIST_USE, 
                                              sizeof(local_name), 
                                              local_name, 
                                              gBLEProfileContext.advtServUUIDlen, 
                                              gBLEProfileContext.advtServUUID,    
                                              0, 
                                              0);

    return retval;
}/* end GL_Sensor_Set_Discoverable() */


/**
 * GL_Add_Services_Characteristics
 * 
* @param[in]: None
 *            
 * It adds Glucose  service & related characteristics
 */ 
tBleStatus GL_Add_Services_Characteristics(void)
{
    tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;
    uint8_t numAttribRec = 0;
    uint16_t uuid = 0;
    
    if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
    {
        GL_DBG_MSG(profiledbgfile,"Main profile initialized, Adding Glucose Service\n");
				
	uuid = GLUCOSE_SERVICE_UUID;
	numAttribRec = 1; /* service attribute */

        /* Add num Attributes records for following characteristics:
         * 
         * Glucose Measurement: 3 (we have to support the client configuration
         *                         descriptor for that characteristic as well);
	 * Glucose Measurement Context: 3 (we have to support the client configuration
         *                                 descriptor for that characteristic as well);
         * Glucose Feature: 2 (no client configuration descriptor)
         * Record Access Control Point (RACP): 3 (we have to support the client configuration
         *                                  descriptor for that characteristic as well);
         *
         * NOTE: Glucose Measurement Context presence depends on specific flag set on the
         *       Glucose Measurement chracteristic. We add the characteristic anyway: it's up
         *       to user application to set measurements records on database using it or not
         */
        numAttribRec = numAttribRec +3 +3 +2 +3; 
        
        hciCmdResult = aci_gatt_add_serv(UUID_TYPE_16, 
                                        (const uint8_t *) &uuid, 
                                        PRIMARY_SERVICE, 
                                        numAttribRec,
                                        &(glucose.glServiceHandle));
                
        if (hciCmdResult == BLE_STATUS_SUCCESS)
        {
           GL_DBG_MSG (profiledbgfile, "GL_Add_Services_Characteristics(), Heart Rate Service is added Successfully %04X\n", glucose.glServiceHandle);
        }
        else 
        {
            GL_DBG_MSG (profiledbgfile,"GL_Add_Services_Characteristics(),FAILED to add Glucose Service, Error: %02X !!\n", hciCmdResult);
            GL_Exit(hciCmdResult); 
            return hciCmdResult;
        }
        
         /* Add Glucose Measurement Characteristic */
        GL_DBG_MSG (profiledbgfile,"GL_Add_Services_Characteristics(),Adding Glucose Measurement Characteristic. \n");
	uuid = GLUCOSE_MEASUREMENT_CHAR_UUID;
        
        hciCmdResult = aci_gatt_add_char(glucose.glServiceHandle, 
                                         UUID_TYPE_16, 
                                         (const uint8_t *) &uuid , 
                                         GLUCOSE_MEASUREMENT_MAX_LEN, 
                                         CHAR_PROP_NOTIFY, 
                                         ATTR_PERMISSION_NONE, 
                                         GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                         10,  /* encryKeySize */
                                         CHAR_VALUE_LEN_VARIABLE, /* isVariable */
                                         &(glucose.MeasurementCharHandle));
       if (hciCmdResult == BLE_STATUS_SUCCESS)
       {
           GL_DBG_MSG (profiledbgfile, "GL_Add_Services_Characteristics(),Glucose Measurement Characteristic Added Successfully  %04X \n", glucose.MeasurementCharHandle);
       }
       else 
       {
           GL_DBG_MSG (profiledbgfile,"GL_Add_Services_Characteristics(),FAILED to add Glucose Measurement Characteristic,  Error: %02X !!\n", hciCmdResult);
           GL_Exit(hciCmdResult); 
           return hciCmdResult;
       }          
       
       /* Add Glucose Measurement Context Characteristic */
       GL_DBG_MSG (profiledbgfile,"GL_Add_Services_Characteristics(),Adding Glucose Measurement Context Characteristic. \n");
        
       /* Add glucose measurement context characteristic */
       uuid = GLUCOSE_MEASUREMENT_CONTEXT_CHAR_UUID;
       hciCmdResult = aci_gatt_add_char(glucose.glServiceHandle, 
                                        UUID_TYPE_16, 
                                        (const uint8_t *) &uuid , 
                                        GLUCOSE_MEASUREMENT_CONTEXT_MAX_LEN, 
                                        CHAR_PROP_NOTIFY, 
                                        ATTR_PERMISSION_NONE, 
                                        GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                        10,  /* encryKeySize */
                                        CHAR_VALUE_LEN_VARIABLE, /* isVariable */
                                        &(glucose.MeasurementContextCharHandle));
       
       if (hciCmdResult == BLE_STATUS_SUCCESS)
       {
           GL_DBG_MSG (profiledbgfile, "GL_Add_Services_Characteristics(),Glucose Measurement Context Characteristic Added Successfully  %04X \n", glucose.MeasurementContextCharHandle);
       }
       else 
       {
           GL_DBG_MSG (profiledbgfile,"GL_Add_Services_Characteristics(),FAILED to add Glucose Measurement Context Characteristic,  Error: %02X !!\n", hciCmdResult);
           GL_Exit(hciCmdResult); 
           return hciCmdResult;
       }          
       
       /* Add Glucose Feature Characteristic */
       GL_DBG_MSG (profiledbgfile,"GL_Add_Services_Characteristics(),Adding Glucose Feature Characteristic. \n");
	
       /* Add glucose feature characteristic */
       uuid = GLUCOSE_FEATURE_CHAR_UUID;
       
       hciCmdResult = aci_gatt_add_char(glucose.glServiceHandle, 
                                        UUID_TYPE_16, 
                                        (const uint8_t *) &uuid , 
                                        GLUCOSE_FEATURE_MAX_LEN, 
                                        CHAR_PROP_READ, 
                                        ATTR_PERMISSION_NONE, 
                                        GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                        10, /* encryKeySize */
                                        CHAR_VALUE_LEN_CONSTANT, /* isVariable */
                                        &(glucose.FeatureCharHandle));
       
       if (hciCmdResult == BLE_STATUS_SUCCESS)
       {
           GL_DBG_MSG (profiledbgfile, "GL_Add_Services_Characteristics(),Glucose Feature Characteristic Added Successfully  %04X \n", glucose.FeatureCharHandle);
       }
       else 
       {
           GL_DBG_MSG (profiledbgfile,"GL_Add_Services_Characteristics(),FAILED to add Glucose Feature Characteristic,  Error: %02X !!\n", hciCmdResult);
           GL_Exit(hciCmdResult); 
           return hciCmdResult;
       }          
       
       /* Add Glucose Record Access Control Point (RACP) Characteristic */
       GL_DBG_MSG (profiledbgfile,"GL_Add_Services_Characteristics(),Adding Glucose Record Access Control Point (RACP) Characteristic. \n");
       
       /* Add Glucose Record Access Control Point (RACP) characteristic */			
       uuid = GLUCOSE_RACP_CHAR_UUID;
       hciCmdResult = aci_gatt_add_char(glucose.glServiceHandle, 
                                        UUID_TYPE_16, 
                                        (const uint8_t *) &uuid , 
                                        RACP_COMMAND_MAX_LEN, 
                                        CHAR_PROP_WRITE|CHAR_PROP_INDICATE, 
                                        ATTR_PERMISSION_AUTHEN_WRITE, 
                                        GATT_NOTIFY_ATTRIBUTE_WRITE|GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP, /* gattEvtMask */
                                        10,  /* encryKeySize */
                                        CHAR_VALUE_LEN_VARIABLE, /* isVariable */
                                        &(glucose.RACPCharHandle));
       
       if (hciCmdResult == BLE_STATUS_SUCCESS)
       {
           GL_DBG_MSG (profiledbgfile, "GL_Add_Services_Characteristics(),Glucose RACP Added Successfully  %04X \n", glucose.FeatureCharHandle);
       }
       else 
       {
           GL_DBG_MSG (profiledbgfile,"GL_Add_Services_Characteristics(),FAILED to add RACP Characteristic,  Error: %02X !!\n", hciCmdResult);
           GL_Exit(hciCmdResult); 
           return hciCmdResult;
       }    
       /* Set security permission of Record Access Control Point (RACP) characteristic descriptor to None (0x00): as defined by profile specification. 
          By default, on BlueNRG-MS the characteristic descriptor has the same security permission of the characteristic. 
          By default, on BlueNRG the characteristic descriptor has security permission equal to None. 
       */
       hciCmdResult =  aci_gatt_set_security_permission(glucose.glServiceHandle, 
                                                        glucose.RACPCharHandle + 2,
                                                        0x00);
       if (hciCmdResult == BLE_STATUS_SUCCESS)
       {
         GL_DBG_MSG (profiledbgfile, "GL_Add_Services_Characteristics(),aci_gatt_set_security_permission() OK\n");
       }
       else 
       {
         GL_DBG_MSG (profiledbgfile,"GL_Add_Services_Characteristics(),FAILED on aci_gatt_set_security_permission(): %02X !!\n", hciCmdResult);
         GL_Exit(hciCmdResult); 
         return hciCmdResult;
       }          
    }
    else
    {
        GL_DBG_MSG (profiledbgfile,"GL_Add_Services_Characteristics(),Main Profile is not yet on Init State,  Current State: %02X !!\n", BLE_Profile_Get_Main_State()); 
    }  
    return hciCmdResult;
}/* end GL_Add_Services_Characteristics() */

/**
* GL_Event_Handler
* 
* @param[in] pckt : Pointer to the ACI packet
* 
* It parses the events received from ACI according to the profile's state machine.
* Inside this function each event must be identified and correctly parsed.
* NOTE: It is the event handler to be called inside HCI_Event_CB() 
*/ 
void GL_Event_Handler(void *pckt)
{
  uint8_t attValLen;
  tBleStatus retval = BLE_STATUS_SUCCESS;
  uint16_t attrHandle;
  
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  GL_DBG_MSG(profiledbgfile, "GL_Event_Handler(), Glucose Sensor State: %02X \n", GLProfile_Read_MainStateMachine()  );
  
  if(hci_pckt->type != HCI_EVENT_PKT)
  {
    GL_DBG_MSG (profiledbgfile,"GL_Event_Handler(),Recevied packed is not an HCI Event: %02X !!\n", hci_pckt->type);
    return;
  }
	
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
          
          GL_DBG_MSG( profiledbgfile, "GL_Event_Handler(), Received event EVT_LE_CONN_COMPLETE %2x\n", cc->status);
          if (cc->status == BLE_STATUS_SUCCESS)
          {
            /* store connection handle */
            glucose.connHandle = cc->handle;
           
            GL_DBG_MSG(profiledbgfile,"GL_Event_Handler(),connected %x\n",glucose.connHandle);
            Blue_NRG_Timer_Stop(glucose.timerID);
            glucose.timerID = 0xFF;
            GLProfile_Write_MainStateMachine(GL_CONNECTED);
          }
        }
	break;
      }
    }
    break;
    case EVT_DISCONN_COMPLETE:
    {
      evt_disconn_complete *cc = (void *)event_pckt->data;
      
      GL_DBG_MSG(profiledbgfile,"GL_Event_Handler(), Received event EVT_DISCONN_COMPLETE %2x\n", cc->status);
      if (cc->status == BLE_STATUS_SUCCESS)
      {
        /* stop the idle connection timer if it is running */
        Blue_NRG_Timer_Stop(glucose.timerID);
        glucose.timerID = 0xFF;
        GLProfile_Write_MainStateMachine(GL_INITIALIZED);
      }
    }
    break;
    case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      
      switch(blue_evt->ecode)
      {
      case EVT_BLUE_GAP_LIMITED_DISCOVERABLE: /* THis event is received when in limited discoverable mode after advertising timeout: ~ 180 sec */
	{
          GL_DBG_MSG(profiledbgfile,"GL_Event_Handler(), Recevied event EVT_BLUE_GAP_LIMITED_DISCOVERABLE \n");
	  if((GLProfile_Read_MainStateMachine() == GL_ADV_IN_FAST_DISC_MODE) ||
	     (GLProfile_Read_MainStateMachine() == GL_ADV_IN_LOW_PWR_MODE))
	  {
	    /* request for main profile's state change */
	    BLE_Profile_Change_Advertise_Sub_State(&glucose.GLtoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
						
	    if(GLProfile_Read_MainStateMachine() == GL_ADV_IN_LOW_PWR_MODE)
	    {
	      GLProfile_Write_MainStateMachine(GL_INITIALIZED);
	      /* notify the application */
	      glucose.applcb(EVT_MP_ADVERTISING_TIMEOUT,0,NULL);
	    }
	  }
	}
	break;
        case EVT_BLUE_GATT_WRITE_PERMIT_REQ:
	{ 
          GL_DBG_MSG(profiledbgfile,"GL_Event_Handler(), Received event EVT_BLUE_GATT_WRITE_PERMIT_REQ \n");
          
          evt_gatt_write_permit_req * write_perm_req = (void*)blue_evt->data;
          attrHandle = write_perm_req->attr_handle;
          attValLen = write_perm_req->data_length;
          
	  if(attrHandle == (glucose.RACPCharHandle + 1)) 
	  {
            /* reset received RACP attribute value */
            BLUENRG_memset(&(glucose.attValue),0,RACP_COMMAND_MAX_LEN);
            
            /* copy the received RACP characteristic command (TBR) */
            BLUENRG_memcpy((void *) &(glucose.attValue), (void *) write_perm_req->data,attValLen); 
            
            /* check if a RACP request with Op Code other than Abort is received while glucose sensor is 
               performing a previously triggered RACP operation */
	    if((glucose.attValue[RACP_OP_CODE_POSITION] != RACP_ABORT_OPERATION_OP_CODE) && (glucose.RACP_In_Progress_Flag))
	    {
	      /* invalid value */
	      GL_DBG_MSG(profiledbgfile,"GL_Event_Handler(),RACP is already in progress\n");
              
              retval = aci_gatt_write_response(glucose.connHandle,
                                               attrHandle,
                                               0x01, /* write_status = 1 (error))*/ // BLE_STATUS_FAILED
                                               RACP_RESPONSE_PROCEDURE_ALREADY_IN_PROGRESS,
                                               attValLen, 
                                               (uint8_t *)&(glucose.attValue));
	    } 
            /* TBR: Check for Client Characteristics Configuration Descriptor Improperly Configured error when
               RACP implies notification */
            else if ((glucose.attValue[RACP_OP_CODE_POSITION] == RACP_REPORT_STORED_RECORDS_OP_CODE) && (glucose.descriptors_configuration != GLUCOSE_DESCRIPTORS_ALL_PROPERLY_CONFIGURED))
            {
              /* invalid value */
	      GL_DBG_MSG(profiledbgfile,"GL_Event_Handler(),Client Descriptor/s are improperly configured\n");
              
	      retval = aci_gatt_write_response(glucose.connHandle,
                                               attrHandle, 
                                               0x01, /* write_status = 1 (error))*/ // BLE_STATUS_FAILED 
                                               RACP_RESPONSE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED,
                                               attValLen, 
                                               (uint8_t *)&(glucose.attValue));
            }
	    else
	    {
	      /* valid value */
	      GL_DBG_MSG(profiledbgfile,"GL_Event_Handler(), valid RACP value\n");
	      retval = aci_gatt_write_response(glucose.connHandle,
                                               attrHandle, 
                                               BLE_STATUS_SUCCESS, 
                                               0x00, 
                                               attValLen, 
                                               (uint8_t *)&(glucose.attValue));
              /* a RACP request has been received and sensor can start processing it */
              /* (TBR) */
	      //glucose.RACP_In_Progress_Flag = SET_FLAG; 
	    }
            if(retval == BLE_STATUS_SUCCESS)
            {
              GL_DBG_MSG(profiledbgfile,"GL_Event_Handler(),HCI_GATT_Write_Response_Cmd OK\n");
            }
            else
            {
              GL_DBG_MSG(profiledbgfile,"GL_Event_Handler(),FAILED HCI_GATT_Write_Response_Cmd %02X \n", retval);
            }       
	  }
	}
	break;
        case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
	{
          if (bnrg_expansion_board == IDB05A1) {
            evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
            GLProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
          else {
            evt_gatt_attr_modified_IDB04A1 *evt = (evt_gatt_attr_modified_IDB04A1*)blue_evt->data;
            GLProfile_Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
          }
	}
	break;
      }/* end switch(response->evtPkt.vendorSpecificEvt.vsEventCode) */
    }/* end case HCI_EVT_VENDOR_SPECIFIC */
    break;
  }/* end switch(event) */
}/* end GL_Event_Handler() */


/**
* GL_StateMachine
* 
* @param[in] None
* 
* GL profile's state machine: to be called on application main loop. 
*/ 
tBleStatus GL_StateMachine(void)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  uint8_t status;
  	
  switch(GLProfile_Read_MainStateMachine())
  {
    case GL_UNINITIALIZED:
    break;
    case GL_WAITING_BLE_INIT:
        if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE){
            /* Add GL service & characteristics */ 
            retval = GL_Add_Services_Characteristics();
            if (retval == BLE_STATUS_SUCCESS)
            {
                GL_DBG_MSG ( profiledbgfile, "GL_StateMachine(), Glucose Sensor Profile is Initialized with Service & Characteristics \n");
                
                /* vote to the main profile to go to advertise state */
                (void) BLE_Profile_Vote_For_Advertisable_State (&glucose.GLtoBLEInf);
                /* Initialization complete */
                BLE_Profile_Add_Advertisment_Service_UUID(GLUCOSE_SERVICE_UUID); 
                GLProfile_Write_MainStateMachine(GL_IDLE); 
            }
            else
            {
                GL_DBG_MSG(profiledbgfile,"GL_StateMachine(), GL_Add_Services_Characteristics() FAILED to add Glucose Service, Characteristics; Error: %02X !!\n", retval);
               
                /* Notify to application that Heart Rate profile initialization has failed */
                glucose.applcb(EVT_GL_INITIALIZED,1,&retval); 
                
                /* Change profile state to represent uninitialized profile */
                GLProfile_Write_MainStateMachine(GL_UNINITIALIZED);

                /* unregister the profile from BLE registered profile list */
                BLE_Profile_Unregister_Profile ( &glucose.GLtoBLEInf );
                /* Need to remove all added services and characteristics ??? */
            }
        }
   break; 
   
  case GL_IDLE:
    {
      //GL_DBG_MSG(profiledbgfile,"GL_IDLE\n");
      if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE)
      {
	GL_DBG_MSG(profiledbgfile,"notify the application that initialization is complete\n");
	status = BLE_STATUS_SUCCESS;
	GLProfile_Write_MainStateMachine(GL_INITIALIZED);
	glucose.applcb(EVT_GL_INITIALIZED,1,&status);
      }
    }
    break;
    
  case GL_ADV_IN_FAST_DISC_MODE_STOP:
    {
        status = GL_Sensor_Set_Discoverable(GL_LOW_PWR_ADV_INTERVAL_MIN,GL_LOW_PWR_ADV_INTERVAL_MAX);
	if(status == BLE_STATUS_SUCCESS)
	{											 
	  GLProfile_Write_MainStateMachine(GL_ADV_IN_LOW_PWR_MODE);
          GL_DBG_MSG(profiledbgfile," GL_StateMachine(),enabled advertising low power connection\n");
	}
	else
	{
	  GLProfile_Write_MainStateMachine(GL_INITIALIZED);
	  glucose.applcb(EVT_MP_ADVERTISE_ERROR,1,&status);
          GL_DBG_MSG(profiledbgfile,"GL_StateMachine(), ERROR: enabled advertising low power connection\n");
	}
    }
    break;
  case GL_UNDIRECTED_ADV_MODE_STOP:
    {        
	/* request for main profile's state change */
	BLE_Profile_Change_Advertise_Sub_State(&glucose.GLtoBLEInf,BLE_PROFILE_STATE_CONNECTABLE_IDLE);
        
	/* the advertising timeout has happened
        * inform the application
        */ 
	GLProfile_Write_MainStateMachine(GL_IDLE);
	glucose.applcb(EVT_MP_ADVERTISING_TIMEOUT,0,NULL);
    }
    break;

  case GL_CONNECTED: 
    {

      if (GLProfile_Read_SubStateMachine() == GL_CONNECTED_DO_NOTIFY) 
      {
        GLProfile_Write_SubStateMachine(GL_CONNECTED_STOP_NOTIFY); 
        GL_DBG_MSG(profiledbgfile,"------> GL_CONNECTED, HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL\n");
        /* Each glucose measurement (+ context) chacteristic notification/indication will trigger the 
        HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL event when the command has been completed with success. 
        In this context, we call the glucoseRACP_SendNextMeasurementRecord() to check if there are
        other measurement records to be sent as consequence of a RACP Report Stored Records request:
        glucoseRACP_SendNextMeasurementRecord() sends next measurement (/context) record if any 
        (return value =  TRUE). If there is a new measurement record to be sent, a new HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL
        will be triggered on glucose measurement (+ context) chacteristic notification/indication 
        */
        //response->evtPkt.commandComplete.commandOpcode = 0; 
        if (!glucoseRACP_SendNextMeasurementRecord())
	{
          /* No other RACP measurements &/o measurements context have to be sent back: */
	  /* unset flag for received RACP abort operation request */
	  glucose.RACP_Abort_Flag = UNSET_FLAG; 
	  /* unset flag for RACP operation (different from abort) already in progress */
	  glucose.RACP_In_Progress_Flag = UNSET_FLAG; //TBR 
          
          
          /* RACP procedure has been completed with success, now we can start the idle timer */
	  Blue_NRG_Timer_Start(GL_IDLE_CONNECTION_TIMER_DURATION,GL_IdleConnection_Handler,&glucose.timerID);
	}        
      }
    }
    break;
  }
  return (retval);
}/* end GL_StateMachine() */

/**
 * GL_AdvTimeout_Handler
 * 
 * advertising timeout handler
 */ 
void GL_AdvTimeout_Handler(void)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  Blue_NRG_Timer_Stop(glucose.timerID);
  glucose.timerID = 0xFF;
  
  switch(GLProfile_Read_MainStateMachine())
  {
    case GL_UNDIRECTED_ADV_MODE: 
      {        
        retval = aci_gap_set_non_discoverable(); 
        if (retval == BLE_STATUS_SUCCESS)
        {
            /* change profile's sub states */
            GLProfile_Write_MainStateMachine(GL_UNDIRECTED_ADV_MODE_STOP); 
        }
      }
      break; 
    case GL_ADV_IN_FAST_DISC_MODE:
      {
        retval = aci_gap_set_non_discoverable(); 
        if (retval == BLE_STATUS_SUCCESS)
        {
            /* change profile's sub states */
             GLProfile_Write_MainStateMachine(GL_ADV_IN_FAST_DISC_MODE_STOP); 
        }
      }
    break;
  }
}/* end GL_AdvTimeout_Handler() */


/**
 * GL_IdleConnection_Handler
 * 
 * informs the application that the connection has been
 * idle for 5 seconds. The application may disconnect the
 * link if desired
 */ 
void GL_IdleConnection_Handler(void)
{
  if(GLProfile_Read_MainStateMachine() == GL_CONNECTED)
  {
    /* for 5 seconds the connection has been idle
     * so disconnect
     * TODO : or just inform the application and the
     * application can decide whether to disconnect
     */
    Blue_NRG_Timer_Stop(glucose.timerID);
    glucose.timerID = 0xFF;
    glucose.applcb(EVT_GL_IDLE_CONNECTION_TIMEOUT,0x00,NULL);
  }
}/* end GL_IdleConnection_Handler() */

/**
 * GL_Exit
 * 
 * @param[in] errCode: reason for exit
 * 
 * informs the application for the reason
 * the initialization failed and de-registers 
 * the profile
 */ 
void GL_Exit(uint8_t errCode)
{
  GLProfile_Write_MainStateMachine(GL_UNINITIALIZED);
  glucose.applcb(EVT_GL_INITIALIZED,1,&errCode);
  BLE_Profile_Unregister_Profile(&glucose.GLtoBLEInf);
}/* end GL_Exit() */
