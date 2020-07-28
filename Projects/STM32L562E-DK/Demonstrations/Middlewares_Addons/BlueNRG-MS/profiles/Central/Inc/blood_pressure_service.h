/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
* File Name          : heart_rate_collector.h
* Author             : AMS - AAS, RF Application Team
* Version            : V1.0.0
* Date               : 04-Febrary-2015
* Description        : Header for blood_pressure_collector.c module
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BLOOD_PRESSURE_SERVICE_H
#define __BLOOD_PRESSURE_SERVICE_H

/* Defines -------------------------------------------------------------------*/

/***************** SERVICE AND ATTRIBUTE CONSTANT ************************/
/**
 * @brief Max number of primary service supported. 
 * - 1 Generic Attribute (GATT) default service 
 * - 1 Generic Access (GAP) default service 
 * - 1 Blood Pressure Service
 * - 1 Device Information Service
 */
#define NUM_MAX_BP_PRIMARY_SERVICE  4  

/**   
 * @brief Max number of Blood Pressure characteristics supported for each service
 */
#define NUM_MAX_BP_CHARAC           3 

/**
 * @brief Max number of device information characteristics
 * supported.
 * @note If this value is set to 9 all the devices information
 * are supported
 */
#define NUM_MAX_DEV_INF_CHARAC      9 

/* flags */
#define FLAG_BLOOD_PRESSURE_UNITS_KPA		(0x01)
#define FLAG_TIME_STAMP_PRESENT			(0x02)
#define FLAG_PULSE_RATE_PRESENT			(0x04)
#define FLAG_USER_ID_PRESENT			(0x08)
#define FLAG_MEASUREMENT_STATUS_PRESENT	        (0x10)

/* measurement status field bitmasks */
#define BODY_MOVEMENT_DETECTION_FLAG		(0x01)
#define CUFF_FIT_DETECTION_FLAG			(0x02)
#define IRREGULAR_PULSE_DETECTION_FLAG		(0x04)
#define PULSE_RATE_RANGE_EXCEEDS_UPPER_LIMIT	(0x08)
#define PULSE_RATE_RANGE_BELOW_LOWER_LIMIT	(0x10)
#define MEASUREMENT_POSITION_DETECTION_FLAG	(0x20)

/* blood pressure feature flags */
#define BODY_MOVEMENT_DETECTION_SUPPORT_BIT	 	(0x01)
#define CUFF_FIT_DETECTION_SUPPORT_BIT	 		(0x02)
#define IRREGULAR_PULSE_DETECTION_SUPPORT_BIT		(0x04)
#define PULSE_RATE_RANGE_DETECTION_SUPPORT_BIT	 	(0x08)
#define MEASUREMENT_POSITION_DETECTION_SUPPORT_BIT	(0x10)
#define MULTIPLE_BOND_SUPPORT_BIT	                (0x20)
  
/* Reserved for future use (RFU) bitmasks */   
#define RFU_5                   (0x20)
#define RFU_6                   (0x40)
#define RFU_7                   (0x80)
#define RFU_8                   (0x100)
#define RFU_9                   (0x200)
#define RFU_10                  (0x400)
#define RFU_11                  (0x800)
#define RFU_12                  (0x1000)
#define RFU_13                  (0x2000)
#define RFU_14                  (0x4000)
#define RFU_15                  (0x8000)
   
#define UNIT_BP_mmHg        0x2781
#define UNIT_BP_kPa         0x2724


/******************************************************************************
* Type definitions
******************************************************************************/

/******************************************************************************
* Function Declarations
******************************************************************************/

#endif /* _BLOOD_PRESSURE_SERVICE_H_ */

