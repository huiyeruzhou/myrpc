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
#define GRPC_SLICE_INLINE_EXTRA_SIZE sizeof(void*)

#define GRPC_SLICE_INLINED_SIZE \
  (sizeof(size_t) + sizeof(uint8_t*) - 1 + GRPC_SLICE_INLINE_EXTRA_SIZE)

struct grpc_slice_refcount;
/** A grpc_slice s, if initialized, represents the byte range
   s.bytes[0..s.length-1].

   It can have an associated ref count which has a destruction routine to be run
   when the ref count reaches zero (see grpc_slice_new() and grp_slice_unref()).
   Multiple grpc_slice values may share a ref count.

   If the slice does not have a refcount, it represents an inlined small piece
   of data that is copied by value.

   As a special case, a slice can be given refcount == uintptr_t(1), meaning
   that the slice represents external data that is not refcounted. */
struct grpc_slice {
    uint8_t length;
    uint8_t bytes[GRPC_SLICE_INLINED_SIZE];
};

#define GRPC_SLICE_BUFFER_INLINE_ELEMENTS 8

/** Represents an -expandable array of slices, to be interpreted as a
   single item. */
typedef struct grpc_slice_buffer {
    /** This is for internal use only. External users (i.e any code outside grpc
     * core) MUST NOT use this field */
    grpc_slice *base_slices;

    /** slices in the array (Points to the first valid grpc_slice in the array) */
    grpc_slice *slices;
    /** the number of slices in the array */
    size_t count;
    /** the number of slices allocated in the array. External users (i.e any code
     * outside grpc core) MUST NOT use this field */
    size_t capacity;
    /** the combined length of all slices in the array */
    size_t length;
    /** inlined elements to avoid allocations */
    grpc_slice inlined[GRPC_SLICE_BUFFER_INLINE_ELEMENTS];
} grpc_slice_buffer;

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


rpc_status MessageBufferList::add(MessageBuffer *buf)
{
    erpc_assert(m_count < MAX_BUFFER_COUNT);
    m_buffers[m_count++] = buf;
    return Success;
}
rpc_status MessageBufferList::append(MessageBuffer *buf)
{
    erpc_assert(m_count < MAX_BUFFER_COUNT);
    m_buffers[m_count++] = buf;
    return Success;
}
rpc_status MessageBufferList::insert(MessageBuffer *buf, uint32_t index)
{
    erpc_assert(index < m_count);
    for (size_t i = index; i < m_count; i++)
    {
        m_buffers[i + 1] = m_buffers[i];
    }
    m_buffers[index] = buf;
    return Success;
}
MessageBuffer *MessageBufferList::get(uint32_t index)
{
    erpc_assert(index < m_count);
    return m_buffers[index];
}
void MessageBufferList::Clear()
{
    m_count = 0;
}

std::string MessageBufferList::JoinIntoString() const {
    std::string result;
    // result.reserve(slice_buffer_.length);
    for (size_t i = 0; i < this->m_count; i++) {
        result.append(reinterpret_cast<const char *>(m_buffers[i]->m_buf), m_buffers[i]->m_write_pos);
    }
    return result;
}
rpc_status MessageBufferList::reset() {
    for (size_t i = 0; i < m_count; i++) {
        delete m_buffers[i];
    }
    m_count = 0;
    return Success;
}
