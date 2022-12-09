/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EMBEDDED_RPC__SIMPLE_SERVER_H_
#define _EMBEDDED_RPC__SIMPLE_SERVER_H_

#include "erpc_threading.h"
#include "erpc_basic_codec.hpp"
#include "erpc_message_buffer.hpp"
#include "erpc_simple_server.hpp"
#include "tcp_worker.hpp"
#include "erpc_server.hpp"
#include "server_worker.hpp"
#include <fcntl.h>

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
 * @brief Based server implementation.
 *
 * @ingroup infra_server
 */
class SimpleServer : public Server
{
public:
    /*!
     * @brief Constructor.
     *
     * This function initializes object attributes.
     */
    SimpleServer(const char *host, uint16_t port, MessageBufferFactory *message_buffer_factory);

    ~SimpleServer();
    /*!
     * @brief Run server in infinite loop.
     *
     * Will never jump out from this function.
     */
    virtual erpc_status_t run(void) override;

    // /*!
    //  * @brief Run server implementation only if exist message to process.
    //  *
    //  * If is message to process, server process it and jumps out from this function,
    //  * useful for bare-metal because doesn't block main loop, when are not messages
    //  * to process.
    //  *
    //  * @return Return true when server is ON, else false.
    //  */
    // virtual erpc_status_t poll(void);

    /*!
     * @brief This function sets server from ON to OFF
     */
    virtual void stop(void) override;


    virtual erpc_status_t open(void) override;
    
    /*!
     * @brief This function disconnects client or stop server host.
     *
     * @param[in] stopServer Specify is server shall be closed as well (stop listen())
     * @retval #kErpcStatus_Success Always return this.
     */
    virtual erpc_status_t close(bool stopServer = true);
    


    
    void onNewSocket(int socketfd, int port);

//     /*!
//      * @brief Disposing message buffers and codecs.
//      *
//      * @param[in] codec Pointer to codec to dispose. It contains also message buffer to dispose.
//      */
//     void disposeBufferAndCodec(Codec *codec);

    bool  m_isServerOn; /*!< Information if server is ON or OFF. */
    Thread m_serverThread; /*!< Pointer to server thread. */
    bool m_runServer;      /*!< Thread is executed while this is true. */
    bool m_isServer;       /*!< If true then server is using transport, else client. */

protected:
    /*!
 * @brief Server thread function.
 */
    void networkpollerThread(void);
    /*!
 * @brief Thread entry point.
 *
 * Control is passed to the networkpollerThread() method of the TCPTransport instance pointed to
 * by the @c arg parameter.
 *
 * @param arg Thread argument. The pointer to the TCPTransport instance is passed through
 *  this argument.
 */
    static void networkpollerStub(void *arg);
};

} // namespace erpc

/*! @} */

#endif // _EMBEDDED_RPC__SIMPLE_SERVER_H_
