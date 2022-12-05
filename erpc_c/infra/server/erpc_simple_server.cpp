/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2019-2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_simple_server.hpp"
#include <stdio.h>
using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

erpc_status_t SimpleServer::run(void)
{
    erpc_status_t err = kErpcStatus_Success;
    while ((err == kErpcStatus_Success) && m_isServerOn)
    {
        // Sleep 10 ms.
         Thread::sleep(10000);
    }
    return err;
}

// erpc_status_t SimpleServer::poll(void)
// {
//     erpc_status_t err;

//     if (m_isServerOn)
//     {
//         if (m_transport->hasMessage() == true)
//         {
//             err = runInternal();
//         }
//         else
//         {
//             err = kErpcStatus_Success;
//         }
//     }
//     else
//     {
//         err = kErpcStatus_ServerIsDown;
//     }

//     return err;
// }

void SimpleServer::stop(void)
{
    m_isServerOn = false;
}

void SimpleServer::onNewSocket(int sockfd, int port) {
    printf("%d\n", sockfd);
    TCPWorker *transport_worker = new TCPWorker(sockfd, port);
    ServerWorker *worker = new ServerWorker(m_firstService, m_messageFactory, m_codecFactory, transport_worker);
    worker->m_workerThread.start(worker);
}

