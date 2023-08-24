/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: server_worker.hpp
 * Description: This file defines the ServerWorker class.Which is the worker for each RPC connect.
 *             It will receive the request from client and call the method to process the request.
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#ifndef SERVER_SERVER_WORKER_HPP_
#define SERVER_SERVER_WORKER_HPP_

extern "C" {
#include <unistd.h>
}

#include <atomic>
#include <cstdio>
#include <memory>
#include <vector>

#include "codec/meta.pb.h"
#include "port/port.h"
#include "server/service.hpp"
#include "transport/tcp_transport.hpp"

#ifdef CHECK_STATUS
#undef CHECK_STATUS
#endif
#define CHECK_STATUS(status, err)                               \
    if (((err) = (status)) != rpc_status::Success) {            \
        LOGE(TAG, "Error occurred: %s", StatusToString((err))); \
        goto done;                                              \
    }
namespace erpc {
class ServerWorker {
   public:
    ServerWorker(std::shared_ptr<MethodVector> methods, TCPTransport *worker, std::shared_ptr<std::atomic_bool> isServerOn);
    ~ServerWorker();
    rpc_status runInternal(void);
    rpc_status resetBuffers(void);
    rpc_status findServiceByMetadata(myrpc_Meta *req);
    rpc_status callMethodByMetadata(myrpc_Meta *req, myrpc_Meta *esp, void *input, void *output);
    static void workerStub(void *arg);
    void start(void);
    bool isServerOn(void) { return *p_isServerOn; }

   private:
    Thread m_worker_thread;
#if ERPC_THREADS_IS(PTHREADS)
    char TAG[CONFIG_MAX_PTHREAD_NAME_LEN];
#elif ERPC_THREADS_IS(FREERTOS)
    char TAG[configMAX_TASK_NAME_LEN];
#endif
    std::shared_ptr<MethodBase> m_method;
    std::shared_ptr<MethodVector> methods;
    std::unique_ptr<TCPTransport> m_worker;  //!< Worker to do transport
    std::shared_ptr<const std::atomic_bool> p_isServerOn;
    int error_count = 0;
    int timeout_count = 0;
};

}  // namespace erpc

#endif  // SERVER_SERVER_WORKER_HPP_
