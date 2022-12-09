/*
 * Copyright 2020 NXP
 * Copyright 2020 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EMBEDDED_RPC__CLIENTSERVERCOMMON_H_
#define _EMBEDDED_RPC__CLIENTSERVERCOMMON_H_

#include "erpc_config_internal.h"
#include "erpc_codec.hpp"
#include "erpc_transport.hpp"
#if CONFIG_HAS_FREERTOS
#include <cstdio>
extern "C" {
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <netinet/tcp.h>
#include <errno.h>
}
#else
#include <cstdio>
extern "C" {
#include <netdb.h>
#include <netinet/tcp.h>
#include <errno.h>
}
#endif
#define TCP_TRANSPORT_DEBUG_LOG (1)
#if TCP_TRANSPORT_DEBUG_LOG
char* print_net_info(const sockaddr *_sockaddr, int _len);
int getPortFormAddr(const sockaddr * _sockaddr, int _len);
#else
#define LOGE
#define LOGI
#define LOGD
#define LOGV
#define TCP_DEBUG_PRINT(_fmt_, ...)
#define TCP_DEBUG_ERR(_msg_)
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
class ClientServerCommon
{
public:
    /*!
     * @brief ClientServerCommon constructor.
     */
    ClientServerCommon(const char *host, uint16_t port):
        m_host(host), m_port(port),m_sockfd(-1),
        m_messageFactory(nullptr), m_codecFactory(nullptr), m_transport(nullptr) {};

    /*!
     * @brief ClientServerCommon destructor
     */
    ~ClientServerCommon(void)= default;

    /*!
     * @brief This function sets message buffer factory to use.
     *
     * @param[in] factory Message buffer factory to use.
     */
    void setMessageBufferFactory(MessageBufferFactory *factory) { m_messageFactory = factory; }

    /*!
     * @brief This function sets codec factory to use.
     *
     * @param[in] factory Codec factory to use.
     */
    void setCodecFactory(CodecFactory *factory) { m_codecFactory = factory; }

    /*!
     * @brief This function sets codec factory to use.
     *
     * @return CodecFactory * Codec factory to use.
     */
    CodecFactory * getCodecFactory(void) { return m_codecFactory; }

    /*!
     * @brief This function sets transport layer to use.
     *
     * It also set messageBufferFactory to the same as in transport layer.
     *
     * @param[in] transport Transport layer to use.
     */
    void setTransport(Transport *transport) { m_transport = transport; }

    /*!
     * @brief This function gets transport instance.
     *
     * @return Transport * Pointer to transport instance.
     */
    Transport *getTransport(void) { return m_transport; }
    const char *m_host;    /*!< Specify the host name or IP address of the computer. */
    uint16_t m_port;       /*!< Specify the listening port number. */
    int m_sockfd;

protected:
    MessageBufferFactory *m_messageFactory; //!< Message buffer factory to use.
    CodecFactory *m_codecFactory;           //!< Codec to use.
    Transport *m_transport;                 //!< Transport layer to use.


};

} // namespace erpc

/*! @} */

#endif // _EMBEDDED_RPC__CLIENTSERVERCOMMON_H_
