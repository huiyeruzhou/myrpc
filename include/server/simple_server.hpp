/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SIMPLE_SERVER_H
#define SIMPLE_SERVER_H

#include "port/port.h"
#include "server/server_base.hpp"
#include "transport/tcp_transport.hpp"
#include "server/server_worker.hpp"
#include <fcntl.h>
#ifdef CONFIG_HAS_ANDROID
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#endif


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
    SimpleServer(const char *host, uint16_t port);

    ~SimpleServer();
    /*!
     * @brief Run server in infinite loop.
     *
     * Will never jump out from this function.
     */
    virtual rpc_status run(void) override;



    /*!
     * @brief This function sets server from ON to OFF
     */
    virtual void stop(void) override;

    /*!
     * @brief Open the server 
     *
     * Awake the network poller thread and begin to listen 
     */
    virtual rpc_status open(void) override;
    
    /*!
     * @brief This function disconnects client or stop server host.
     *
     * @param[in] stopServer Specify is server shall be closed as well (stop listen())
     * @retval #Success Always return this.
     */
    virtual rpc_status close(void);
    

private:
    /*!
     * @brief callback called when new socket accepted
     *
     * Create and start a server_worker to handle this connection
     * @param[in] sockfd return value of accept()
     * @param[in] port port numeber of the tcp connection
     */
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
    * Listen on the given port, after socket accpeted it will call the `onNewSocket` function
    */
    void networkpollerThread(void);
    /*!
    * @brief Thread entry point.
    *
    * Control is passed to the networkpollerThread() method of the SimpleServer instance pointed to
    * by the @c arg parameter.
    *
    * @param arg Thread argument. The pointer to the SimpleServer instance is passed through
    *  this argument.
    */
    static void networkpollerStub(void *arg);
};

} // namespace erpc

/*! @} */

#endif // SIMPLE_SERVER_H
