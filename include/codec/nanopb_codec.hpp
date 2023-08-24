/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: nanopb_codec.hpp
 * Description: This file defines the NanopbCodec class.
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#ifndef CODEC_NANOPB_CODEC_HPP_
#define CODEC_NANOPB_CODEC_HPP_

#include "codec/message_buffer.hpp"
#include "port/port.h"
#include "rpc_status.hpp"
#ifdef __cplusplus
extern "C" {
#endif
#include "nanopb/pb.h"
#include "nanopb/pb_common.h"
#include "nanopb/pb_decode.h"
#include "nanopb/pb_encode.h"

namespace erpc {
class NanopbCodec {
   public:
    ~NanopbCodec() = default;
    rpc_status write(MessageBuffer *obuf, const pb_msgdesc_t *fields, const void *src_struct);
    rpc_status get(MessageBuffer *ibuf, const pb_msgdesc_t *fields, void *dst_struct);
};
}  // namespace erpc
#ifdef __cplusplus
}
#endif
#endif  // CODEC_NANOPB_CODEC_HPP_
