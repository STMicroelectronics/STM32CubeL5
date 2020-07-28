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
*   FILENAME        -  glucose_service.h
*
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      01/07/2014
*   $Revision$:  first version
*   $Author$:    
*   Comments:    glucose service
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file for the glucose service, characteristics
* 
******************************************************************************/

#ifndef _GLUCOSE_SERVICE_H_
#define _GLUCOSE_SERVICE_H_

/******************************************************************************
* Macro Declarations
******************************************************************************/

/******** Glucose measurement characteristic *********************************/

/* Glucose measurement: values for Flags (8 bit) field (to be used for 
   selecting presence of characteristic optional fields) */
#define GLUCOSE_MEASUREMENT_FLAGS_TIME_OFFSET_IS_PRESENT         (0x01)
#define GLUCOSE_MEASUREMENT_FLAGS_CONCENTRATION_IS_PRESENT       (0x02)
#define GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS                    (0x04) 
#define GLUCOSE_MEASUREMENT_FLAGS_STATUS_ANNUNCIATION_IS_PRESENT (0x08)
/* If a Glucose Measurement characteristic includes contextual information 
   (i.e., a corresponding Glucose Measurement Context characteristic), 
   the Context Information Follows Flag (bit 4 of the Flags field)
   shall be set to 1 */
#define GLUCOSE_MEASUREMENT_FLAGS_CONTEXT_INFO_IS_PRESENT        (0x10)

/* Flags all present: (to be used for selecting all the  
   characteristic optional fields) */
#define GLUCOSE_MEASUREMENT_FLAGS_ALL_PRESENT (GLUCOSE_MEASUREMENT_FLAGS_TIME_OFFSET_IS_PRESENT |         \
                                               GLUCOSE_MEASUREMENT_FLAGS_CONCENTRATION_IS_PRESENT |       \
                                               GLUCOSE_MEASUREMENT_FLAGS_STATUS_ANNUNCIATION_IS_PRESENT | \
                                               GLUCOSE_MEASUREMENT_FLAGS_CONTEXT_INFO_IS_PRESENT )

/* Flags all present except Glucose Measurement Context characteristic */
#define GLUCOSE_MEASUREMENT_FLAGS_ALL_NO_CONTEXT_PRESENT (GLUCOSE_MEASUREMENT_FLAGS_TIME_OFFSET_IS_PRESENT |         \
                                                          GLUCOSE_MEASUREMENT_FLAGS_CONCENTRATION_IS_PRESENT |       \
                                                          GLUCOSE_MEASUREMENT_FLAGS_STATUS_ANNUNCIATION_IS_PRESENT )
  
/* Glucose measurement: list of available Key values for Type field (nibble) */
#define GLUCOSE_TYPE_CAPILLARY_WHOLE_BLOOD     (0x1)
#define GLUCOSE_TYPE_CAPILLARY_PLASMA          (0x2)
#define GLUCOSE_TYPE_VENOUS_WHOLE_BLOOD        (0x3)
#define GLUCOSE_TYPE_VENOUS_PLASMA             (0x4)
#define GLUCOSE_TYPE_ARTERIAL_WHOLE_BLOOD      (0x5)
#define GLUCOSE_TYPE_ARTERIAL_PLASMA           (0x6)
#define GLUCOSE_TYPE_UNDERTERMINED_WHOLE_BLOOD (0x7)
#define GLUCOSE_TYPE_UNDERTERMINED_PLASMA      (0x8)
#define GLUCOSE_TYPE_INTERSISTIAL_FLUID        (0x9) /* IFD*/
#define GLUCOSE_TYPE_CONTROL                   (0xA)
/* NOTE: 0x0, 0xB - 0xF keys values are reserved for future use */

/* Glucose measurement: list of available Key values for Sample Location field (nibble) */
#define GLUCOSE_SAMPLE_LOCATION_FINGER              (0x10)
#define GLUCOSE_SAMPLE_LOCATION_AST                 (0x20) /* Alternate Site Test */
#define GLUCOSE_SAMPLE_LOCATION_EARLOBE             (0x30)
#define GLUCOSE_SAMPLE_LOCATION_CONTROL_SOLUTION    (0x40)
#define GLUCOSE_SAMPLE_LOCATION_VALUE_NOT_AVAILABLE (0xF0)
/* NOTE: 0x0, 0x5 - 0xE keys values are reserved for future use */

