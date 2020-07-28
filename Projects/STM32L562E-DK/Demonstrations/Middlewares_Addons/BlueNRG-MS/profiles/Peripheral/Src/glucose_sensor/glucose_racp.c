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
*   FILENAME        -  glucose_racp.c
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/07/2014
*   $Revision$:  first version
*   $Author$:    
*   Comments:    glucose record access point handler
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  Source code for the  glucose record access point handler.
*  This file provides a reference example for handling glucose sensor
*  database. Database record data are coming from an internal database with
*  "not real but emulated" glucose sensor data.
*  User can adapt, modify it in order to add support for a real glucose sensor
*  with related implications (Base Time values coming from a internal real-time 
*  clock or equivalent, Time Offset updated through a specific user interface, ...
*  
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
#include <ble_profile.h>
#include <ble_events.h>
#include <uuid.h>

#include <glucose_service.h>
#include <glucose_sensor.h>

#include <glucose_racp.h>

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Base Time values */
#define SECONDS_IN_MINUTE (60)
#define SECONDS_IN_HOUR   (SECONDS_IN_MINUTE * 60)
#define SECONDS_IN_DAY    (SECONDS_IN_HOUR * 24)
#define MINUTES_IN_HOUR   (60)
#define INITIAL_YEAR      (2000)

/* Days values */
#define JANUARY              (0x01)
#define FEBRUARY             (0x02)
#define MARCH                (0x03)
#define APRIL                (0x04)
#define MAY                  (0x05)
#define JUNE                 (0x06)
#define JULY                 (0x07)
#define AUGUST               (0x08)
#define SEPTEMBER            (0x09)
#define OCTOBER              (0x0A)
#define NOVEMBER             (0x0B)
#define DECEMBER             (0x0C)
/* Number of days values */
#define FEBRUARY_NORMAL_DAYS (28)
#define FEBRUARY_LEAP_DAYS   (29)
#define EAVEN_MONTH_DAYS     (30)
#define ODD_MONTH_DAYS       (31) 
#define NORMAL_YEAR_DAYS     (365)
#define LEAP_YEAR_DAYS       (366)
/* is a leap year (366 days) or not (365 days) */
#define	IS_LEAP_YEAR(year)    (!((year) % 400) ||               \
			       (((year) % 100) && !((year) % 4)))

/******************************************************************************
* type definitions
******************************************************************************/

/* typedef used for storing the specific filter type to be used */
typedef union _tfilterType{
  uint16_t sequence_number;
  tBasetime user_facing_time;
}tfilterType;

/* input filter values (sequence numbers or user facing times) */
static tfilterType racp_filter_1;
static tfilterType racp_filter_2;
static uint8_t racpval[RACP_RESPONSE_LENGHT];

/******************************************************************************
* Variable Declarations
******************************************************************************/

/******************************************************************************
* Imported Variable
******************************************************************************/
extern void *profiledbgfile;

/******************************************************************************
 * Function Prototypes
******************************************************************************/
/**
  * @brief Get the number of seconds from a baseTime time value
  *        (it counts since INITIAL_YEAR)
  * @param input_time: baseTime time value  
  * @retval number of seconds 
  */
static uint32_t glucose_GetSecondsFrom_UserFacingTime(tBasetime input_time)
{
  uint16_t total_days = input_time.day;/* number of day on input month */
  uint16_t total_years; 
  uint8_t total_month; 

  /* Calculate how many days related to all previous years 
     (current years day are not counted here) */
  for (total_years = (input_time.year - 1); total_years >= INITIAL_YEAR; total_years--)
  {
    /* Add number of day for "total_years" year */
    total_days += IS_LEAP_YEAR(total_years) ? LEAP_YEAR_DAYS: NORMAL_YEAR_DAYS;
  }

  /* Calculate how many days related to months on current input year.
     (days on current input month are not counted here) */
  for (total_month = (input_time.month - 1); total_month > 0; total_month--)
  {
    switch (total_month)
    {
      case JANUARY:
      case MARCH:
      case MAY:
      case JULY:
      case AUGUST:
      case OCTOBER:
      	total_days += ODD_MONTH_DAYS;
      break; 
      case APRIL:
      case JUNE:
      case NOVEMBER:
      case SEPTEMBER:
	total_days += EAVEN_MONTH_DAYS;
      break; 
      case FEBRUARY:
	total_days += IS_LEAP_YEAR(input_time.year) ? FEBRUARY_LEAP_DAYS: FEBRUARY_NORMAL_DAYS;
      break;
    }
  }
  /* return overall seconds as follow:
   *
   * input_time.seconds  --> number of input seconds 
   * (input_time.minutes + (input_time.hours * MINUTES_IN_HOUR))* SECONDS_IN_MINUTE --> input minutes & hours converted in seconds
   * total_days * SECONDS_IN_DAY --> total days coming from previous years days + days in months in current year + input days 
   */
  return (input_time.seconds + ((input_time.minutes + (input_time.hours * MINUTES_IN_HOUR))* SECONDS_IN_MINUTE) + (total_days * SECONDS_IN_DAY));
}

