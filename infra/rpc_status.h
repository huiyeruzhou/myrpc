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

/*!
 * @addtogroup infra
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/*! @brief eRPC status return codes. */
enum rpc_status_t
{
    //! No error occurred.
    rpc_status_success = 0,

    //! Generic failure.
    rpc_status_fail = 1,

    //! Argument is an invalid value.
    rpc_status_invalid_argument = 4,

    //! Operated timed out.
    rpc_status_timeout = 5,

    //! Message header contains an unknown version.
    kErpcStatus_InvalidMessageVersion = 6,

    //! Expected a reply message but got another message type.
    kErpcStatus_ExpectedReply,

    //! Attempt to read or write past the end of a buffer.
    kErpcStatus_BufferOverrun,

    //! Could not find host with given name.
    kErpcStatus_UnknownName,

    //! Failed to connect to host.
    kErpcStatus_ConnectionFailure,

    //! Connected closed by peer.
    kErpcStatus_ConnectionClosed,

    //! Memory allocation error.
    kErpcStatus_MemoryError,

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
    kErpcStatus_NestedCallFailure,

    //! When sending address from bigger architecture to smaller.
    kErpcStatus_BadAddressScale
};

/*! @brief Type used for all status and error return values. */
typedef enum rpc_status_t rpc_status_t;

/*! @} */

#endif /* RPC_STATUS_H */