/* Glucose measurement: list of available Key Values for
   Sensor Status Annunciation field (16 bit) */
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_DEVICE_BATTERY_LOW          (0x0001)
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_MALFUNCTION          (0x0002)
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SAMPLE_SIZE                 (0x0004)
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_STRIP_INSERTION_ERROR       (0x0008)
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_STRIP_TYPE_INCORRECT        (0x0010)
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_RESULT_TOO_HIGH      (0x0020)
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_RESULT_TOO_LOW       (0x0040)
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_TEMPERATURE_TOO_HIGH (0x0080)
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_TEMPERATURE_TOO_LOW  (0x0100)
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_READ_INTERRUPTED     (0x0200)
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_GENERAL_DEVICE_FAULT        (0x0400)
#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_TIME_FAULT                  (0x0800)
/* NOTE: 0xC - 0xF keys values are reserved for future use */

#define GLUCOSE_SENSOR_STATUS_ANNUNCIATION_ALL (GLUCOSE_SENSOR_STATUS_ANNUNCIATION_DEVICE_BATTERY_LOW          | \
                                                GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_MALFUNCTION          | \
                                                GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SAMPLE_SIZE                 | \
                                                GLUCOSE_SENSOR_STATUS_ANNUNCIATION_STRIP_INSERTION_ERROR       | \
                                                GLUCOSE_SENSOR_STATUS_ANNUNCIATION_STRIP_TYPE_INCORRECT        | \
                                                GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_RESULT_TOO_HIGH      | \
                                                GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_RESULT_TOO_LOW       | \
                                                GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_TEMPERATURE_TOO_HIGH | \
                                                GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_TEMPERATURE_TOO_LOW  | \
                                                GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_READ_INTERRUPTED     | \
                                                GLUCOSE_SENSOR_STATUS_ANNUNCIATION_GENERAL_DEVICE_FAULT        | \
                                                GLUCOSE_SENSOR_STATUS_ANNUNCIATION_TIME_FAULT)

/******** Glucose measurement context characteristic ****************/
/* Glucose measurement context: values for Flags (8 bit) field (to be used for 
   selecting presence of characteristic optional fields) */

#define GLUCOSE_MEASUREMENT_CONTEXT_FLAG_CARBOHYDRATE_IS_PRESENT           (0x01)
#define GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEAL_IS_PRESENT                   (0x02)
#define GLUCOSE_MEASUREMENT_CONTEXT_FLAG_TESTER_HEALTH_IS_PRESENT          (0x04)
#define GLUCOSE_MEASUREMENT_CONTEXT_FLAG_EXERCISE_DURATION_IS_PRESENT      (0x08)
#define GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_ID_IS_PRESENT          (0x10)
#define GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS            (0x20) 
#define GLUCOSE_MEASUREMENT_CONTEXT_FLAG_HB1A1C_IS_PRESENT                 (0x40)
#define GLUCOSE_MEASUREMENT_CONTEXT_FLAG_EXTENDED_IS_PRESENT               (0x80)

/* Flags all present: (to be used for selecting all the  
   characteristic optional fields: extended flags TBR) */

#define GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT (GLUCOSE_MEASUREMENT_CONTEXT_FLAG_CARBOHYDRATE_IS_PRESENT |           \
                                                      GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEAL_IS_PRESENT |                   \
                                                      GLUCOSE_MEASUREMENT_CONTEXT_FLAG_TESTER_HEALTH_IS_PRESENT |          \
                                                      GLUCOSE_MEASUREMENT_CONTEXT_FLAG_EXERCISE_DURATION_IS_PRESENT |      \
                                                      GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_ID_IS_PRESENT |          \
                                                      GLUCOSE_MEASUREMENT_CONTEXT_FLAG_HB1A1C_IS_PRESENT|                  \
                                                      GLUCOSE_MEASUREMENT_CONTEXT_FLAG_EXTENDED_IS_PRESENT)
                                                     
