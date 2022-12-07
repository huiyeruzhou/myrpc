/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ERPC_MBF_SETUP_H_
#define _ERPC_MBF_SETUP_H_


/*!
 * @addtogroup message_buffer_factory_setup
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

//! @brief Opaque MessageBufferFactory object type.
typedef struct ErpcMessageBufferFactory *erpc_mbf_t;

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

//! @name MessageBufferFactory setup
//@{

/*!
 * @brief Create MessageBuffer factory which is using static allocated buffers.
 */
erpc_mbf_t erpc_mbf_static_init(void);

/*!
 * @brief Create MessageBuffer factory which is using dynamic allocated buffers.
 */
erpc_mbf_t erpc_mbf_dynamic_init(void);

//@}

#ifdef __cplusplus
}
#endif

/*! @} */

#endif // _ERPC_MBF_SETUP_H_
