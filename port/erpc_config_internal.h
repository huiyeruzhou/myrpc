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


   ////////////////////////////////////////////////////////////////////////////////
   // Declarations
   ////////////////////////////////////////////////////////////////////////////////

   //! @name Threading model options
   //@{

// Set default buffer size.
#ifndef ERPC_DEFAULT_BUFFER_SIZE
    //! @brief Size of buffers allocated by BasicMessageBufferFactory in setup functions.
#define ERPC_DEFAULT_BUFFER_SIZE (256U)
#endif

#ifndef CONFIG_MAX_TASK_NAME_LEN
    //! @brief Size of 
#define CONFIG_MAX_TASK_NAME_LEN (16U)
#endif

#define ERPC_ALLOCATION_POLICY_DYNAMIC (0U) //!< Dynamic allocation policy

#define ERPC_THREADS_PTHREADS (1U) //!< POSIX pthreads.
#define ERPC_THREADS_FREERTOS (2U) //!< FreeRTOS.

#define ERPC_NOEXCEPT_DISABLED (0U) //!< Disabling noexcept feature.
#define ERPC_NOEXCEPT_ENABLED (1U)  //!<  Enabling noexcept feature.n enabled..

//@}

//! @name Configuration options
//@{

//! @def ERPC_ALLOCATION_POLICY
//!
//! @brief Choose which allocation policy should be used.
//!
//! Set ERPC_ALLOCATION_POLICY_DYNAMIC if dynamic allocations should be used.
//! Set ERPC_ALLOCATION_POLICY_STATIC if static allocations should be used.
//!
//! Default value is ERPC_ALLOCATION_POLICY_DYNAMIC or in case of FreeRTOS it can be auto-detected if __has_include() is
//! supported by compiler. Uncomment comment bellow to use static allocation policy. In case of static implementation
//! user need consider another values to set (ERPC_CODEC_COUNT, ERPC_MESSAGE_LOGGERS_COUNT,
//! ERPC_CLIENTS_THREADS_AMOUNT).
// #define ERPC_ALLOCATION_POLICY (ERPC_ALLOCATION_POLICY_STATIC)


//! @def ERPC_THREADS
//!
//! @brief Select threading model.
//!
//! Set to one of the @c ERPC_THREADS_x macros to specify the threading model used by eRPC.
//!
//! Leave commented out to attempt to auto-detect. Auto-detection works well for pthreads.
//! FreeRTOS can be detected when building with compilers that support __has_include().
//! Otherwise, the default is no threading.
//#define ERPC_THREADS (ERPC_THREADS_FREERTOS)

//! @def ERPC_DEFAULT_BUFFER_SIZE
//!
//! Uncomment to change the size of buffers allocated by one of MessageBufferFactory.
//! (@ref client_setup and @ref server_setup). The default size is set to 256.
//! For RPMsg transport layer, ERPC_DEFAULT_BUFFER_SIZE must be 2^n - 16.
//#define ERPC_DEFAULT_BUFFER_SIZE (256U)

//! @def ERPC_NOEXCEPT
//!
//! @brief Disable/enable noexcept support.
//!
//! Uncomment for using noexcept feature.
//#define ERPC_NOEXCEPT (ERPC_NOEXCEPT_ENABLED)

//! @name Assert function definition
//@{
//! User custom asser defition. Include header file if needed before bellow line. If assert is not enabled, default will
//! be used.
// #define erpc_assert(condition)
//@}

//! @def ENDIANES_HEADER
//!
//! Include header file that controls the communication endianness
//!
//! Uncomment for example behaviour for endianness agnostic with:
//!  1. communication in little endian.
//!  2. current processor is big endian.
//!  3. pointer size is 32 bit.
//!  4. float+double scheme not defined, so throws assert if passes.
//! #define ERPC_PROCESSOR_ENDIANNESS_LITTLE 0
//! #define ERPC_COMMUNICATION_LITTLE        1
//! #define ERPC_POINTER_SIZE_16             0
//! #define ERPC_POINTER_SIZE_32             1
//! #define ERPC_POINTER_SIZE_64             0
//! #define ENDIANNESS_HEADER "erpc_endianness_agnostic_example.h"


