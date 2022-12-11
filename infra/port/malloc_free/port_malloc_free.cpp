/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "port_malloc_free.h"
#include "../config.h"

#if CONFIG_HAS_POSIX
#include <cstdlib>

void *erpc_malloc(size_t size)
{
    void *p = malloc(size);
    return p;
}

void erpc_free(void *ptr)
{
    free(ptr);
}
#elif CONFIG_HAS_FREERTOS
#include "freertos/FreeRTOS.h"
void *erpc_malloc(size_t size)
{
    void *p = pvPortMalloc(size);
    return p;
}

void erpc_free(void *ptr)
{
    vPortFree(ptr);
}
#endif//CONFIG_HAS_POSIX

/* Provide function for pure virtual call to avoid huge demangling code being linked in ARM GCC */
#if ((defined(__GNUC__)) && (defined(__arm__)))
extern "C" void __cxa_pure_virtual(void)
{
    for (;;)
    {
    };
}
#endif

