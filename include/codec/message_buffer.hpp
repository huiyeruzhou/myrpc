#ifndef CODEC_MESSAGE_BUFFER_HPP_
#define CODEC_MESSAGE_BUFFER_HPP_
#include "port/port.h"
#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <string>
extern "C" {
#endif
#include "rpc_status.hpp"

namespace erpc {

class MessageBuffer {
 public:
  MessageBuffer(void)
      : m_buf(reinterpret_cast<uint8_t *>(malloc(256 * sizeof(uint8_t)))),
        m_len(256),
        m_read_pos(0),
        m_write_pos(0) {}
  ~MessageBuffer(void) { free(m_buf); }

  MessageBuffer(uint8_t *buffer, uint16_t length)
      : m_buf(buffer), m_len(length), m_read_pos(0), m_write_pos(0) {}

  /*!
   * @brief This function returns pointer to buffer to read/write.
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

  uint16_t getReadPos(void) { return m_read_pos; }
  uint16_t getWritePos(void) { return m_write_pos; }

  /*!
   * @brief This function read data from local buffer.
   *
   * @param[in] offset Offset in local buffer.
   * @param[inout] data Given buffer to save read data.
   * @param[in] length Length of data to read.
   *
   * @return Status from reading data.
   */
  rpc_status read(void *data, uint32_t length) {
    return read(data, length, m_read_pos);
  }
  rpc_status read(void *data, uint32_t length, uint16_t offset);

  /*!
   * @brief This function write data to local buffer.
   *
   * @param[in] offset Offset in local buffer.
   * @param[inout] data Given buffer from which are copied data.
   * @param[in] length Length of data to write.
   *
   * @return Status from reading data.
   */
  rpc_status write(const void *data, uint32_t length) {
    return write(data, length, m_write_pos);
  }
  rpc_status write(const void *data, uint32_t length, uint16_t offset);

  /*!
   * @brief This function reset the buffer by setting read/write position to 0.
   */
  rpc_status reset(void) {
    m_read_pos = 0;
    m_write_pos = 0;
    return rpc_status::Success;
  }

  /*!
   * @brief move the position of read/write.
   * @param length the distance of position to move.
   * @return if success, return rpc_status::Success.
   *     if failed when length > m_read_pos/m_write_pos, return
   * rpc_status::InvalidArgument. if length < 0, return
   * rpc_status::InvalidArgument.
   */
  rpc_status moveReadPos(uint32_t length, uint16_t offset);
  rpc_status moveReadPos(uint32_t length) {
    return moveReadPos(length, m_read_pos);
  }

  rpc_status moveWritePos(uint32_t length, uint16_t offset);
  rpc_status moveWritePos(uint32_t length) {
    return moveWritePos(length, m_write_pos);
  }

  /*!
   * @brief Prepare buffer for  read/write data.
   *          do nothing if length > 0.
   * @param length the length of data to  read/write.
   * @return status, if success, return rpc_status::Success.
   *      if failed when realloc, return rpc_status::MemoryError.
   *      if length < 0, return rpc_status::InvalidArgument.
   */
  rpc_status prepareForWrite(uint32_t length, uint16_t offset);
  rpc_status prepareForWrite(uint32_t length) {
    return prepareForWrite(length, m_write_pos);
  }
  rpc_status prepareForRead(uint32_t length, uint16_t offset);
  rpc_status prepareForRead(uint32_t length) {
    return prepareForRead(length, m_read_pos);
  }

 private:
  uint8_t *m_buf;       /*!< Buffer used to read write data. */
  uint16_t m_len;       /*!< Length of buffer. */
  uint16_t m_read_pos;  /*!< Used buffer bytes. */
  uint16_t m_write_pos; /*!< Left buffer bytes. */
};
}  // namespace erpc

/*! @} */

#ifdef __cplusplus
}
#endif
#endif  // CODEC_MESSAGE_BUFFER_HPP_
