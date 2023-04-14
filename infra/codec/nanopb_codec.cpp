#include <codec/nanopb_codec.hpp>
__attribute__((unused)) static const char *TAG = "nanopb";
namespace erpc {
    rpc_status NanopbCodec::read(MessageBuffer *ibuf, const pb_msgdesc_t *fields, void *dst_struct) {
        //read the size of this message
        if (5 > ibuf->getReadSize()) {
            return BufferOverrun;
        }
        // skip the compress flag
        ibuf->m_read_pos += 1;
        // read message size
        uint32_t size = ntohl(*(uint32_t *) ibuf->getRead());

        //decode the message
        if (size + 5 > ibuf->getReadSize()) {
            return BufferOverrun;
        }
        pb_istream_t istream = pb_istream_from_buffer(ibuf->getRead() + sizeof(uint32_t), (size_t) size);
        if (!pb_decode(&istream, fields, dst_struct)) {
            LOGE(TAG, "pb_decode_delimited() failed: %s", PB_GET_ERROR(&istream));
            return NanopbCodecError;
        }
        return Success;

    }
    rpc_status NanopbCodec::write(MessageBuffer *obuf, const pb_msgdesc_t *fields, const void *src_struct) {
        //encode the size
        size_t size;
        if (!pb_get_encoded_size(&size, fields, src_struct)) {
            LOGE(TAG, "pb_get_encoded_size() failed");
            return NanopbCodecError;
        }
        if (obuf->getWriteSize() < size + sizeof(uint32_t)) {
            return BufferOverrun;
        }
        //write size, write_pos will be set.
        uint32_t size_n = htonl((uint32_t) size);
        obuf->write((void *) &size_n, sizeof(size_n));


        //encode the message
        pb_ostream_t ostream = pb_ostream_from_buffer(obuf->getWrite(), obuf->getWriteSize());
        if (!pb_encode(&ostream, fields, src_struct)) {
            LOGE(TAG, "pb_encode_delimited() failed: %s", PB_GET_ERROR(&ostream));
            return NanopbCodecError;
        }
        // set write_pos
        obuf->m_write_pos += size;
        return Success;

    }
    rpc_status NanopbCodec::get(MessageBuffer *ibuf, const pb_msgdesc_t *fields, void *dst_struct) {
        //read the size of this message
        if (4 > ibuf->getReadSize()) {
            return BufferOverrun;
        }
        uint32_t size = ntohl(*(uint32_t *) ibuf->getRead());

        //decode the message
        if (size + 4 > ibuf->getReadSize()) {
            return BufferOverrun;
        }
        pb_istream_t istream = pb_istream_from_buffer(ibuf->getRead() + sizeof(uint32_t), (size_t) size);
        if (!pb_decode(&istream, fields, dst_struct)) {
            LOGE(TAG, "pb_decode_delimited() failed: %s", PB_GET_ERROR(&istream));
            return NanopbCodecError;
        }
        else {
            // set read position so that other may not read this poroto message again
            ibuf->m_read_pos += size + sizeof(uint32_t);
            return Success;
        }
    }
}