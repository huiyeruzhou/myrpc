#ifndef PORT_LOG_PORT_NET_INFO_HPP_
#define PORT_LOG_PORT_NET_INFO_HPP_
#include "port/config.h"
#include "port/log/port_prt_scn.h"

#if CONFIG_HAS_FREERTOS
extern "C" {
#include <arpa/inet.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
}
#else

extern "C" {
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
}
#endif
#include <cstdio>
#include <cstring>
void sprint_net_info(char *netinfo, int netinfo_len, const sockaddr *__sockaddr,
                     int __len);
int get_port_from_addr(const sockaddr *__sockaddr, int __len);

#endif  // PORT_LOG_PORT_NET_INFO_HPP_
