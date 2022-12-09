/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EMBEDDED_RPC__TRANSPORT_H_
#define _EMBEDDED_RPC__TRANSPORT_H_

#include "erpc_status.h"
#include "erpc_message_buffer.hpp"


/*!
 * @addtogroup infra_transport
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

namespace erpc {

/*!
 * @brief Abstract interface for transport layer.
 *
 * @ingroup infra_transport
 */
class Transport
{
public:
    /*!
     * @brief Constructor.
     */
    Transport() = default;

    /*!
     * @brief Transport destructor
     */
    virtual ~Transport() = default;

    /*!
     * @brief Prototype for receiving message.
     *
     * Each transport layer need define this function.
     *
     * @param[out] message Will return pointer to received message buffer.
     *
     * @return based on receive implementation.
     */
    virtual erpc_status_t receive(MessageBuffer *message) = 0;

    /*!
     * @brief Prototype for send message.
     *
     * Each transport layer need define this function.
     *
     * @param[in] message Pass message buffer to send.
     *
     * @return based on send implementation.
     */
    virtual erpc_status_t send(MessageBuffer *message) = 0;
};

/*!
 * @brief Abstract interface for transport factory.
 *
 * @ingroup infra_transport
 */
class TransportFactory
{
public:
    /*!
     * @brief Constructor.
     */
    TransportFactory() = default;
    /*!
     * @brief TransportFactory destructor
     */
    virtual ~TransportFactory() = default;
    /*!
     * @brief Return created transport object.
     *
     * @return Pointer to created transport object.
     */
    virtual Transport *create() = 0;
};

} // namespace erpc

/*! @} */

#endif // _EMBEDDED_RPC__TRANSPORT_H_
