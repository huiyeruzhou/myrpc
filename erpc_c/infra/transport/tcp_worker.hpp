#ifndef _TCP_WORKER_H_
#define _TCP_WORKER_H_
#include <cstdint>
#include "erpc_status.h"
#include "erpc_framed_transport.hpp"

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////
namespace erpc {
    class TCPWorker : public FramedTransport {
    public:
        TCPWorker(int sockfd) :
            FramedTransport(),
            m_socket(sockfd)
        {}
        virtual ~TCPWorker(void);
        erpc_status_t underlyingReceive(uint8_t *data, uint32_t size);
        erpc_status_t underlyingSend(const uint8_t *data, uint32_t size);
    private:
        int m_socket;
    };
}
#endif//_TCP_WORKER_H_