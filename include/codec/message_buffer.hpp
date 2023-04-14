/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MESSAGE_BUFFER_H
#define MESSAGE_BUFFER_H
#include "port/port.h"
#ifdef  __cplusplus  
extern "C" {
#endif  
#include "rpc_status.hpp"
#include <cstddef>
#include <string>
#include <new>
#include <cstring>
#include <cstdint>

    /*!
     * @addtogroup infra_codec
     * @{
     * @file
     */

     ////////////////////////////////////////////////////////////////////////////////
     // Classes
     ////////////////////////////////////////////////////////////////////////////////

    namespace erpc {
        /*!
         * @brief Represents a memory buffer containing a message.
         *
         * The MessageBuffer object does not own the buffer memory. It simply provides an interface
         * to accessing that memory in a convenient manner.
         *
         * @ingroup infra_codec
         */
        class MessageBuffer
        {
        public:
            /*!
             * @brief Constructor.
             *
             * This function initializes object attributes.
             */
            MessageBuffer(void)
                : m_buf(NULL)
                , m_len(0)
                , m_read_pos(0)
                , m_write_pos(4) 
            {
            }

            /*!
             * @brief Constructor.
             *
             * This function initializes object attributes.
             *
             * @param[in] buffer Pointer to buffer.
             * @param[in] length Length of buffer.
             */
            MessageBuffer(uint8_t *buffer, uint16_t length)
                : m_buf(buffer)
                , m_len(length)
                , m_read_pos(0), m_write_pos(4) {
            }

            /*!
             * @brief This function set new buffer and his length.
             *
             * This function set buffer to read/write data.
             *
             * @param[in] buffer Pointer to another buffer to read/write data.
             * @param[in] length Length of buffer.
             */
            void set(uint8_t *buffer, uint16_t length) {
                m_buf = buffer;
                m_len = length;
                m_read_pos = 0;
                m_write_pos = 4;
            }

            /*!
             * @brief This function returns pointer to buffer to read/write.
             *
             * @return Pointer to buffer to read/write.
             */
            uint8_t *get(void) { return m_buf; }

            /*!
             * @brief This function returns pointer to buffer to read/write.
             *
             * @return Pointer to buffer to read/write.
             */
            const uint8_t *get(void) const { return m_buf; }

            uint8_t *getRead(void) { return m_buf + m_read_pos; }
 
            const uint8_t *getRead(void) const { return m_buf + m_read_pos; }

            uint8_t *getWrite(void) { return m_buf + m_write_pos; }

            const uint8_t *getWrite(void) const { return m_buf + m_write_pos; }

            uint16_t getReadSize(void) { return m_len - m_read_pos; }
            uint16_t getWriteSize(void) { return m_len - m_write_pos; }

            /*!
             * @brief This function read data from local buffer.
             *
             * @param[in] offset Offset in local buffer.
             * @param[inout] data Given buffer to save read data.
             * @param[in] length Length of data to read.
             *
             * @return Status from reading data.
             */
            rpc_status read(void *data, uint32_t length, uint16_t offset);
            rpc_status read(void *data, uint32_t length) { read(data, length, m_read_pos); }
            /*!
             * @brief This function write data to local buffer.
             *
             * @param[in] offset Offset in local buffer.
             * @param[inout] data Given buffer from which are copied data.
             * @param[in] length Length of data to write.
             *
             * @return Status from reading data.
             */
            rpc_status write(const void *data, uint32_t length, uint16_t offset);
            rpc_status write(const void *data, uint32_t length) { write(data, length, m_write_pos); }


            uint8_t *m_buf;  /*!< Buffer used to read write data. */
            uint16_t m_len;  /*!< Length of buffer. */
            uint16_t m_read_pos; /*!< Used buffer bytes. */
            uint16_t m_write_pos; /*!< Left buffer bytes. */
        };
        #define MAX_BUFFER_COUNT 10
        class MessageBufferList {
        public:
            MessageBufferList(void)
                : m_count(0)
                , m_buffers(new MessageBuffer *[MAX_BUFFER_COUNT])
            {
            }
            ~MessageBufferList(void) {
                for (size_t i = 0; i < m_count; i++) {
                    m_buffers[i]->~MessageBuffer();
                }
                delete[] m_buffers;
            }
            //assert
            rpc_status add(MessageBuffer *buf);
            rpc_status append(MessageBuffer *buf);
            rpc_status insert(MessageBuffer *buf, uint32_t index);
            MessageBuffer *get(uint32_t index);
            rpc_status destroy(void);
            void Clear();
            std::string JoinIntoString() const;
            uint32_t count(void) { return m_count; }
        private:
            MessageBuffer **m_buffers;
            size_t m_count;
        };
    } // namespace erpc

    /*! @} */

#ifdef  __cplusplus  
}
#endif  
#endif // MESSAGE_BUFFER_H
