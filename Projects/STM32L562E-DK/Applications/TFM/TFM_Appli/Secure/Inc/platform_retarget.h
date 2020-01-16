/*
 * Copyright (c) 2017-2018 Arm Limited
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file platform_retarget.h
 * \brief This file defines all the peripheral base addresses  */

#ifndef __STM_L552ZE_Q_H__
#define __STM_L552ZE_Q_H__



/* ======= Peripheral configuration structure declarations */

#include "stm32l5xx.h"           /* Platform registers */
#include "system_cmsdk_stm32l5xx.h"


/* Internal SRAM memory */

#define MPC_ISRAM1_RANGE_BASE_NS   0x20000000
#define MPC_ISRAM1_RANGE_LIMIT_NS  0x2002FFFF
#define MPC_ISRAM1_RANGE_BASE_S    0x30000000
#define MPC_ISRAM1_RANGE_LIMIT_S   0x3002FFFF

#define MPC_ISRAM2_RANGE_BASE_NS   0x20030000
#define MPC_ISRAM2_RANGE_LIMIT_NS  0x2003FFFF
#define MPC_ISRAM2_RANGE_BASE_S    0x30030000
#define MPC_ISRAM2_RANGE_LIMIT_S   0x3003FFFF



/* Internal Flash memory */
#define MPC_FLASH_RANGE_BASE_NS        (0x08000000)
#define MPC_FLASH_RANGE_LIMIT_NS       (0x08080000)
#define MPC_FLASH_RANGE_BASE_S         (0x0c000000)
#define MPC_FLASH_RANGE_LIMIT_S        (0x0c080000)
#endif /* __STM_NUCLEO_L552ZE_Q_H__ */
