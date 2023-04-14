/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RPC_STATUS_H
#define RPC_STATUS_H
#ifdef  __cplusplus  
extern "C" {
#endif  
/*!
 * @addtogroup infra
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/*! @brief eRPC status return codes. */

extern const char *status_string[];
extern const char *StatusToString(int stat);
#define CHECK_STATUS(status, err) \
    if (((err) = (status)) != rpc_status::Success) {\
        LOGE(TAG,"Error occurred: %s", StatusToString((err)));\
        return (err); \
    }
enum rpc_status
{
    //! No error occurred.
    Success= 0,

    //! Generic failure.
    Fail = 1,

    //! Argument is an invalid value.
    InvalidArgument,

    //! Operated timed out.
    rpc_status_timeout,

    //! Message header contains an unknown version.
    InvalidMessageVersion,

    //! Failed to encode or decode a message.
    NanopbCodecError,

    //! Expected a reply message but got another message type.
    UnExpectedMsgType,

    UnknownService,

    //! Attempt to read or write past the end of a buffer.
    BufferOverrun,

    //! Could not find host with given name.
    kErpcStatus_UnknownName,

    //! Failed to connect to host.
    kErpcStatus_ConnectionFailure,

    //! Connected closed by peer.
    kErpcStatus_ConnectionClosed,

    //! Memory allocation error.
    MemoryError,

    //! Server is stopped.
    kErpcStatus_ServerIsDown,

    //! Transport layer initialization failed.
    kErpcStatus_InitFailed,

    //! Failed to receive data.
    kErpcStatus_ReceiveFailed,

    //! Failed to send data.
    kErpcStatus_SendFailed,

    //! Sending/Receiving callback function which is not defined in IDL.
    kErpcStatus_UnknownCallback,

    //! Calling eRPC function from another eRPC function. For more information see erpc_status.h.
    UnexpectedTransportOperation,

    //! When sending address from bigger architecture to smaller.
    kErpcStatus_BadAddressScale,

    IOError
};

/*! @} */
#ifdef  __cplusplus  
}
#endif  
#endif /* RPC_STATUS_H */
