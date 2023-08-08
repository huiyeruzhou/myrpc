#include <codec/nanopb_codec.hpp>

__attribute__((unused)) static const char *TAG = "nanopb";
namespace erpc {
rpc_status NanopbCodec::write(MessageBuffer *obuf, const pb_msgdesc_t *fields,
                              const void *src_struct) {
  // encode the size
  size_t size;
  rpc_status err = rpc_status::Success;
  if (!pb_get_encoded_size(&size, fields, src_struct)) {
    LOGE(TAG, "pb_get_encoded_size() failed");
    return NanopbCodecError;
  }
  // write size, write_pos will be set.
  uint8_t no_compress = 0;
  CHECK_STATUS(
      obuf->write(reinterpret_cast<void *>(&no_compress), sizeof(no_compress)),
      err);
  uint32_t size_n = htonl((uint32_t)size);
  CHECK_STATUS(obuf->write(reinterpret_cast<void *>(&size_n), sizeof(size_n)),
               err);

  // encode the message
  obuf->prepareForWrite(size);
  pb_ostream_t ostream = pb_ostream_from_buffer(obuf->getWrite(), size);
  if (!pb_encode(&ostream, fields, src_struct)) {
    LOGE(TAG, "pb_encode() failed: %s", PB_GET_ERROR(&ostream));
    return NanopbCodecError;
  }
  // set write_pos
  obuf->moveWritePos(size);
  return Success;
}

rpc_status NanopbCodec::get(MessageBuffer *ibuf, const pb_msgdesc_t *fields,
                            void *dst_struct) {
  // read the size of this message
  rpc_status err = rpc_status::Success;
  CHECK_STATUS(ibuf->prepareForRead(1 + sizeof(uint32_t)), err);
  ibuf->moveReadPos(1);
  uint32_t size = ntohl(*reinterpret_cast<uint32_t *>(ibuf->getRead()));
  ibuf->moveReadPos(sizeof(uint32_t));
  // LOGE(TAG, "m_read_pos = %" PRIu16, ibuf->m_read_pos);

  // decode the message
  CHECK_STATUS(ibuf->prepareForRead(size), err);
  pb_istream_t istream =
      pb_istream_from_buffer(ibuf->getRead(), static_cast<size_t>(size));
  if (!pb_decode(&istream, fields, dst_struct)) {
    LOGE(TAG, "pb_decode() failed: %s", PB_GET_ERROR(&istream));
    return NanopbCodecError;
  } else {
    // set read position so that other may not read this poroto message again
    ibuf->moveReadPos(size);
    // LOGE(TAG, "m_read_pos = %" PRIu16, ibuf->m_read_pos);
    return err;
  }
}
}  // namespace erpc
