#include "erpc_client_server_common.hpp"
void print_net_info(const sockaddr *__sockaddr, int __len) {
    char host[32];
    char service[32];
    getnameinfo(__sockaddr, __len, host, 32, service, 32, 0);
    printf("%s:%s\n", host, service);
}

int getPortFormAddr(const sockaddr *__sockaddr, int __len) {
    char service[32];
    int port;
    getnameinfo(__sockaddr, __len, NULL, 0, service, 32, 0);
    if (sscanf(service, "%d", &port) == 1)
    {
        return port;
    }
    return -1;
}