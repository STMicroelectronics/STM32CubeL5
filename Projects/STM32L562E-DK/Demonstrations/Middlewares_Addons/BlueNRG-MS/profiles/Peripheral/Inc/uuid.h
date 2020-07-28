/******************************************************************************/
/*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  uuid.h
*
*   COMPILER        -  EWARM (gnu gcc)
*
*******************************************************************************
*
*   CHANGE HISTORY
* ---------------------------
*
*   $Date$:      05/09/2012
*   $Revision$:  first version
*   $Author$:    
*   Comments:    
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  contains the UUIDs of all the services and caharcteristics
******************************************************************************/
#ifndef _UUID_H_
#define _UUID_H_

/* UUIDs of immediate alert service */
#define IMMEDIATE_ALERT_SERVICE_UUID                    (0x1802)
#define ALERT_LEVEL_CHARACTERISTIC_UUID                 (0x2A06)

/* UUIDs for proximity profile */
#define LINK_LOSS_SERVICE_UUID                          (0x1803)
#define TX_POWER_SERVICE_UUID                           (0x1804)
#define TX_POWER_LEVEL_CHARACTERISTIC_UUID              (0x2A07)

/* Descriptor UUIDs */
#define CHAR_EXTENDED_PROPERTIES_DESCRIPTOR_UUID         (0x2900)
#define CHAR_URES_DESCRIPTION_DESCRIPTOR_UUID            (0x2901)
#define CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID               (0x2902)
#define SERVER_CHAR_CONFIG_DESCRIPTOR_UUID               (0x2903)
#define CHAR_PRESENTATION_FORMAT_DESCRIPTOR_UUID         (0x2904)
#define CHAR_AGGREGATE_FORMAT_DESCRIPTOR_UUID            (0x2905)
#define VALID_RANGE_DESCRIPTOR_UUID                      (0x2906)
#define EXTERNAL_REPORT_REFERENCE_DESCRIPTOR_UUID        (0x2907)
#define REPORT_REFERENCE_DESCRIPTOR_UUID                 (0x2908)

/* UUIDs for Heart Rate Service */
#define HEART_RATE_SERVICE_UUID                        (0x180D)
#define CLIENT_CHARACTERISTIC_CONFIG_DESCRIPTOR_UUID   (0x2902)
#define HEART_RATE_MEASURMENT_UUID                     (0x2A37)
#define SENSOR_LOCATION_UUID                           (0x2A38)
#define CONTROL_POINT_UUID                             (0x2A39)

/*
 * UUIDs for Device Information Service 
 */
#define DEVICE_INFORMATION_SERVICE_UUID (0x180A)
#define SYSTEM_ID_UUID                  (0x2A23)
#define MODEL_NUMBER_UUID               (0x2A24)
#define SERIAL_NUMBER_UUID              (0x2A25)
#define FIRMWARE_REVISION_UUID          (0x2A26)
#define HARDWARE_REVISION_UUID          (0x2A27)
#define SOFTWARE_REVISION_UUID          (0x2A28)
#define MANUFACTURER_NAME_UUID          (0x2A29)
#define IEEE_CERTIFICATION_UUID         (0x2A2A)
#define PNP_ID_UUID                     (0x2A50)

/* UUIDs for Time Service */
#define HUMAN_INTERFACE_DEVICE_SERVICE_UUID                 (0x1812)
#define PROTOCOL_MODE_CHAR_UUID                             (0x2A4E)
#define REPORT_CHAR_UUID                                    (0x2A4D)
#define REPORT_MAP_CHAR_UUID                                (0x2A4B)
#define BOOT_KEYBOARD_INPUT_REPORT_CHAR_UUID                (0x2A22)
#define BOOT_KEYBOARD_OUTPUT_REPORT_CHAR_UUID               (0x2A32)
#define BOOT_MOUSE_INPUT_REPORT_CHAR_UUID                   (0x2A33)
#define HID_INFORMATION_CHAR_UUID                           (0x2A4A)
#define HID_CONTROL_POINT_CHAR_UUID                         (0x2A4C)

