//
// Created by huiyeruzhou on 2022/12/10.
//

#ifndef RPC_PORT_PRT_SCN_H
#define RPC_PORT_PRT_SCN_H
#include <cinttypes>
//#if __TIMESIZE == 64
#define PRItime_t PRIi64
#define SCNtime_t SCNi64
//#else
//#define PRItime_t PRIi32
//#define SCNtime_t SCNi32
//#endif
#define PRIin_port_t PRIu16
#define SCNin_port_t SCNu16
#endif //RPC_PORT_PRT_SCN_H
