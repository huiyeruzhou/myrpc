#ifndef PORT_LOG_PORT_PRT_SCN_H_
#define PORT_LOG_PORT_PRT_SCN_H_
#include <cinttypes>
// #if __TIMESIZE == 64
#define PRItime_t PRIi64
#define SCNtime_t SCNi64
// #else
// #define PRItime_t PRIi32
// #define SCNtime_t SCNi32
// #endif
#define PRIin_port_t PRIu16
#define SCNin_port_t SCNu16

#endif  // PORT_LOG_PORT_PRT_SCN_H_
