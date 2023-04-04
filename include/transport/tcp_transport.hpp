#ifndef _TCP_WORKER_H_
#define _TCP_WORKER_H_
#include "rpc_status.h"
#include "framed_transport.hpp"

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////
namespace erpc {
    class TCPWorker : public FramedTransport {
    public:
        TCPWorker(int sockfd, int port) :
            FramedTransport(),
            m_socket(sockfd),
            m_port(port)
        {}
        virtual ~TCPWorker(void);
        rpc_status underlyingReceive(uint8_t *data, uint32_t size);
        rpc_status underlyingSend(const uint8_t *data, uint32_t size);
    private:
        friend class ServerWorker;
        int m_socket;
        uint16_t m_port;
    };
}
#endif//_TCP_WORKER_H_