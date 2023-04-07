#ifndef _TCP_WORKER_H_
#define _TCP_WORKER_H_
#include "rpc_status.hpp"
#include "transport/transport_base.hpp"

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////
namespace erpc {
    class TCPWorker : public Transport {
    public:
        TCPWorker(int sockfd, int port) :
            m_socket(sockfd),
            m_port(port)
        {}
        virtual ~TCPWorker(void);
        virtual rpc_status receive(uint8_t *data, uint32_t size);
        virtual rpc_status send(const uint8_t *data, uint32_t size);
    private:
        friend class ServerWorker;
        friend class NanopbTransport;
        int m_socket;
        uint16_t m_port;
    };
}
#endif//_TCP_WORKER_H_