/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_threading.h"
#include <cerrno>
static const char *TAG = "Thread";
#if ERPC_THREADS_IS(FREERTOS)

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

Thread::Thread(const char *name)
    : m_entry(nullptr)
    , m_arg(nullptr)
    , m_stackSize(0)
    , m_priority(0)
    , m_task(nullptr)
{
    strncpy(m_name, name, CONFIG_MAX_TASK_NAME_LEN);
}

Thread::Thread(thread_entry_t entry, uint32_t priority, uint32_t stackSize, const char *name)
    : m_entry(entry)
    , m_arg(nullptr)
    , m_stackSize(stackSize)
    , m_priority(priority)
    , m_task(nullptr)
{
    strncpy(m_name, name, CONFIG_MAX_TASK_NAME_LEN);
}

Thread::~Thread(void) = default;

void Thread::init(thread_entry_t entry, uint32_t priority, uint32_t stackSize)
{
    LOGW(TAG, "init");
    m_entry = entry;
    m_stackSize = stackSize;
    m_priority = priority;
}

void Thread::start(void *arg)
{

    m_arg = arg;

    // Enter a critical section to disable preemptive scheduling until we add the newly
    // created thread to the linked list. This prevents a race condition if the new thread is
    // higher priority than the current thread, and the new thread calls getCurrenThread(),
    // which will scan the linked list.

    LOGI(TAG, "stacksize = %luB", (configSTACK_DEPTH_TYPE) (m_stackSize));

    if (pdPASS ==
        xTaskCreate(threadEntryPointStub, (m_name[0] ? m_name : "task"),
            (configSTACK_DEPTH_TYPE) (m_stackSize),
            // 0,
            this, m_priority, &m_task))
    {
        LOGI(TAG, "task `%s` created\n", m_name);
    }
}

bool Thread::operator==(Thread &o)
{
    return m_task == o.m_task;
}

void Thread::sleep(uint32_t usecs)
{
#if INCLUDE_vTaskDelay
        vTaskDelay(usecs / 1000U / portTICK_PERIOD_MS);
#endif
}

void Thread::threadEntryPoint()
{
    if (m_entry != nullptr)
    {
        LOGI(TAG, "start running");
        m_entry(m_arg);
    }
}

void Thread::threadEntryPointStub(void *arg)
{
    auto *_this = reinterpret_cast<Thread *>(arg);
    erpc_assert((_this != nullptr) && ("Reinterpreting 'void *arg' to 'Thread *' failed." != nullptr));
    _this->threadEntryPoint();

    // Handle a task returning from its function. Delete or suspend the task, if the API is
    // available. If neither API is included, then just enter an infinite loop. If vTaskDelay()
    // is available, the loop sleeps this task the maximum time each cycle. If not, it just
    // yields.
#if INCLUDE_vTaskDelete
    _this->m_task = nullptr;
    vTaskDelete(nullptr);
#elif INCLUDE_vTaskSuspend
    vTaskSuspend(NULL);
#else // INCLUDE_vTaskSuspend
    while (true)
    {
#if INCLUDE_vTaskDelay
        vTaskDelay(portMAX_DELAY);
#else  // INCLUDE_vTaskDelay
        taskYIELD();
#endif // INCLUDE_vTaskDelay
    }
#endif // INCLUDE_vTaskDelete
}

Mutex::Mutex(void)
    : m_mutex(nullptr)
{
    m_mutex = xSemaphoreCreateRecursiveMutex();
}

Mutex::~Mutex(void)
{
    vSemaphoreDelete(m_mutex);
}

bool Mutex::tryLock(void)
{
    // Pass a zero timeout to poll the mutex.
    return (pdTRUE == xSemaphoreTakeRecursive(m_mutex, 0) ? true : false);
}

bool Mutex::lock(void)
{
    return (pdTRUE == xSemaphoreTakeRecursive(m_mutex, portMAX_DELAY) ? true : false);
}

bool Mutex::unlock(void)
{
    return (pdTRUE == xSemaphoreGiveRecursive(m_mutex) ? true : false);
}

Semaphore::Semaphore(int count)
    : m_sem(nullptr)
{
    m_sem = xSemaphoreCreateCounting(0x7fffffffu, (UBaseType_t) count);
}

Semaphore::~Semaphore(void)
{
    vSemaphoreDelete(m_sem);
}

void Semaphore::put(void)
{
    (void) xSemaphoreGive(m_sem);
}

void Semaphore::putFromISR(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    (void) xSemaphoreGiveFromISR(m_sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

bool Semaphore::get(uint32_t timeoutUsecs)
{
    if (timeoutUsecs != kWaitForever)
    {
        if (timeoutUsecs > 0U)
        {
            timeoutUsecs /= 1000U * portTICK_PERIOD_MS;
            if (timeoutUsecs == 0U)
            {
                timeoutUsecs = 1U;
            }
#if configUSE_16_BIT_TICKS
            else if (timeoutUsecs > (portMAX_DELAY - 1))
            {
                timeoutUsecs = portMAX_DELAY - 1;
            }
#endif
        }
    }
#if configUSE_16_BIT_TICKS
    else
    {
        timeoutUsecs = portMAX_DELAY;
    }
#endif

    return (pdTRUE == xSemaphoreTake(m_sem, (TickType_t) timeoutUsecs));
}

int Semaphore::getCount(void) const
{
    return static_cast<int>(uxQueueMessagesWaiting(m_sem));
}
#endif /* ERPC_THREADS_IS(FREERTOS) */

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
