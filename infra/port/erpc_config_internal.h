/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * Copyright 2020 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ERPC_DETECT_H_
#define _ERPC_DETECT_H_

#include "config_detect_platform.h"
#include "malloc_free/dynamic_memory_manage.h"
#include "log/log.h"
#include "threading/erpc_threading.h"

#ifndef ERPC_DEFAULT_BUFFER_SIZE
//! @brief Size of buffers allocated by BasicMessageBufferFactory in setup functions.
#define ERPC_DEFAULT_BUFFER_SIZE (256U)
#endif

#ifndef CONFIG_MAX_PTHREAD_NAME_LEN
//! @brief Size of
    #define CONFIG_MAX_PTHREAD_NAME_LEN (16U)
#endif


#ifndef erpc_assert
    #if CONFIG_HAS_FREERTOS
        #ifdef __cplusplus
            extern "C" {
        #endif
        #include "freertos/FreeRTOS.h"
        #include "freertos/task.h"
        #ifdef __cplusplus
            }
        #endif
        #define erpc_assert(condition) configASSERT(condition)
    #else
        #ifdef __cplusplus
            #include <cassert>
        #else
            #include "assert.h"
        #endif
        #define erpc_assert(condition) assert(condition)
    #endif
#endif

/* clang-format on */
#endif // _ERPC_DETECT_H_
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
