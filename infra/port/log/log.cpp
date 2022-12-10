//
// Created by huiyeruzhou on 2022/12/10.
//

#include "log.h"

#if CONFIG_HAS_POSIX
extern const std::time_t begin = _getTimeStamp();

std::time_t _getTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    return timestamp;
}

// static std::tm *gettm(uint64_t timestamp)
// {
//     uint64_t milli = timestamp;
//     milli += (uint64_t) 8 * 60 * 60 * 1000;//add to beijing time zone.
//     auto mTime = std::chrono::milliseconds(milli);
//     auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(mTime);
//     auto tt = std::chrono::system_clock::to_time_t(tp);
//     std::tm *now = std::gmtime(&tt);
//     return now;
// }

// static std::string getTimeStr()
// {
//     time_t timep;
//     timep = getTimeStamp();
//     struct tm *info;
//     info = gettm(timep);

//     char tmp[27] = { 0 };
//     printf("%02d:%02d:%02d.%06ld", info->tm_hour, info->tm_min, info->tm_sec, timep % 1000000);
//     return tmp;
// }
static void _printTime()
{
    time_t timep;
    timep = getTimeStamp();

    printf("(%" PRItime_t ")",timep-begin);
}
#endif
