/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "server/server_base.hpp"

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
    this->methods.insert(this->methods.end(), service->methods.begin(), service->methods.end());
}

void Server::removeService(Service *service)
{
    this->methods.erase(std::remove_if(this->methods.begin(), this->methods.end(), [&](MethodBase *method) {
        return std::find(service->methods.begin(), service->methods.end(), method) != service->methods.end();
    }), this->methods.end());
}



