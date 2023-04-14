#include "server/server_worker.hpp"


using namespace erpc;


ServerWorker::ServerWorker(Service *services, TCPWorker *worker)
    :m_workerThread(workerStub)
    , m_firstService(services)
    , m_worker(worker)
{
    snprintf(TAG, sizeof(TAG) - 1, "worker %" PRIin_port_t, m_worker->m_port);
    m_workerThread.setName(TAG);
}
ServerWorker::~ServerWorker() {
    m_service->destroyMsg(m_worker->to_recv_msg, m_worker->to_send_msg);
    delete m_worker;
};
rpc_status ServerWorker::runInternal(void)
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
    LOGI(TAG, "receiveFrame()");
    //recv inital metadata
    CHECK_STATUS(m_worker->recv_inital_md(), err);
    LOGI(TAG, "recv_inital_md()");

    //find the method
    CHECK_STATUS(findServiceByMetadata(req_md),err);
    //filled message desc, so that we can receive message
    m_service->filledMsgDesc(input_desc, input_msg, output_desc, output_msg);
    
    //recv messgae
    CHECK_STATUS(m_worker->recv_msg(), err);
    LOGI(TAG, "recv_msg()");
    //recv trailing metadata
    CHECK_STATUS(m_worker->recv_trailing_md(), err);
    LOGI(TAG, "recv_trailing_md()");
    
    CHECK_STATUS(callMethodByMetadata(req_md, rsp_md, m_worker->to_recv_msg, m_worker->to_send_msg), err);

    //send initial metadata
    CHECK_STATUS(m_worker->send_inital_md(), err);
    LOGI(TAG, "send_inital_md()");
    //send msg
    CHECK_STATUS(m_worker->send_msg(), err);
    LOGI(TAG, "send_msg()");
    //sned trailing metadata
done:
    rpc_status has_error = err;
    if (err != rpc_status::Success && ((rsp_md->has_status && rsp_md->status == rpc_status::Success) || !rsp_md->has_status)) {
        rsp_md->has_status = true;
        rsp_md->status = err;
    }
    if (err == rpc_status::kErpcStatus_ConnectionClosed) {
        m_worker->close();
        return kErpcStatus_ConnectionClosed;
    }
    err = m_worker->send_trailing_md();
    if (rpc_status::Success != err) {
        LOGE(TAG, "Failed to send trailing md");
    }
    LOGI(TAG, "send_trailing_md()");
    if (has_error != rpc_status::Success) {
        LOGW(TAG, "Error occurred: %s", StatusToString(has_error));
    }
    else {
        LOGI(TAG, "RPC Call Success\n");
    }
    resetBuffers();
    return err;
}
rpc_status ServerWorker::resetBuffers(void)
{
    m_service->destroyMsg(m_worker->to_recv_msg, m_worker->to_send_msg);
    m_service = NULL;
    return m_worker->resetBuffers();
}
rpc_status ServerWorker::findServiceByMetadata(myrpc_Meta *req) {
    Service *service = m_firstService;
    while (service != NULL) {
        // find the last '/' in path
        if (strcmp(service->getServiceId(), req->path) == 0) {
            break;
        }
        service = service->getNext();
    }
    if (!service) {
        return UnknownService;
    }
    m_service = service;
    LOGI(this->TAG, "service `%s`", service->getServiceId());
    return rpc_status::Success;
}
rpc_status ServerWorker::callMethodByMetadata(myrpc_Meta *req, myrpc_Meta *rsp, void *input, void *output)
{
    if (!m_service) {
        LOGE(TAG, "Unknown service but called");
        return rpc_status::UnknownService;
    }
    rpc_status err = m_service->handleInvocation(input, output);
    rsp->seq = req->seq;
    rsp->version = req->version;
    rsp->path = req->path;
    rsp->has_status = true;
    rsp->status = err;
    rsp->has_send_end = false;
    return err;
}

void ServerWorker::workerStub(void *arg)
{
    rpc_status err = rpc_status::Success;
    ServerWorker *This = reinterpret_cast<ServerWorker *>(arg);

    if (This != NULL)
    {
        while (err == rpc_status::Success)
        {
            err = This->runInternal();
        }
    }
    LOGI(This->TAG, "work done\n");
    return;
}

void ServerWorker::start(void)
{
    m_workerThread.start(this);
}