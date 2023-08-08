/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SERVER_SIMPLE_SERVER_HPP_
#define SERVER_SIMPLE_SERVER_HPP_

#include <fcntl.h>

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

  rpc_status run(void) override;

  void stop(void) override;

  rpc_status open(void) override;

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
  /* Information if server is ON or OFF. OFF means never
    accept incoming connection.
    Call stop() will set m_isServerOn to false*/
  bool m_isServerOn;
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
