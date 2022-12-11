/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "server_base.hpp"

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

#if ERPC_NESTED_CALLS_DETECTION
extern bool nestingDetection;
bool nestingDetection = false;
#endif

void Server::addService(Service *service)
{
    if (m_firstService == NULL)
    {
        m_firstService = service;
    }
    else
    {

        Service *link = m_firstService;
        while (link->getNext() != NULL)
        {
            link = link->getNext();
        }

        link->setNext(service);
    }
}

void Server::removeService(Service *service)
{
    Service *link = m_firstService;

    if (link == service)
    {
        m_firstService = link->getNext();
    }
    else
    {
        while (link != NULL)
        {
            if (link->getNext() == service)
            {
                link->setNext(link->getNext()->getNext());
                break;
            }
            link = link->getNext();
        }
    }
}



