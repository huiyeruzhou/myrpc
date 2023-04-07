#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include "port/port.h"
#include "server/service.hpp"
#include "transport/tcp_transport.hpp"
#include <cstdio>
extern "C" {
#include "codec/meta.pb.h"
#include <unistd.h>
}
namespace erpc
{
    class ServerWorker {
    public:
        ServerWorker(Service *services, MessageBufferFactory *messageFactory,
            TCPWorker *worker);
        ~ServerWorker(){}
        void disposeBufferAndCodec();
        rpc_status runInternal(void);
        rpc_status runInternalBegin(myrpc_Meta *meta);
        rpc_status runInternalEnd(myrpc_Meta *meta);
        Service *findServiceWithId(uint32_t serviceId);
        rpc_status processMessage(myrpc_Meta *meta);
        rpc_status readHeadOfMessage(myrpc_Meta *meta);
        
        static void workerStub(void *arg);
        void start(void);
    private:
        Thread m_workerThread;
#if ERPC_THREADS_IS(PTHREADS)
        char TAG[CONFIG_MAX_PTHREAD_NAME_LEN];
#elif ERPC_THREADS_IS(FREERTOS)
        char TAG[configMAX_TASK_NAME_LEN];
#endif
        
        Service *m_firstService;
        MessageBufferFactory *m_messageFactory; //!< Message buffer factory to use.
        NanopbTransport *m_worker;                 //!< Worker to do transport
        
    };

} // namespace erpc

#endif//SERVER_WORKER_H