/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_framed_transport.hpp"

#include "erpc_config_internal.h"
#include "erpc_message_buffer.hpp"

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

FramedTransport::~FramedTransport(void) = default;


erpc_status_t FramedTransport::receive(MessageBuffer *message)
{
    Header h{};
    erpc_status_t retVal;

    {
#if !ERPC_THREADS_IS(NONE)
        Mutex::Guard lock(m_receiveLock);
#endif

        // Receive header first.
        retVal = underlyingReceive((uint8_t *)&h, sizeof(h));

        if (retVal == kErpcStatus_Success)
        {

            // received size can't be zero.
            if (h.m_messageSize == 0U)
            {
                retVal = kErpcStatus_ReceiveFailed;
                printf("transport:    received size can't be zero.\n");
            }
        }

        if (retVal == kErpcStatus_Success)
        {
            // received size can't be larger then buffer length.
            if (h.m_messageSize > message->getLength())
            {
                retVal = kErpcStatus_ReceiveFailed;
                printf("transport:    received size can't be larger then buffer length.\n");
            }
        }

        if (retVal == kErpcStatus_Success)
        {
            // Receive rest of the message now we know its size.
            retVal = underlyingReceive(message->get(), h.m_messageSize);
        }
    }

    if (retVal == kErpcStatus_Success) {
        // Receive rest of the message now we know its size.
        message->setUsed(h.m_messageSize);
    }
    return retVal;
}

erpc_status_t FramedTransport::send(MessageBuffer *message)
{
    erpc_status_t ret;
    uint16_t messageLength;
    Header h{};

#if !ERPC_THREADS_IS(NONE)
    Mutex::Guard lock(m_sendLock);
#endif

    messageLength = message->getUsed();

    // Send header first.
    h.m_messageSize = messageLength;

    ret = underlyingSend((uint8_t *)&h, sizeof(h));
    if (ret == kErpcStatus_Success)
    {
        ret = underlyingSend(message->get(), messageLength);
    }

    return ret;
}
