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
#include "transport/nanopb_transport.hpp"
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
Client::Client(const char *host, uint16_t port, MessageBufferFactory *messageFactory)
    : CSBase(host, port)
    , m_sequence(0)
    , m_errorHandler(NULL)
{
    setMessageBufferFactory(messageFactory);
}

/*!
 * @brief Client destructor
 */
Client::~Client(void)
{
    delete this->m_messageFactory;
    delete this->m_transport;
}
RequestContext Client::createRequest(bool isOneway)
{
    return RequestContext(++m_sequence, isOneway, m_messageFactory);
}

void Client::performRequest(RequestContext &request)
{
    rpc_status err;

    // Send invocation request to server.
    if (request.isStatusOk() == true) {
        err = m_transport->send();
        request.updateStatus(err);
    }

    // If the request is oneway, then there is nothing more to do.
    if (!request.isOneway()) {
        if (request.isStatusOk() == true) {
            // Receive reply.
            err = m_transport->receive();
            request.updateStatus(err);
        }

        // Check the reply.
        if (request.isStatusOk() == true) {
            verifyReply(request);
        }
    }
}

void Client::verifyReply(RequestContext &request)
{
    rpc_status err;
    myrpc_Meta meta = myrpc_Meta_init_zero;
    err = m_transport->read(&myrpc_Meta_msg, &meta);
    
    request.updateStatus(err);
    if (err != Success)
    {
        return;
    }

    // Verify that this is a reply to the request we just sent.
    if ((meta.type != myrpc_Meta_msgType_RESPONSE) || (meta.seq != request.getSequence()))
    {
        request.updateStatus(UnExpectedMsgType);
    }
}

void Client::callErrorHandler(rpc_status err, uint32_t functionID)
{
    if (m_errorHandler != NULL)
    {
        m_errorHandler(err, functionID);
    }
}

rpc_status Client::open(void)
{
    rpc_status status = Success;
    struct addrinfo hints = {};
    char portString[8];
    struct addrinfo *res0;
    int result, set;
    int sock = -1;
    struct addrinfo *res;

    if (m_sockfd != -1)
    {
        LOGE("%s", "socket already connected, error: %s", strerror(errno));
    }
    else
    {
        // Fill in hints structure for getaddrinfo.
        hints.ai_flags = AI_NUMERICSERV;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        // Convert port number to a string.
        result = snprintf(portString, sizeof(portString), "%d", m_port);
        if (result < 0)
        {
            LOGE(TAG, "snprintf failed, error: %s", strerror(errno));
            status = Fail;
        }

        if (status == Success)
        {
            // Perform the name lookup.
            result = getaddrinfo(m_host, portString, &hints, &res0);
            if (result != 0)
            {
                // TODO check EAI_NONAME
                LOGE(TAG, "gettaddrinfo failed, error: %s", strerror(errno));
                status = kErpcStatus_UnknownName;
            }
        }

        if (status == Success)
        {
            // Iterate over result addresses and try to connect. Exit the loop on the first successful
            // connection.
            for (res = res0; res; res = res->ai_next)
            {
                // Create the socket.
                sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
                if (sock < 0)
                {
                    LOGW(TAG, "try create a socket, but failed");
                    continue;
                }
                // Attempt to connect.
                char netinfo[24];
                sprint_net_info(netinfo, sizeof(netinfo), res->ai_addr, res->ai_addrlen);
                LOGI(TAG, "try to connect to %s", netinfo);
                if (connect(sock, res->ai_addr, res->ai_addrlen) < 0)
                {
                    LOGW(TAG, "try to connect for one of socket, but failed");
                    ::close(sock);
                    sock = -1;
                    continue;
                }
                else
                {
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
            if (sock < 0)
            {
                // TODO check EADDRNOTAVAIL:
                LOGE(TAG, "connecting failed, error: %s", strerror(errno));
                status = kErpcStatus_ConnectionFailure;
            }
        }

        if (status == Success)
        {
            set = 1;
            if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *) &set, sizeof(int)) < 0)
            {
                ::close(sock);
                LOGE(TAG, "setsockopt failed, error: %s", strerror(errno));
                status = Fail;
            }
        }

        if (status == Success)
        {
            // globally disable the SIGPIPE signal
            signal(SIGPIPE, SIG_IGN);
            m_sockfd = sock;
        }
    }
    if (Success == status)
    {
        m_transport_worker = new TCPWorker(m_sockfd, m_port);
        m_transport = new NanopbTransport(m_transport_worker, m_messageFactory);
    }
    else
    {
        LOGE(TAG, "connecting failed, error: %s", strerror(errno));
    }
    return status;
}
