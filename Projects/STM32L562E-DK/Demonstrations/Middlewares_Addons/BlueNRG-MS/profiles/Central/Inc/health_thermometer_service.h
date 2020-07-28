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
#ifndef __HEALTH_THERMOMETER_SERVICE_H
#define __HEALTH_THERMOMETER_SERVICE_H

/* Defines -------------------------------------------------------------------*/

/***************** SERVICE AND ATTRIBUTE CONSTANT ************************/
/**
 * @brief Max number of primary service supported. 
 * - 1 Generic Attribute (GATT) default service 
 * - 1 Generic Access (GAP) default service 
 * - 1 Health Thermometer Service
 * - 1 Device Information Service
 */
#define NUM_MAX_HT_PRIMARY_SERVICE 4  

/**   
 * @brief Max number of Health Thermometer characteristics supported for each service
 */
#define NUM_MAX_HT_CHARAC      4 

/**
 * @brief Max number of device information characteristics
 * supported.
 * @note If this value is set to 9 all the devices information
 * are supported
 */
#define NUM_MAX_DEV_INF_CHARAC  9 

/* TEMPERATURE MEASUREMENT and INTERMEDIATE TEMPERATURE characteristics */
/* Bit Masks for FLAGS Value Fields  */
/* Temperature Unit in Celsius (bit 0 with value 0), in Fahrenheit (bit 0 with value 1) */
#define FLAG_TEMPERATURE_UNITS_FARENHEIT 	  (0x01)
#define FLAG_TIMESTAMP_PRESENT 		          (0x02)
#define FLAG_TEMPERATURE_TYPE 		          (0x04)

/* Keys-Values for TEMPERATURE TYPE Value Fields  */
#define TEMPERATURE_TYPE_ARMPIT                    (0x01) 
#define TEMPERATURE_TYPE_BODY                      (0x02)  
#define TEMPERATURE_TYPE_EAR                       (0x03) 
#define TEMPERATURE_TYPE_FINGER                    (0x04) 
#define TEMPERATURE_TYPE_GASTRO_INTESTINAL_TRACT   (0x05)  
#define TEMPERATURE_TYPE_MOUTH                     (0x06) 
#define TEMPERATURE_TYPE_RECTUM                    (0x07) 
#define TEMPERATURE_TYPE_TOE                       (0x08)  
#define TEMPERATURE_TYPE_TYMPANUM                  (0x09) 

/* Keys-Values for MEASUREMENT INTERVAL Value Fields */
#define OUT_OF_RANGE                      (0xFF)
#define MEASUREMENT_INTERVAL_MIN_VALUE    (0x0001)  
#define MEASUREMENT_INTERVAL_MAX_VALUE    (0xFFFF) //max value 65535

#define UNIT_TEMP_CELSIUS       0x272F
#define UNIT_TEMP_FAHRENHEIT    0x27AC    
                  
/******************************************************************************
* Type definitions
******************************************************************************/

/******************************************************************************
* Function Declarations
******************************************************************************/

#endif /* _HEALTH_THERMOMETER_SERVICE_H_ */

