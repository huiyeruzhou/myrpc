/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: port_net_info.cpp
 * Description: the portable net info implementation
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#include "port/log/port_net_info.hpp"

#include "port/log/port_log.h"
#if CONFIG_HAS_FREERTOS
// only declared but not implemented by esp-idf framework, therefore we should
// implement one by ourselves, remember to make sure that it is thread-safe
int getnameinfo(const struct sockaddr *addr, socklen_t addrlen, char *host, socklen_t hostlen, char *serv, socklen_t servlen, int flags)
{
    auto casted_addr = reinterpret_cast<const struct sockaddr_in *>(addr);
    inet_ntop(AF_INET, &(casted_addr->sin_addr), host, hostlen);
    snprintf(serv, servlen, "%" SCNin_port_t, ntohs(casted_addr->sin_port));
    return 0;
}
#endif
void sprint_net_info(char *netinfo, int netinfo_len, const sockaddr *__sockaddr, int __len)
{
    // get host ip by sockaddr
    char host[18];
    if (__sockaddr->sa_family == AF_INET) {
        inet_ntop(AF_INET, &(reinterpret_cast<const sockaddr_in *>(__sockaddr))->sin_addr, host, 16);
    } else {
        snprintf(host, sizeof(host), "AF:%d-notipv4", __sockaddr->sa_family);
    }
    snprintf(netinfo, netinfo_len, "%s:%d", host, get_port_from_addr(__sockaddr, __len));
}

int get_port_from_addr(const sockaddr *__sockaddr, int __len)
{
    if (__sockaddr->sa_family == AF_INET) {
        auto casted_addr = reinterpret_cast<const struct sockaddr_in *>(__sockaddr);
        return ntohs(casted_addr->sin_port);
    } else if (__sockaddr->sa_family == AF_INET6) {
        auto casted_addr = reinterpret_cast<const struct sockaddr_in6 *>(__sockaddr);
        return ntohs(casted_addr->sin6_port);
    } else {
        LOGW("net info log", "unknown SA_FAMILY: %d, interpret as AF_INET", __sockaddr->sa_family);
        auto casted_addr = reinterpret_cast<const struct sockaddr_in *>(__sockaddr);
        return ntohs(casted_addr->sin_port);
    }
    return -1;
}
