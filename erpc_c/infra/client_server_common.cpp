#include "erpc_client_server_common.hpp"
#include <cstring>
#if CONFIG_HAS_FREERTOS
int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
    char *host, socklen_t hostlen,
    char *serv, socklen_t servlen, int flags) {
    auto casted_addr = reinterpret_cast<const struct sockaddr_in*>(addr);
    inet_ntop(AF_INET, &(casted_addr->sin_addr), host, hostlen);
    snprintf(serv, servlen, "%d", ntohs(casted_addr->sin_port));
    return 0;
}
#endif
char* print_net_info(const sockaddr *__sockaddr, int __len) {
    static char netinfo[32] = {};
    char host[16];
    char service[6];
    getnameinfo(__sockaddr, __len, host, 32, service, 32, 0);
    snprintf(netinfo, 24, "%s:%s", host, service);
    return netinfo;
}

int getPortFormAddr(const sockaddr *__sockaddr, int __len) {
    char service[32] = {};
    int port;
    getnameinfo(__sockaddr, __len, NULL, 0, service, 32, 0);
    if (sscanf(service, "%d", &port) == 1)
    {
        return port;
    }
    return -1;
}