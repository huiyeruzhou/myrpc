#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H
#include "port.h"
#include "codec_base.hpp"
#include "service.hpp"
#include "tcp_transport.hpp"
#include <cstdio>
extern "C" {
#include <unistd.h>
}
namespace erpc
{
    class ServerWorker {
    public:
        ServerWorker(Service *services, MessageBufferFactory *messageFactory,
            CodecFactory *codecFactory,
            TCPWorker *worker);
        ~ServerWorker(){}
        void disposeBufferAndCodec(Codec *codec);
        rpc_status runInternal(void);
        rpc_status runInternalBegin(Codec **codec, MessageBuffer &buff, message_type_t &msgType,
                                    uint32_t &serviceId, uint32_t &methodId, uint32_t &sequence);
        rpc_status runInternalEnd(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
                                  uint32_t sequence);
        Service *findServiceWithId(uint32_t serviceId);
        rpc_status processMessage(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId, uint32_t sequence);
        rpc_status readHeadOfMessage(Codec *codec, message_type_t &msgType, uint32_t &serviceId, uint32_t &methodId,
                                     uint32_t &sequence);
        
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
        CodecFactory *m_codecFactory;           //!< Codec to use.
        TCPWorker *m_worker;                 //!< Worker to do transport
        
    };

} // namespace erpc

#endif//SERVER_WORKER_H