#define GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_NO_EXT_FLAG  (GLUCOSE_MEASUREMENT_CONTEXT_FLAG_CARBOHYDRATE_IS_PRESENT |           \
                                                           GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEAL_IS_PRESENT |                   \
                                                           GLUCOSE_MEASUREMENT_CONTEXT_FLAG_TESTER_HEALTH_IS_PRESENT |          \
                                                           GLUCOSE_MEASUREMENT_CONTEXT_FLAG_EXERCISE_DURATION_IS_PRESENT |      \
                                                           GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_ID_IS_PRESENT |          \
                                                           GLUCOSE_MEASUREMENT_CONTEXT_FLAG_HB1A1C_IS_PRESENT)
                                                          

/* Glucose measurement context: list of available Key values for Carbohydrate field (uint8_t) */
#define GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_BREAKFAST (0x01)
#define GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_LUNCH     (0x02)
#define GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_DINNER    (0x03)
#define GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_SNACK     (0x04)
#define GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_DRINK     (0x05)
#define GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_SUPPER    (0x06)
#define GLUCOSE_MEASUREMENT_CONTEXT_CARBHYDRATEO_BRUNCH    (0x07)
/* NOTE: 0x0,  0x8 - 0xFF keys values are reserved for future use */

/* Glucose measurement context: list of available Key values for Meal field (uint8_t) */
#define GLUCOSE_MEASUREMENT_CONTEXT_MEAL_PREPRANDIAL  (0x1)
#define GLUCOSE_MEASUREMENT_CONTEXT_MEAL_POSTPRANDIAL (0x2)
#define GLUCOSE_MEASUREMENT_CONTEXT_MEAL_FASTING      (0x3)
#define GLUCOSE_MEASUREMENT_CONTEXT_MEAL_CASUAL       (0x4)
#define GLUCOSE_MEASUREMENT_CONTEXT_MEAL_BEDTIME      (0x5)
/* NOTE: 0x0,  0x6 - 0xFF keys values are reserved for future use */

/* Glucose measurement context: list of available Key values for Tester field (nibble) */
#define GLUCOSE_MEASUREMENT_CONTEXT_TESTER_SELF                     (0x1)
#define GLUCOSE_MEASUREMENT_CONTEXT_TESTER_HEALTH_CARE_PROFESSIONAL (0x2)
#define GLUCOSE_MEASUREMENT_CONTEXT_TESTER_LAB_TEST                 (0x3)
#define GLUCOSE_MEASUREMENT_CONTEXT_TESTER_NOT_AVAILABLE            (0xF)
/* NOTE: 0x0,  0x4 - 0xE keys values are reserved for future use */

/* Glucose measurement context: list of available Key values for Health field (nibble) */
#define GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_MINOR_ISSUES        (0x10)
#define GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_MAJOR_ISSUES        (0x20)
#define GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_DURING_MENSES       (0x30)
#define GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_UNDER_STRESS        (0x40)
#define GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_NO_ISSUE            (0x50)
#define GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_VALUE_NOT_AVAILABLE (0xF0)
/* NOTE: 0x0,  0x6 - 0xE keys values are reserved for future use */

/* Glucose measurement context: list of available Key values for Medication ID (uint8_t) */
#define GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_ID_RAPID_ACTING_INSULIN     (0x1)
#define GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_SHORT_ACTING_INSULIN        (0x2)
#define GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_INTERMEDIATE_ACTING_INSULIN (0x3)
#define GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_LONG_ACTING_INSULIN         (0x4)
#define GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_PRE_MIXED_INSULIN           (0x5)


/******** Glucose feature characteristic **************************/
/* Glucose feature values (16  bit): to be used for identify 
   supported features  */