/**
  * @brief Send a RACP response as consequence of a RACP request
  * @param op_code:   RACP request op code  
  * @param response: RACP response op code or number of stored records
  * @retval none
  */
static void glucoseRACP_SendResponse(uint8_t op_code, uint16_t response_code)
{
  tBleStatus retval = BLE_STATUS_FAILED; 

  if (op_code == RACP_REPORT_NUMBER_STORED_RECORDS_OP_CODE)
  {
    /* Send the number of stored records */
    racpval[0] = RACP_NUMBER_STORED_RECORDS_RESPONSE_OP_CODE;
    racpval[1] = RACP_OPERATOR_NULL;
    STORE_LE_16(&racpval[2], response_code); /* number of stored records */
    //racpval[2] = response_code;
    //racpval[3] = 0x0;
  }
  else
  {
    /* Send the response to the received RACP request */
    racpval[0] = RACP_RESPONSE_OP_CODE;
    racpval[1] = RACP_OPERATOR_NULL;
    racpval[2] = op_code;
    racpval[3] = (uint8_t) response_code;
  }

  /* RACP response is indicated */
  retval = aci_gatt_update_char_value(glucose.glServiceHandle,
                                      glucose.RACPCharHandle, 
                                      0,
                                      RACP_RESPONSE_LENGHT, 
				      (uint8_t *) racpval);
  if(retval == BLE_STATUS_SUCCESS)
  {
    GL_DBG_MSG(profiledbgfile,"RACP response sent successfully opcode: 0x%X, response_code: 0x%X\n",racpval[2],racpval[3]);
    /* unset flag for RACP operation (different from abort) already in progress */
    glucose.RACP_In_Progress_Flag = UNSET_FLAG; //TBR 
  }
  else 
  {
    GL_DBG_MSG(profiledbgfile,"FAILED to send RACP response %02X \n", retval);
  }
}/* end glucoseRACP_SendResponse()*/

/**
  * @brief  Set/Unset the database record status flag
  *         for the specific RACP procedure
  * @param  index:                        record index on database records 
  * @param  record_status_flag:           value of the status flag to be used
  * @param  record_status_flag_operation: operation to be done (set or clear) with the record_status_flag
  * @retval none 
  */
