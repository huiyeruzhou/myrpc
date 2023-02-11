//
// Created by huiyeruzhou on 2022/12/12.
//

#ifndef RPC_PORT_NET_INFO_H
#define RPC_PORT_NET_INFO_H
#include "config.h"
#include "port_prt_scn.h"

#if CONFIG_HAS_FREERTOS
#include <cstdio>
extern "C" {
#include <lwip/netdb.h>
#include <lwip/sockets.h>
}
#else
#include <cstdio>
extern "C" {
#include <netdb.h>
}
#endif
#include <cstring>
void sprint_net_info(char *netinfo, int netinfo_len, const sockaddr *__sockaddr, int __len);
int getPortFormAddr(const sockaddr * __sockaddr, int __len);

#endif //RPC_PORT_NET_INFO_H
