/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "rpc_client.h"
#include "tcp_transport.hpp"
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
    BasicCodecFactory *codecFactory;

    // Init factories.
    codecFactory = new BasicCodecFactory();

    setCodecFactory(codecFactory);
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
    // Create codec to read and write the request.
    Codec *codec = createBufferAndCodec();

    return RequestContext(++m_sequence, codec, isOneway);
}

void Client::performRequest(RequestContext &request)
{
    bool performRequest;

    // Check the codec status
    performRequest = request.getCodec()->isStatusOk();

    if (performRequest)
    {
        performClientRequest(request);
    }
}

void Client::performClientRequest(RequestContext &request)
{
    rpc_status err;

    // Send invocation request to server.
    if (request.getCodec()->isStatusOk() == true)
    {
        err = m_transport->send(request.getCodec()->getBuffer());
        request.getCodec()->updateStatus(err);
    }

    // If the request is oneway, then there is nothing more to do.
    if (!request.isOneway())
    {
        if (request.getCodec()->isStatusOk() == true)
        {
            // Receive reply.
            err = m_transport->receive(request.getCodec()->getBuffer());
            request.getCodec()->updateStatus(err);
        }

        // Check the reply.
        if (request.getCodec()->isStatusOk() == true)
        {
            verifyReply(request);
        }
    }
}


void Client::verifyReply(RequestContext &request)
{
    message_type_t msgType;
    uint32_t service;
    uint32_t requestNumber;
    uint32_t sequence;

    // Some transport layers change the request's message buffer pointer (for things like zero
    // copy support), so inCodec must be reset to work with correct buffer.
    request.getCodec()->reset();

    // Extract the reply header.
    request.getCodec()->startReadMessage(&msgType, &service, &requestNumber, &sequence);

    if (request.getCodec()->isStatusOk() == true)
    {
        // Verify that this is a reply to the request we just sent.
        if ((msgType != kReplyMessage) || (sequence != request.getSequence()))
        {
            request.getCodec()->updateStatus(kErpcStatus_ExpectedReply);
        }
    }
}

Codec *Client::createBufferAndCodec(void)
{
    Codec *codec = m_codecFactory->create();
    MessageBuffer message;

    if (codec != NULL)
    {
        message = m_messageFactory->create();
        if (NULL != message.get())
        {
            codec->setBuffer(message);
        }
        else
        {
            // Dispose of buffers and codecs.
            m_codecFactory->dispose(codec);
            codec = NULL;
        }
    }

    return codec;
}

void Client::releaseRequest(RequestContext &request)
{
    if (request.getCodec() != NULL)
    {
        m_messageFactory->dispose(request.getCodec()->getBuffer());
        m_codecFactory->dispose(request.getCodec());
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
        m_transport = new TCPWorker(m_sockfd, m_port);
    }
    else
    {
        LOGE(TAG, "connecting failed, error: %s", strerror(errno));
    }
    return status;
}
