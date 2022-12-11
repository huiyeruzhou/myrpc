//
// Created by huiyeruzhou on 2022/12/10.
//

#ifndef PORT_LOG_H
#define PORT_LOG_H
#include "../config.h"
#include "port_prt_scn.h"
#include "port_net_info.hpp"

#define color_default     "\033[00m"
#define color_bold     "\033[01m"
#define color_red     "\033[31m"
#define color_green     "\033[32m"
#define color_yellow     "\033[33m"
#define color_blue     "\033[34m"
#define color_magenta     "\033[35m"
#define color_cyan     "\033[36m"
#define color_orange     "\033[38;5;172m"
#define color_light_blue     "\033[38;5;039m"
#define color_gray     "\033[38;5;008m"
#define color_purple     "\033[38;5;097m"

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
//线程安全性还有待检查, 但已经避免了对任何全局变量进行操作(begin是全局常量), 不过
//如果一个线程运行途中发生了上下文切换, 日志打印可能会出现交错
#include <string>
#include <chrono>
extern const std::time_t begin;
std::time_t _getTimeStamp();
void _printTime();
#define colorE color_red
#define colorW color_yellow
#define colorI color_green
#define colorD color_default
#define colorV color_default
#define Istr "I"
#define Dstr "D"
#define Estr "E"
#define Wstr "W"
#define Vstr "V"
#define FILENAME basename(__FILE__)
#define FUNCNAME __func__
#define printLevel(level) printf(color##level " " level##str " ");
#define printTrace() printf("%s[%s]: ", FILENAME, FUNCNAME);
#define printTAG(tag) printf(" %s: ", tag);

#define LOGE( tag, format, arg... ) printLevel(E);_printTime();printTAG(tag)printf(format, ##arg);printf(color_default "\n");
#define LOGW( tag, format, arg... ) printLevel(W);_printTime();printTAG(tag)printf(format, ##arg);printf(color_default "\n");
#define LOGI( tag, format, arg... ) printLevel(I);_printTime();printTAG(tag)printf(format, ##arg);printf(color_default "\n");
#define LOGD( tag, format, arg... ) printLevel(D);_printTime();printTAG(tag)printf(format, ##arg);printf(color_default "\n");
#define LOGV( tag, format, arg... ) printLevel(V);_printTime();printTAG(tag)printf(format, ##arg);printf(color_default "\n");
#endif
#endif //PORT_LOG_H
