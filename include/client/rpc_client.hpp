/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * Copyright 2020-2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include "rpc_status.hpp"
#include "port/port.h"
#include "rpc_base.hpp"
#include "codec/message_buffer.hpp"
#include <netdb.h>
#ifdef CONFIG_HAS_POSIX
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

/*!
 * @addtogroup infra_client
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

namespace erpc {
#if ERPC_NESTED_CALLS
class Server;
#endif

/*!
 * @brief Base client implementation.
 *
 * @ingroup infra_client
 */
class Client : public CSBase
{
public:
    /*!
     * @brief Constructor.
     *
     * This function initializes object attributes.
     */
    Client(const char *host, uint16_t port);

    /*!
     * @brief Client destructor
     */
    virtual ~Client(void);

    /*!
    * @brief This function connect client to the server.
    *
    * @retval Success When client connected successfully.
    * @retval Fail When client doesn't connected successfully.
    */
    virtual rpc_status open(void);
protected:
    virtual rpc_status performRequest(char *path, const pb_msgdesc_t *req_desc, void *req, const pb_msgdesc_t *rsp_desc, void *rsp);
private:
    uint32_t m_sequence;                    //!< Sequence number.
    Client(const Client &other);            //!< Disable copy ctor.
    Client &operator=(const Client &other); //!< Disable copy ctor.
};



} // namespace erpc

/*! @} */

#endif // RPC_CLIENT_H
