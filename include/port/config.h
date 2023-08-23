/*
 * Copyright 2023 YuHongli
 *
 * File: port_config.h
 * Description: the header to detect the platform
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: YuHongli
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    YuHongli       Create and initialize
 */
#ifndef PORT_CONFIG_H_
#define PORT_CONFIG_H_

// Determine what platform we are on.
#ifndef CONFIG_HAS_ANDROID
#define CONFIG_HAS_ANDROID (0)
#if defined(__has_include)
#if __has_include("Android/log.h")
#undef CONFIG_HAS_ANDROID
#define CONFIG_HAS_ANDROID (1)
#endif  // __has_include("Android/log.h")
#endif  // defined(__has_include)
#endif  // CONFIG_HAS_ANDROID

#ifndef CONFIG_HAS_POSIX
// Detect Linux, BSD, Cygwin, and Mac OS X.
#if defined(__linux__) || defined(__GNU__) || defined(__FreeBSD__) ||        \
    defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) || \
    defined(__CYGWIN__) || defined(__MACH__)
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
#undef CONFIG_HAS_FREERTOS
#define CONFIG_HAS_FREERTOS (1)
#endif
#endif
#endif

#endif  // PORT_CONFIG_H_