static void glucoseDataBase_SetFlag(uint16_t index,uint8_t record_status_flag,BOOL record_status_flag_operation)
{
   /* Check if record_status_flag has to be set */
   if(record_status_flag_operation)
   {
     /* Set (value = 1) record_status_flag */
     (glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag |= record_status_flag;
   }
   else
   {
     /* Unset (value = 0) record_status_flag */
     (glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag &= ~record_status_flag;
   }
}/* end glucoseDataBase_SetFlag() */


/**
  * @brief  Set the database record status flag based on
  *         specific RACP procedure
  * @param  racp_operator:                RACP operator 
  * @param  filter_type:                  RACP filter type   
  * @param  filter_parameter_1:           filter parameter 1 (NULL if not needed)
  * @param  filter_parameter_1:           filter parameter 2 (NULL if not needed)
  * @param  record_status_flag:           value of the status flag to be used
  * @param  record_status_flag_operation: operation to be done (set or clear) with the record_status_flag
  * @retval number of not deleted records 
  */
static uint16_t glucoseDatabase_RecordsSelection(uint8_t racp_operator,
                                                 uint8_t filter_type, 
                                                 tfilterType* filter_parameter_1,
                                                 tfilterType* filter_parameter_2,
                                                 uint8_t record_status_flag, 
                                                 BOOL record_status_flag_operation)
{
   uint16_t index;
   /* number of database record not deleted */
   uint16_t number_of_not_deleted_records = 0;
   uint32_t filter_parameter_1_seconds = 0;
   uint32_t filter_parameter_2_seconds = 0;
   uint32_t record_time_seconds = 0;

   if (racp_operator == RACP_OPERATOR_LAST_RECORD)
   {
     for (index = glucose.max_number_of_used_records; index > 0; index--)
     {
       /* look for last not deleted database record */
       if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index -1)->record_status_flag & GLUCOSE_RECORD_NOT_DELETED_FROM_RACP)
       {
	 /* Select the last not deleted database record */
	 glucoseDataBase_SetFlag(index - 1,record_status_flag,record_status_flag_operation);
	 number_of_not_deleted_records = 1;
	 /* stop for loop */
	 break; 
       }
     }
   }
   else
   {
     /* set the time filter values to be used for filtering */
     if (filter_type == RACP_FILTER_TYPE_USER_FACING_TIME)
     {
       switch(racp_operator)
       {
         case RACP_OPERATOR_LESS_EQUAL:
         case RACP_OPERATOR_GREATER_EQUAL:
	   /* convert input filter_parameter_1->user_facing_time to seconds */
	   filter_parameter_1_seconds = glucose_GetSecondsFrom_UserFacingTime(filter_parameter_1->user_facing_time);
	 break; 
         case RACP_OPERATOR_WITHIN_RANGE:
	   /* convert input filter_parameter_2->user_facing_time & input filter_parameter_2->user_facing_time to seconds */
	   filter_parameter_1_seconds = glucose_GetSecondsFrom_UserFacingTime(filter_parameter_1->user_facing_time);
	   filter_parameter_2_seconds = glucose_GetSecondsFrom_UserFacingTime(filter_parameter_2->user_facing_time);
	 break;
       }
     }
     for(index = 0; index < glucose.max_number_of_used_records; index++)
     {
       switch(racp_operator)
       {
          case RACP_OPERATOR_ALL:
	    /* look for all not deleted database records */
            if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag & GLUCOSE_RECORD_NOT_DELETED_FROM_RACP)
	    {
	      number_of_not_deleted_records +=1;
	    }
	    glucoseDataBase_SetFlag(index,record_status_flag,record_status_flag_operation);
          break;
          case RACP_OPERATOR_LESS_EQUAL:
	    if (filter_type == RACP_FILTER_TYPE_SEQUENCE_NUMBER)
	    {
	      /* check if input filter is valid for record at index position */
              if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->sequenceNumber <= filter_parameter_1->sequence_number)
	      {
                if((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag & GLUCOSE_RECORD_NOT_DELETED_FROM_RACP)
		{
		  number_of_not_deleted_records +=1;
		}
		glucoseDataBase_SetFlag(index,record_status_flag,record_status_flag_operation);
	      }
	    }
	    else if (filter_type == RACP_FILTER_TYPE_USER_FACING_TIME)
	    {  
	      /* convert record measurement baseTime + timeOffset to seconds */            
              record_time_seconds = glucose_GetSecondsFrom_UserFacingTime((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->baseTime) + (glucose.glucoseDatabase_MeasurementRecords_ptr + index)->timeOffset * SECONDS_IN_MINUTE; /* timeOffset is in minutes */
              /* check if input filter is valid for record at index position */
	      if (record_time_seconds <= filter_parameter_1_seconds)
	      {
                if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag & GLUCOSE_RECORD_NOT_DELETED_FROM_RACP)
		{
		  number_of_not_deleted_records +=1;
		}
		glucoseDataBase_SetFlag(index,record_status_flag,record_status_flag_operation);
	      }
	    }
          break;
          case RACP_OPERATOR_GREATER_EQUAL:
	    if (filter_type == RACP_FILTER_TYPE_SEQUENCE_NUMBER)
	    {
              if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)-> sequenceNumber >= filter_parameter_1->sequence_number)
	      {
		/* check if input filter is valid for record at index position */
                if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag & GLUCOSE_RECORD_NOT_DELETED_FROM_RACP)
		{
		  number_of_not_deleted_records +=1;
		}
		glucoseDataBase_SetFlag(index,record_status_flag,record_status_flag_operation);
	      }
	    }
	    else if (filter_type == RACP_FILTER_TYPE_USER_FACING_TIME)
	    {
	      /* convert record measurement baseTime + timeOffset to seconds */
              record_time_seconds = glucose_GetSecondsFrom_UserFacingTime((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->baseTime) + (glucose.glucoseDatabase_MeasurementRecords_ptr + index)->timeOffset * SECONDS_IN_MINUTE; /* timeOffset is in minutes */
            
              
              /* check if input filter is valid for record at index position */
	      if (record_time_seconds >= filter_parameter_1_seconds)
	      {
                if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag & GLUCOSE_RECORD_NOT_DELETED_FROM_RACP)
		{
		  number_of_not_deleted_records +=1;
		}
		glucoseDataBase_SetFlag(index,record_status_flag,record_status_flag_operation);
	      }
	    }
	  break;
          case RACP_OPERATOR_WITHIN_RANGE:
	    if (filter_type == RACP_FILTER_TYPE_SEQUENCE_NUMBER)
	    {
	      /* check if input filter is valid for record at index position */
              if (((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->sequenceNumber >= filter_parameter_1->sequence_number) && 
		  ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->sequenceNumber <= filter_parameter_2->sequence_number))
	      {
                if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag & GLUCOSE_RECORD_NOT_DELETED_FROM_RACP)
		{
		  number_of_not_deleted_records +=1;
		}
		glucoseDataBase_SetFlag(index,record_status_flag,record_status_flag_operation);
	      }
	    }
	    else if (filter_type == RACP_FILTER_TYPE_USER_FACING_TIME)
	    {
	      /* convert record measurement baseTime + timeOffset to seconds */             
              record_time_seconds = glucose_GetSecondsFrom_UserFacingTime((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->baseTime) + (glucose.glucoseDatabase_MeasurementRecords_ptr + index)->timeOffset * SECONDS_IN_MINUTE; /* timeOffset is in minutes */
              /* check if input filters are valid for record at index position */
	      if ((record_time_seconds >= filter_parameter_1_seconds) && (record_time_seconds <= filter_parameter_2_seconds))
	      {
                if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag & GLUCOSE_RECORD_NOT_DELETED_FROM_RACP)
		{
		  number_of_not_deleted_records +=1;
		}
		glucoseDataBase_SetFlag(index,record_status_flag,record_status_flag_operation);
	      }
	    }	 
	  break;
          case RACP_OPERATOR_FIRST_RECORD: 
            if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag & GLUCOSE_RECORD_NOT_DELETED_FROM_RACP)
	    {
              number_of_not_deleted_records +=1;
              glucoseDataBase_SetFlag(index,record_status_flag,record_status_flag_operation);
              /* stop for loop */
	      index = glucose.max_number_of_used_records;
	    }
          break;
          default:
          break;
         }/* end switch() */
      }/* end for() */
   }/* end else {} */
   return (number_of_not_deleted_records);
}/* end glucoseDatabase_RecordsSelection() */

