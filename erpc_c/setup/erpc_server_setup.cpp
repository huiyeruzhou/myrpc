/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * Copyright 2020-2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_server_setup.h"

#include "erpc_basic_codec.hpp"
#include "erpc_message_buffer.hpp"
#include "erpc_simple_server.hpp"
#include "tcp_worker.hpp"

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

erpc_server_t erpc_server_init(const char *host, uint16_t port, erpc_mbf_t message_buffer_factory)
{
    erpc_assert(message_buffer_factory != NULL);

    BasicCodecFactory *codecFactory;
    SimpleServer *simpleServer;

    // Init factories.
    codecFactory = new BasicCodecFactory();

    // Init the client manager.
    simpleServer = new SimpleServer(host, port);

    if ((codecFactory == NULL) || (simpleServer == NULL))
    {
        if (codecFactory != NULL)
        {
            delete codecFactory;
        }
        if (simpleServer != NULL)
        {
            delete simpleServer;
        }
        simpleServer = NULL;
    }

    if (simpleServer != NULL)
    {
        // Init server with the provided transport.
        simpleServer->setCodecFactory(codecFactory);
        simpleServer->setMessageBufferFactory(reinterpret_cast<MessageBufferFactory *>(message_buffer_factory));
    }
    if (simpleServer->open() != kErpcStatus_Success)
    {
        delete simpleServer;
        simpleServer = NULL;
    }
    
    return reinterpret_cast<erpc_server_t>(simpleServer);
}

void erpc_server_deinit(erpc_server_t server)
{
    erpc_assert(server != NULL);
    SimpleServer *simpleServer = reinterpret_cast<SimpleServer *>(server);

    delete simpleServer->getCodecFactory();
    delete simpleServer;
}

void erpc_add_service_to_server(erpc_server_t server, void *service)
{
    erpc_assert(server != NULL);

    SimpleServer *simpleServer = reinterpret_cast<SimpleServer *>(server);

    simpleServer->addService(static_cast<erpc::Service *>(service));
}

void erpc_remove_service_from_server(erpc_server_t server, void *service)
{
    erpc_assert(server != NULL);
    erpc_assert(service != NULL);

    SimpleServer *simpleServer = reinterpret_cast<SimpleServer *>(server);

    simpleServer->removeService(static_cast<erpc::Service *>(service));
}

erpc_status_t erpc_server_run(erpc_server_t server)
{
    erpc_assert(server != NULL);

    SimpleServer *simpleServer = reinterpret_cast<SimpleServer *>(server);

    return simpleServer->run();
}

// erpc_status_t erpc_server_poll(erpc_server_t server)
// {
//     erpc_assert(server != NULL);

//     SimpleServer *simpleServer = reinterpret_cast<SimpleServer *>(server);

//     return simpleServer->poll();
// }

void erpc_server_stop(erpc_server_t server)
{
    erpc_assert(server != NULL);

    SimpleServer *simpleServer = reinterpret_cast<SimpleServer *>(server);

    simpleServer->stop();
}


