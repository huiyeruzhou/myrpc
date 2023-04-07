#include "server/server_worker.hpp"


using namespace erpc;


ServerWorker::ServerWorker(Service *services, MessageBufferFactory *messageFactory,
    TCPWorker *worker)
    :m_workerThread(workerStub)
    , m_firstService(services)
{
    m_worker = new NanopbTransport(worker, messageFactory);
    snprintf(TAG, sizeof(TAG) - 1, "worker %" PRId16, static_cast<TCPWorker *>(m_worker->worker)->m_port);
    m_workerThread.setName(TAG);
}

void ServerWorker::disposeBufferAndCodec()
{
}

rpc_status ServerWorker::runInternal(void)
{
    // Handle the request.
    myrpc_Meta meta = myrpc_Meta_init_zero;
    rpc_status err = runInternalBegin(&meta);
    if (err == Success)
    {
        err = runInternalEnd(&meta);
    }
    return err;
}

rpc_status ServerWorker::runInternalBegin(myrpc_Meta *meta)
{
    rpc_status err = Success;

    // Receive the next invocation request.
    if (err == Success)
    {
        err = m_worker->receive();
    }


    if (err != Success)
    {
        // Dispose of buffers.
    }
    else
    {
        myrpc_Meta meta = myrpc_Meta_init_zero;
        m_worker->read(myrpc_Meta_fields, &meta);
        LOGI(TAG, "%s", "new RPC call");
        
        LOGI(this->TAG, "read head of message:    "
            "msgType: %" "d"
            ", serviceId: %" PRIu32
            ",  methodId: %" PRIu32
            ", sequence: %" PRIu32 "",
            meta.type, meta.serviceId, meta.methodId, meta.seq);
        if (err != Success)
        {
            // Dispose of buffers and codecs.
            disposeBufferAndCodec();
        }
    }
    if (err != Success)
    {

        LOGW(this->TAG, "runInternalBegin err: %d\n", err);
    }

    return err;
}

rpc_status ServerWorker::runInternalEnd(myrpc_Meta *meta)
{
    rpc_status err = processMessage(meta);

    if (err == Success)
    {
        err = m_worker->send();
    }
    // Dispose of buffers and codecs.
    disposeBufferAndCodec();
    if (err != Success)
    {
        LOGW(this->TAG, "runInternalEnd err: %d\n", err);
    }
    else
    {
        LOGI(TAG, "%s", "RPC call finished\n");
    }

    return err;
}


rpc_status ServerWorker::processMessage(myrpc_Meta *meta)
{
    Service *service;

    if ((meta->type != myrpc_Meta_msgType_REQUEST) && (meta->type != myrpc_Meta_msgType_ONEWAY))
        return UnExpectedMsgType;
    

    if (NULL == findServiceWithId(meta->serviceId))
        return UnknownService;

    rpc_status err;
    err = service->handleInvocation(m_worker, meta);
    if (err != Success) {
        LOGI(this->TAG, "processMessage err: %d\n", err);
        return err;
    }

    LOGI(this->TAG, "service `%s` invoked", service->m_name);
    return Success;
}

Service *ServerWorker::findServiceWithId(uint32_t serviceId)
{
    Service *service = m_firstService;
    while (service != NULL)
    {
        if (service->getServiceId() == serviceId)
        {
            break;
        }

        service = service->getNext();
    }
    LOGI(this->TAG, "service No.%" PRIu32 " found:  `%s`", serviceId, service->m_name);
    return service;
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