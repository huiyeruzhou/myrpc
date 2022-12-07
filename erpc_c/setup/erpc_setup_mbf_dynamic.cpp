/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_config_internal.h"
#include "erpc_mbf_setup.h"
#include "erpc_message_buffer.hpp"

#include <new>

using namespace std;
using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Dynamic message buffer factory.
 */
class DynamicMessageBufferFactory : public MessageBufferFactory
{
public:
    virtual MessageBuffer create(void)
    {
        uint8_t *buf = new (nothrow) uint8_t[ERPC_DEFAULT_BUFFER_SIZE];
        return MessageBuffer(buf, ERPC_DEFAULT_BUFFER_SIZE);
    }

    virtual void dispose(MessageBuffer *buf)
    {
        erpc_assert(buf != NULL);
        if (buf->get() != NULL)
        {
            delete[] buf->get();
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////


erpc_mbf_t erpc_mbf_dynamic_init(void)
{
    auto msgFactory = new DynamicMessageBufferFactory();
    return reinterpret_cast<erpc_mbf_t>(msgFactory);
}