/**
  * @brief Manage requested RACP Op Codes & associated operators
  * @param op_code:            RACP Op Code
  * @param racp_operator:      RACP Operator
  * @param filter_type:        RACP Filter Type 
  * @param filter_parameter_1: RACP Filter type parameter (if needed)
  * @param filter_parameter_2: RACP Filter type parameter (if needed)
  * @retval none
  */
static void glucoseRACP_ManageRequestedOpCode(uint8_t op_code, 
					      uint8_t racp_operator, 
					      uint8_t filter_type, 
					      tfilterType * filter_parameter_1, 
					      tfilterType * filter_parameter_2)
{
  uint16_t count = 0; 
  /* reset abort flag on reception of a new RACP request */
  glucose.RACP_Abort_Flag = UNSET_FLAG; /* TBR:is it correct here? */
  
  switch(op_code)
  {
     case RACP_REPORT_STORED_RECORDS_OP_CODE:
       /* set in progress flag on reception of a new RACP request */
       glucose.RACP_In_Progress_Flag = SET_FLAG;
       /* Reset the database record status flag for the new op code (only not deleted records are considered) */ 
       glucoseDatabase_ResetAllNotDeletedRecords(SET_FLAG);
       /* Identify all the not deleted database records (if any) to be notified 
          according to the requested op code, and set related flag status GLUCOSE_RECORD_FILTERED_FROM_RACP 
          since it satisfy the requested op code filter types */
       count = glucoseDatabase_RecordsSelection(racp_operator, 
                                                filter_type, 
                                                filter_parameter_1,
                                                filter_parameter_2,
                                                GLUCOSE_RECORD_FILTERED_FROM_RACP, 
                                                SET_FLAG);
       if (count > 0)
       {
	 /* Identify first selected record on database */
	 glucose.RACP_NextMeasurementRecord_Index = glucoseDataBase_FindNextSelectedRecord(0);
         /* there is at lease one measurement record to be sent: set related glucose context flag */
	 glucose.RACP_SendNextMeasurementRecord_Flag = (count == 1) ? UNSET_FLAG : SET_FLAG;
	 /* send next measurement record value: first selected one */
         
         /* TBR ??? */
         //GL_DBG_MSG(profiledbgfile,"glucoseRACP_ManageRequestedOpCode(), ---> glucose.RACP_NextMeasurementRecord_Index = %x, glucoseMeasurementVal.sequenceNumber = %x,\n",glucose.RACP_NextMeasurementRecord_Index,(glucose.glucoseDatabase_MeasurementRecords_ptr + glucose.RACP_NextMeasurementRecord_Index)->sequenceNumber ); 
         
         GL_Send_Glucose_Measurement(*(glucose.glucoseDatabase_MeasurementRecords_ptr + glucose.RACP_NextMeasurementRecord_Index)); 
       }
       else
       {
         /* return request op_code and response code value: no records found */
	 glucoseRACP_SendResponse(op_code, RACP_RESPONSE_NO_RECORDS);
       }
     break;

     case RACP_DELETE_STORED_RECORDS_OP_CODE:
       /* set in progress flag on reception of a new RACP request */
       glucose.RACP_In_Progress_Flag = SET_FLAG;
        /* Reset the database record status flag for the new op code (only not deleted records are considered) */ 
       glucoseDatabase_ResetAllNotDeletedRecords(SET_FLAG);
        /* Identify all the not deleted database records (if any) to be deleted
           according to the requested op code, and unset both flag status:
           GLUCOSE_RECORD_NOT_DELETED_FROM_RACP (since it is deleted) and GLUCOSE_RECORD_FILTERED_FROM_RACP 
           since it is not selected anymore */
       if (glucoseDatabase_RecordsSelection(racp_operator,
                                             filter_type, 
					     filter_parameter_1,
					     filter_parameter_2,
					     GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED, 
					     UNSET_FLAG) > 0)
       {
         /* return request op code and response code value: success operation */
	 glucoseRACP_SendResponse(op_code, RACP_RESPONSE_SUCCESS);
         
         /* unset flag for RACP operation (different from abort) already in progress */
	  glucose.RACP_In_Progress_Flag = UNSET_FLAG; /* TBR */
       }
       else
       {
	 /* return request op code and response code value: no records found */
	 glucoseRACP_SendResponse(op_code, RACP_RESPONSE_NO_RECORDS);
         /* unset flag for RACP operation (different from abort) already in progress */
	  glucose.RACP_In_Progress_Flag = UNSET_FLAG; /* TBR */
       }
     break;

     case RACP_ABORT_OPERATION_OP_CODE:
       /* Stop Timer for glucose measurement (+ context)  notification as requested by 
          received RACP request */ 
       /* Any current RACP has to be stopped */
       glucose.RACP_Abort_Flag = SET_FLAG;
       /* Reset global flag for sending glucose measurements (&/o context) records */
       glucose.RACP_SendNextMeasurementRecord_Flag = UNSET_FLAG;
       glucose.RACP_SendNextMeasurementContextRecord_Flag = UNSET_FLAG;
       /* return request op code and response code value: success operation */
       glucoseRACP_SendResponse(op_code, RACP_RESPONSE_SUCCESS);
     break;

     case RACP_REPORT_NUMBER_STORED_RECORDS_OP_CODE:
     {
       uint16_t count = 0; 
       /* set in progress flag on reception of a new RACP request */
       glucose.RACP_In_Progress_Flag = SET_FLAG;
       /* Reset the database record status flag for the new op code (only not deleted records are considered) */ 
       glucoseDatabase_ResetAllNotDeletedRecords(SET_FLAG);
      
       /* Identify all the not deleted database records (if any) according to the requested op code, 
          and set related flag status GLUCOSE_RECORD_FILTERED_FROM_RACP 
          since it satisfy the requested op code filter types */
       count = glucoseDatabase_RecordsSelection(racp_operator, 
						filter_type, 
						filter_parameter_1,
						filter_parameter_2,
						GLUCOSE_RECORD_FILTERED_FROM_RACP, 
						SET_FLAG);
       /* return number of stored records response */
       glucoseRACP_SendResponse(op_code, count);
     }
     break;

     default:
     break;
  }/* end switch */
}/* end glucoseRACP_ManageRequestedOpCode() */


