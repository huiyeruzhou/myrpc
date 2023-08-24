/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: tcp_transport.cpp
 * Description: the tcp transport implementation
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#include "transport/tcp_transport.hpp"

#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

__attribute__((unused)) static const char *TAG = "tcp";
erpc::TCPTransport::TCPTransport(int sockfd, int port)
    : m_socket(sockfd),
      m_port(port),
      to_recv_md(new myrpc_Meta()),
      to_send_md(new myrpc_Meta()),
      m_read_buffer(new MessageBuffer()),
      m_initial_md_buffer(new MessageBuffer()),
      m_msg_buffer(new MessageBuffer())
{
}
erpc::TCPTransport::~TCPTransport(void)
{
    ::close(m_socket);
    delete m_read_buffer;
    delete m_initial_md_buffer;
    delete m_msg_buffer;
    delete to_recv_md;
    delete to_send_md;
}
rpc_status erpc::TCPTransport::close()
{
    if (0 != ::close(m_socket)) {
        return rpc_status::IOError;
    }
    return rpc_status::Success;
}
rpc_status erpc::TCPTransport::receive(uint8_t *data, uint32_t size)
{
    rpc_status status = Success;

    if (m_socket <= 0) {
        LOGE(TAG, "m_socket <= 0: %d, data: %s, size: %" PRIu32, m_socket, data, size);
        status = ConnectionClosed;
        return status;
    }

    int ret = ::recv(m_socket, data, size, 0);
    if (ret <= 0) {
        int recv_errno = errno;
        if (ret == 0) {
            LOGE(TAG, "recv returns 0, ConnectionClosed.");
            LOGE(TAG, "m_socket: %d, ret: %d, data: %s, size: %" PRIu32, m_socket, ret, data, size);
            status = rpc_status::ConnectionClosed;
        } else {
            switch (recv_errno) {
                case EAGAIN:
#if EAGAIN != EWOULDBLOCK
                case EWOULDBLOCK:
#endif
                    status = rpc_status::Timeout;
                    break;
                case EBADF:
                case EFAULT:
                case ENOTSOCK:
                case EINVAL:
                    LOGE(TAG, "InvalidArgument: %s", strerror(errno));
                    LOGE(TAG, "m_socket: %d, ret: %d, data: %s, size: %" PRIu32, m_socket, ret, data, size);
                    status = rpc_status::InvalidArgument;
                    break;
                case ECONNREFUSED:
                case ENOTCONN:
                    LOGE(TAG, "ConnectionFailure: %s", strerror(errno));
                    LOGE(TAG, "m_socket: %d, ret: %d, data: %s, size: %" PRIu32, m_socket, ret, data, size);
                    status = rpc_status::ConnectionFailure;
                    break;
                default:
                    LOGE(TAG, "Fail: %s", strerror(errno));
                    LOGE(TAG, "m_socket: %d, ret: %d, data: %s, size: %" PRIu32, m_socket, ret, data, size);
                    status = rpc_status::Fail;
                    break;
            }
        }
    } else if (ret < size) {
        LOGE(TAG, "recvd %d bytes of data, excepted %" PRIu32, ret, size);
    }
    return status;
}

rpc_status erpc::TCPTransport::send(const uint8_t *data, uint32_t size)
{
    rpc_status status = Success;
    // ssize_t result;

    if (m_socket <= 0) {
        LOGE(TAG, "m_socket <= 0: %d, data: %s, size: %" PRIu32, m_socket, data, size);
        status = ConnectionClosed;
    } else {
        int ret = ::send(m_socket, data, size, 0);
        if (ret < 0) {
            LOGE(TAG, "recv error: %s", strerror(errno));
            LOGE(TAG, "m_socket: %d, ret: %d, data: %s, size: %" PRIu32, m_socket, ret, data, size);
            status = ConnectionClosed;
        } else if (ret < size) {
            LOGE(TAG, "sent %d bytes of data, excepted %" PRIu32, ret, size);
        }
    }

    return status;
}

rpc_status erpc::TCPTransport::recv_inital_md()
{
    rpc_status err = rpc_status::NanopbCodecError;
    if (!md_recvd) {
        CHECK_STATUS(m_codec.get(m_read_buffer, myrpc_Meta_fields, to_recv_md), err);
    }
    return err;
}

rpc_status erpc::TCPTransport::recv_msg()
{
    rpc_status err = rpc_status::NanopbCodecError;
    if (recv_desc && to_recv_msg) {
        CHECK_STATUS(m_codec.get(m_read_buffer, recv_desc, to_recv_msg), err);
    } else {
        LOGE(TAG, "recv_desc or to_recv_msg is NULL");
    }
    return err;
}
rpc_status erpc::TCPTransport::recv_trailing_md() { return rpc_status::Success; }
rpc_status erpc::TCPTransport::send_inital_md()
{
    rpc_status err = rpc_status::NanopbCodecError;
    CHECK_STATUS(m_codec.write(m_initial_md_buffer, myrpc_Meta_fields, to_send_md), err);
    return err;
}
rpc_status erpc::TCPTransport::send_msg()
{
    rpc_status err = rpc_status::NanopbCodecError;
    if (send_desc && to_send_msg) {
        CHECK_STATUS(m_codec.write(m_msg_buffer, send_desc, to_send_msg), err);
    } else {
        LOGE(TAG, "send_desc or to_send_msg  is NULL");
    }
    return err;
}
rpc_status erpc::TCPTransport::send_trailing_md()
{
    rpc_status err = rpc_status::Success;
    CHECK_STATUS(sendFrame(), err);
    return err;
}

rpc_status erpc::TCPTransport::receiveFrame()
{
    uint32_t h = 0;
    rpc_status retVal;
    MessageBuffer *message = m_read_buffer;

    // Receive header first.
    retVal = receive(reinterpret_cast<uint8_t *>(&h), sizeof(h));
#ifdef TRACE_TCP
    LOGI(TAG, "receive header: %" PRIu32, h);
#endif

    if (retVal == Success) {
        // received size can't be zero.
        if (h == 0U) {
            retVal = ReceiveFailed;
            LOGE(TAG, "transport:    received size can't be zero.\n");
        }
    }

    if (retVal == Success) {
        // received size can't be larger then buffer length.
        CHECK_STATUS(message->prepareForWrite(h), retVal);
    }

    if (retVal == Success) {
        // Receive rest of the message now we know its size.
        retVal = receive(message->get(), h);
    }
#ifdef TRACE_TCP
    LOGI(TAG, "receive message");
#endif

    return retVal;
}

rpc_status erpc::TCPTransport::sendFrame()
{
    rpc_status ret;
    std::string message;
    message.append(reinterpret_cast<const char *>(m_initial_md_buffer->get()), m_initial_md_buffer->getWritePos());
    message.append(reinterpret_cast<const char *>(m_msg_buffer->get()), m_msg_buffer->getWritePos());
    uint32_t h = message.size();
    // LOGE(TAG, "length is %zu",
    //      std::string(reinterpret_cast<char *>(&h), sizeof(h)).size());
    message = std::string(reinterpret_cast<char *>(&h), sizeof(h)) + message;
    ret = send(reinterpret_cast<const uint8_t *>(message.c_str()), h + sizeof(h));
    return ret;
}
rpc_status erpc::TCPTransport::resetBuffers()
{
    m_read_buffer->reset();
    m_initial_md_buffer->reset();
    m_msg_buffer->reset();
    recv_desc = NULL;
    send_desc = NULL;
    to_recv_msg = NULL;
    to_send_msg = NULL;
    return rpc_status::Success;
}
