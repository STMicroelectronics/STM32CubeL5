/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2015 STMicroelectronics International NV
*
*   FILENAME        -  glucose_collector_racp_CB.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/07/2015
*   $Revision$:  first version
*   $Author$:    AMS - VMA, RF Application Team
*   Comments:    glucose collector callbacks x racp operation notifications, 
*                indications, errors, read, ...
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the  glucose collector racp operation callbacks handler.
* 
******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
#include <host_config.h>
#include <hci.h>
#include <debug.h>
#include <ble_list.h>
#include <timer.h>
#include <uuid.h>
#include "math.h"
#include "hal.h"

#if (BLE_CURRENT_PROFILE_ROLES & GLUCOSE_COLLECTOR)

#include <glucose_service.h>
#include <glucose_collector.h>
#include <glucose_collector_racp_CB.h>

#include <glucose_collector_racp.h>
/******************************************************************************
* Macro Declarations
******************************************************************************/

/******************************************************************************
* type definitions
******************************************************************************/

/******************************************************************************
* Variable Declarations
******************************************************************************/

/* glucose measurement characteristic */
static tGlucoseMeasurementVal GlucoseMeasurement; 
/* glucose measurement context characteristic */
static tGlucoseMeasurementContextVal GlucoseMeasurementContext;

/******************************************************************************
* Imported Variable
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/
void GL_Collector_RACP_Notifications_CB(uint16_t attr_handle, uint8_t data_lenght,uint8_t * value);


/* Convert an uint16_t to an SFLOAT */
static float get_glucose_value(uint16_t charValue)
{
  int8_t exponent; 
  int16_t mantissa;

  float gl_value = 0;
  
  /* get exponent value */
  exponent = (int8_t) (charValue >> 12);    
  /* get mantissa value */
  mantissa = (int16_t) (charValue & 0x0FFF);  
 
  if (exponent >= 0x0008) 
  {
    exponent = -((0x000F + 1) - exponent);
  }

  if ((mantissa >= RESERVED_FOR_FUTURE_USE) && (mantissa <= NEGATIVE_INFINITY_VALUE)) 
  {
    return gl_value; //TBR
  } 
  else 
  {
    if (mantissa >= 0x0800) 
    {
        mantissa = -((0x0FFF + 1) - mantissa);
    }
    gl_value = mantissa * pow(10.0f, exponent);
  }  
  
  return gl_value;
}

/*******************************************************************************
* exported CallBacks
*******************************************************************************/  

void GL_Collector_RACP_Notifications_Storage(uint16_t attr_handle, uint8_t data_length,uint8_t * value)
{
  if (glucoseCollectorContext.number_of_notifications_for_racp == glucoseCollectorContext.max_num_of_single_racp_notifications)
  {
    /* reset the storage index to 0 */
    glucoseCollectorContext.number_of_notifications_for_racp  = 0;
  }
  
  /* Store the received notification on the available storage area */
  BLUENRG_memcpy (&(glucose_collector_notifications_for_racp[glucoseCollectorContext.number_of_notifications_for_racp].data_value[0]),
               value,
               data_length);
  glucose_collector_notifications_for_racp[glucoseCollectorContext.number_of_notifications_for_racp].attr_handle = attr_handle;
  glucose_collector_notifications_for_racp[glucoseCollectorContext.number_of_notifications_for_racp].data_length = data_length;
  
  if (glucoseCollectorContext.Is_Write_Ongoing == TRUE) 
  {
    /* stop the  timer */
    Blue_NRG_Timer_Stop(glucoseCollectorContext.timerID); 
    glucoseCollectorContext.timerID = 0xFF;
    /* restart the timer */
    Blue_NRG_Timer_Start(GL_COLLECTOR_PROCEDURE_TIMEOUT,GL_Collector_ProcedureTimeoutHandler,&glucoseCollectorContext.timerID); 
  }
  glucoseCollectorContext.number_of_notifications_for_racp++;  
  
}/* end GL_Collector_RACP_Notifications_Storage */

void GL_Collector_RACP_Notifications_PostProcessing(void)
{
  /* Display and Analyse the received glcuose measurements and context notifications as consequence of the RACP procedure */
  GL_Collector_RACP_Notifications_CB(glucose_collector_notifications_for_racp[glucoseCollectorContext.racp_notifications_index].attr_handle,
                                     glucose_collector_notifications_for_racp[glucoseCollectorContext.racp_notifications_index].data_length,
                                     &(glucose_collector_notifications_for_racp[glucoseCollectorContext.racp_notifications_index].data_value[0])
                                     );

  glucoseCollectorContext.racp_notifications_index +=1;
 
}/* end GL_Collector_RACP_Notifications_PostProcessing() */