#define GLUCOSE_FEATURE_LOW_BATTERY_DETECTION_IS_SUPPORTED                 (0x0001) 
#define GLUCOSE_FEATURE_SENSOR_MALFUNCTION_DETECTION_IS_SUPPORTED          (0x0002) 
#define GLUCOSE_FEATURE_SAMPLE_SIZE_IS_SUPPORTED                           (0x0004)
#define GLUCOSE_FEATURE_SENSOR_STRIP_INSERTION_ERROR_IS_SUPPORTED          (0x0008)
#define GLUCOSE_FEATURE_SENSOR_STRIP_TYPE_ERROR_IS_SUPPORTED               (0x0010)
#define GLUCOSE_FEATURE_SENSOR_RESULT_HIGH_LOW_DETECTION_IS_SUPPORTED      (0x0020)
#define GLUCOSE_FEATURE_SENSOR_TEMPERATURE_HIGH_LOW_DETECTION_IS_SUPPORTED (0x0040)
#define GLUCOSE_FEATURE_SENSOR_READ_INTERRUPT_DETECTION_IS_SUPPORTED       (0x0080)
#define GLUCOSE_FEATURE_GENERAL_DEVICE_FAULT_IS_SUPPORTED                  (0x0100)
#define GLUCOSE_FEATURE_TIME_FAULT_IS_SUPPORTED                            (0x0200)
#define GLUCOSE_FEATURE_MULTIPLE_BOND_IS_SUPPORTED                         (0x0400)

/* NOTE: Multi bond is not supported */
#define GLUCOSE_FEATURE_ALL_SUPPORTED (GLUCOSE_FEATURE_LOW_BATTERY_DETECTION_IS_SUPPORTED |                 \
                                       GLUCOSE_FEATURE_SENSOR_MALFUNCTION_DETECTION_IS_SUPPORTED |          \
                                       GLUCOSE_FEATURE_SAMPLE_SIZE_IS_SUPPORTED |                           \
                                       GLUCOSE_FEATURE_SENSOR_STRIP_INSERTION_ERROR_IS_SUPPORTED |          \
                                       GLUCOSE_FEATURE_SENSOR_STRIP_TYPE_ERROR_IS_SUPPORTED |               \
                                       GLUCOSE_FEATURE_SENSOR_RESULT_HIGH_LOW_DETECTION_IS_SUPPORTED |      \
                                       GLUCOSE_FEATURE_SENSOR_TEMPERATURE_HIGH_LOW_DETECTION_IS_SUPPORTED | \
                                       GLUCOSE_FEATURE_SENSOR_READ_INTERRUPT_DETECTION_IS_SUPPORTED |       \
                                       GLUCOSE_FEATURE_GENERAL_DEVICE_FAULT_IS_SUPPORTED |                  \
                                       GLUCOSE_FEATURE_TIME_FAULT_IS_SUPPORTED |                            \
                                       GLUCOSE_FEATURE_MULTIPLE_BOND_IS_SUPPORTED)

/* Multi bond is not supported */
#define GLUCOSE_FEATURE_ALL_SUPPORTED_NO_MULTI_BOND (GLUCOSE_FEATURE_LOW_BATTERY_DETECTION_IS_SUPPORTED |                 \
						     GLUCOSE_FEATURE_SENSOR_MALFUNCTION_DETECTION_IS_SUPPORTED | \
						     GLUCOSE_FEATURE_SAMPLE_SIZE_IS_SUPPORTED |	\
						     GLUCOSE_FEATURE_SENSOR_STRIP_INSERTION_ERROR_IS_SUPPORTED | \
						     GLUCOSE_FEATURE_SENSOR_STRIP_TYPE_ERROR_IS_SUPPORTED | \
						     GLUCOSE_FEATURE_SENSOR_RESULT_HIGH_LOW_DETECTION_IS_SUPPORTED | \
						     GLUCOSE_FEATURE_SENSOR_TEMPERATURE_HIGH_LOW_DETECTION_IS_SUPPORTED | \
						     GLUCOSE_FEATURE_SENSOR_READ_INTERRUPT_DETECTION_IS_SUPPORTED | \
						     GLUCOSE_FEATURE_GENERAL_DEVICE_FAULT_IS_SUPPORTED | \
						     GLUCOSE_FEATURE_TIME_FAULT_IS_SUPPORTED)

/******** Glucose record access control point (RACP) characteristic *****/

/* Record Access Control Point: Op Code Values */

/* RACP requests */
#define RACP_REPORT_STORED_RECORDS_OP_CODE          (0x01)
#define RACP_DELETE_STORED_RECORDS_OP_CODE          (0x02) 
#define RACP_ABORT_OPERATION_OP_CODE                (0x03)
#define RACP_REPORT_NUMBER_STORED_RECORDS_OP_CODE   (0x04)
/* RACP responses */
#define RACP_NUMBER_STORED_RECORDS_RESPONSE_OP_CODE (0x05)
#define RACP_RESPONSE_OP_CODE                       (0x06)
 
