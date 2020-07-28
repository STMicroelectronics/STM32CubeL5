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
*   FILENAME        -  glucose_database.h
*
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      29/09/2014
*   $Revision$:  second version
*   $Author$:    
*   Comments:    glucose database measurements records 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  header file x Glucose database measurement records.
*  Database record data are coming from an internal database with
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

#ifndef _GLUCOSE_DATABASE_H_
#define _GLUCOSE_DATABASE_H_

#include <glucose_service.h>
#include <glucose_racp.h>
#include <glucose_sensor.h>


/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Define the Glucose database record max size */
#define GLUCOSE_DATABASE_MAX_RECORDS  (9) /* up to 255 could be supported */

/* Number of test records requested by SIG PTS glucose test suite
   (to be UPDATED based on specific test case: it has to be <= 
   GLUCOSE_DATABASE_MAX_RECORDS.
   
   NOTEs: 
   a) Set to 3 for all PTS Glucose Service Tests from 4.3.1 to 4.8.3, 4.8.5, 
   
   b) Set to 1 for test 4.8.4 (Glucose concentration is set to mol/L)
   c) Set to 5 for tests from 4.8.6 to 4.8.10, 4.8.12, 4.8.13(record with measurement
      context  + [liter is requested for record 5 measurement context] [it's default configuration].
   d) Set to 5 for test 4.8.11 + [Kg is requested for record 5 measurement context: MODIFY (REBUILD CODE): 
     NOTE: restore liter on record 5 measurement context after test 4.8.11. 

   e) Set to 3 for test 4.9.1 
   f) Set to 3 for tests 4.9.2,4.9.3, 4.9.4, 4.9.6, 4.9.7 
   g) Set to 4 for tests 4.9.5 (no context from record 4)
        
   h) Set to 3 for test 4.10.1 (delete all records).
      NOTE: It requires user action for sending 1 more record after all records are deleted:
            On profile_test_application uncomment: glucoseDatabase_Change_Database_Manually(1, SET_FLAG); [it's default configuration].
            When running PTS test, press Push Button when PTS ask for performing an action which will induce it to generate 1 more record.
   i) Set to 4 for test 4.10.2 
   l) NOTE: test 4.11 (abort operation)  requires ~ 200 records (extend database to run it)
   m) Set to 3 for test 4.12.1, 4.12.2, 
   n) Set to 3 for test 4.12.3 (n. of records not indicated).
      NOTE: It requires user action for deleting all records before running the PTS test. 
            On profile_test_application uncomment: glucoseDatabase_Change_Database_Manually(3, UNSET_FLAG); MODIFY (REBUILD CODE).
            When running the PTS test, press Push Button first time PTS asks  for  Secure ID.
            Restore profile_test_application.c after running this test.
   o) Set to 9 for test 4.13.1.
   p) Set to 3 for test 4.14.1 to 4.14.6, 4.14.8, 4.14.9
   q) NOTE: test 4.14.7 requires ~ 100 records (extend database to run it)
   
        
   For PTS tests don't change the glucose data content of the  available database 
   records 
*/
/* Enable PTS_TEST_GLUCOSE when using PTS tests */
#define PTS_TEST_GLUCOSE 1 
#define GLUCOSE_PTS_NUMBER_OF_RECORDS (3)  // (3), (9), (4), (5), (1)

#ifdef PTS_TEST_GLUCOSE
#define GLUCOSE_NUMBER_OF_STORED_RECORDS GLUCOSE_PTS_NUMBER_OF_RECORDS
#else
#define GLUCOSE_NUMBER_OF_STORED_RECORDS  GLUCOSE_DATABASE_MAX_RECORDS
#endif
/******************************************************************************
* Type definitions
******************************************************************************/

/******************************************************************************
* Imported Variable
******************************************************************************/

