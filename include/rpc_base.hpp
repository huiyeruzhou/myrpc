/*
 * Copyright 2020 NXP
 * Copyright 2020 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RPC_BASE_H
#define RPC_BASE_H

#include "port/port.h"
#include "transport/transport_base.hpp"
#include "transport/nanopb_transport.hpp"
#if CONFIG_HAS_FREERTOS
#include <cstdio>
#ifdef __cplusplus
extern "C" {
#endif
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <netinet/tcp.h>
#include <errno.h>
#ifdef __cplusplus
}
#endif
#else
#include <cstdio>
#ifdef __cplusplus
extern "C" {
#endif
#include <netdb.h>
#include <netinet/tcp.h>
#include <errno.h>
#ifdef __cplusplus
}
#endif
#endif


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
 * @brief Common class inheritand by client and server class.
 *
 * @ingroup infra_utility
 */
class CSBase
{
public:
    /*!
     * @brief CSBase constructor.
     */
    CSBase(const char *host, uint16_t port):
        m_host(host), m_port(port),m_sockfd(-1),
        m_messageFactory(NULL),  m_transport(NULL) {};

    /*!
     * @brief CSBase destructor
     */
    ~CSBase(void){};

    /*!
     * @brief This function sets message buffer factory to use.
     *
     * @param[in] factory Message buffer factory to use.
     */
    void setMessageBufferFactory(MessageBufferFactory *factory) { m_messageFactory = factory; }

    /*!
     * @brief This function sets transport layer to use.
     *
     * It also set messageBufferFactory to the same as in transport layer.
     *
     * @param[in] transport Transport layer to use.
     */
    void setTransport(Transport *transport) { m_transport_worker = transport; }

    /*!
     * @brief This function gets transport instance.
     *
     * @return Transport * Pointer to transport instance.
     */
    Transport *getTransport(void) { return m_transport_worker; }
    const char *m_host;    /*!< Specify the host name or IP address of the computer. */
    uint16_t m_port;       /*!< Specify the listening port number. */
    int m_sockfd;

protected:
    MessageBufferFactory *m_messageFactory; //!< Message buffer factory to use.
    Transport *m_transport_worker;
    NanopbTransport *m_transport;                 //!< Transport layer to use.

};

} // namespace erpc

/*! @} */

#endif // RPC_BASE_H
