/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * Copyright 2020-2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_client_setup.h"
#include "erpc_basic_codec.hpp"
#include "erpc_client_manager.h"
#include "erpc_message_buffer.hpp"

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

// global client variables
ClientManager *g_client;
#pragma weak g_client

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

erpc_client_t erpc_client_init(const char* host , uint16_t port, erpc_mbf_t message_buffer_factory)
{
    erpc_assert(message_buffer_factory != NULL);

    BasicCodecFactory *codecFactory;
    ClientManager *client;

    // Init factories.
    codecFactory = new BasicCodecFactory();


    // Init the client manager.
    client = new ClientManager(host, port);

    if ((codecFactory == NULL) || (client == NULL))
    {
        if (codecFactory != NULL)
        {
            delete codecFactory;
        }
        if (client != NULL)
        {
            delete client;
        }
        client = NULL;
    }

    if (client != NULL)
    {
        if (kErpcStatus_Success != client->open())
        {
            delete client;
            return NULL;
        }
        client->setCodecFactory(codecFactory);
        client->setMessageBufferFactory(reinterpret_cast<MessageBufferFactory *>(message_buffer_factory));
    }

    g_client = client;

    return reinterpret_cast<erpc_client_t>(client);
}

void erpc_client_set_error_handler(erpc_client_t client, client_error_handler_t error_handler)
{
    erpc_assert(client != NULL);

    ClientManager *clientManager = reinterpret_cast<ClientManager *>(client);

    clientManager->setErrorHandler(error_handler);
}


void erpc_client_deinit(erpc_client_t client)
{
    erpc_assert(client != NULL);
    ClientManager *clientManager = reinterpret_cast<ClientManager *>(client);

    delete clientManager->getCodecFactory();
    delete clientManager;
}
