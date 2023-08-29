/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: port_log.cpp
 * Description: the portable log implementation
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#include "port/log/port_log.h"

#if CONFIG_HAS_POSIX && !CONFIG_HAS_ANDROID
static const std::time_t begin = _getTimeStamp();

std::time_t _getTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    return timestamp;
}

void _printTime()
{
    time_t timep;
    timep = _getTimeStamp();

    printf("(%" PRItime_t ")", timep - begin);
}
#endif
