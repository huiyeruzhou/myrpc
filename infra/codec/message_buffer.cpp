/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: message_buffer.cpp
 * Description: the message buffer implementation
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#include "codec/message_buffer.hpp"

__attribute__((unused)) static const char *TAG = "Message Buffer";

rpc_status erpc::MessageBuffer::read(void *data, uint32_t length, uint16_t offset)
{
    rpc_status err = Success;
    if (data == NULL) {
        return MemoryError;
    }
    CHECK_STATUS(prepareForRead(length, offset), err);
    (void)memcpy(data, &m_buf[offset], length);
    CHECK_STATUS(moveReadPos(length, offset), err);
    return err;
}

rpc_status erpc::MessageBuffer::write(const void *data, uint32_t length, uint16_t offset)
{
    rpc_status err = Success;
    CHECK_STATUS(prepareForWrite(length, offset), err);
    if (data == NULL) {
        return InvalidArgument;
    }
    (void)memcpy(&m_buf[offset], data, length);
    CHECK_STATUS(moveWritePos(length, offset), err);
    return Success;
}

rpc_status erpc::MessageBuffer::moveReadPos(uint32_t length, uint16_t offset)
{
    // check if dst_pos > m_len
    if (offset + length > m_len) {
        LOGE(TAG, "moveReadPos BufferOverrun: offset = %" PRIu16 ", length = %" PRIu32 ", m_len = %" PRIu16, offset, length, m_len);
        return rpc_status::BufferOverrun;
    }
    m_read_pos = offset + length;
    return rpc_status::Success;
}

rpc_status erpc::MessageBuffer::moveWritePos(uint32_t length, uint16_t offset)
{
    // check if dst_pos > m_len
    if (offset + length > m_len) {
        LOGE(TAG, "moveWritePos BufferOverrun: offset = %" PRIu16 ", length = %" PRIu32 ", m_len = %" PRIu16, offset, length, m_len);
        return rpc_status::BufferOverrun;
    }
    m_write_pos = offset + length;
    return rpc_status::Success;
}

rpc_status erpc::MessageBuffer::prepareForWrite(uint32_t length, uint16_t offset)
{
    // realloc memory if needed
    uint16_t dst = offset + length;
    if (dst > m_len) {
        uint16_t new_len = dst < 2 * m_len ? 2 * m_len : dst;
        uint8_t *new_buf = static_cast<uint8_t *>(realloc(m_buf, new_len));
        m_buf = new_buf;
        if (new_buf == NULL) {
            LOGE(TAG, "prepareForWrite failed to realloc: offset = %" PRIu16 ", length = %" PRIu32 ", m_len = %" PRIu16 ", new_len = %" PRIu16,
                 offset, length, m_len, new_len);
            return rpc_status::MemoryError;
        }
        LOGW(TAG, "prepareForWrite realloc: offset = %" PRIu16 ", length = %" PRIu32 ", m_len = %" PRIu16 ", new_len = %" PRIu16, offset, length,
             m_len, new_len);
        m_len = new_len;
    }
    return rpc_status::Success;
}

rpc_status erpc::MessageBuffer::prepareForRead(uint32_t length, uint16_t offset)
{
    uint16_t dst = offset + length;
    if (dst > m_len) {
        uint16_t new_len = dst < 2 * m_len ? 2 * m_len : dst;
        uint8_t *new_buf = static_cast<uint8_t *>(realloc(m_buf, new_len));
        m_buf = new_buf;
        if (new_buf == NULL) {
            LOGE(TAG, "prepareForRead failed to realloc: offset = %" PRIu16 ", length = %" PRIu32 ", m_len = %" PRIu16 ", new_len = %" PRIu16, offset,
                 length, m_len, new_len);
            return rpc_status::MemoryError;
        }
        LOGW(TAG, "prepareForRead realloc: offset = %" PRIu16 ", length = %" PRIu32 ", m_len = %" PRIu16 ", new_len = %" PRIu16, offset, length,
             m_len, new_len);
        m_len = new_len;
    }
    return rpc_status::Success;
}