/* Glucose measurement database: it's a simulated database with some default values */
tGlucoseMeasurementVal glucoseDatabase_MeasurementRecords[GLUCOSE_DATABASE_MAX_RECORDS] =
{
  /* glucose measurement: record 1 */
  {
    GLUCOSE_RECORD_NOT_DELETED_FROM_RACP, /* GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED ? */
    GLUCOSE_MEASUREMENT_FLAGS_ALL_NO_CONTEXT_PRESENT | GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS, /* mmol/L */   
    1,                                    /* Sequence Number field */  
    {2013,2,22,4,0,0},                    /* Base Time  field */  
    0,                                    /* Time offset field */                        
    0xD046,                               /* SFLOAT exponent = -3(0xD); value: 0.070 mol/L Concentration field */ 
    GLUCOSE_TYPE_VENOUS_PLASMA | GLUCOSE_SAMPLE_LOCATION_CONTROL_SOLUTION, /* Type & Location fields */
    GLUCOSE_SENSOR_STATUS_ANNUNCIATION_ALL,                               /* Status Annunciation field */
  },
  /* glucose measurement: record 2 */
  {
    GLUCOSE_RECORD_NOT_DELETED_FROM_RACP, 
    GLUCOSE_MEASUREMENT_FLAGS_ALL_PRESENT | GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS,  /* mmol/L */   
    //GLUCOSE_MEASUREMENT_FLAGS_ALL_NO_CONTEXT_PRESENT | GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS, /* (It is used for test 4.9.2 - 4.9.4, 4.9.6,4.9.7)*/
    2,                                    /* Sequence Number field */  
    {2013,3,22,4,2,0},                    /* Base Time  field */  
    0,                                    /* Time offset field */               
    0xD032,                               /* SFLOAT exponent = -3(0xD); value: 0.050 mol/L Concentration field */ 
    GLUCOSE_TYPE_CAPILLARY_PLASMA | GLUCOSE_SAMPLE_LOCATION_AST ,/* Type & Location fields */
    GLUCOSE_SENSOR_STATUS_ANNUNCIATION_ALL,                               /* Status Annunciation field */
  },

  /* glucose measurement: record 3 */
  {
    GLUCOSE_RECORD_NOT_DELETED_FROM_RACP, /* GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED ? */
    GLUCOSE_MEASUREMENT_FLAGS_ALL_NO_CONTEXT_PRESENT & ~GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS,   /* kg/L */
    3,                                    /* Sequence Number field */  
    {2013,8,1,12,0,0},                    /* Base Time  field */  
    0,                                    /* Time offset field */    
    0xB050,                               /* SFLOAT exponent = -5(0xB);80(0x50) mg/dl  Concentration field */
    GLUCOSE_TYPE_ARTERIAL_PLASMA | GLUCOSE_SAMPLE_LOCATION_EARLOBE,/* Type & Location fields */
    GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_TEMPERATURE_TOO_HIGH | GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_TEMPERATURE_TOO_LOW /* Status Annunciation field */
  },
  /* glucose measurement: record 4 */
  {
    GLUCOSE_RECORD_NOT_DELETED_FROM_RACP,/* GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED ? */
    //GLUCOSE_MEASUREMENT_FLAGS_ALL_PRESENT & ~GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS,   /* kg/L */
    GLUCOSE_MEASUREMENT_FLAGS_ALL_NO_CONTEXT_PRESENT | GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS, /* it is used for test 4.9.5 */
    4,                                   /* Sequence Number field */  
    {2013,10,19,9,30,50},                /* Base Time  field */
    60,                                   /* Time offset field = +1hr */ 
    /* 0xB062,                             SFLOAT exponent = -5(0xB);98(0x62) mg/dl  Concentration field */
    0xD037,                              /* SFLOAT exponent = -3(0xD); value: 0.055 mol/L Concentration field */ 
    GLUCOSE_TYPE_INTERSISTIAL_FLUID | GLUCOSE_SAMPLE_LOCATION_AST,/* Type & Location fields */
    0                                    /* Status Annunciation field */
  },
  /* glucose measurement: record 5 */
  {
    GLUCOSE_RECORD_NOT_DELETED_FROM_RACP, /* GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED ? */
    GLUCOSE_MEASUREMENT_FLAGS_ALL_PRESENT | GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS,/* mol/L */ /* (Used for test 4.8.6 - 4.8.13)*/
    5,                                    /* Sequence Number field */  
    {2013,12,18,13,10,10},                /* Base Time  field */
    60,                                   /* Time offset field = +1hr */ 
    0xD040,                               /* SFLOAT exponent = -3(0xD); value: 0.064 mol/L Concentration field */ 
    GLUCOSE_TYPE_CAPILLARY_WHOLE_BLOOD | GLUCOSE_SAMPLE_LOCATION_FINGER,
    GLUCOSE_SENSOR_STATUS_ANNUNCIATION_DEVICE_BATTERY_LOW | GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_MALFUNCTION /* Status Annunciation field */
  },
      
  /* glucose measurement: record 6 */
  {
    GLUCOSE_RECORD_NOT_DELETED_FROM_RACP, /* GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED ? */
    GLUCOSE_MEASUREMENT_FLAGS_ALL_NO_CONTEXT_PRESENT | GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS,/* mol/L */ /* (Used for test 4.8.6 - 4.8.13)*/
    6,                                    /* Sequence Number field */  
    {2013,12,18,13,10,16},                /* Base Time  field */
    60,                                   /* Time offset field = +1hr */ 
    0xD040,                               /* SFLOAT exponent = -3(0xD); value: 0.064 mol/L Concentration field */ 
    GLUCOSE_TYPE_CAPILLARY_WHOLE_BLOOD | GLUCOSE_SAMPLE_LOCATION_FINGER,
    GLUCOSE_SENSOR_STATUS_ANNUNCIATION_DEVICE_BATTERY_LOW | GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_MALFUNCTION /* Status Annunciation field */
  },
  
  /* glucose measurement: record 7 */
  {
    GLUCOSE_RECORD_NOT_DELETED_FROM_RACP, /* GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED ? */
    GLUCOSE_MEASUREMENT_FLAGS_ALL_NO_CONTEXT_PRESENT | GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS,/* mol/L */ /* (Used for test 4.8.6 - 4.8.13)*/
    7,                                    /* Sequence Number field */  
    {2013,12,18,13,10,18},                /* Base Time  field */
    -60,                                   /* Time offset field = -1 hrs */ 
    0xD040,                               /* SFLOAT exponent = -3(0xD); value: 0.064 mol/L Concentration field */ 
    GLUCOSE_TYPE_CAPILLARY_WHOLE_BLOOD | GLUCOSE_SAMPLE_LOCATION_FINGER,
    GLUCOSE_SENSOR_STATUS_ANNUNCIATION_DEVICE_BATTERY_LOW | GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_MALFUNCTION /* Status Annunciation field */
  },
  
  /* glucose measurement: record 8 */
  {
    GLUCOSE_RECORD_NOT_DELETED_FROM_RACP, /* GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED ? */
    GLUCOSE_MEASUREMENT_FLAGS_ALL_PRESENT | GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS,/* mol/L */ /* (Used for test 4.8.6 - 4.8.13)*/
    8,                                    /* Sequence Number field */  
    {2013,12,18,13,10,19},                /* Base Time  field */
     -60,                                   /* Time offset field = -1 hrs */ 
    0xD040,                               /* SFLOAT exponent = -3(0xD); value: 0.064 mol/L Concentration field */ 
    GLUCOSE_TYPE_CAPILLARY_WHOLE_BLOOD | GLUCOSE_SAMPLE_LOCATION_FINGER,
    GLUCOSE_SENSOR_STATUS_ANNUNCIATION_DEVICE_BATTERY_LOW | GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_MALFUNCTION /* Status Annunciation field */
  },
  
  /* glucose measurement: record 9 */
  {
    GLUCOSE_RECORD_NOT_DELETED_FROM_RACP, /* GLUCOSE_RECORD_FILTERED_AND_NOT_DELETED ? */
    GLUCOSE_MEASUREMENT_FLAGS_ALL_NO_CONTEXT_PRESENT | GLUCOSE_MEAUREMENTS_FLAG_MMOL_L_UNITS,/* mol/L */ /* (Used for test 4.8.6 - 4.8.13)*/
    9,                                    /* Sequence Number field */  
    {2013,12,18,13,10,20},                /* Base Time  field */
     -60,                                   /* Time offset field = -1 hrs */ 
    0xD040,                               /* SFLOAT exponent = -3(0xD); value: 0.064 mol/L Concentration field */ 
    GLUCOSE_TYPE_CAPILLARY_WHOLE_BLOOD | GLUCOSE_SAMPLE_LOCATION_FINGER,
    GLUCOSE_SENSOR_STATUS_ANNUNCIATION_DEVICE_BATTERY_LOW | GLUCOSE_SENSOR_STATUS_ANNUNCIATION_SENSOR_MALFUNCTION /* Status Annunciation field */
  }
};/* end glucoseDatabase_MeasurementRecords[] */


