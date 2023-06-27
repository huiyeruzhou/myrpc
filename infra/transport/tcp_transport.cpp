#include "transport/tcp_transport.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
using namespace erpc;
__attribute__((unused)) const static char *TAG = "tcp";
TCPTransport::TCPTransport(int sockfd, int port):
    m_socket(sockfd),
    m_port(port),
    to_recv_md(new myrpc_Meta()),
    to_send_md(new myrpc_Meta()),
    m_read_buffer(new MessageBuffer()),
    m_send_buffer_list(new MessageBufferList())
{
}
TCPTransport::~TCPTransport(void) {
    ::close(m_socket);
    delete m_read_buffer;
    m_send_buffer_list->~MessageBufferList();
    delete to_recv_md;
    delete to_send_md;
}
rpc_status TCPTransport::close() {
    if (0 != ::close(m_socket)) {
        return rpc_status::IOError;
    }
    return rpc_status::Success;
}
rpc_status TCPTransport::receive(uint8_t *data, uint32_t size)
{
    // ssize_t length;
    rpc_status status = Success;

    if(m_socket <= 0)
    {
        status = ConnectionFailure;
        return status;
    }

    // Loop until all requested data is received.
    // while (size > 0U)
    // {
    //     length = read(m_socket, data, size);

    //     // Length will be zero if the connection is closed.
    //     if (length > 0)
    //     {
    //         size -= length;
    //         data += length;
    //     }
    //     else
    //     {
    //         if (length == 0)
    //         {
    //             // close socket, not server
    //             close();
    //             status = ConnectionClosed;
    //         }
    //         else
    //         {
    //             status = ReceiveFailed;
    //             printf("transport:   unknown error from tcp, return value of read is %zu\n", length);
    //         }
    //         break;
    //     }
    // }
    ::recv(m_socket, data, size, 0);

    return status;
}

rpc_status TCPTransport::send(const uint8_t *data, uint32_t size)
{
    rpc_status status = Success;
    // ssize_t result;

    if (m_socket <= 0)
    {
        // we should not pretend to have a succesful Send or we create a deadlock
        status = ConnectionFailure;
    }
    else
    {
        // Loop until all data is sent.
        // while (size > 0U)
        // {
        //     result = write(m_socket, data, size);
        //     if (result >= 0)
        //     {
        //         size -= result;
        //         data += result;
        //     }
        //     else
        //     {
        //         if (result == EPIPE)
        //         {
        //             // close socket, not server
        //             close();
        //             status = ConnectionClosed;
        //         }
        //         else
        //         {
        //             status = SendFailed;
        //         }
        //         break;
        //     }
        // }
        ::send(m_socket, data, size, 0);
    }

    return status;
}

rpc_status TCPTransport::recv_inital_md() {
    rpc_status err = rpc_status::NanopbCodecError;
    if (!md_recvd) {
        CHECK_STATUS(m_codec.get(m_read_buffer, myrpc_Meta_fields, to_recv_md), err);
    }
    return err;
}

rpc_status TCPTransport::recv_msg() {
    rpc_status err = rpc_status::NanopbCodecError;
    if (recv_desc && to_recv_msg) {
        CHECK_STATUS(m_codec.get(m_read_buffer, recv_desc, to_recv_msg), err);
    }
    else {
        LOGE(TAG, "recv_desc or to_recv_msg is NULL");
    }
    return err;
}
rpc_status TCPTransport::recv_trailing_md() {
    return rpc_status::Success;
}
rpc_status TCPTransport::send_inital_md() {
    MessageBuffer *initial_md_buffer = new MessageBuffer();
    rpc_status err = rpc_status::NanopbCodecError;
    CHECK_STATUS(m_codec.write(initial_md_buffer, myrpc_Meta_fields, to_send_md), err);
    CHECK_STATUS(m_send_buffer_list->add(initial_md_buffer), err);
    return err;
}
rpc_status TCPTransport::send_msg() {
    MessageBuffer *msg_buffer = new MessageBuffer();
    rpc_status err = rpc_status::NanopbCodecError;
    if (send_desc && to_send_msg) {
        CHECK_STATUS(m_codec.write(msg_buffer, send_desc, to_send_msg), err);
    }
    else {
        LOGE(TAG, "send_desc or to_send_msg  is NULL");
    }
    m_send_buffer_list->add(msg_buffer);
    return err;
}
rpc_status TCPTransport::send_trailing_md() {
    rpc_status err = rpc_status::Success;
    CHECK_STATUS(sendFrame(), err);
    return err;
}

rpc_status TCPTransport::receiveFrame() {
    uint32_t h;
    rpc_status retVal;
    MessageBuffer *message = m_read_buffer;

    {
        // Receive header first.
        retVal = receive((uint8_t *) &h, sizeof(h));
        LOGI(TAG, "receive header");

        if (retVal == Success) {
            // received size can't be zero.
            if (h == 0U) {
                retVal = ReceiveFailed;
                printf("transport:    received size can't be zero.\n");
            }
        }

        if (retVal == Success) {
            // received size can't be larger then buffer length.
            if (h > message->getWriteSize()) {
                retVal = ReceiveFailed;
                printf("transport:    received size can't be larger then buffer length.\n");
            }
        }

        if (retVal == Success) {
            // Receive rest of the message now we know its size.
            retVal = receive(message->get(), h);
        }
        LOGI(TAG, "receive message");
    }
    return retVal;
}

rpc_status TCPTransport::sendFrame() {
    rpc_status ret;
    MessageBufferList *message_list = m_send_buffer_list;
    //walk through message_list
    std::string message = message_list->JoinIntoString();
    uint32_t h = message.size();
    ret = send((uint8_t *) &h, sizeof(h));
    if (ret == Success) {
        ret = send((uint8_t *) message.c_str(), h);
    }

    return ret;
}
rpc_status TCPTransport::resetBuffers() {
    m_read_buffer->reset();
    m_send_buffer_list->reset();
    recv_desc = NULL;
    send_desc = NULL;
    to_recv_msg = NULL;
    to_send_msg = NULL;
    return rpc_status::Success;
}