/* Record Access Control Point: procedure operator values */

#define RACP_OPERATOR_NULL          (0x00)
#define RACP_OPERATOR_ALL           (0x01)  
#define RACP_OPERATOR_LESS_EQUAL    (0x02)
#define RACP_OPERATOR_GREATER_EQUAL (0x03)
#define RACP_OPERATOR_WITHIN_RANGE  (0x04)
#define RACP_OPERATOR_FIRST_RECORD  (0x05)
#define RACP_OPERATOR_LAST_RECORD   (0x06)  

/* Record Access Control Point: filter types values */
#define RACP_FILTER_TYPE_NONE             (0x00)
#define RACP_FILTER_TYPE_SEQUENCE_NUMBER  (0x01)
#define RACP_FILTER_TYPE_USER_FACING_TIME (0x02)

/* Record Access Control Point: filter types valid lenght */
#define RACP_FILTER_TYPE_NO_OPERAND_LENGHT                    (2)
#define RACP_FILTER_TYPE_LESS_GREATER_SEQUENCE_NUMBER_LENGHT  (5)
#define RACP_FILTER_TYPE_LESS_GREATER_USER_FACING_TIME_LENGHT (10)
#define RACP_FILTER_TYPE_WITHIN_RANGE_SEQUENCE_NUMBER_LENGHT  (7)
#define RACP_FILTER_TYPE_WITHIN_RANGE_USER_FACING_TIME_LENGHT (17)

/* Record Access Control Point: fields position */
#define RACP_OP_CODE_POSITION     (0)
#define RACP_OPERATOR_POSITION    (1)
#define RACP_FILTER_TYPE_POSITION (2)

/* Record Access Control Point: responses values */

#define RACP_RESPONSE_SUCCESS                                     (0x01)
#define RACP_RESPONSE_OPCODE_NOT_SUPPORTED                        (0x02)
#define RACP_RESPONSE_INVALID_OPERATOR                            (0x03)
#define RACP_RESPONSE_OPERATOR_NOT_SUPPORTED                      (0x04)
#define RACP_RESPONSE_INVALID_OPERAND                             (0x05)
#define RACP_RESPONSE_NO_RECORDS                                  (0x06) 
#define RACP_RESPONSE_ABORT_FAILED                                (0x07) 
#define RACP_RESPONSE_PROCEDURE_NOT_COMPLETED                     (0x08)
#define RACP_RESPONSE_OPERAND_NOT_SUPPORTED                       (0x09) /* TBR: RACP_RESPONSE_FILTER_TYPE_NOT_SUPPORTED */

/* Glucose specification definition: 0x80, PTS 6.0.0 */
#define RACP_RESPONSE_PROCEDURE_ALREADY_IN_PROGRESS               (0x80)
/* New values defined on Core spec supplement (CSS v4), common profile and service error code
   It is used on PTS 5.3.0 */
//#define RACP_RESPONSE_PROCEDURE_ALREADY_IN_PROGRESS (0xFE)

/* Glucose specification definition: 0x81 & PTS 6.0.0*/
#define RACP_RESPONSE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED (0x81)
/* New values defined on Core spec supplement (CSS v4), common profile and service error code
   It is used on PTS 5.3.0 */
//#define RACP_RESPONSE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED (0xFD)

/******** Special Short Float Values *****************************************/

/* Not a number value */
#define NAN_VALUE               (0x07FF)
/* Not at this resolution value */
#define NRES_VALUE              (0x0800)
#define POSITIVE_INFINITY_VALUE (0x07FE)
#define NEGATIVE_INFINITY_VALUE (0x0802)
/* Reserved for future use value */
#define RESERVED_FOR_FUTURE_USE (0x0801)

/* Procedure timeout (ms) */
#define GLUCOSE_PROCEDURE_TIMEOUT             30000
/******************************************************************************
* Type definitions
******************************************************************************/

/******************************************************************************
* Function Declarations
******************************************************************************/

#endif /* _GLUCOSE_SERVICE_H_ */
