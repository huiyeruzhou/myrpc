#ifndef _TCP_WORKER_H_
#define _TCP_WORKER_H_
#include <cstdint>
#include "erpc_status.h"

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////
namespace erpc {
    class TCPWorker {
    public:
        erpc_status_t underlyingReceive(uint8_t *data, uint32_t size);
        erpc_status_t underlyingSend(const uint8_t *data, uint32_t size);
    private:
        int m_socket;
    };
}
#endif//_TCP_WORKER_H_