/**
  * @brief RACP basic check on requested RACP
  *         op code & operator combination
  * @param op_code: RACP op code
  * @param racp_operator: RACP operator
  * @retval TRUE: if (op_code, racp_operator) combination is correct, FALSE otherwise.
  */
static BOOL glucoseRACP_CheckOperatorIsCorrect(uint8_t op_code, uint8_t racp_operator)
{
  BOOL op_status = TRUE; 
  switch(op_code)
  {
    case RACP_REPORT_STORED_RECORDS_OP_CODE:
    case RACP_DELETE_STORED_RECORDS_OP_CODE:
    case RACP_REPORT_NUMBER_STORED_RECORDS_OP_CODE:
    if(racp_operator == RACP_OPERATOR_NULL)
    {
      glucoseRACP_SendResponse(op_code,RACP_RESPONSE_INVALID_OPERATOR);
      op_status = FALSE; 
    }
    break;
    case RACP_ABORT_OPERATION_OP_CODE:
      if(racp_operator!= RACP_OPERATOR_NULL)
      {
	glucoseRACP_SendResponse(op_code,RACP_RESPONSE_INVALID_OPERATOR);
        op_status = FALSE; 
      }
    break;
    default:
      /* racp op_code is not supported */
      glucoseRACP_SendResponse(op_code,RACP_RESPONSE_OPCODE_NOT_SUPPORTED);
      op_status = FALSE; 
    break;
  }
  return(op_status);
} /* end glucoseRACP_CheckOperatorIsCorrect()*/

/*******************************************************************************
* exported SAP
*******************************************************************************/

/**
  * @brief Manage the notification of next glucose measurment from database records 
  *        It should be called on HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL event, GL_CONNECTED state.
  * @retval TRUE: if other selected measurement has to be sent; false otherwise
  */
