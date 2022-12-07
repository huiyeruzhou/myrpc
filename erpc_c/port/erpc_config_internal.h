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
#define ERPC_ALLOCATION_POLICY_DYNAMIC (0U) //!< Dynamic allocation policy
#define ERPC_ALLOCATION_POLICY_STATIC (1U)  //!< Static allocation policy

#define ERPC_THREADS_NONE (0U)     //!< No threads.
#define ERPC_THREADS_PTHREADS (1U) //!< POSIX pthreads.
#define ERPC_THREADS_FREERTOS (2U) //!< FreeRTOS.
#define ERPC_THREADS_ZEPHYR (3U)   //!< ZEPHYR.
#define ERPC_THREADS_MBED (4U)     //!< Mbed OS
#define ERPC_THREADS_WIN32 (5U)    //!< WIN32
#define ERPC_THREADS_THREADX (6U)  //!< THREADX

#define ERPC_NOEXCEPT_DISABLED (0U) //!< Disabling noexcept feature.
#define ERPC_NOEXCEPT_ENABLED (1U)  //!<  Enabling noexcept feature.

#define ERPC_NESTED_CALLS_DISABLED (0U) //!< No nested calls support.
#define ERPC_NESTED_CALLS_ENABLED (1U)  //!< Nested calls support.

#define ERPC_NESTED_CALLS_DETECTION_DISABLED (0U) //!< Nested calls detection disabled.
#define ERPC_NESTED_CALLS_DETECTION_ENABLED (1U)  //!< Nested calls detection enabled.

#define ERPC_MESSAGE_LOGGING_DISABLED (0U) //!< Trace functions disabled.
#define ERPC_MESSAGE_LOGGING_ENABLED (1U)  //!< Trace functions enabled.

#define ERPC_TRANSPORT_MU_USE_MCMGR_DISABLED (0U) //!< Do not use MCMGR for MU ISR management.
#define ERPC_TRANSPORT_MU_USE_MCMGR_ENABLED (1U)  //!< Use MCMGR for MU ISR management.

#define ERPC_PRE_POST_ACTION_DISABLED (0U) //!< Pre post shim callbacks functions disabled.
#define ERPC_PRE_POST_ACTION_ENABLED (1U)  //!< Pre post shim callback functions enabled.

#define ERPC_PRE_POST_ACTION_DEFAULT_DISABLED (0U) //!< Pre post shim default callbacks functions disabled.
#define ERPC_PRE_POST_ACTION_DEFAULT_ENABLED (1U)  //!< Pre post shim default callback functions enabled.
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

//! @def ERPC_CODEC_COUNT
//!
//! @brief Set amount of codecs objects used simultaneously in case of ERPC_ALLOCATION_POLICY is set to
//! ERPC_ALLOCATION_POLICY_STATIC. For example if client or server is used in one thread then 1. If both are used in one
//! thread per each then 2, ... Default value 2.
// #define ERPC_CODEC_COUNT (2U)

//! @def ERPC_MESSAGE_LOGGERS_COUNT
//!
//! @brief Set amount of message loggers objects used simultaneously  in case of ERPC_ALLOCATION_POLICY is set to
//! ERPC_ALLOCATION_POLICY_STATIC.
//! For example if client or server is used in one thread then 1. If both are used in one thread per each then 2, ...
//! For arbitrated client 1 is enough.
//! Default value 0 (May not be used).
// #define ERPC_MESSAGE_LOGGERS_COUNT (0U)

//! @def ERPC_CLIENTS_THREADS_AMOUNT
//!
//! @brief Set amount of client threads objects used in case of ERPC_ALLOCATION_POLICY is set to
//! ERPC_ALLOCATION_POLICY_STATIC. Default value 1 (Most of current cases).
// #define ERPC_CLIENTS_THREADS_AMOUNT (1U)

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

//! @def ERPC_DEFAULT_BUFFERS_COUNT
//!
//! Uncomment to change the count of buffers allocated by one of statically allocated messages.
//! Default value is set to 2.
//#define ERPC_DEFAULT_BUFFERS_COUNT (2U)

//! @def ERPC_NOEXCEPT
//!
//! @brief Disable/enable noexcept support.
//!
//! Uncomment for using noexcept feature.
//#define ERPC_NOEXCEPT (ERPC_NOEXCEPT_ENABLED)

//! @def ERPC_NESTED_CALLS
//!
//! Default set to ERPC_NESTED_CALLS_DISABLED. Uncomment when callbacks, or other eRPC
//! functions are called from server implementation of another eRPC call. Nested functions
//! need to be marked as @nested in IDL.
//#define ERPC_NESTED_CALLS (ERPC_NESTED_CALLS_ENABLED)

