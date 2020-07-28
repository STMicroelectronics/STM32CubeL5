/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
* File Name          : heart_rate_collector.h
* Author             : AMS - AAS, RF Application Team
* Version            : V1.0.0
* Date               : 14-January-2015
* Description        : Header for master.c module
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HEART_RATE_SERVICE_H
#define __HEART_RATE_SERVICE_H

/* Defines -------------------------------------------------------------------*/

/***************** SERVICE AND ATTRIBUTE CONSTANT ************************/
/**
 * @brief Max number of primary service supported. 
 * - 1 Generic Attribute (GATT) default service 
 * - 1 Generic Access (GAP) default service 
 * - 1 Heart Rate Service
 * - 1 Device Information Service
 */
#define NUM_MAX_HR_PRIMARY_SERVICE 4  

/**   
 * @brief Max number of Heart Rate characteristics supported for each service
 */
#define NUM_MAX_HR_CHARAC      3 

/**
 * @brief Max number of device information characteristics
 * supported.
 * @note If this value is set to 9 all the devices information
 * are supported
 */
#define NUM_MAX_DEV_INF_CHARAC  9 

/* Heart Rate Measurement Characteristic */
/* Bit Masks for FLAGS Value Fields  */
#define HEART_RATE_VALUE_FORMAT_MASK	        (0x01)  // 00000001 
/** the Sensor Contact Status bits are bit 1 and 2 with value 0,1,2,3 (00, 01, 10, 11)
 *  the bit 2 indicate if the Sensor Contact Feature is supported (10, 11)
 *  the bit 1 indicate if the Sensor Contact Feature is supported and is or not detected 
 */
#define SENSOR_CONTACT_DETECTED_STATUS_MASK     (0x02)  // 00000010 
#define SENSOR_CONTACT_SUPPORTED_STATUS_MASK    (0x04)  // 00000100 
#define ENERGY_EXPENDED_STATUS_MASK             (0x08)  // 00001000
#define RR_INTERVAL_MASK                        (0x10)  // 00010000

/* Keys-Values for BODY SENSOR LOCATION Value Fields  */
#define BODY_SENSOR_LOCATION_OTHER	        (0x00)  
#define BODY_SENSOR_LOCATION_CHEST              (0x01) 
#define BODY_SENSOR_LOCATION_WRIST              (0x02)  
#define BODY_SENSOR_LOCATION_FINGER             (0x03) 
#define BODY_SENSOR_LOCATION_HAND               (0x04) 
#define BODY_SENSOR_LOCATION_EARLOBE            (0x05)  
#define BODY_SENSOR_LOCATION_FOOT               (0x06) 

/* Keys-Values for HEART RATE CONTROL POINT Value Fields  */
#define HR_CNTL_POINT_VALUE_NOT_SUPPORTED       (0x80)
#define HR_CONTROL_POINT_RESET_ENERGY_EXPEND    (0x01)  
#define HR_CONTROL_POINT_RESERVED               (0x00) 
#define HR_CONTROL_POINT_MAX_VALUE              (0xFF) 

/* Procedure timeout (ms) */
//#define HEART_RATE_PROCEDURE_TIMEOUT             30000
/******************************************************************************
* Type definitions
******************************************************************************/

/******************************************************************************
* Function Declarations
******************************************************************************/

#endif /* _HEART_RATE_SERVICE_H_ */

