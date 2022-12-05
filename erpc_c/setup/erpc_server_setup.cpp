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
#include "erpc_manually_constructed.hpp"
#include "erpc_message_buffer.hpp"
#include "erpc_simple_server.hpp"
#include "erpc_tcp_transport.hpp"

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

// global server variables
ERPC_MANUALLY_CONSTRUCTED_STATIC(SimpleServer, s_server);
ERPC_MANUALLY_CONSTRUCTED_STATIC(BasicCodecFactory, s_codecFactory);
ERPC_MANUALLY_CONSTRUCTED_STATIC(Crc16, s_crc16);

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

erpc_server_t erpc_server_init(erpc_transport_t transport, erpc_mbf_t message_buffer_factory)
{
    erpc_assert(transport != NULL);
    erpc_assert(message_buffer_factory != NULL);

    TCPTransport *castedTransport;
    BasicCodecFactory *codecFactory;
    SimpleServer *simpleServer;

#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_STATIC
    if (s_codecFactory.isUsed() || s_crc16.isUsed() || s_server.isUsed())
    {
        simpleServer = NULL;
    }
    else
    {
        // Init factories.
        s_codecFactory.construct();
        codecFactory = s_codecFactory.get();

        s_crc16.construct();
        crc16 = s_crc16.get();

        // Init the client manager.
        s_server.construct();
        simpleServer = s_server.get();
    }
#elif ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
    // Init factories.
    codecFactory = new BasicCodecFactory();


    // Init the client manager.
    simpleServer = new SimpleServer();

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
#else
#error "Unknown eRPC allocation policy!"
#endif

    if (simpleServer != NULL)
    {
        // Init server with the provided transport.
        castedTransport = reinterpret_cast<TCPTransport *>(transport);
        simpleServer->setTransport(castedTransport);
        castedTransport->m_server = simpleServer;
        simpleServer->setCodecFactory(codecFactory);
        simpleServer->setMessageBufferFactory(reinterpret_cast<MessageBufferFactory *>(message_buffer_factory));
    }

    return reinterpret_cast<erpc_server_t>(simpleServer);
}

void erpc_server_deinit(erpc_server_t server)
{
#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_STATIC
    (void)server;
    erpc_assert(server == s_server.get());
    s_crc16.destroy();
    s_codecFactory.destroy();
    s_server.destroy();
#elif ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
    erpc_assert(server != NULL);
    SimpleServer *simpleServer = reinterpret_cast<SimpleServer *>(server);

    delete simpleServer->getCodecFactory();
    delete simpleServer;
#else
#error "Unknown eRPC allocation policy!"
#endif
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