//! @def ERPC_NESTED_CALLS_DETECTION
//!
//! Default set to ERPC_NESTED_CALLS_DETECTION_ENABLED when NDEBUG macro is presented.
//! This serve for locating nested calls in code. Nested calls are calls where inside eRPC function
//! on server side is called another eRPC function (like callbacks). Code need be a bit changed
//! to support nested calls. See ERPC_NESTED_CALLS macro.
//#define ERPC_NESTED_CALLS_DETECTION (ERPC_NESTED_CALLS_DETECTION_DISABLED)

//! @def ERPC_MESSAGE_LOGGING
//!
//! Enable eRPC message logging code through the eRPC. Take look into "erpc_message_loggers.h". Can be used for base
//! printing messages, or sending data to another system for data analysis. Default set to
//! ERPC_MESSAGE_LOGGING_DISABLED.
//!
//! Uncomment for using logging feature.
//#define ERPC_MESSAGE_LOGGING (ERPC_MESSAGE_LOGGING_ENABLED)

//! @def ERPC_TRANSPORT_MU_USE_MCMGR
//!
//! @brief MU transport layer configuration.
//!
//! Set to one of the @c ERPC_TRANSPORT_MU_USE_MCMGR_x macros to configure the MCMGR usage in MU transport layer.
//!
//! MU transport layer could leverage the Multicore Manager (MCMGR) component for Inter-Core
//! interrupts / MU interrupts management or the Inter-Core interrupts can be managed by itself (MUX_IRQHandler
//! overloading). By default, ERPC_TRANSPORT_MU_USE_MCMGR is set to ERPC_TRANSPORT_MU_USE_MCMGR_ENABLED when mcmgr.h
//! is part of the project, otherwise the ERPC_TRANSPORT_MU_USE_MCMGR_DISABLED option is used. This settings can be
//! overwritten from the erpc_config.h by uncommenting the ERPC_TRANSPORT_MU_USE_MCMGR macro definition. Do not forget
//! to add the MCMGR library into your project when ERPC_TRANSPORT_MU_USE_MCMGR_ENABLED option is used! See the
//! erpc_mu_transport.h for additional MU settings.
//#define ERPC_TRANSPORT_MU_USE_MCMGR ERPC_TRANSPORT_MU_USE_MCMGR_DISABLED
//@}

//! @def ERPC_PRE_POST_ACTION
//!
//! Enable eRPC pre and post callback functions shim code. Take look into "erpc_pre_post_action.h". Can be used for
//! detection of eRPC call freeze, ... Default set to ERPC_PRE_POST_ACTION_DISABLED.
//!
//! Uncomment for using pre post callback feature.
//#define ERPC_PRE_POST_ACTION (ERPC_PRE_POST_ACTION_ENABLED)

//! @def ERPC_PRE_POST_ACTION_DEFAULT
//!
//! Enable eRPC pre and post default callback functions. Take look into "erpc_setup_extensions.h". Can be used for
//! detection of eRPC call freeze, ... Default set to ERPC_PRE_POST_ACTION_DEFAULT_DISABLED.
//!
//! Uncomment for using pre post default callback feature.
//#define ERPC_PRE_POST_ACTION_DEFAULT (ERPC_PRE_POST_ACTION_DEFAULT_ENABLED)

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
#if !defined(CONFIG_HAS_POSIX)
    // Detect Linux, BSD, Cygwin, and Mac OS X.
    #if defined(__linux__) || defined(__GNU__) || defined(__FreeBSD__) || defined(__NetBSD__) || \
        defined(__OpenBSD__) || defined(__DragonFly__) || defined(__CYGWIN__) || defined(__MACH__)
        #define CONFIG_HAS_POSIX (1)
    #else
        #define CONFIG_HAS_POSIX (0)
    #endif
#endif

// Determine if we are targeting WIN32 environment
#if !defined(CONFIG_HAS_WIN32)
    #if defined(_WIN32)
        #define CONFIG_HAS_WIN32 (1)
    #else
        #define CONFIG_HAS_WIN32 (0)
    #endif
#endif

// Safely detect FreeRTOSConfig.h.
#if !defined(CONFIG_HAS_FREERTOS)
    #define CONFIG_HAS_FREERTOS (0)
    #if defined(__has_include)
        #if __has_include("FreeRTOSConfig.h")
            #undef CONFIG_HAS_FREERTOS
            #define CONFIG_HAS_FREERTOS (1)
        #endif
    #endif
#endif

