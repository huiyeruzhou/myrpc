#ifndef PORT_ASSERT_H
#define PORT_ASSERT_H

#ifndef erpc_assert
#if CONFIG_HAS_FREERTOS
#ifdef __cplusplus
extern "C" {
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#ifdef __cplusplus
}
#endif
#define erpc_assert(condition) configASSERT(condition)
#else
#ifdef __cplusplus
#include <cassert>
#else
#include "assert.h"
#endif
#define erpc_assert(condition) assert(condition)
#endif
#endif

#endif