BOOL glucoseRACP_SendNextMeasurementRecord(void)
{
  /* Check if measurement context has to be sent for current selected measurement record  (TBR: --> check to be changed?) */
  if ((glucose.RACP_SendNextMeasurementContextRecord_Flag) && (!glucose.RACP_Abort_Flag))
  {
    /* unset flag for sending next measurement context record */
    glucose.RACP_SendNextMeasurementContextRecord_Flag = UNSET_FLAG; 
    /* send next  measurement context record */
    GL_Send_Glucose_Measurement_Context(*(glucose.glucoseDataBase_MeasurementContextRecords_ptr + glucose.RACP_NextMeasurementRecord_Index));
    return (TRUE);
  }
  /* chek if another measurement context has to be sent  as consequence of a report stored RACP request */
  else if((glucose.RACP_SendNextMeasurementRecord_Flag) && (!glucose.RACP_Abort_Flag))
  {
    glucose.RACP_NextMeasurementRecord_Index = glucoseDataBase_FindNextSelectedRecord(glucose.RACP_NextMeasurementRecord_Index + 1);

    if (glucose.RACP_NextMeasurementRecord_Index == glucose.max_number_of_used_records)
    {
      /* no other measurement record to be sent */
      glucose.RACP_SendNextMeasurementRecord_Flag = UNSET_FLAG;
      /* reset index for next measurement record to be sent */
      glucose.RACP_NextMeasurementRecord_Index = 0; 
      /* return request op code: RACP_REPORT_STORED_RECORDS_OP_CODE and response code value: success operation */
      glucoseRACP_SendResponse(RACP_REPORT_STORED_RECORDS_OP_CODE, RACP_RESPONSE_SUCCESS);
      return (FALSE);
    }
    else
    {
      /* send next glucose measurement value */
      GL_Send_Glucose_Measurement(*(glucose.glucoseDatabase_MeasurementRecords_ptr + glucose.RACP_NextMeasurementRecord_Index)); 
      return (TRUE);
    }
  }
  else if ((!glucose.RACP_SendNextMeasurementContextRecord_Flag) && (glucose.RACP_In_Progress_Flag) && (!glucose.RACP_Abort_Flag)) /* (TBR: Is it correct ?)*/
  {
    /* only one measurement has to be sent: return RACP response */
    /* reset index for next measurement record to be sent */
    glucose.RACP_NextMeasurementRecord_Index = 0; 
    /* return request op code: RACP_REPORT_STORED_RECORDS_OP_CODE and response code value: success operation */
    glucoseRACP_SendResponse(RACP_REPORT_STORED_RECORDS_OP_CODE, RACP_RESPONSE_SUCCESS);
    return (FALSE);
  }
  return (FALSE);
}/* end glucoseRACP_SendNextMeasurementRecord() */

/**
  * @brief  Reset all the not deleted records to  non filtered status
  *         before doing a new operation.
  * @param  action: TRUE (reset flag); FALSE (set flag)
  * @retval number of not deleted records 
  */
uint16_t glucoseDatabase_ResetAllNotDeletedRecords(BOOL action)
{
  uint16_t index;
  uint16_t number_of_valid_records = 0;

  for(index = 0; index < glucose.max_number_of_used_records; index++)
  {
    if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag & GLUCOSE_RECORD_NOT_DELETED_FROM_RACP)
    {
      number_of_valid_records +=1;
      /* reset the GLUCOSE_RECORD_FILTERED_FROM_RACP flag status for a new operation */
      if (action)
        (glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag &= ~GLUCOSE_RECORD_FILTERED_FROM_RACP;
      else
        (glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag |= GLUCOSE_RECORD_FILTERED_FROM_RACP;
    }
  }
  return (number_of_valid_records);
}/* end glucoseDataBase_ResetAllNotDeletedRecords() */

/**
  * @brief  Count number of not delelted records on database
  * @retval number of not deleted records 
  */
uint16_t  glucoseDatabase_NumNotDeletedRecords(void)
{
  uint16_t index;
  uint16_t count = 0; 
  
  for(index = 0; index < glucose.max_number_of_used_records; index++)
  {
    if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag & GLUCOSE_RECORD_NOT_DELETED_FROM_RACP)
    {
      count += 1; 
    }
  }
  return (count);
}/* end glucoseDatabase_NumNotDeletedRecords() */

/**
  * @brief  Find next filtered record on glucose database
  * @param  num: index from which starting the search
  * @retval database index
  */
uint16_t glucoseDataBase_FindNextSelectedRecord(uint16_t database_index)
{
  uint16_t index;

  if(database_index >= glucose.max_number_of_used_records)
    return (glucose.max_number_of_used_records);

  for(index = database_index; index < glucose.max_number_of_used_records; index++)
  {
    if ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag == GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED) //GLUCOSE_RECORD_NOT_DELETED_FROM_RACP 
    {
      break;
    }
  }
  return (index);
}/* end glucoseDataBase_FindNextSelectedRecord() */ 

/**
  * @brief RACP request handler 
  * @param attValueLen: received RACP request lenght
  * @retval none
  */
