#ifndef NANOPB_TRANSPORT_H_
#define NANOPB_TRANSPORT_H_
#include "pb_decode.h"
#include "pb_encode.h"
#include <transport/tcp_transport.hpp>
namespace erpc {
    class NanopbTransport{
    public:
        NanopbTransport(Transport *worker, MessageBufferFactory *messageFactory)
            : worker(worker)
            , messageFactory(messageFactory)
            , ibuf(messageFactory->create())
            , obuf(messageFactory->create()) {
        }
        ~NanopbTransport() = default;
        rpc_status read(const pb_msgdesc_t *fields, void *dst_struct);
        rpc_status write(const pb_msgdesc_t *fields, const void *src_struct);
        rpc_status get(const pb_msgdesc_t *fields, void *dst_struct);
        rpc_status receive();
        virtual rpc_status send();
    public:
        friend class ServerWorker;
        Transport *worker;
        MessageBufferFactory *messageFactory;
        MessageBuffer *ibuf;
        MessageBuffer *obuf;
    };
}
#endif//NANOPB_TRANSPORT_H_