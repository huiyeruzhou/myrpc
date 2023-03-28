/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PORT_THREADING_H
#define PORT_THREADING_H
#include <cstdint>
#include <cstring>
#include "port/port.h"
// Threading model
#define ERPC_THREADS_NONE (0U)     //!< No threads.
#define ERPC_THREADS_PTHREADS (1U) //!< POSIX pthreads.
#define ERPC_THREADS_FREERTOS (2U) //!< FreeRTOS.

// Handy macro to test threading model. You can also ERPC_THREADS directly to test for threading
// support, i.e. "#if ERPC_THREADS", because ERPC_THREADS_NONE has a value of 0.
#define ERPC_THREADS_IS(_n_) (ERPC_THREADS == (ERPC_THREADS_##_n_))

// Detect threading model if not already set.
#ifndef ERPC_THREADS
#if CONFIG_HAS_POSIX
// Default to pthreads for POSIX systems.
        #define ERPC_THREADS (ERPC_THREADS_PTHREADS)
#elif CONFIG_HAS_FREERTOS
// Use FreeRTOS if we can auto detect it.
        #define ERPC_THREADS (ERPC_THREADS_FREERTOS)
#endif
#endif

// Exclude the rest of the file if threading is disabled.
#if ERPC_THREADS_IS(PTHREADS)
#include <pthread.h>
#elif ERPC_THREADS_IS(FREERTOS)
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#endif // ERPC_THREADS

/*!
 * @addtogroup port_threads
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

//! @brief Thread function type.
//!
//! @param arg User provided argument that was passed into the start() method.
typedef void (*thread_entry_t)(void *arg);

////////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)

namespace erpc {
/*!
 * @brief Simple thread class.
 *
 * @ingroup port_threads
 */
class   Thread
{
public:
    //! @brief Unique identifier for a thread.
    typedef void *thread_id_t;


    /*!
     * @brief Default constructor for use with the init() method.
     *
     * If this constructor is used, the init() method must be called before the thread can be
     * started.
     *
     * @param[in] name Optional name for the thread.
     */
    Thread(const char *name = 0);

    /*!
     * @brief Constructor.
     *
     * This constructor fully initializes the thread object.
     *
     * @param[in] entry
     * @param[in] priority
     * @param[in] stackSize
     * @param[in] name Optional name for the thread.
     * @param[in] stackPtr Mandatory task stack pointer for static api usage.
     */
    //TODO
    Thread(thread_entry_t entry, uint32_t priority = 0, uint32_t stackSize = 2048, const char *name = "");

    /*!
     * @brief Destructor.
     */
    virtual ~Thread(void);

    /*!
     * @brief This function sets name for thread.
     *
     * @param[in] name Name for thread.
     */
    void setName(const char *name) {
#if ERPC_THREADS_IS(PTHREADS)
        strncpy(m_name, name, CONFIG_MAX_PTHREAD_NAME_LEN);
#elif ERPC_THREADS_IS(FREERTOS)
        strncpy(m_name, name, configMAX_TASK_NAME_LEN);
#endif
    }

    /*!
     * @brief This function returns name of thread.
     *
     * @return Name of thread.
     */
    const char *getName(void) const { return m_name; }

    /*!
     * @brief This function initializes thread.
     *
     * @param[in] entry Entry function.
     * @param[in] priority Task priority.
     * @param[in] stackSize Stack size.
     */
    void init(thread_entry_t entry, uint32_t priority = 0, uint32_t stackSize = 0);

    /*!
     * @brief This function starts thread execution.
     *
     * @param[in] arg Function argument.
     */
    void start(void *arg = 0);

    /*!
     * @brief This function puts thread to sleep.
     *
     * @param[in] usecs Time for sleeping in [us].
     */
    static void sleep(uint32_t usecs);

    /*!
     * @brief This function returns current thread id.
     *
     * @return Thread id.
     */
    thread_id_t getThreadId(void) const
    {
#if ERPC_THREADS_IS(PTHREADS)
        return reinterpret_cast<thread_id_t>(m_thread);
#elif ERPC_THREADS_IS(FREERTOS)
        return reinterpret_cast<thread_id_t>(m_task);
#endif
    }

    /*!
     * @brief This function returns thread id where function is called.
     *
     * @return Thread id.
     */
    static thread_id_t getCurrentThreadId(void)
    {
#if ERPC_THREADS_IS(PTHREADS)
        return reinterpret_cast<thread_id_t>(pthread_self());
#elif ERPC_THREADS_IS(FREERTOS)
        return reinterpret_cast<thread_id_t>(xTaskGetCurrentTaskHandle());
#endif
    }


    /*!
     * @brief Compare operator compares two threads.
     *
     * @param[in] o Thread instance.
     *
     * @retval true When threads are same.
     * @retval false When threads aren't same.
     */
    bool operator==(Thread &o);

protected:
    /*!
     * @brief This function execute entry function.
     */
    virtual void threadEntryPoint(void);

private:
#if ERPC_THREADS_IS(FREERTOS)
    char m_name[configMAX_TASK_NAME_LEN];//任务名字
#elif ERPC_THREADS_IS(PTHREADS)
    char m_name[CONFIG_MAX_PTHREAD_NAME_LEN];//任务名字
#endif
    thread_entry_t m_entry;          /*!< Thread entry function. */
    void *m_arg;                     /*!< Entry parameter. */
    uint32_t m_stackSize;            /*!< Stack size. */
    uint32_t m_priority;             /*!< Task priority. */
#if ERPC_THREADS_IS(PTHREADS)
    static pthread_key_t s_threadObjectKey; /*!< Thread key. */
    pthread_t m_thread;                     /*!< Current thread. */
#elif ERPC_THREADS_IS(FREERTOS)
    TaskHandle_t m_task;       /*!< Current task. */
    Thread *m_next;            /*!< Pointer to next Thread. */
#endif

#if ERPC_THREADS_IS(PTHREADS)

