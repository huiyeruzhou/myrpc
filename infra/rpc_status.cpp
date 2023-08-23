/*
 * Copyright 2023 YuHongli
 *
 * File: rpc_status.cpp
 * Description: the rpc status code and StatusToString function
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: YuHongli
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    YuHongli       Create and initialize
 */
#include "rpc_status.hpp"
const char *StatusToString(int stat)
{
    static const char *status_string[] = {
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
    return status_string[stat];
}
