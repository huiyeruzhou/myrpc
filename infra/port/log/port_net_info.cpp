#include "port_net_info.hpp"

#if CONFIG_HAS_FREERTOS
//only declared but not implemented by esp-idf framework, therefore we should implement one
//by ourselves, remember to make sure that it is thread-safe
int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
    char *host, socklen_t hostlen,
    char *serv, socklen_t servlen, int flags) {
    auto casted_addr = reinterpret_cast<const struct sockaddr_in*>(addr);
    inet_ntop(AF_INET, &(casted_addr->sin_addr), host, hostlen);
    snprintf(serv, servlen, "%" SCNin_port_t, ntohs(casted_addr->sin_port));
    return 0;
}
#endif
void sprint_net_info(char *netinfo, int netinfo_len, const sockaddr *__sockaddr, int __len) {
    char host[16];
    char service[6];
    getnameinfo(__sockaddr, __len, host, 32, service, 32, 0);
    snprintf(netinfo, netinfo_len, "%s:%s", host, service);
}

int getPortFormAddr(const sockaddr *__sockaddr, int __len) {
    char service[32] = {};
    in_port_t port;
    getnameinfo(__sockaddr, __len, NULL, 0, service, 32, 0);
    if (sscanf(service, "%" SCNin_port_t, &port) == 1)
    {
        return port;
    }
    return -1;
}