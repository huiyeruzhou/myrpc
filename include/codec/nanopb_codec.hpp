#ifndef NANOPB_TRANSPORT_H_
#define NANOPB_TRANSPORT_H_

#include "port/port.h"
#include "codec/message_buffer.hpp"
#include "rpc_status.hpp"
#ifdef  __cplusplus  
extern "C" {
#endif  
#include "nanopb/pb.h"
#include "nanopb/pb_decode.h"
#include "nanopb/pb_encode.h"
#include "nanopb/pb_common.h"

namespace erpc {
    class NanopbCodec{
    public:
        ~NanopbCodec() = default;
        rpc_status write(MessageBuffer *obuf, const pb_msgdesc_t *fields, const void *src_struct);
        rpc_status get(MessageBuffer *ibuf, const pb_msgdesc_t *fields, void *dst_struct);
    };
}
#ifdef  __cplusplus  
}
#endif  
#endif//NANOPB_TRANSPORT_H_