/* Glucose measurement context database: it's a simulated database with some default values */
tGlucoseMeasurementContextVal glucoseDataBase_MeasurementContextRecords[GLUCOSE_DATABASE_MAX_RECORDS] =
{
  /* glucose measurement context: record 1 */
  {
    GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT & ~GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS, /* flags & kg(mg)unit */
    1,                                                          /* sequence number */
    0,                                                          /* extended flags */
    GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_LUNCH,             /* carbohydrate Id */
    0xD00F,                                                     /* SFLOAT exponent = -3(0xD), 15 mg carbohydrate units */
    GLUCOSE_MEASUREMENT_CONTEXT_MEAL_POSTPRANDIAL,              /* meal */
    GLUCOSE_MEASUREMENT_CONTEXT_TESTER_SELF | GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_DURING_MENSES,/* tester health */
    1500,                                                       /* exercise duration */
    50,                                                         /* exercise intensity */
    GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_SHORT_ACTING_INSULIN,/* medication Id */
    0xA05A,                                                     /* SFLOAT exponent = -6(0xA); value: 90(0x05A) mg medication units */
    40                                                          /* HbA1c */
  },
  /* glucose measurement context: record 2 */
  {
    GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT & ~GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS, /* flags & kg(mg)unit */
    //GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_EX_F_PRESENT & ~GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS, /* flags & kg(mg)unit */
    2,                                                          /* sequence number */
    0,                                                          /* extended flags */
    GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_SNACK,             /* carbohydrate Id */
    0xD002,                                                     /* SFLOAT exponent = -3(0xD), 2 mg carbohydrate units */
    GLUCOSE_MEASUREMENT_CONTEXT_MEAL_PREPRANDIAL,               /* meal */
    GLUCOSE_MEASUREMENT_CONTEXT_TESTER_SELF | GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_NO_ISSUE,/* tester health */
    1500,                                                       /* exercise duration */
    50,                                                         /* exercise intensity */
    GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_INTERMEDIATE_ACTING_INSULIN,/* medication Id */
    0xA032,                                                     /* SFLOAT exponent = -6(0xA); value: 50(0x032) mg medication units */
    40                                                          /* HbA1c */
  },
  
  /* glucose measurement context: record 3 */
  {
    GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT | GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS,/* flags & liter unit */
    3,                                                          /* sequence number */
    0,                                                          /* extended flags */
    GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_DRINK,             /* carbohydrate Id */
    0xD003,                                                     /* SFLOAT exponent = -3(0xD), 3 mg carbohydrate units */
    GLUCOSE_MEASUREMENT_CONTEXT_MEAL_CASUAL,                    /* meal */
    GLUCOSE_MEASUREMENT_CONTEXT_TESTER_LAB_TEST | GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_UNDER_STRESS,/* tester health */
    1500,                                                       /* exercise duration */
    50,                                                         /* exercise intensity */
    GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_LONG_ACTING_INSULIN, /* medication Id */
    0xD050,                                                     /* SFLOAT exponent = -3(0xD); value: 80(0x050) ml medication units */
    40                                                          /* HbA1c */
  },
  /* glucose measurement context: record 4 */
  {
    GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT | GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS,/* flags & liter unit */
    4,                                                          /* sequence number */
    0,                                                          /* extended flags */
    GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_SUPPER,            /* carbohydrate Id */
    0xD004,                                                     /* SFLOAT exponent = -3(0xD), 4 mg carbohydrate units */
    GLUCOSE_MEASUREMENT_CONTEXT_MEAL_BEDTIME,                   /* meal */
    GLUCOSE_MEASUREMENT_CONTEXT_TESTER_HEALTH_CARE_PROFESSIONAL | GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_UNDER_STRESS,/* tester health */
    1500,                                                       /* exercise duration */
    50,                                                         /* exercise intensity */
    GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_PRE_MIXED_INSULIN,   /* medication Id */
    0xD046,                                                     /* SFLOAT exponent = -3(0xD); value: 70(0x046) ml medication units */
    40                                                          /* HbA1c */
  },

  /* glucose measurement context: record 5 */
  {
    GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT | GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS,/* flags & liter unit */ /* tests from 4.8.6 to 4.8.13 except 4.8.11*/
    //GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT & ~GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS, /* flags & kg(mg)unit */ /* only x test 4.8.11 */
    5,                                                          /* sequence number */
    0,                                                          /* extended flags */
    GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_DINNER,            /* carbohydrate Id */
    0xD005,                                                     /* SFLOAT exponent = -3(0xD), 5 mg carbohydrate units */
    GLUCOSE_MEASUREMENT_CONTEXT_MEAL_PREPRANDIAL,               /* meal */
    GLUCOSE_MEASUREMENT_CONTEXT_TESTER_SELF | GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_MINOR_ISSUES,/* tester health */
    1500,                                                       /* exercise duration */
    50,                                                         /* exercise intensity */
    GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_ID_RAPID_ACTING_INSULIN,/* medication Id */
    0xD03C,                                                    /* SFLOAT exponent = -3(0xD); value: 60(0x03C) ml medication units */ /* tests from 4.8.6 to 4.8.13 except 4.8.11*/
    //0xA031,                                                     /* SFLOAT exponent = -6(0xA); value: 49(0x031) mg medication units */ /* only x test 4.8.11 */
    40                                                          /* HbA1c */
  },
  
  /* glucose measurement context: record 6 */
  {
    GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT | GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS,/* flags & liter unit */ 
    //GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT & ~GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS, /* flags & kg(mg)unit */ 
    6,                                                          /* sequence number */
    0,                                                          /* extended flags */
    GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_DINNER,            /* carbohydrate Id */
    0xD005,                                                     /* SFLOAT exponent = -3(0xD), 5 mg carbohydrate units */
    GLUCOSE_MEASUREMENT_CONTEXT_MEAL_PREPRANDIAL,               /* meal */
    GLUCOSE_MEASUREMENT_CONTEXT_TESTER_SELF | GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_MINOR_ISSUES,/* tester health */
    1500,                                                       /* exercise duration */
    50,                                                         /* exercise intensity */
    GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_ID_RAPID_ACTING_INSULIN,/* medication Id */
    0xD03C,                                                    /* SFLOAT exponent = -3(0xD); value: 60(0x03C) ml medication units */ /* tests from 4.8.6 to 4.8.13 except 4.8.11*/
    //0xA031,                                                     /* SFLOAT exponent = -6(0xA); value: 49(0x031) mg medication units */ /* only x test 4.8.11 */
    41                                                          /* HbA1c */
  },
  
  /* glucose measurement context: record 7 */
  {
    GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT | GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS,/* flags & liter unit */ 
    //GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT & ~GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS, /* flags & kg(mg)unit */ 
    7,                                                          /* sequence number */
    0,                                                          /* extended flags */
    GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_DINNER,            /* carbohydrate Id */
    0xD005,                                                     /* SFLOAT exponent = -3(0xD), 5 mg carbohydrate units */
    GLUCOSE_MEASUREMENT_CONTEXT_MEAL_PREPRANDIAL,               /* meal */
    GLUCOSE_MEASUREMENT_CONTEXT_TESTER_SELF | GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_MINOR_ISSUES,/* tester health */
    1500,                                                       /* exercise duration */
    50,                                                         /* exercise intensity */
    GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_ID_RAPID_ACTING_INSULIN,/* medication Id */
    0xD03C,                                                    /* SFLOAT exponent = -3(0xD); value: 60(0x03C) ml medication units */ /* tests from 4.8.6 to 4.8.13 except 4.8.11*/
    //0xA031,                                                     /* SFLOAT exponent = -6(0xA); value: 49(0x031) mg medication units */ /* only x test 4.8.11 */
    42                                                          /* HbA1c */
  },
  
  /* glucose measurement context: record 8 */
  {
    GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT | GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS,/* flags & liter unit */ 
    //GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT & ~GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS, /* flags & kg(mg)unit */ /* only x test 4.8.11 */
    8,                                                          /* sequence number */
    0,                                                          /* extended flags */
    GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_DINNER,            /* carbohydrate Id */
    0xD005,                                                     /* SFLOAT exponent = -3(0xD), 5 mg carbohydrate units */
    GLUCOSE_MEASUREMENT_CONTEXT_MEAL_PREPRANDIAL,               /* meal */
    GLUCOSE_MEASUREMENT_CONTEXT_TESTER_SELF | GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_MINOR_ISSUES,/* tester health */
    1500,                                                       /* exercise duration */
    50,                                                         /* exercise intensity */
    GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_ID_RAPID_ACTING_INSULIN,/* medication Id */
    0xD03C,                                                    /* SFLOAT exponent = -3(0xD); value: 60(0x03C) ml medication units */ /* tests from 4.8.6 to 4.8.13 except 4.8.11*/
    //0xA031,                                                     /* SFLOAT exponent = -6(0xA); value: 49(0x031) mg medication units */ /* only x test 4.8.11 */
    42                                                          /* HbA1c */
  },
  
  /* glucose measurement context: record 9 */
  {
    GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT | GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS,/* flags & liter unit */ 
    //GLUCOSE_MEASUREMENT_CONTEXT_FLAG_ALL_PRESENT & ~GLUCOSE_MEASUREMENT_CONTEXT_FLAG_MEDICATION_LITER_UNITS, /* flags & kg(mg)unit */ 
    9,                                                          /* sequence number */
    0,                                                          /* extended flags */
    GLUCOSE_MEASUREMENT_CONTEXT_CARBOHYDRATE_DINNER,            /* carbohydrate Id */
    0xD005,                                                     /* SFLOAT exponent = -3(0xD), 5 mg carbohydrate units */
    GLUCOSE_MEASUREMENT_CONTEXT_MEAL_PREPRANDIAL,               /* meal */
    GLUCOSE_MEASUREMENT_CONTEXT_TESTER_SELF | GLUCOSE_MEASUREMENT_CONTEXT_HEALTH_MINOR_ISSUES,/* tester health */
    1500,                                                       /* exercise duration */
    50,                                                         /* exercise intensity */
    GLUCOSE_MEASUREMENT_CONTEXT_MEDICATION_ID_RAPID_ACTING_INSULIN,/* medication Id */
    0xD03C,                                                    /* SFLOAT exponent = -3(0xD); value: 60(0x03C) ml medication units */ /* tests from 4.8.6 to 4.8.13 except 4.8.11*/
    //0xA031,                                                     /* SFLOAT exponent = -6(0xA); value: 49(0x031) mg medication units */ /* only x test 4.8.11 */
    43                                                          /* HbA1c */
  }
};/* end glucoseDataBase_MeasurementContextRecords[] */

/******************************************************************************
* Function Declarations
******************************************************************************/

#endif /* _GLUCOSE_DATABASE_H_ */
