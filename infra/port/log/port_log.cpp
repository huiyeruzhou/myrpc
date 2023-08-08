#include "port/log/port_log.h"

#if CONFIG_HAS_POSIX && !CONFIG_HAS_ANDROID
extern const std::time_t begin = _getTimeStamp();

std::time_t _getTimeStamp() {
  std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>
      tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now());
  auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(
      tp.time_since_epoch());
  std::time_t timestamp = tmp.count();
  return timestamp;
}

void _printTime() {
  time_t timep;
  timep = _getTimeStamp();

  printf("(%" PRItime_t ")", timep - begin);
}
#endif
