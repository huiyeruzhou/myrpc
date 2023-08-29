/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: server_worker.cpp
 * Description: the server worker class implementation
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#include "server/server_worker.hpp"

erpc::ServerWorker::ServerWorker(std::shared_ptr<MethodVector> methods, TCPTransport *worker, std::shared_ptr<std::atomic_bool> isServerOn)
    : m_worker_thread(workerStub, 5), methods(methods), m_worker(worker), p_isServerOn(isServerOn)
{
#ifdef TRACE_MEMORY
    LOGE("memory", "worker construct methods=%ld", this->methods.use_count());
#endif  // TRACE_MEMORY
    snprintf(TAG, sizeof(TAG) - 1, "worker %" PRIin_port_t, m_worker->m_port);
    m_worker_thread.setName(TAG);
}
erpc::ServerWorker::~ServerWorker()
{
#ifdef TRACE_MEMORY
    LOGE("memory", "worker deconstruct methods=%ld", this->methods.use_count());
#endif  // TRACE_MEMORY
    if (m_method) {
        m_method->destroyMsg(m_worker->to_recv_msg, m_worker->to_send_msg);
    }
}

rpc_status erpc::ServerWorker::runInternal(void)
{
    rpc_status err;
    myrpc_Meta *req_md = m_worker->to_recv_md;
    myrpc_Meta *rsp_md = m_worker->to_send_md;
    const pb_msgdesc_t **input_desc = &m_worker->recv_desc;
    const pb_msgdesc_t **output_desc = &m_worker->send_desc;
    void **input_msg = &m_worker->to_recv_msg;
    void **output_msg = &m_worker->to_send_msg;
    // get data frame
    CHECK_STATUS(m_worker->receiveFrame(), err);
#ifdef TRACE_SERVER_WORKER
    LOGI(TAG, "receiveFrame()");
#endif  // TRACE_SERVER_WORKER

    // recv inital metadata
    CHECK_STATUS(m_worker->recv_inital_md(), err);
#ifdef TRACE_SERVER_WORKER
    LOGI(TAG, "recv_inital_md()");
#endif  // TRACE_SERVER_WORKER

    // find the method
    CHECK_STATUS(findServiceByMetadata(req_md), err);
#ifdef TRACE_SERVER_WORKER
    LOGI(TAG, "findServiceByMetadata()");
#endif  // TRACE_SERVER_WORKER

    // filled message desc, so that we can receive message
    m_method->filledMsgDesc(input_desc, input_msg, output_desc, output_msg);
#ifdef TRACE_SERVER_WORKER
    LOGI(TAG, "filledMsgDesc()");
#endif  // TRACE_SERVER_WORKER

    // recv messgae
    CHECK_STATUS(m_worker->recv_msg(), err);
#ifdef TRACE_SERVER_WORKER
    LOGI(TAG, "recv_msg()");
#endif  // TRACE_SERVER_WORKER

    // recv trailing metadata
    CHECK_STATUS(m_worker->recv_trailing_md(), err);
#ifdef TRACE_SERVER_WORKER
    LOGI(TAG, "recv_trailing_md()");
#endif  // TRACE_SERVER_WORKER

    // call method and filled the meta.
    err = callMethodByMetadata(req_md, rsp_md, m_worker->to_recv_msg, m_worker->to_send_msg);

    // send initial metadata
    CHECK_STATUS(m_worker->send_inital_md(), err);
#ifdef TRACE_SERVER_WORKER
    LOGI(TAG, "send_inital_md()");
#endif  // TRACE_SERVER_WORKER

    // send msg
    CHECK_STATUS(m_worker->send_msg(), err);
#ifdef TRACE_SERVER_WORKER
    LOGI(TAG, "send_msg()");
#endif  // TRACE_SERVER_WORKER

    // send trailing metadata and the whole frame
    rsp_md->has_status = true;
    rsp_md->status = err;
    CHECK_STATUS(m_worker->send_trailing_md(), err);
#ifdef TRACE_SERVER_WORKER
    LOGI(TAG, "send_trailing_md()");
#endif  // TRACE_SERVER_WORKER

done:

    if (err != rpc_status::Success) {
        LOGW(TAG, "RPC Call Failed Because: %s", StatusToString(err));
        error_count++;
        if (err == rpc_status::UnknownService) {
            LOGW(TAG, "Request Service: %s", req_md->path);
            LOGE(TAG, "Available Services:");
            std::for_each(methods->begin(), methods->end(), [&](std::shared_ptr<MethodBase> &method) { LOGE(TAG, "  %s", method->getPath()); });
        } else if (err == rpc_status::ConnectionClosed) {
            LOGW(TAG, "Connection Closed");
            m_worker->close();
        } else if (err == rpc_status::Timeout) {
            /*For server, we don't want closed beacuse
             * timeout. Instead, we want to keep the connection
             * and wait for the next request, untill the server
             * is closed.
             */
            timeout_count++;
            error_count--;
            LOGE(TAG, "Timeout. %d seconds no incoming data.\n", 5 * timeout_count);
        }
        /*if there are more than three consecutive errors, close.
         * if there are more than 15 consecutive timeouts(75seconds), close.
         */
        if (error_count > 3 || timeout_count > 15) {
            LOGE(TAG, "Too many errors, close the connection\n");
            m_worker->close();
        }
    } else {
        error_count = 0;
        timeout_count = 0;
        LOGI(TAG, "RPC Call Success\n");
    }
    resetBuffers();
    return err;
}
rpc_status erpc::ServerWorker::resetBuffers(void)
{
    if (m_method) {
        m_method->destroyMsg(m_worker->to_recv_msg, m_worker->to_send_msg);
    }
    m_method = NULL;
    return m_worker->resetBuffers();
}
rpc_status erpc::ServerWorker::findServiceByMetadata(myrpc_Meta *req)
{
    auto iter_method = std::find_if(methods->begin(), methods->end(),
                                    [&](std::shared_ptr<MethodBase> &method) -> bool { return !strcmp(method->getPath(), req->path); });
    if (iter_method == methods->end()) {
        m_method = NULL;
        return rpc_status::UnknownService;
    } else {
#ifdef TRACE_MEMORY
        LOGE("memory", "worker findServiceByMetadata iter_method=%ld", iter_method->use_count());
#endif  // TRACE_MEMORY

        m_method = *iter_method;

#ifdef TRACE_MEMORY
        LOGE("memory", "worker findServiceByMetadata m_method=%ld", m_method.use_count());
#endif  // TRACE_MEMORY
    }
    LOGI(this->TAG, "service `%s`", m_method->getPath());
#ifdef TRACE_MEMORY
    LOGE("memory", "worker findServiceByMetadata iter_method=%ld", iter_method->use_count());
    LOGE("memory", "worker findServiceByMetadata m_method=%ld", this->m_method.use_count());
#endif  // TRACE_MEMORY
    return rpc_status::Success;
}
rpc_status erpc::ServerWorker::callMethodByMetadata(myrpc_Meta *req, myrpc_Meta *rsp, void *input, void *output)
{
    rpc_status err;
    rsp->seq = req->seq;
    rsp->version = req->version;
    rsp->path = req->path;
    rsp->has_status = true;
    rsp->has_send_end = false;
    if (!m_method) {
        LOGE(TAG, "Unknown service but called");
        rsp->status = rpc_status::UnknownService;
        return rpc_status::UnknownService;
    }
    err = m_method->handleInvocation(input, output);
    rsp->status = err;
    return err;
}

void erpc::ServerWorker::workerStub(void *arg)
{
    rpc_status err = rpc_status::Success;

    if (arg) {
        std::unique_ptr<ServerWorker> This(reinterpret_cast<ServerWorker *>(arg));
        while (This->isServerOn() && (err == rpc_status::Success || err == rpc_status::Timeout)) {
            err = This->runInternal();
        }
        LOGI(This->TAG, "work done\n");
        if (!This->isServerOn()) {
            LOGI(This->TAG, "closed because server shut down\n");
        }

#ifdef TRACE_MEMORY
        LOGE("memory", "work done: methods=%ld, m_method=%ld", This->methods.use_count(), This->m_method.use_count());
#endif  // TRACE_MEMORY

    } else {
        LOGE("Worke Stub", "This is NULL");
    }
}

void erpc::ServerWorker::start() { m_worker_thread.start(this); }
