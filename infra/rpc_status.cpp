#include "rpc_status.hpp"
const char *status_string[] = {
   "Success",
   "Fail",
   "InvalidArgument",
   "rpc_status_timeout",

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
   "kErpcStatus_UnknownName",

   //! Failed to connect to host.
   "kErpcStatus_ConnectionFailure",

   //! Connected closed by peer.
   "kErpcStatus_ConnectionClosed",

   //! Memory allocation error.
   "MemoryError",

   //! Server is stopped.
   "kErpcStatus_ServerIsDown",

   //! Transport layer initialization failed.
   "kErpcStatus_InitFailed",

   //! Failed to receive data.
   "kErpcStatus_ReceiveFailed",

   //! Failed to send data.
   "kErpcStatus_SendFailed",

   //! Sending/Receiving callback function which is not defined in IDL.
   "kErpcStatus_UnknownCallback",

   //! Calling eRPC function from another eRPC function. For more information see erpc_status.h.
   "kErpcStatus_NestedCallFailure",

   //! When sending address from bigger architecture to smaller.
   "kErpcStatus_BadAddressScale",
   "IOError",
};
extern const char *StatusToString(int stat) {
    return status_string[stat];
}