/**
  ******************************************************************************
  * @file    motion_cp.h
  * @author  MEMS Application Team
  * @version V2.0.2
  * @date    15-February-2019
  * @brief   Header for motion_cp module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
  ********************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MOTION_CP_H_
#define _MOTION_CP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup MIDDLEWARES
  * @{
  */

/** @defgroup MOTION_CP MOTION_CP
  * @{
  */


/** @defgroup MOTION_CP_Exported_Types MOTION_CP_Exported_Types
 * @{
 */
/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
 typedef struct
{
  float AccX;           /* Acceleration in X axis in [g] */
  float AccY;           /* Acceleration in Y axis in [g] */
  float AccZ;           /* Acceleration in Z axis in [g] */
} MCP_input_t;

typedef enum
{
  MCP_UNKNOWN             = 0x00,
  MCP_ONDESK              = 0x01,
  MCP_INHAND              = 0x02,
  MCP_NEARHEAD            = 0x03,
  MCP_SHIRTPOCKET         = 0x04,
  MCP_TROUSERPOCKET       = 0x05,
  MCP_ARMSWING            = 0x06,
  MCP_JACKETPOCKET        = 0x07
} MCP_output_t;

/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/** @defgroup MOTION_CP_Exported_Functions MOTION_CP_Exported_Functions
 * @{
 */

/* Exported functions ------------------------------------------------------- */

/**
 * @brief  Initialize the MotionCP engine
 * @param  none
 * @retval none
 */
void MotionCP_Initialize(void);

/**
 * @brief  Set the MotionCP accelerometer data orientation
 * @param  *acc_orientation: reference system of the accelerometer raw data (for instance: south west up became "swu", north east up became "ned")
 * @retval  none
 */
void MotionCP_SetOrientation_Acc(const char *acc_orientation);

/**
 * @brief  Run carry position recognition algorithm
 * @param  data_in  pointer to accaleration in [g]
 * @param  data_out  pointer to carry position index
 * @retval none
 */
void MotionCP_Update(MCP_input_t *data_in, MCP_output_t *data_out);

/**
 * @brief  Get the library version
 * @param  version pointer to an array of 35 char
 * @retval Number of characters in the version string
 */
uint8_t MotionCP_GetLibVersion(char *version);


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _MOTION_CP_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