////////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////////
/* clang-format off */

// Determine if this is a POSIX system.
#ifndef CONFIG_HAS_POSIX
    // Detect Linux, BSD, Cygwin, and Mac OS X.
    #if defined(__linux__) || defined(__GNU__) || defined(__FreeBSD__) || defined(__NetBSD__) || \
        defined(__OpenBSD__) || defined(__DragonFly__) || defined(__CYGWIN__) || defined(__MACH__)
        #define CONFIG_HAS_POSIX (1)
    #else
        #define CONFIG_HAS_POSIX (0)
    #endif
#endif

// Safely detect FreeRTOSConfig.h.
#ifndef CONFIG_HAS_FREERTOS
    #define CONFIG_HAS_FREERTOS (0)
    #if defined(__has_include)
    #if __has_include("FreeRTOSConfig.h")
    #pragma message("FreeRTOS detected")
    #undef CONFIG_HAS_FREERTOS
            #define CONFIG_HAS_FREERTOS (1)
        #endif
    #endif
#endif



#include <new>
#define ERPC_ALLOCATION_POLICY (ERPC_ALLOCATION_POLICY_DYNAMIC)

#if CONFIG_HAS_FREERTOS
#include "esp_log.h"
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define LOGE( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR,   tag, format __VA_OPT__(,) __VA_ARGS__)
#define LOGW( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_WARN,    tag, format __VA_OPT__(,) __VA_ARGS__)
#define LOGI( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO,    tag, format __VA_OPT__(,) __VA_ARGS__)
#define LOGD( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_DEBUG,   tag, format __VA_OPT__(,) __VA_ARGS__)
#define LOGV( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_VERBOSE, tag, format __VA_OPT__(,) __VA_ARGS__)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define LOGE( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR,   tag, format, ##__VA_ARGS__)
#define LOGW( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_WARN,    tag, format, ##__VA_ARGS__)
#define LOGI( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO,    tag, format, ##__VA_ARGS__)
#define LOGD( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_DEBUG,   tag, format, ##__VA_ARGS__)
#define LOGV( tag, format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))
#elif CONFIG_HAS_POSIX
// #include <ctime>
// #include <iostream>
// #include <iomanip>
// static std::time_t time_now = std::time(nullptr);
// #define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)    // only show filename and not it's path (less clutter)
// #define LOG(LEVEL) std::cout << std::put_time(std::localtime(&time_now), "%y-%m-%d %OH:%OM:%OS") << " [LEVEL] " << __FILENAME__ << "(" << __FUNCTION__ << ":" << __LINE__ << ") >> "

#define LOGE( tag, format, ... ) 
#define LOGW( tag, format, ... ) 
#define LOGI( tag, format, ... )
#define LOGD( tag, format, ... )
#define LOGV( tag, format, ... ) 
#endif

// Detect threading model if not already set.
#ifndef ERPC_THREADS
    #if CONFIG_HAS_POSIX
        // Default to pthreads for POSIX systems.
        #define ERPC_THREADS (ERPC_THREADS_PTHREADS)
    #elif CONFIG_HAS_FREERTOS
        // Use FreeRTOS if we can auto detect it.
        #define ERPC_THREADS (ERPC_THREADS_FREERTOS)
    #endif
#endif

// Handy macro to test threading model. You can also ERPC_THREADS directly to test for threading
// support, i.e. "#if ERPC_THREADS", because ERPC_THREADS_NONE has a value of 0.
#define ERPC_THREADS_IS(_n_) (ERPC_THREADS == (ERPC_THREADS_##_n_))




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

#define ERPC_TRANSPORT_MU_USE_MCMGR (ERPC_TRANSPORT_MU_USE_MCMGR_DISABLED)


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
