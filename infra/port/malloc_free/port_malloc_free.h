/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PORT_MALLOC_FREE_H
#define PORT_MALLOC_FREE_H

#include "../config.h"
#include <new>

#define ERPC_NOEXCEPT_DISABLED (0U) //!< Disabling noexcept feature.
#define ERPC_NOEXCEPT_ENABLED (1U)  //!<  Enabling noexcept feature.n enabled..

// Disable/enable noexcept.
#ifndef ERPC_NOEXCEPT
#if CONFIG_HAS_POSIX
#define ERPC_NOEXCEPT (ERPC_NOEXCEPT_ENABLED)
#else
#define ERPC_NOEXCEPT (ERPC_NOEXCEPT_DISABLED)
#endif
#endif

//NOEXCEPT support
#if defined(__cplusplus) && __cplusplus >= 201103 && ERPC_NOEXCEPT
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif // NOEXCEPT


#if defined(__CC_ARM) || defined(__ARMCC_VERSION) /* Keil MDK */
#define THROW_BADALLOC throw(std::bad_alloc)
    #define THROW throw()
#else
#define THROW_BADALLOC
#define THROW
#endif
/*!
 * @addtogroup port_mem
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/*!
 * @brief This function is used for allocating space.
 *
 * @param[in] size Size of memory block which should be allocated.
 *
 * @return Pointer to allocated space or NULL.
 */
void *erpc_malloc(size_t size);

/*!
 * @brief This function free given memory block.
 *
 * @param[in] ptr Pointer to memory which should be freed, or NULL.
 */
void erpc_free(void *ptr);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif // PORT_MALLOC_FREE_H
