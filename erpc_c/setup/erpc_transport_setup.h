/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2019 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ERPC_TRANSPORT_SETUP_H_
#define _ERPC_TRANSPORT_SETUP_H_

/*!
 * @addtogroup transport_setup
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

//! @brief Opaque transport object type.
typedef struct ErpcTransport *erpc_transport_t;
//! @brief Ready callback object type for RPMsg-Lite transport.
typedef void (*rpmsg_ready_cb)(void);

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

//! @name TCP transport setup
//@{

/*!
 * @brief Create and open TCP transport
 *
 * For server, create a TCP listen socket and wait for connections
 * For client, connect to server
 *
 * @param[in] host hostname/IP address to listen on or server to connect to
 * @param[in] port port to listen on or server to connect to
 * @param[in] isServer true if we are a server
 *
 * @return Return NULL or erpc_transport_t instance pointer.
 */
erpc_transport_t erpc_transport_tcp_init(const char *host, uint16_t port, bool isServer);

/*!
 * @brief Close TCP connection
 *
 * For server, stop listening and close all sockets. Note that the server mode
 * uses and accept() which is a not-recommended blocking method so we can't exit
 * until a connection attempts is made. This is a deadlock but assuming that TCP
 * code is supposed to be for test, I assume it's acceptable. Otherwise a non-blocking
 * socket or select() shoudl be used
 * For client, close server connection
 *
 * @return Return TRUE if listen/connection successful
 */
void erpc_transport_tcp_close(void);
//@}
#ifdef __cplusplus
}
#endif

/*! @} */

#endif // _ERPC_TRANSPORT_SETUP_H_