    /*!
     * @brief This function execute threadEntryPoint function.
     *
     * @param[in] arg Thread to execute.
     */
    static void *threadEntryPointStub(void *arg);
#elif ERPC_THREADS_IS(FREERTOS)

    /*!
     * @brief This function execute threadEntryPoint function.
     *
     * @param[in] arg Thread to execute.
     */
    static void threadEntryPointStub(void *arg);
#endif

private:
    /*!
     * @brief Copy constructor.
     *
     * @param[in] o Thread to copy.
     */
    Thread(const Thread &o);

    /*!
     * @brief Assign operator.
     *
     * @param[in] o Thread to assign.
     */
    Thread &operator=(const Thread &o);
};

/*!
 * @brief Mutex.
 *
 * If the OS supports it, the mutex will be recursive.
 *
 * @ingroup port_threads
 */
class Mutex
{
public:
    /*!
     * @brief
     */
    class Guard
    {
    public:
        /*!
         * @brief Constructor.
         *
         * @param[in] mutex to lock.
         */
        Guard(Mutex &mutex)
        : m_mutex(mutex)
        {
            (void)m_mutex.lock();
        }
        /*!
         * @brief Destructor.
         */
        ~Guard(void) { (void)m_mutex.unlock(); }

    private:
        Mutex &m_mutex; /*!< Mutex to lock. */
    };

    /*!
     * @brief Constructor.
     */
    Mutex(void);

    /*!
     * @brief Destructor.
     */
    ~Mutex(void);

    /*!
     * @brief This function try lock mutex.
     *
     * @retval true When mutex locked successfully.
     * @retval false When mutex didn't locked.
     */
    bool tryLock(void);

    /*!
     * @brief This function lock mutex.
     *
     * @retval true When mutex locked successfully.
     * @retval false When mutex didn't locked.
     */
    bool lock(void);

    /*!
     * @brief This function unlock mutex.
     *
     * @retval true When mutex unlocked successfully.
     * @retval false When mutex didn't unlocked.
     */
    bool unlock(void);

#if ERPC_THREADS_IS(PTHREADS)
    /*!
     * @brief This function returns pointer to mutex.
     *
     * @return Pointer to mutex.
     */
    pthread_mutex_t *getPtr(void) { return &m_mutex; }
#endif

private:
#if ERPC_THREADS_IS(PTHREADS)
    pthread_mutex_t m_mutex; /*!< Mutex.*/
#elif ERPC_THREADS_IS(FREERTOS)
    SemaphoreHandle_t m_mutex;       /*!< Mutex.*/
    StaticSemaphore_t m_staticQueue; /*!< Static queue. */
#endif

private:
    /*!
     * @brief Copy constructor.
     *
     * @param[in] o Mutex to copy.
     */
    Mutex(const Mutex &o);
    /*!
     * @brief Copy constructor.
     *
     * @param[in] o Mutex to copy.
     */
    Mutex &operator=(const Mutex &o);
};

/*!
 * @brief Simple semaphore class.
 *
 * @ingroup port_threads
 */
class Semaphore
{
public:
    /*!
     * @brief Variable for semaphore to wait forever.
     */
    static const uint32_t kWaitForever = 0xffffffffu;

    /*!
     * @brief Constructor.
     *
     * @param[in] count Semaphore number.
     */
    Semaphore(int count = 0);

    /*!
     * @brief Destructor.
     */
    ~Semaphore(void);

    /*!
     * @brief This function puts semaphore.
     */
    void put(void);

#if ERPC_THREADS_IS(FREERTOS)
    /*!
     * @brief This function puts semaphore from interrupt.
     */
    void putFromISR(void);
#endif // ERPC_HAS_FREERTOS

    /*!
     * @brief This function get semaphore.
     *
     * @param[in] timeoutUsecs Time how long can wait for getting semaphore in [us].
     *
     * @retval true When semaphore got successfully.
     * @retval false When mutex didn't get.
     */
    bool get(uint32_t timeoutUsecs = kWaitForever);

    /*!
     * @brief This function returns semaphore count number.
     *
     * @return Semaphore count number.
     */
    int getCount(void) const;

private:
#if ERPC_THREADS_IS(PTHREADS)
    int m_count;           /*!< Semaphore count number. */
    pthread_cond_t m_cond; /*!< Condition variable. Allows threads to suspend execution and relinquish the processors
                              until some predicate on shared data is satisfied. */
    Mutex m_mutex;         /*!< Mutext. */
#elif ERPC_THREADS_IS(FREERTOS)
    SemaphoreHandle_t m_sem;         /*!< Semaphore. */
    StaticSemaphore_t m_staticQueue; /*!< Static queue. */
#endif

private:
    /*!
     * @brief Copy constructor.
     *
     * @param[in] o Semaphore to copy.
     */
    Semaphore(const Semaphore &o);
    /*!
     * @brief Copy constructor.
     *
     * @param[in] o Semaphore to copy.
     */
    Semaphore &operator=(const Semaphore &o);
};

} // namespace erpc

#endif // defined(__cplusplus)

/*! @} */

#endif // ERPC_THREADS

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
