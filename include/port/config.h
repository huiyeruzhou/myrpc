//
// Created by huiyeruzhou on 2022/12/10.
//

#ifndef CONFIG_H
#define CONFIG_H


////////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////////

// Determine if this is a POSIX system.
#ifndef CONFIG_HAS_ANDROID
#define CONFIG_HAS_ANDROID (0)
#if defined(__has_include)
#if __has_include("Android/log.h")
#undef CONFIG_HAS_ANDROID
#define CONFIG_HAS_ANDROID (1)
#endif//__has_include("Android/log.h")
#endif//defined(__has_include)
#endif//CONFIG_HAS_ANDROID

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

#endif //CONFIG_H