void glucoseRACP_RequestHandler(uint8_t attValueLen)
{
  uint8_t op_code = glucose.attValue[RACP_OP_CODE_POSITION];
  uint8_t racp_operator = glucose.attValue[RACP_OPERATOR_POSITION];
       
  GL_DBG_MSG(profiledbgfile,"attValueLen:  %x, op_code: %x, racp_operator: %x\n",attValueLen,op_code,racp_operator);
  /* perform basic check on op_code,racp_operator combination: is it allowed ?*/
  if(glucoseRACP_CheckOperatorIsCorrect(op_code,racp_operator))
  {
    switch(racp_operator)
    {
      case RACP_OPERATOR_NULL:
      case RACP_OPERATOR_ALL:
      case RACP_OPERATOR_FIRST_RECORD:
      case RACP_OPERATOR_LAST_RECORD:
	if(attValueLen == RACP_FILTER_TYPE_NO_OPERAND_LENGHT)
	{
          glucoseRACP_ManageRequestedOpCode(op_code, 
					    racp_operator,
					    RACP_FILTER_TYPE_NONE,
					    NULL,
					    NULL);
	}
	else
	{
	  /* No operand is expected */ 
	  glucoseRACP_SendResponse(op_code,RACP_RESPONSE_INVALID_OPERAND);
	}
      break;
      case RACP_OPERATOR_LESS_EQUAL:
      case RACP_OPERATOR_GREATER_EQUAL:
	/* filter type = attValue[RACP_FILTER_TYPE_POSITION] */
	if ((glucose.attValue[RACP_FILTER_TYPE_POSITION] == RACP_FILTER_TYPE_SEQUENCE_NUMBER) && (attValueLen == RACP_FILTER_TYPE_LESS_GREATER_SEQUENCE_NUMBER_LENGHT))
	{
	  /* set filter sequence number 1 */
          BLUENRG_memcpy((void *) &(racp_filter_1.sequence_number), 
		      (void *) &(glucose.attValue[RACP_FILTER_TYPE_POSITION+1]),
		      2);
	  glucoseRACP_ManageRequestedOpCode(op_code, 
					    racp_operator,
					    RACP_FILTER_TYPE_SEQUENCE_NUMBER,
					    &racp_filter_1,
					    NULL);
	}
	else if ((glucose.attValue[RACP_FILTER_TYPE_POSITION] == RACP_FILTER_TYPE_USER_FACING_TIME) && (attValueLen == RACP_FILTER_TYPE_LESS_GREATER_USER_FACING_TIME_LENGHT))
	{
	  /* set filter user facing time 1*/
          BLUENRG_memcpy((void *) &(racp_filter_1.user_facing_time.year), 
		      (void *) &(glucose.attValue[RACP_FILTER_TYPE_POSITION+1]),
		      2);
	  racp_filter_1.user_facing_time.month = glucose.attValue[RACP_FILTER_TYPE_POSITION + 3];
	  racp_filter_1.user_facing_time.day = glucose.attValue[RACP_FILTER_TYPE_POSITION + 4];
	  racp_filter_1.user_facing_time.hours = glucose.attValue[RACP_FILTER_TYPE_POSITION + 5];
	  racp_filter_1.user_facing_time.minutes = glucose.attValue[RACP_FILTER_TYPE_POSITION + 6];
	  racp_filter_1.user_facing_time.seconds = glucose.attValue[RACP_FILTER_TYPE_POSITION + 7];
          glucoseRACP_ManageRequestedOpCode(op_code, 
					    racp_operator,
					    RACP_FILTER_TYPE_USER_FACING_TIME,
					    &racp_filter_1,
					    NULL);
	}
	else 
	{
	  /* filter type is not supported: TBR -->as RACP_RESPONSE_FILTER_TYPE_NOT_SUPPORTED */
	  glucoseRACP_SendResponse(op_code,RACP_RESPONSE_OPERAND_NOT_SUPPORTED);
	}
      break;   
      case RACP_OPERATOR_WITHIN_RANGE:
	/* filter type = attValue[RACP_FILTER_TYPE_POSITION] */
	if ((glucose.attValue[RACP_FILTER_TYPE_POSITION] == RACP_FILTER_TYPE_SEQUENCE_NUMBER) && (attValueLen == RACP_FILTER_TYPE_WITHIN_RANGE_SEQUENCE_NUMBER_LENGHT))
	{
	  /* set filter sequence number 1 & 2 */
          BLUENRG_memcpy((void *) &(racp_filter_1.sequence_number), 
                      (void *) &(glucose.attValue[RACP_FILTER_TYPE_POSITION+1]),
		      2);
          BLUENRG_memcpy((void *) &(racp_filter_2.sequence_number), 
		      (void *) &(glucose.attValue[RACP_FILTER_TYPE_POSITION+3]),
		      2);
	  if (racp_filter_1.sequence_number <= racp_filter_2.sequence_number)
	  {
	    glucoseRACP_ManageRequestedOpCode(op_code, 
					      racp_operator,
					      RACP_FILTER_TYPE_SEQUENCE_NUMBER,
					      &racp_filter_1,
					      &racp_filter_2);
	  }
	  else
	  {
	    /* Operand are invalid since sequence_number_1 > sequence_number_2 */ 
	    glucoseRACP_SendResponse(op_code,RACP_RESPONSE_INVALID_OPERAND);
	  }
	}
        else if ((glucose.attValue[RACP_FILTER_TYPE_POSITION] == RACP_FILTER_TYPE_USER_FACING_TIME) && (attValueLen == RACP_FILTER_TYPE_WITHIN_RANGE_USER_FACING_TIME_LENGHT))
	{
	  /* set filter user facing time 1*/
          BLUENRG_memcpy((void *) &(racp_filter_1.user_facing_time.year), 
		      (void *) &(glucose.attValue[RACP_FILTER_TYPE_POSITION+1]),
		      2);
	  racp_filter_1.user_facing_time.month = glucose.attValue[RACP_FILTER_TYPE_POSITION + 3];
	  racp_filter_1.user_facing_time.day = glucose.attValue[RACP_FILTER_TYPE_POSITION + 4];
	  racp_filter_1.user_facing_time.hours = glucose.attValue[RACP_FILTER_TYPE_POSITION + 5];
	  racp_filter_1.user_facing_time.minutes = glucose.attValue[RACP_FILTER_TYPE_POSITION + 6];
	  racp_filter_1.user_facing_time.seconds = glucose.attValue[RACP_FILTER_TYPE_POSITION + 7];

	   /* set filter user facing time 2*/
          BLUENRG_memcpy((void *) &(racp_filter_2.user_facing_time.year), 
		      (void *) &(glucose.attValue[RACP_FILTER_TYPE_POSITION+8]),
		      2);
	  racp_filter_2.user_facing_time.month = glucose.attValue[RACP_FILTER_TYPE_POSITION + 10];
	  racp_filter_2.user_facing_time.day = glucose.attValue[RACP_FILTER_TYPE_POSITION + 11];
	  racp_filter_2.user_facing_time.hours = glucose.attValue[RACP_FILTER_TYPE_POSITION + 12];
	  racp_filter_2.user_facing_time.minutes = glucose.attValue[RACP_FILTER_TYPE_POSITION + 13];
	  racp_filter_2.user_facing_time.seconds = glucose.attValue[RACP_FILTER_TYPE_POSITION + 14];
          glucoseRACP_ManageRequestedOpCode(op_code, 
					    racp_operator,
					    RACP_FILTER_TYPE_USER_FACING_TIME,
					    &racp_filter_1,
					    &racp_filter_2);
	}
	else
	{
	  /* Operand is invalid */ 
	  //glucoseRACP_SendResponse(op_code,RACP_RESPONSE_INVALID_OPERAND);
          /* filter type is not supported: TBR--> as RACP_RESPONSE_FILTER_TYPE_NOT_SUPPORTED */
	  glucoseRACP_SendResponse(op_code,RACP_RESPONSE_OPERAND_NOT_SUPPORTED);
	}
	break;
        default:
	  /* racp_operator is not supported */
	  glucoseRACP_SendResponse(op_code,RACP_RESPONSE_OPERATOR_NOT_SUPPORTED);
	break;
    }
  }
}/* end glucoseRACP_RequestHandler() */
          
