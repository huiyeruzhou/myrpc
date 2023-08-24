/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: port_net_info.hpp
 * Description: This file defines the portable net info function
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
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
void sprint_net_info(char *netinfo, int netinfo_len, const sockaddr *__sockaddr, int __len);
int get_port_from_addr(const sockaddr *__sockaddr, int __len);

#endif  // PORT_LOG_PORT_NET_INFO_HPP_
