#include "rpc_status.hpp"
const char *status_string[] = {
    //! Success
   "Success",
   //! Success
   "Fail",
   "InvalidArgument",
   "Timeout",

   //! Message header contains an unknown version.
   "InvalidMessageVersion",

   //! Failed to encode or decode a message.
   "NanopbCodecError",

   //! Expected a reply message but got another message type.
   "UnExpectedMsgType",

   "UnknownService",

   //! Attempt to read or write past the end of a buffer.
   "BufferOverrun",

   //! Could not find host with given name.
   "UnknownAddress",

   //! Failed to connect to host.
   "ConnectionFailure",

   //! Connected closed by peer.
   "ConnectionClosed",

   //! Memory allocation error.
   "MemoryError",

   //! Failed to receive data.
   "ReceiveFailed",

   //! Failed to send data.
   "SendFailed",

   "UnexpectedTransportOperation",
   "IOError",

   "UnimplmentedService",
};
extern const char *StatusToString(int stat) {
    return status_string[stat];
}