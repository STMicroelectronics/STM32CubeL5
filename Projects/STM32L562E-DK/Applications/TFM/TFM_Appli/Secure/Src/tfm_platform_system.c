/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform/include/tfm_platform_system.h"
#include "cmsis.h"
void Error_Handler(void)
{
	/* Reset the system */
    NVIC_SystemReset();
}


void tfm_platform_hal_system_reset(void)
{
    /* Reset the system */
    NVIC_SystemReset();
}

enum tfm_plat_err_t
tfm_platform_hal_pin_service(const psa_invec  *in_vec,  uint32_t num_invec,
                             const psa_outvec *out_vec, uint32_t num_outvec)
{
	return TFM_PLAT_ERR_SYSTEM_ERR;
} 
