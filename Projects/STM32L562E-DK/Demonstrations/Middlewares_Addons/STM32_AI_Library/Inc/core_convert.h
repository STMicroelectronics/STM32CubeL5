/**
  ******************************************************************************
  * @file    core_utils.h
  * @author  AST Embedded Analytics Research Platform
  * @date    16-Aug-2018
  * @brief   header file of core utils routines
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef __CORE_CONVERT_H_
#define __CORE_CONVERT_H_
#pragma once

#include "ai_platform.h"
#include "ai_platform_interface.h"

#include "core_common.h"

AI_API_DECLARE_BEGIN

/*!
 * @defgroup core_convert Core Convert Routines
 * @brief Implementation of core node format conversion routines
 *   (Q7 to float, ... etc.)
 */


/*!
 * @brief Convert tensors from float to quantized or vice-versa
 * @ingroup core_convert
 * @param[in] pNode in a handler to node (layer or operator)
 */
AI_INTERNAL_API
void node_convert(ai_node *pNode);

/*!
 * @brief Convert integer tensors between QM.N formats (8/16 bits)
 * @ingroup core_convert
 * @param[in] pNode in a handler to node (layer or operator)
 */
AI_INTERNAL_API
void node_convert_fixed(ai_node *pNode);

/*!
 * @brief Convert integer tensors between signed and unsigned (int8/uint8) formats
 * @ingroup core_convert
 * @param[in] pNode in a handler to node (layer or operator)
 */
AI_INTERNAL_API
void node_convert_integer(ai_node *pNode);

/*!
 * @brief Convert a shape struct into a stride struct
 * @ingroup core_convert
 * @param[in] in a pointer to a shape to convert
 * @return a condverted stride datastruct
 */
AI_INTERNAL_API
void core_shape_to_stride(ai_stride* out, const ai_shape* in);


#endif    /*__CORE_CONVERT_H_*/
