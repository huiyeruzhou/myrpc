/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: simple_server.hpp
 * Description: This file defines the SimpleServer class. Which controls the network thread and the worker threads.
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */

#ifndef SERVER_SIMPLE_SERVER_HPP_
#define SERVER_SIMPLE_SERVER_HPP_

#include <fcntl.h>

#include <atomic>
#include <memory>

#include "port/port.h"
#include "server/server_base.hpp"
#include "server/server_worker.hpp"
#include "transport/tcp_transport.hpp"
#ifdef CONFIG_HAS_ANDROID
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

namespace erpc {
/*!
 * @brief Based server implementation.
 *
 * @ingroup infra_server
 */
class SimpleServer : public Server {
   public:
    SimpleServer(const char *host, uint16_t port);

    ~SimpleServer();

    /*!
     * @brief This function will start the network thread to accept new
     * connections.
     */
    rpc_status run(void) override;

    /*!
     * @brief This function will stop the network thread and all worker thread, if it is running,
     * the Server object will then be deleted by the network thread.
     */
    void stop(void) override;

    /*!
     * @brief This function will open the network thread to accept new
     * connections.
     */
    rpc_status open(void) override;

    /*!
     * @brief This function will close the network thread, but do not effect the worker threads that
     * has been created. It will NOT delete the Server object.
     */
    virtual rpc_status close(void);

    /*!
     * @brief This function is for check if the server is running.If we want to delete the server when it
     * is not running. We should just delete the pointer. Otherwise, we should call stop() and do not delete it.
     */
    bool isServerOn(void) { return *m_isServerOn; }

   private:
    /*!
     * @brief callback called when new socket accepted
     *
     * Create and start a server_worker to handle this connection
     * @param[in] sockfd return value of accept()
     * @param[in] port port numeber of the tcp connection
     */
    void onNewSocket(int socketfd, int port);
    /* Information if server is ON or OFF. OFF means never
      accept incoming connection.
      Call stop() will set m_isServerOn to false*/
    std::shared_ptr<std::atomic_bool> m_isServerOn;
    Thread m_serverThread; /* Pointer to server thread. */
    /* Thread is executed while this is true. Call close() will set it to false*/
    bool m_runServer;

   protected:
    /*!
     * @brief Server thread function.
     * Listen on the given port, after socket accpeted it will call the
     * `onNewSocket` function
     */
    void networkpollerThread(void);
    /*!
     * @brief Thread entry point.
     *
     * Control is passed to the networkpollerThread() method of the SimpleServer
     * instance pointed to by the @c arg parameter.
     *
     * @param arg Thread argument. The pointer to the SimpleServer instance is
     * passed through this argument.
     */
    static void networkpollerStub(void *arg);
};

}  // namespace erpc

#endif  // SERVER_SIMPLE_SERVER_HPP_
