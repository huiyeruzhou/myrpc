/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "transport/framed_transport.hpp"

#include "port/port.h"
#include "codec/message_buffer.hpp"

#include <cstdio>

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

FramedTransport::FramedTransport(void)
: Transport()
#if !ERPC_THREADS_IS(NONE)
, m_sendLock()
, m_receiveLock()
#endif
{
}

FramedTransport::~FramedTransport(void) {}


rpc_status FramedTransport::receive(MessageBuffer *message)
{
    Header h;
    rpc_status retVal;

    {
#if !ERPC_THREADS_IS(NONE)
        Mutex::Guard lock(m_receiveLock);
#endif

        // Receive header first.
        retVal = underlyingReceive((uint8_t *)&h, sizeof(h));

        if (retVal == Success)
        {

            // received size can't be zero.
            if (h.m_messageSize == 0U)
            {
                retVal = kErpcStatus_ReceiveFailed;
                printf("transport:    received size can't be zero.\n");
            }
        }

        if (retVal == Success)
        {
            // received size can't be larger then buffer length.
            if (h.m_messageSize > message->getLength())
            {
                retVal = kErpcStatus_ReceiveFailed;
                printf("transport:    received size can't be larger then buffer length.\n");
            }
        }

        if (retVal == Success)
        {
            // Receive rest of the message now we know its size.
            retVal = underlyingReceive(message->get(), h.m_messageSize);
        }
    }

    if (retVal == Success) {
        // Receive rest of the message now we know its size.
        message->setUsed(h.m_messageSize);
    }
    return retVal;
}

rpc_status FramedTransport::send(MessageBuffer *message)
{
    rpc_status ret;
    uint16_t messageLength;
    Header h;

#if !ERPC_THREADS_IS(NONE)
    Mutex::Guard lock(m_sendLock);
#endif

    messageLength = message->getUsed();

    // Send header first.
    h.m_messageSize = messageLength;

    ret = underlyingSend((uint8_t *)&h, sizeof(h));
    if (ret == Success)
    {
        ret = underlyingSend(message->get(), messageLength);
    }

    return ret;
}
