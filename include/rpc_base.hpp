/*
 * Copyright 2023 YuHongli
 *
 * File: rpc_base.hpp
 * Description: the base class of rpc client and server.
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: YuHongli
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    YuHongli       Create and initialize
 */
#ifndef RPC_BASE_HPP_
#define RPC_BASE_HPP_

#include "port/port.h"
#include "transport/tcp_transport.hpp"

#if CONFIG_HAS_FREERTOS
#ifdef __cplusplus
extern "C" {
#endif
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#ifdef __cplusplus
}
#endif
#else
#ifdef __cplusplus
extern "C" {
#endif
#include <netdb.h>
#ifdef __cplusplus
}
#endif
#endif

#include <cstdio>
#ifdef __cplusplus
extern "C" {
#endif
#include <errno.h>
#include <netinet/tcp.h>
#ifdef __cplusplus
}
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
class CSBase {
   public:
    /*!
     * @brief CSBase constructor.
     */
    CSBase(const char *host, uint16_t port) : m_host(host), m_port(port), m_sockfd(-1), m_transport(NULL) {}

    /*!
     * @brief CSBase destructor
     */
    ~CSBase(void) {}

    /*!
     * @brief This function sets transport layer to use.
     *
     * It also set messageBufferFactory to the same as in transport layer.
     *
     * @param[in] transport Transport layer to use.
     *
     */
    void setTransport(TCPTransport *transport) { m_transport = transport; }

    /*!
     * @brief This function gets transport instance.
     *
     * @return Transport * Pointer to transport instance.
     */
    TCPTransport *getTransport(void) { return m_transport; }

   protected:
    const char *m_host; /*!< Specify the host name or IP address of the computer. */
    uint16_t m_port;    /*!< Specify the listening port number. */
    int m_sockfd;
    TCPTransport *m_transport;
};

}  // namespace erpc

#endif  // RPC_BASE_HPP_
