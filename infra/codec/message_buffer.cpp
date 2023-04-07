/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "codec/message_buffer.hpp"


using namespace erpc;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
rpc_status MessageBuffer::read(void *data, uint32_t length, uint16_t offset)
{
    rpc_status err = Success;
    uint16_t dest_pos = offset + length;
    
    if (length > 0U)
    {
        if (data == NULL)
        {
            err = MemoryError;
        }
        else if ((dest_pos) > m_len || (dest_pos) < offset)
        {
            err = BufferOverrun;
        }
        else
        {
            (void) memcpy(data, &m_buf[offset], length);
            m_read_pos = dest_pos > m_read_pos ? dest_pos : m_read_pos;
        }
    }
    return err;
}

rpc_status MessageBuffer::write(const void *data, uint32_t length, uint16_t offset)
{
    rpc_status err = Success;
    uint16_t dest_pos = offset + length;
    
    if (length > 0U)
    {
        if (data == NULL)
        {
            err = MemoryError;
        }
        else if ((dest_pos) > m_len || (dest_pos) < offset)
        {
            err = BufferOverrun;
        }
        else
        {
            (void) memcpy(&m_buf[offset], data, length);
            m_write_pos = dest_pos > m_write_pos ? dest_pos : m_write_pos;
        }
    }
    return err;
}

rpc_status MessageBufferFactory::prepareServerBufferForSend(MessageBuffer *message)
{
    message->m_read_pos = 0;
    message->m_write_pos = 4;
    return Success;
}
MessageBuffer *MessageBufferFactory::create(void)
{
    uint8_t *buf = new (nothrow) uint8_t[ERPC_DEFAULT_BUFFER_SIZE];
    return new MessageBuffer(buf, ERPC_DEFAULT_BUFFER_SIZE);
}
void MessageBufferFactory::dispose(MessageBuffer *buf)
    {
        erpc_assert(buf != NULL);
        if (buf->get() != NULL)
        {
            delete[] buf->get();
        }
    }

