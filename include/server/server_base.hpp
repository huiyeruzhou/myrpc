/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * Copyright 2020 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EMBEDDED_RPC__SERVER_H_
#define _EMBEDDED_RPC__SERVER_H_

#include "rpc_base.hpp"
#include "port/port.h"
#include "server/service.hpp"


/*!
 * @addtogroup infra_server
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

namespace erpc {
/*!
 * @brief Abstract interface for service, which can be executed on server side.
 *
 * @ingroup infra_server
 */
/*!
 * @brief Based server functionality.
 *
 * @ingroup infra_server
 */
class Server : public CSBase
{
public:
    /*!
     * @brief Constructor.
     *
     * This function initializes object attributes.
     */
    Server(const char *host, uint16_t port)
    : CSBase(host, port)
    , m_firstService(NULL)
    {
    }

    /*!
     * @brief Server destructor
     */
    virtual ~Server(void) {}

    /*!
     * @brief Add service.
     *
     * @param[in] service Service to use.
     */
    void addService(Service *service);

    /*!
     * @brief Remove service.
     *
     * @param[in] service Service to remove.
     */
    void removeService(Service *service);

    /*!
* @brief This function will create host on server side, or connect client to the server.
*
* @retval #Success When creating host was successful or client connected successfully.
* @retval #kErpcStatus_UnknownName Host name resolution failed.
* @retval #kErpcStatus_ConnectionFailure Connecting to the specified host failed.
*/
    virtual rpc_status open(void) = 0;
    
    /*!
     * @brief This function runs the server.
     */
    virtual rpc_status run(void) = 0;

    /*!
     * @brief This function stop the server.
     */
    virtual void stop(void) = 0;

protected:
    Service *m_firstService; /*!< Contains pointer to first service. */

private:
    // Disable copy ctor.
    Server(const Server &other);            /*!< Disable copy ctor. */
    Server &operator=(const Server &other); /*!< Disable copy ctor. */
};

} // namespace erpc

/*! @} */

#endif // _EMBEDDED_RPC__SERVER_H_