// Detect allocation policy if not already set.
#if !defined(ERPC_ALLOCATION_POLICY)
    #if CONFIG_HAS_FREERTOS
        #ifdef __cplusplus
            extern "C" {
        #endif
        #include "FreeRTOSConfig.h"
        #ifdef __cplusplus
            }
        #endif
        #if defined(configSUPPORT_STATIC_ALLOCATION) && configSUPPORT_STATIC_ALLOCATION
            #define ERPC_ALLOCATION_POLICY (ERPC_ALLOCATION_POLICY_STATIC)
        #else
            #define ERPC_ALLOCATION_POLICY (ERPC_ALLOCATION_POLICY_DYNAMIC)
        #endif
    #else
        #define ERPC_ALLOCATION_POLICY (ERPC_ALLOCATION_POLICY_DYNAMIC)
    #endif
#endif

#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_STATIC
    #if !defined(ERPC_CODEC_COUNT)
        #define ERPC_CODEC_COUNT (2U)
    #endif
    #if !defined(ERPC_MESSAGE_LOGGERS_COUNT)
        #define ERPC_MESSAGE_LOGGERS_COUNT (0U)
    #endif
    #if !defined(ERPC_CLIENTS_THREADS_AMOUNT)
        #define ERPC_CLIENTS_THREADS_AMOUNT (1U)
    #endif
#endif

// Detect threading model if not already set.
#if !defined(ERPC_THREADS)
    #if CONFIG_HAS_POSIX
        // Default to pthreads for POSIX systems.
        #define ERPC_THREADS (ERPC_THREADS_PTHREADS)
    #elif CONFIG_HAS_FREERTOS
        // Use FreeRTOS if we can auto detect it.
        #define ERPC_THREADS (ERPC_THREADS_FREERTOS)
    #elif CONFIG_HAS_WIN32
        #define ERPC_THREADS (ERPC_THREADS_WIN32）
    #else
        // Otherwise default to no threads.
        #define ERPC_THREADS (ERPC_THREADS_NONE)
    #endif
#endif

// Handy macro to test threading model. You can also ERPC_THREADS directly to test for threading
// support, i.e. "#if ERPC_THREADS", because ERPC_THREADS_NONE has a value of 0.
#define ERPC_THREADS_IS(_n_) (ERPC_THREADS == (ERPC_THREADS_##_n_))

// Set default buffer size.
#if !defined(ERPC_DEFAULT_BUFFER_SIZE)
    //! @brief Size of buffers allocated by BasicMessageBufferFactory in setup functions.
    #define ERPC_DEFAULT_BUFFER_SIZE (256U)
#endif

// Set default buffers count.
#if !defined(ERPC_DEFAULT_BUFFERS_COUNT)
    //! @brief Count of buffers allocated by StaticMessageBufferFactory.
    #define ERPC_DEFAULT_BUFFERS_COUNT (2U)
#endif

// Disable/enable noexcept.
#if !defined(ERPC_NOEXCEPT)
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


// Enabling nesting calls detection as default for debug.
#if !defined(ERPC_NESTED_CALLS_DETECTION)
    #if defined(NDEBUG) || (ERPC_NESTED_CALLS == ERPC_NESTED_CALLS_ENABLED)
        #define ERPC_NESTED_CALLS_DETECTION (ERPC_NESTED_CALLS_DETECTION_DISABLED)
    #else
        #define ERPC_NESTED_CALLS_DETECTION (ERPC_NESTED_CALLS_DETECTION_ENABLED)
    #endif
#endif

// Disabling tracing the eRPC.
#if !defined(ERPC_MESSAGE_LOGGING)
    #define ERPC_MESSAGE_LOGGING (ERPC_MESSAGE_LOGGING_DISABLED)
#endif

#if defined(__CC_ARM) || defined(__ARMCC_VERSION) /* Keil MDK */
    #define THROW_BADALLOC throw(std::bad_alloc)
    #define THROW throw()
#else
    #define THROW_BADALLOC
    #define THROW
#endif

#define ERPC_TRANSPORT_MU_USE_MCMGR (ERPC_TRANSPORT_MU_USE_MCMGR_DISABLED)


#if !defined(erpc_assert)
    #if CONFIG_HAS_FREERTOS
        #ifdef __cplusplus
            extern "C" {
        #endif
        #include "FreeRTOS.h"
        #include "task.h"
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

// // Disabling endianness agnostic feature.
// #ifndef ENDIANNESS_HEADER
//     #define ENDIANNESS_HEADER "erpc_endianness_undefined.h"
// #endif

/* clang-format on */
#endif // _ERPC_DETECT_H_
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
