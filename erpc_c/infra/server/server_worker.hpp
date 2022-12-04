#ifndef _SERVER_WORKER_H_
#define _SERVER_WORKER_H_
#include "erpc_config_internal.h"
#include "erpc_codec.hpp"
#include "service.hpp"
#include "tcp_worker.hpp"
namespace erpc
{
    class ServerWorker {
    public:
        ServerWorker(Service *services, MessageBufferFactory *messageFactory,
            CodecFactory *codecFactory,
            Transport *worker);
        ~ServerWorker(){}
        void disposeBufferAndCodec(Codec *codec);
        erpc_status_t runInternal(void);
        erpc_status_t runInternalBegin(Codec **codec, MessageBuffer &buff, message_type_t &msgType,
            uint32_t &serviceId, uint32_t &methodId, uint32_t &sequence);
        erpc_status_t runInternalEnd(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
            uint32_t sequence);
        Service *findServiceWithId(uint32_t serviceId);
        erpc_status_t processMessage(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId, uint32_t sequence);
        erpc_status_t readHeadOfMessage(Codec *codec, message_type_t &msgType, uint32_t &serviceId, uint32_t &methodId,
            uint32_t &sequence);
        Thread m_workerThread;
        static void workerStub(void *arg);
    private:
        Service *m_firstService;
        MessageBufferFactory *m_messageFactory; //!< Message buffer factory to use.
        CodecFactory *m_codecFactory;           //!< Codec to use.
        Transport *m_worker;                 //!< Worker to do transport
        
    };

} // namespace erpc

#endif//_SERVER_WORKER_H_