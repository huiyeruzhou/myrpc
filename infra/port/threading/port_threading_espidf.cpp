/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 *
 * File: port_threading_freertos.cpp
 * Description: portable threading implementation for FreeRTOS
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: eRPC, Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#include "port/threading/port_threading.h"

#if ERPC_THREADS_IS(FREERTOS)
__attribute__((unused)) static const char *TAG = "Thread";

erpc::Thread::Thread(thread_entry_t entry, uint32_t priority, uint32_t stackSize, const char *name)
    : m_entry(entry), m_arg(0), m_stackSize(stackSize), m_priority(priority), m_task(0), m_next(0)
{
    strncpy(m_name, name, configMAX_TASK_NAME_LEN);
}

erpc::Thread::~Thread(void) {}

void erpc::Thread::init(thread_entry_t entry, uint32_t priority, uint32_t stackSize)
{
    LOGW(TAG, "init");
    m_entry = entry;
    m_stackSize = stackSize;
    m_priority = priority;
}

void erpc::Thread::start(void *arg)
{
    m_arg = arg;

    // Enter a critical section to disable preemptive scheduling until we add the
    // newly created thread to the linked list. This prevents a race condition if
    // the new thread is higher priority than the current thread, and the new
    // thread calls getCurrenThread(), which will scan the linked list.

    LOGI(TAG, "stacksize = %" PRIu32, (configSTACK_DEPTH_TYPE)(m_stackSize));

    if (pdPASS == xTaskCreate(threadEntryPointStub, (m_name[0] ? m_name : "task"), (configSTACK_DEPTH_TYPE)(m_stackSize),
                              // 0,
                              this, m_priority, &m_task)) {
        LOGI(TAG, "task `%s` created\n", m_name);
    }
}
// cpplint: we are defining an operator, while cpplint think it is a expression
bool erpc::Thread::operator==(Thread &o) { return m_task == o.m_task; }  // NOLINT

void erpc::Thread::sleep(uint32_t usecs)
{
#if INCLUDE_vTaskDelay
    vTaskDelay(usecs / 1000U / portTICK_PERIOD_MS);
#endif
}

void erpc::Thread::threadEntryPoint(void)
{
    if (m_entry != NULL) {
        LOGI(TAG, "start running");
        m_entry(m_arg);
    }
}

void erpc::Thread::threadEntryPointStub(void *arg)
{
    Thread *_this = reinterpret_cast<Thread *>(arg);
    assert((_this != NULL) && ("Reinterpreting 'void *arg' to 'Thread *' failed." != NULL));
    _this->threadEntryPoint();

    // Handle a task returning from its function. Delete or suspend the task, if
    // the API is available. If neither API is included, then just enter an
    // infinite loop. If vTaskDelay() is available, the loop sleeps this task the
    // maximum time each cycle. If not, it just yields.
#if INCLUDE_vTaskDelete
    _this->m_task = 0;
    vTaskDelete(NULL);
#elif INCLUDE_vTaskSuspend
    vTaskSuspend(NULL);
#else  // INCLUDE_vTaskSuspend
    while (true) {
#if INCLUDE_vTaskDelay
        vTaskDelay(portMAX_DELAY);
#else   // INCLUDE_vTaskDelay
        taskYIELD();
#endif  // INCLUDE_vTaskDelay
    }
#endif  // INCLUDE_vTaskDelete
}

erpc::Mutex::Mutex(void) : m_mutex(0)
{
#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_STATIC
    m_mutex = xSemaphoreCreateRecursiveMutexStatic(&m_staticQueue);
#elif configSUPPORT_DYNAMIC_ALLOCATION
    m_mutex = xSemaphoreCreateRecursiveMutex();
#else
#error "Allocation method didn't match"
#endif
}

erpc::Mutex::~Mutex(void) { vSemaphoreDelete(m_mutex); }

bool erpc::Mutex::tryLock(void)
{
    // Pass a zero timeout to poll the mutex.
    return (pdTRUE == xSemaphoreTakeRecursive(m_mutex, 0) ? true : false);
}

bool erpc::Mutex::lock(void) { return (pdTRUE == xSemaphoreTakeRecursive(m_mutex, portMAX_DELAY) ? true : false); }

bool erpc::Mutex::unlock(void) { return (pdTRUE == xSemaphoreGiveRecursive(m_mutex) ? true : false); }

erpc::Semaphore::Semaphore(int count) : m_sem(0) { m_sem = xSemaphoreCreateCounting(0x7fffffffu, (UBaseType_t)count); }

erpc::Semaphore::~Semaphore(void) { vSemaphoreDelete(m_sem); }

void erpc::Semaphore::put(void) { (void)xSemaphoreGive(m_sem); }

void erpc::Semaphore::putFromISR(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    (void)xSemaphoreGiveFromISR(m_sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

bool erpc::Semaphore::get(uint32_t timeoutUsecs)
{
    if (timeoutUsecs != kWaitForever) {
        if (timeoutUsecs > 0U) {
            timeoutUsecs /= 1000U * portTICK_PERIOD_MS;
            if (timeoutUsecs == 0U) {
                timeoutUsecs = 1U;
            }
/* cpplint: since the "else" block is inside the "#if",
it can not be put in the same line with the previous "}"*/
#if configUSE_16_BIT_TICKS
            else if (timeoutUsecs > (portMAX_DELAY - 1)) {  // NOLINT
                timeoutUsecs = portMAX_DELAY - 1;
            }
#endif
        }
    }
/* cpplint: since the "else" block is inside the "#if",
it can not be put in the same line with the previous "}"*/
#if configUSE_16_BIT_TICKS
    else {  // NOLINT
        timeoutUsecs = portMAX_DELAY;
    }
#endif

    return (pdTRUE == xSemaphoreTake(m_sem, (TickType_t)timeoutUsecs));
}

int erpc::Semaphore::getCount(void) const { return static_cast<int>(uxQueueMessagesWaiting(m_sem)); }
#endif /* ERPC_THREADS_IS(FREERTOS) */