/*******************************************************************************
* exported test utilities
*******************************************************************************/

/**
  * @brief  Change current database sequence numbers and restore status to not deleted
  *         to simulate new measurements on user initiative
  *         (only for internal test purposes) (TBR).
  * @param  num_record: number of records to be restored
*         flag_status: flag status
  * @retval none
  */ 
void glucoseDatabase_Change_Database_Manually(uint16_t num_record, uint8_t flag_status) 
{
  uint16_t index;
  for (index =0; index < num_record; index++)
  {
    /* Increase overall sequence number of stored glucose measurement records on database */
    glucose.sequenceNumber +=1;
    /* Set new sequence number on both arrays of measurements */
    (glucose.glucoseDatabase_MeasurementRecords_ptr + index)->sequenceNumber = glucose.sequenceNumber;
    (glucose.glucoseDataBase_MeasurementContextRecords_ptr + index)->sequenceNumber = glucose.sequenceNumber;
    
    if (flag_status)
      /* set measurement record as not deleted */
      (glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag |= GLUCOSE_RECORD_NOT_DELETED_FROM_RACP;
    else
      /* set measurement record as deleted */
      (glucose.glucoseDatabase_MeasurementRecords_ptr + index)->record_status_flag &= ~GLUCOSE_RECORD_NOT_DELETED_FROM_RACP;
  }
  /* reset index for next measurement record to be sent */
  glucose.RACP_NextMeasurementRecord_Index = 0; /* TBR */
}/* end glucoseDatabase_Change_Database_Manually() */

uint16_t GL_Debug_Get_DB_SequenceNumber(uint16_t index)
{
  return ((glucose.glucoseDatabase_MeasurementRecords_ptr + index)->sequenceNumber);  /* TBR */
}
