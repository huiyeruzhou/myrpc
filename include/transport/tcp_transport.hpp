/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: tcp_transport.hpp
 * Description: This file defines the TCPTransport class.Which will manage the IO and encoding/decoding operation.
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#ifndef TRANSPORT_TCP_TRANSPORT_HPP_
#define TRANSPORT_TCP_TRANSPORT_HPP_
#include "codec/message_buffer.hpp"
#include "codec/meta.pb.h"
#include "codec/nanopb_codec.hpp"
#include "port/port.h"
#include "port/threading/port_threading.h"
#include "rpc_status.hpp"

namespace erpc {
class TCPTransport {
   public:
    TCPTransport(int sockfd, int port);
    virtual ~TCPTransport(void);
    rpc_status close();
    virtual rpc_status receive(uint8_t *data, uint32_t size);
    virtual rpc_status send(const uint8_t *data, uint32_t size);
    rpc_status receiveFrame();
    rpc_status sendFrame();
    rpc_status recv_inital_md();
    rpc_status recv_msg();
    rpc_status recv_trailing_md();
    rpc_status send_inital_md();
    rpc_status send_msg();
    rpc_status send_trailing_md();
    rpc_status resetBuffers();

   private:
    friend class ServerWorker;
    friend class NanopbCodec;
    friend class Client;
    bool is_client;
    int m_socket;
    uint16_t m_port;
    NanopbCodec m_codec{};

    bool md_recvd = false;
    myrpc_Meta *to_recv_md = NULL;
    myrpc_Meta *to_send_md = NULL;

    const pb_msgdesc_t *recv_desc = NULL;
    const pb_msgdesc_t *send_desc = NULL;
    void *to_recv_msg = NULL;
    void *to_send_msg = NULL;

    MessageBuffer *m_read_buffer;
    MessageBuffer *m_initial_md_buffer;
    MessageBuffer *m_msg_buffer;
    //        MessageBufferList *m_send_buffer_list;
};
}  // namespace erpc
#endif  // TRANSPORT_TCP_TRANSPORT_HPP_