/* UUIDs for battery service */ 
#define BATTERY_SERVICE_SERVICE_UUID                        (0x180F)
#define BATTERY_LEVEL_CHAR_UUID                             (0x2A19)

/* UUIDs for scan parameter service */
#define SCAN_PARAMETER_SERVICE_UUID                         (0x1813)
#define SCAN_INTERVAL_WINDOW_CHAR_UUID                      (0x2A4F)
#define SCAN_REFRESH_CHAR_UUID                              (0x2A31)

/* UUIDs for Time service */
#define CURRENT_TIME_SERVICE_UUID                   (0x1805)
#define NEXT_DST_CHANGE_SERVICE_UUID                (0x1807)
#define REFERENCE_UPDATE_TIME_SERVICE_UUID          (0x1806)

#define CURRENT_TIME_CHAR_UUID                      (0x2A2B)
#define LOCAL_TIME_INFORMATION_CHAR_UUID            (0x2A0F)
#define REFERENCE_TIME_INFORMATION_CHAR_UUID        (0x2A14)
#define TIME_WITH_DST_CHAR_UUID                     (0x2A11)
#define TIME_UPDATE_CONTROL_POINT_CHAR_UUID         (0x2A16)
#define TIME_UPDATE_STATE_CHAR_UUID                 (0x2A17)

/* UUIDs for Phone Alert status profile */
#define PHONE_ALERT_SERVICE_UUID					(0x180E)
#define PHONE_ALERT_STATUS_CHARAC_UUID				(0x2A3F)
#define RINGER_CNTRL_POINT_CHARAC_UUID				(0x2A40)
#define RINGER_SETTING_CHARAC_UUID					(0x2A41)

/* UUIDs for Blood Pressure profile */
#define BLOOD_PRESSURE_SERVICE_UUID					(0x1810)
#define BLOOD_PRESSURE_MEASUREMENT_CHAR_UUID		(0x2A35)
#define INTERMEDIATE_CUFF_PRESSURE_CHARAC_UUID	 	(0x2A36)
#define BLOOD_PRESSURE_FEATURE_CHARAC				(0x2A49)

/* UUIDs for health thermometer profile */
#define HEALTH_THERMOMETER_SERVICE_UUID				(0x1809)
#define TEMPERATURE_MEASUREMENT_CHAR_UUID			(0x2A1C)
#define TEMPERATURE_TYPE_CHAR_UUID					(0x2A1D)
#define INTERMEDIATE_TEMPERATURE_CHAR_UUID			(0x2A1E)
#define MEASUREMENT_INTERVAL_CHAR_UUID				(0x2A21)

/* UUIDs for alert notification profile */
#define ALERT_NOTIFICATION_SERVICE_UUID				(0x1811)
#define SUPPORTED_NEW_ALERT_CATEGORY_CHAR_UUID		(0x2A47)
#define NEW_ALERT_CHAR_UUID							(0x2A46)
#define SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR_UUID	(0x2A48)
#define UNREAD_ALERT_STATUS_CHAR_UUID				(0x2A45)
#define ALERT_NOTIFICATION_CONTROL_POINT_CHAR_UUID	(0x2A44)

/* UUIDs for glucose profile */
#define GLUCOSE_SERVICE_UUID				(0x1808)
#define GLUCOSE_MEASUREMENT_CHAR_UUID			(0x2A18)
#define GLUCOSE_MEASUREMENT_CONTEXT_CHAR_UUID		(0x2A34)
#define GLUCOSE_FEATURE_CHAR_UUID			(0x2A51)
/* Record Access Control Point (RACP) */
#define GLUCOSE_RACP_CHAR_UUID				(0x2A52)

#endif  /* _UUID_H_ */
