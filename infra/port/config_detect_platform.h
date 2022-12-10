//
// Created by huiyeruzhou on 2022/12/10.
//

#ifndef RPC_DETECT_PLATFORM_H
#define RPC_DETECT_PLATFORM_H



////////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////////

//! @name Threading model options
//@{



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
#endif //RPC_CONFIG_FLAG_H
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
//    #pragma message("FreeRTOS detected")
            #undef CONFIG_HAS_FREERTOS
            #define CONFIG_HAS_FREERTOS (1)
        #endif
    #endif
#endif

#endif //RPC_DETECT_PLATFORM_H
