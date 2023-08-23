#ifndef RPC_STATUS_HPP_
#define RPC_STATUS_HPP_
#ifdef __cplusplus
extern "C" {
#endif

const char *StatusToString(int stat);
#define CHECK_STATUS(status, err)                                                           \
    if (((err) = (status)) != rpc_status::Success) {                                        \
        LOGE(TAG, "In function `%s`, error occurred: %s", __func__, StatusToString((err))); \
        return (err);                                                                       \
    }
enum rpc_status {
    //! No error occurred.
    Success = 0,

    //! Generic failure.
    Fail = 1,

    //! Argument is an invalid value.
    InvalidArgument,

    //! Time out.
    Timeout,

    //! Message header contains an unknown version.
    InvalidMessageVersion,

    //! Failed to encode or decode a message.
    NanopbCodecError,

    //! Expected a reply message but got another message type.
    UnExpectedMsgType,

    //! Failed to find the service by given path in the register vector.
    UnknownService,

    //! Attempt to read or write past the end of a buffer.
    BufferOverrun,

    //! Could not find server with given host:port.
    UnknownAddress,

    //! Failed to connect to host.
    ConnectionFailure,

    //! Connected closed by peer.
    ConnectionClosed,

    //! Memory allocation error.
    MemoryError,

    //! Failed to receive data.
    ReceiveFailed,

    //! Failed to send data.
    SendFailed,

    //! received init/trailing/msg but unexpected.
    UnexpectedTransportOperation,

    //! close(fd) return negative number.
    IOError,

    //! Unimplement stub function.
    UnimplmentedService,
};

#ifdef __cplusplus
}
#endif
#endif  // RPC_STATUS_HPP_
