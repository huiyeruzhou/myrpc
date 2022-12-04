/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_client_manager.h"

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

#if ERPC_NESTED_CALLS_DETECTION
extern bool nestingDetection;
#ifndef _WIN32
#pragma weak nestingDetection
bool nestingDetection = false;
#endif
#endif

RequestContext ClientManager::createRequest(bool isOneway)
{
    // Create codec to read and write the request.
    Codec *codec = createBufferAndCodec();

    return RequestContext(++m_sequence, codec, isOneway);
}

void ClientManager::performRequest(RequestContext &request)
{
    bool performRequest;

    // Check the codec status
    performRequest = request.getCodec()->isStatusOk();

    if (performRequest)
    {
        performClientRequest(request);
    }
}

void ClientManager::performClientRequest(RequestContext &request)
{
    erpc_status_t err;

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


void ClientManager::verifyReply(RequestContext &request)
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

Codec *ClientManager::createBufferAndCodec(void)
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

void ClientManager::releaseRequest(RequestContext &request)
{
    if (request.getCodec() != NULL)
    {
        m_messageFactory->dispose(request.getCodec()->getBuffer());
        m_codecFactory->dispose(request.getCodec());
    }
}

void ClientManager::callErrorHandler(erpc_status_t err, uint32_t functionID)
{
    if (m_errorHandler != NULL)
    {
        m_errorHandler(err, functionID);
    }
}
