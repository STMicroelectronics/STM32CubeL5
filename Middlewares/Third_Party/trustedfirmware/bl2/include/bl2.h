/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __BL2_H__
#define __BL2_H__


#ifdef __cplusplus
extern "C" {
#endif

	struct arm_vector_table {
		uint32_t msp;
		uint32_t reset;
	};

	void jumper(struct arm_vector_table *vt);

	/*!
	 * \brief bl2 main function
	 * \return no return
	 */
	int bl2_main(void);

#ifdef __cplusplus
}
#endif

#endif /* __BL2_H_ */
