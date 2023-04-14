/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "codec/meta.pb.h"
#include "client/rpc_client.hpp"
#include "transport/tcp_transport.hpp"
#include <string>


extern "C" {
#include <signal.h>
    // #include <sys/socket.h>
    // #include <sys/types.h>
    // #include <unistd.h>
}
using namespace erpc;

__attribute__((unused)) const static char *TAG = "client";
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

    /*!
     * @brief Constructor.
     *
     * This function initializes object attributes.
     */
Client::Client(const char *host, uint16_t port)
    : CSBase(host, port)
    , m_sequence(0) {
}

/*!
 * @brief Client destructor
 */
Client::~Client(void) {
    delete this->m_transport;
}
RequestContext Client::createRequest(bool isOneway) {
    return RequestContext(++m_sequence, isOneway);
}

rpc_status Client::performRequest(char *path, const pb_msgdesc_t *req_desc, void *req, const pb_msgdesc_t *rsp_desc, void *rsp) {

    m_transport->to_send_msg = req;
    m_transport->to_recv_msg = rsp;
    m_transport->send_desc = req_desc;
    m_transport->recv_desc = rsp_desc;
    myrpc_Meta *req_md = m_transport->to_send_md;
    req_md->seq = ++m_sequence;
    req_md->has_send_end = true;
    req_md->send_end = true;
    req_md->path = path;
    req_md->version = 0;
    req_md->has_status = false;

    rpc_status err;

    //send initial metadata
    CHECK_STATUS(m_transport->send_inital_md(), err);
    LOGI(TAG, "send_inital_md()");
    //send msg
    CHECK_STATUS(m_transport->send_msg(), err);
    LOGI(TAG, "send_msg()");
    //sned trailing metadata
    CHECK_STATUS(m_transport->send_trailing_md(), err);
    LOGI(TAG, "send_trailing_md()");

    // get data frame
    CHECK_STATUS(m_transport->receiveFrame(), err);
    LOGI(TAG, "receiveFrame()");
    //recv inital metadata
    CHECK_STATUS(m_transport->recv_inital_md(), err);
    LOGI(TAG, "recv_inital_md()");
    //recv messgae
    CHECK_STATUS(m_transport->recv_msg(), err);
    LOGI(TAG, "recv_msg()");
    //recv trailing metadata
    CHECK_STATUS(m_transport->recv_trailing_md(), err);
    LOGI(TAG, "recv_trailing_md()");

    err = m_transport->to_recv_md->has_status ? static_cast<rpc_status>(m_transport->to_recv_md->status)
        : UnExpectedMsgType;

    m_transport->resetBuffers();
    return err;
}


rpc_status Client::open(void) {
    rpc_status status = Success;
    struct addrinfo hints = {};
    char portString[8];
    struct addrinfo *res0;
    int result, set;
    int sock = -1;
    struct addrinfo *res;

    if (m_sockfd != -1) {
        LOGE("%s", "socket already connected, error: %s", strerror(errno));
    }
    else {
        // Fill in hints structure for getaddrinfo.
        hints.ai_flags = AI_NUMERICSERV;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        // Convert port number to a string.
        result = snprintf(portString, sizeof(portString), "%d", m_port);
        if (result < 0) {
            LOGE(TAG, "snprintf failed, error: %s", strerror(errno));
            status = Fail;
        }

        if (status == Success) {
            // Perform the name lookup.
            result = getaddrinfo(m_host, portString, &hints, &res0);
            if (result != 0) {
                // TODO check EAI_NONAME
                LOGE(TAG, "gettaddrinfo failed, error: %s", strerror(errno));
                status = kErpcStatus_UnknownName;
            }
        }

        if (status == Success) {
            // Iterate over result addresses and try to connect. Exit the loop on the first successful
            // connection.
            for (res = res0; res; res = res->ai_next) {
                // Create the socket.
                sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
                if (sock < 0) {
                    LOGW(TAG, "try create a socket, but failed");
                    continue;
                }
                // Attempt to connect.
                char netinfo[24];
                sprint_net_info(netinfo, sizeof(netinfo), res->ai_addr, res->ai_addrlen);
                LOGI(TAG, "try to connect to %s", netinfo);
                if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
                    LOGW(TAG, "try to connect for one of socket, but failed");
                    ::close(sock);
                    sock = -1;
                    continue;
                }
                else {
                    // Exit the loop for the first successful connection.
                    char netinfo[24];
                    sprint_net_info(netinfo, sizeof(netinfo), res->ai_addr, res->ai_addrlen);
                    LOGI(TAG, "successful connection to %s", netinfo);
                    break;
                }
            }

            // Free the result list.
            freeaddrinfo(res0);

            // Check if we were able to open a connection.
            if (sock < 0) {
                // TODO check EADDRNOTAVAIL:
                LOGE(TAG, "connecting failed, error: %s", strerror(errno));
                status = kErpcStatus_ConnectionFailure;
            }
        }

        if (status == Success) {
            set = 1;
            if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *) &set, sizeof(int)) < 0) {
                ::close(sock);
                LOGE(TAG, "setsockopt failed, error: %s", strerror(errno));
                status = Fail;
            }
        }

        if (status == Success) {
            // globally disable the SIGPIPE signal
            signal(SIGPIPE, SIG_IGN);
            m_sockfd = sock;
        }
    }
    if (Success == status) {
        m_transport = new TCPWorker(m_sockfd, m_port);
    }
    else {
        LOGE(TAG, "connecting failed, error: %s", strerror(errno));
    }
    return status;
}