void GL_Collector_RACP_Notifications_CB(uint16_t attr_handle, uint8_t data_lenght,uint8_t * value)
{ 
  uint8_t count; 
  
  /* Check notification handle: glucose measurement characteristic notification */ 
  if (attr_handle == glucoseCollectorContext.MeasurementCharValHandle)
  {
    count = 0;
    GL_INFO_MSG(profiledbgfile,"\n");
    glucoseCollectorContext.number_of_measurement_notifications++; 
    GL_INFO_MSG(profiledbgfile,"*** Glucose Measurement: %d, (data_lenght = %d)\n",glucoseCollectorContext.number_of_measurement_notifications, data_lenght);
    /* Check consistency with previous received glucose measurement flags: if Glucose Measurement Context was expected */
    if (glucoseCollectorContext.MeasurementContext_Is_Expected)
    {
      GL_DBG_MSG(profiledbgfile,"A Glucose Measurement Context was expected! \n");
      /* reset flag */
      glucoseCollectorContext.MeasurementContext_Is_Expected = FALSE; //TBR
    }

    BLUENRG_memset(&GlucoseMeasurement, 0 , sizeof(GlucoseMeasurement)); 

    /* Display the received data */

    /* Store Glucose Measurement Flags */
    GlucoseMeasurement.flags = *value;
    value += 1;
    count += 1;

    /* Store Glucose Measurement Sequence Number */
    GlucoseMeasurement.sequenceNumber = LE_TO_HOST_16(value);
    value += 2;
    count += 2;

    /* Store Glucose Measurement Base time */
    BLUENRG_memcpy(&GlucoseMeasurement.baseTime, value, RACP_BASE_TIME_SIZE);
    value +=  RACP_BASE_TIME_SIZE; 
    count += RACP_BASE_TIME_SIZE; 
    
    GL_INFO_MSG(profiledbgfile,"flags = 0x%X, sequenceNumber = 0x%04X\n", GlucoseMeasurement.flags, GlucoseMeasurement.sequenceNumber);

    GL_INFO_MSG(profiledbgfile,"year:%d,month:%d, day:%d, hours:%d, minutes:%d, seconds:%d\n",GlucoseMeasurement.baseTime.year,GlucoseMeasurement.baseTime.month,GlucoseMeasurement.baseTime.day,GlucoseMeasurement.baseTime.hours,GlucoseMeasurement.baseTime.minutes,GlucoseMeasurement.baseTime.seconds);
   
    /* Check if a measurement context is expected */
    if (GlucoseMeasurement.flags & GLUCOSE_MEASUREMENT_FLAGS_CONTEXT_INFO_IS_PRESENT)
    {
       /* set flag for checking if next notification is a glucose measurement context */
      glucoseCollectorContext.MeasurementContext_Is_Expected = TRUE; 
    }
    /* Store Glucose Measurement Time offset */
    if (GlucoseMeasurement.flags & GLUCOSE_MEASUREMENT_FLAGS_TIME_OFFSET_IS_PRESENT)
    {
      GlucoseMeasurement.timeOffset = LE_TO_HOST_16(value);
      GL_INFO_MSG(profiledbgfile,"timeOffset =  0x%04X (%d) \n", GlucoseMeasurement.timeOffset,GlucoseMeasurement.timeOffset);
      value += 2;
      count += 2;
    }

    /* Store Glucose concentration */
    if (GlucoseMeasurement.flags & GLUCOSE_MEASUREMENT_FLAGS_CONCENTRATION_IS_PRESENT)
    {
      GlucoseMeasurement.glucoseConcentration = LE_TO_HOST_16(value);

      if (GlucoseMeasurement.flags & GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS)
      {
        GL_INFO_MSG(profiledbgfile,"Concentration (MMOL_L_UNITS) = 0x%04X (%g)\n", GlucoseMeasurement.glucoseConcentration,get_glucose_value(GlucoseMeasurement.glucoseConcentration));
      }      
      else
      {
        GL_INFO_MSG(profiledbgfile,"Concentration (MG_DL_UNITS) = 0x%04X (%g)\n", GlucoseMeasurement.glucoseConcentration,get_glucose_value(GlucoseMeasurement.glucoseConcentration));
      }
      
      value+= 2;
      count += 2;
      /* Store Type sample location*/
      GlucoseMeasurement.typeSampleLocation = *value;
      value+= 1;
      count += 1;
      GL_INFO_MSG(profiledbgfile,"typeSampleLocation = 0x%02X\n", GlucoseMeasurement.typeSampleLocation);
    }
    if (glucoseCollectorContext.glucose_feature & GLUCOSE_FEATURE_MULTIPLE_BOND_IS_SUPPORTED)
    {
      GL_INFO_MSG(profiledbgfile,"MULTIPLE BOND\n");
    }
    else 
    {
      GL_INFO_MSG(profiledbgfile,"SINGLE BOND\n");  
    }
    
    /* Store Sensor status annunciation */
    if (GlucoseMeasurement.flags & GLUCOSE_MEASUREMENT_FLAGS_STATUS_ANNUNCIATION_IS_PRESENT)
    {
      GlucoseMeasurement.sensorStatusAnnunciation = LE_TO_HOST_16(value);
      GL_INFO_MSG(profiledbgfile,"sensorStatusAnnunciation =  0x%04X\n", GlucoseMeasurement.sensorStatusAnnunciation);
   
      value += 2;
      count += 2;
    }  
    
    /* Check if additional bytes have been received within the glucose measurement characteristic value TBR */
    if (data_lenght > count)
    {
      GL_INFO_MSG(profiledbgfile,"Received additional %d bytes: ", (data_lenght-count));
      for (uint8_t i=0; i<(data_lenght-count-1); i++)
        GL_INFO_MSG(profiledbgfile,"0x%02X", *(value+i));
      GL_INFO_MSG(profiledbgfile,"0x%02X\n", *(value+(data_lenght-count-1)));
    }
    //GL_INFO_MSG(profiledbgfile,"GL_Collector_Notifications_CB(): End Glucose Measurement Notification .......................\n");
  }/* end (glucose_notification->attr_handle == MeasurementCharHandle) */
  
  /* Check notification handle: glucose measurement characteristic context notification */ 
  else if (attr_handle == glucoseCollectorContext.MeasurementContextCharValHandle)
  {    
    count = 0; 
    glucoseCollectorContext.number_of_measurement_context_notifications++;
    GL_INFO_MSG(profiledbgfile,"*** Glucose Measurement Context: %d, (data_lenght = %d)\n",glucoseCollectorContext.number_of_measurement_context_notifications, data_lenght);
    
    /* Check consistency with previous received glucose measurement flags: if Glucose Measurement Context is not expected */
    if (!(glucoseCollectorContext.MeasurementContext_Is_Expected))
    {
      GL_DBG_MSG(profiledbgfile,"A Glucose Measurement Context was not expected! \n");
    }
    else
      /* reset MeasurementContext_Is_Expected flag */
      glucoseCollectorContext.MeasurementContext_Is_Expected = FALSE;

    BLUENRG_memset(&GlucoseMeasurementContext, 0 , sizeof(GlucoseMeasurementContext));        

    /* Store Glucose Measurement Context Flags */
    GlucoseMeasurementContext.flags = *value;
    value+= 1;
    count += 1;
    /* Store Glucose Measurement Context Sequence number */
    GlucoseMeasurementContext.sequenceNumber = LE_TO_HOST_16(value);

    /* check consistency with previous received measurement sequence number */
    if (GlucoseMeasurementContext.sequenceNumber != GlucoseMeasurement.sequenceNumber)
    {
      GL_DBG_MSG(profiledbgfile,"Invalid record sequence number (measurement = 0x%04X, measurement context = 0x%04X, \n", GlucoseMeasurement.sequenceNumber, GlucoseMeasurementContext.sequenceNumber);
    }
    value += 2;
    count += 2;

    GL_INFO_MSG(profiledbgfile,"flags = 0x%02X, sequenceNumber = 0x%04X\n", GlucoseMeasurementContext.flags, GlucoseMeasurementContext.sequenceNumber);

    if (GlucoseMeasurementContext.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_EXTENDED_IS_PRESENT)
    {
       /* Store Glucose Measurement Context Extended flags */
       GlucoseMeasurementContext.extendedFlags = *value;
       value+= 1;
       count += 1;
       GL_INFO_MSG(profiledbgfile,"extendedFlags = 0x%04X\n", GlucoseMeasurementContext.extendedFlags);
    }
      
    if (GlucoseMeasurementContext.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_CARBOHYDRATE_IS_PRESENT)
    {
      /* Store Glucose Measurement Context carbohydrate ID */
      GlucoseMeasurementContext.carbohydrateId = *value;
      value+= 1;
      count += 1;
      
      /* Store Glucose Measurement Context carbohydrate Units */
      GlucoseMeasurementContext.carbohydrateUnits = LE_TO_HOST_16(value);  
      GL_INFO_MSG(profiledbgfile,"carbohydrateId = 0x%02X, carbohydrateUnits = 0x%04X (%g)\n", GlucoseMeasurementContext.carbohydrateId, GlucoseMeasurementContext.carbohydrateUnits, get_glucose_value(GlucoseMeasurementContext.carbohydrateUnits));
      value += 2;
      count += 2;
    }
      
    if (GlucoseMeasurementContext.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEAL_IS_PRESENT)
    {
      /* Store Glucose Measurement Context  Meal */
      GlucoseMeasurementContext.meal = *value;
      value+= 1;
      count += 1;
      GL_INFO_MSG(profiledbgfile,"meal = 0x%02X\n", GlucoseMeasurementContext.meal);
    }
      
    if (GlucoseMeasurementContext.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_TESTER_HEALTH_IS_PRESENT)
    {
      /* Store Glucose Measurement Context  Tester Health */
      GlucoseMeasurementContext.testerHealth = *value;
      value+= 1;
      count += 1;
      GL_INFO_MSG(profiledbgfile,"testerHealth = 0x%02X\n", GlucoseMeasurementContext.testerHealth); //TBR
      
    }
    if (GlucoseMeasurementContext.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_EXERCISE_DURATION_IS_PRESENT)
    {
      /* Store Glucose Measurement Context Duration */
      GlucoseMeasurementContext.exerciseDuration = LE_TO_HOST_16(value);    
      value += 2;
      count += 2;
      
      /* Store Glucose Measurement Context Intensity */
      GlucoseMeasurementContext.exerciseIntensity = *value;
      value+= 1;
      count += 1;
      
      GL_INFO_MSG(profiledbgfile,"exerciseDuration = 0x%04X (%d), exerciseIntensity = 0x%02X\n", GlucoseMeasurementContext.exerciseDuration, GlucoseMeasurementContext.exerciseDuration, GlucoseMeasurementContext.exerciseIntensity);
    }

    if (GlucoseMeasurementContext.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_ID_IS_PRESENT)
    {
      /* Store Glucose Measurement Context Medication ID */
      GlucoseMeasurementContext.medicationId = *value;
      value+= 1;
      count += 1;
      /* Store Glucose Measurement Context  Medication */
      GlucoseMeasurementContext.medicationUnits = LE_TO_HOST_16(value); 
      
      if (GlucoseMeasurementContext.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS)
      {
        GL_INFO_MSG(profiledbgfile,"medicationId = 0x%02X, medicationUnits(ml) = 0x%04X (%g)\n", GlucoseMeasurementContext.medicationId, GlucoseMeasurementContext.medicationUnits,get_glucose_value(GlucoseMeasurementContext.medicationUnits));
      }      
      else
      {
        GL_INFO_MSG(profiledbgfile,"medicationId = 0x%02X, medicationUnits(mg) = 0x%04X (%g)\n", GlucoseMeasurementContext.medicationId, GlucoseMeasurementContext.medicationUnits,get_glucose_value(GlucoseMeasurementContext.medicationUnits));
      }
      value += 2;
      count += 2;
    }

    if (GlucoseMeasurementContext.flags & GLUCOSE_MEASUREMENT_CONTEXT_FLAG_HB1A1C_IS_PRESENT)
    {
      /* Store Glucose Measurement Context HbA1c */
      GlucoseMeasurementContext.HbA1c = LE_TO_HOST_16(value);     
      GL_INFO_MSG(profiledbgfile,"HbA1c = 0x%04X (%g)\n", GlucoseMeasurementContext.HbA1c, get_glucose_value(GlucoseMeasurementContext.HbA1c));
      value += 2;
      count += 2;
    }
    /* Check if additional bytes have been received within the glucose measurement context characteristic value TBR */
    if (data_lenght > count)
    {
      GL_INFO_MSG(profiledbgfile,"Received additional %d bytes: ", (data_lenght-count));
      for (uint8_t i=0; i<(data_lenght-count-1); i++)
        GL_INFO_MSG(profiledbgfile,"0x%02X ", *(value+i));
      GL_INFO_MSG(profiledbgfile,"0x%02X\n", *(value+(data_lenght-count-1)));
    }
  }/* end (glucose_notification->attr_handle == MeasurementContextCharHandle) */
  
}/* end GL_Collector_Notifications_CB() */

#endif /* (BLE_CURRENT_PROFILE_ROLES & GLUCOSE_COLLECTOR) */
