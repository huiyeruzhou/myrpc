/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2021 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * File: port_threading_freertos.cpp
 * Description: portable threading implementation for Linux/Android
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: YuHongli
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    YuHongli       Create and initialize
 */
#include "port/threading/port_threading.h"

#if ERPC_THREADS_IS(PTHREADS)
#include <errno.h>
#include <sys/time.h>
#include <time.h>

/*!
 * @brief Thread object key.
 */
pthread_key_t erpc::Thread::s_threadObjectKey = 0;

/*!
 * @brief Second to microseconds.
 */
const uint32_t sToUs = 1000000;

erpc::Thread::Thread(thread_entry_t entry, uint32_t priority, uint32_t stackSize, const char *name)
    : m_entry(entry), m_arg(0), m_stackSize(stackSize), m_priority(priority), m_thread(0)
{
    strncpy(m_name, name, CONFIG_MAX_PTHREAD_NAME_LEN);
}

erpc::Thread::~Thread(void) {}

void erpc::Thread::init(thread_entry_t entry, uint32_t priority, uint32_t stackSize)
{
    m_entry = entry;
    m_stackSize = stackSize;
    m_priority = priority;
}

void erpc::Thread::start(void *arg)
{
    if (s_threadObjectKey == 0) {
        pthread_key_create(&s_threadObjectKey, NULL);
    }

    m_arg = arg;
    pthread_create(&m_thread, NULL, threadEntryPointStub, this);
    pthread_setspecific(s_threadObjectKey, reinterpret_cast<void *>(this));
    pthread_detach(m_thread);
}
// cpplint: we are defining an operator, while cpplint think it is a expression
bool erpc::Thread::operator==(Thread &o)
{  // NOLINT
    return pthread_equal(m_thread, o.m_thread);
}

void erpc::Thread::sleep(uint32_t usecs)
{
    struct timespec rq;
    struct timespec actual = {0, 0};

    // Sleep for the requested number of microseconds.
    rq.tv_sec = usecs / sToUs;
    rq.tv_nsec = (usecs % sToUs) * 1000U;

    // Keep sleeping until the requested time elapses even if we get interrupted
    // by a signal.
    while (nanosleep(&rq, &actual) == EINTR) {
        rq.tv_sec -= actual.tv_sec;
        rq.tv_nsec -= actual.tv_nsec;
        if (rq.tv_nsec < 0) {
            --rq.tv_sec;
            rq.tv_nsec += 1000000000;
        }
    }
}

void erpc::Thread::threadEntryPoint(void)
{
    if (m_entry != NULL) {
        m_entry(m_arg);
    }
}

void *erpc::Thread::threadEntryPointStub(void *arg)
{
    Thread *_this = reinterpret_cast<Thread *>(arg);

    if (_this != NULL) {
        _this->threadEntryPoint();
    }

    return 0;
}

erpc::Mutex::Mutex(void)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&m_mutex, &attr);

    pthread_mutexattr_destroy(&attr);
}

erpc::Mutex::~Mutex(void) { pthread_mutex_destroy(&m_mutex); }

bool erpc::Mutex::tryLock(void) { return pthread_mutex_trylock(&m_mutex) == 0; }

bool erpc::Mutex::lock(void) { return pthread_mutex_lock(&m_mutex) == 0; }

bool erpc::Mutex::unlock(void) { return pthread_mutex_unlock(&m_mutex) == 0; }

erpc::Semaphore::Semaphore(int count) : m_count(count), m_mutex() { pthread_cond_init(&m_cond, NULL); }

erpc::Semaphore::~Semaphore(void) { pthread_cond_destroy(&m_cond); }

void erpc::Semaphore::put(void)
{
    erpc::Mutex::Guard guard(m_mutex);
    if (m_count == 0) {
        pthread_cond_signal(&m_cond);
    }
    ++m_count;
}

bool erpc::Semaphore::get(uint32_t timeoutUsecs)
{
    erpc::Mutex::Guard guard(m_mutex);
    bool retVal = true;
    int err;

    while (m_count == 0) {
        if (timeoutUsecs == kWaitForever) {
            err = pthread_cond_wait(&m_cond, m_mutex.getPtr());
            if (err != 0) {
                retVal = false;
                break;
            }
        } else {
            if (timeoutUsecs > 0U) {
                // Create an absolute timeout time.
                struct timeval tv;
                gettimeofday(&tv, NULL);
                struct timespec wait;
                wait.tv_sec = tv.tv_sec + (timeoutUsecs / sToUs);
                wait.tv_nsec = (timeoutUsecs % sToUs) * 1000U;
                err = pthread_cond_timedwait(&m_cond, m_mutex.getPtr(), &wait);
                if (err != 0) {
                    retVal = false;
                    break;
                }
            }
        }
    }

    if (retVal) {
        --m_count;
    }

    return retVal;
}

int erpc::Semaphore::getCount(void) const { return m_count; }

#endif
