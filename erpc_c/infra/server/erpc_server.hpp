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

#include "erpc_client_server_common.hpp"
#include "erpc_config_internal.h"
#include "service.hpp"


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
class Server : public ClientServerCommon
{
public:
    /*!
     * @brief Constructor.
     *
     * This function initializes object attributes.
     */
    Server(void)
    : ClientServerCommon()
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
     * @brief This function runs the server.
     */
    virtual erpc_status_t run(void) = 0;

    /*!
     * @brief This function stop the server.
     */
    virtual void stop(void) = 0;

protected:
    Service *m_firstService; /*!< Contains pointer to first service. */

    /*!
    //  * @brief Process message.
    //  *
    //  * @param[in] codec Inout codec to use.
    //  * @param[in] msgType Type of received message. Based on message type will be (will be not) sent respond.
    //  * @param[in] serviceId To identify interface.
    //  * @param[in] methodId To identify function in interface.
    //  * @param[in] sequence To connect correct answer with correct request.
    //  *
    //  * @returns #kErpcStatus_Success or based on codec startReadMessage.
    //  */
    // virtual erpc_status_t processMessage(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
    //                                      uint32_t sequence);

    // /*!
    //  * @brief Read head of message to identify type of message.
    //  *
    //  * @param[in] codec Inout codec to use.
    //  * @param[out] msgType Type of received message. Based on message type will be (will be not) sent respond.
    //  * @param[out] serviceId To identify interface.
    //  * @param[out] methodId To identify function in interface.
    //  * @param[out] sequence To connect correct answer with correct request.
    //  *
    //  * @returns #kErpcStatus_Success or based on service handleInvocation.
    //  */
    // virtual erpc_status_t readHeadOfMessage(Codec *codec, message_type_t &msgType, uint32_t &serviceId,
    //                                         uint32_t &methodId, uint32_t &sequence);

    // /*!
    //  * @brief This function finds service base on service ID.
    //  *
    //  * @param[in] serviceId Service id number.
    //  *
    //  * @return Pointer to service or NULL.
    //  */
    // virtual Service *findServiceWithId(uint32_t serviceId);



private:
    // Disable copy ctor.
    Server(const Server &other);            /*!< Disable copy ctor. */
    Server &operator=(const Server &other); /*!< Disable copy ctor. */
};

} // namespace erpc

/*! @} */

#endif // _EMBEDDED_RPC__SERVER_H_
