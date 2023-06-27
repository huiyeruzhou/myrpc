//
// Created by huiyeruzhou on 2022/12/12.
//

#ifndef RPC_PORT_NET_INFO_H
#define RPC_PORT_NET_INFO_H
#include "port/config.h"
#include "port/log/port_prt_scn.h"

#if CONFIG_HAS_FREERTOS
#include <cstdio>
extern "C" {
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <arpa/inet.h>
}
#else
#include <cstdio>
extern "C" {
#include <netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
}
#endif
#include <cstring>
void sprint_net_info(char *netinfo, int netinfo_len, const sockaddr *__sockaddr, int __len);
int getPortFormAddr(const sockaddr * __sockaddr, int __len);

#endif //RPC_PORT_NET_INFO_H
