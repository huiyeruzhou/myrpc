#ifndef NANOPB_TRANSPORT_H_
#define NANOPB_TRANSPORT_H_
#include "port/port.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "codec/message_buffer.hpp"
namespace erpc {
    class NanopbCodec{
    public:
        ~NanopbCodec() = default;
        rpc_status read(MessageBuffer *ibuf, const pb_msgdesc_t *fields, void *dst_struct);
        rpc_status write(MessageBuffer *obuf, const pb_msgdesc_t *fields, const void *src_struct);
        rpc_status get(MessageBuffer *ibuf, const pb_msgdesc_t *fields, void *dst_struct);
    };
}
#endif//NANOPB_TRANSPORT_H_