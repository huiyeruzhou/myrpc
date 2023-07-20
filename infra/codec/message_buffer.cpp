#include "codec/message_buffer.hpp"


using namespace erpc;
using namespace std;

const static char *TAG = "Message Buffer";
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
    LOGE(TAG, "m_read_pos = %" PRIu16, m_read_pos);
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
