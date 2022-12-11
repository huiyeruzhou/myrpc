#include "server_worker.hpp"


using namespace erpc;

ServerWorker::ServerWorker(Service *services, MessageBufferFactory *messageFactory,
    CodecFactory *codecFactory,
    TCPWorker *worker)
    : m_workerThread(workerStub)
    , m_firstService(services)
    , m_messageFactory(messageFactory)
    , m_codecFactory(codecFactory)
    , m_worker(worker)
{
    snprintf(TAG, sizeof(TAG) - 1, "worker %" PRId16, m_worker->m_port);
    m_workerThread.setName(TAG);
}

void ServerWorker::disposeBufferAndCodec(Codec *codec)
{
    if (codec != NULL)
    {
        if (codec->getBuffer() != NULL)
        {
            m_messageFactory->dispose(codec->getBuffer());
        }
        m_codecFactory->dispose(codec);
    }
}

rpc_status_t ServerWorker::runInternal(void)
{
    MessageBuffer buff;
    Codec *codec = NULL;

    // Handle the request.
    message_type_t msgType;
    uint32_t serviceId;
    uint32_t methodId;
    uint32_t sequence;

    rpc_status_t err = runInternalBegin(&codec, buff, msgType, serviceId, methodId, sequence);
    if (err == rpc_status_success)
    {
        err = runInternalEnd(codec, msgType, serviceId, methodId, sequence);
    }

    return err;
}

rpc_status_t ServerWorker::runInternalBegin(Codec **codec, MessageBuffer &buff, message_type_t &msgType,
                                            uint32_t &serviceId, uint32_t &methodId, uint32_t &sequence)
{
    rpc_status_t err = rpc_status_success;

    //创建接收缓冲区
    if (m_messageFactory->createServerBuffer() == true)
    {
        buff = m_messageFactory->create();
        if (NULL == buff.get())
        {
            err = kErpcStatus_MemoryError;
        }
    }

    // Receive the next invocation request.
    if (err == rpc_status_success)
    {
        err = m_worker->receive(&buff);
    }

    //创建codec
    if (err == rpc_status_success)
    {
        *codec = m_codecFactory->create();
        if (*codec == NULL)
        {
            err = kErpcStatus_MemoryError;
        }
    }

    //
    if (err != rpc_status_success)
    {
        // Dispose of buffers.
        if (buff.get() != NULL)
        {
            m_messageFactory->dispose(&buff);
        }
    }

    if (err == rpc_status_success)
    {
        (*codec)->setBuffer(buff);

        err = readHeadOfMessage(*codec, msgType, serviceId, methodId, sequence);
        
        LOGI(this->TAG, "read head of message\n                "
            "msgType: %" "d" 
            ", serviceId: %" PRIu32
            ",  methodId: %" PRIu32
            ", sequence: %" PRIu32 "\n",
            msgType, serviceId, methodId, sequence);
        if (err != rpc_status_success)
        {
            // Dispose of buffers and codecs.
            disposeBufferAndCodec(*codec);
        }
    }
    if (err != rpc_status_success)
    {
        
        LOGI(this->TAG,"runInternalBegin err: %d\n",   err);
    }

    return err;
}

rpc_status_t ServerWorker::runInternalEnd(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
                                          uint32_t sequence)
{
    rpc_status_t err = processMessage(codec, msgType, serviceId, methodId, sequence);

    if (err == rpc_status_success)
    {
        if (msgType != kOnewayMessage)
        {

            err = m_worker->send(codec->getBuffer());
        }
    }
    // Dispose of buffers and codecs.
    disposeBufferAndCodec(codec);
    if (err != rpc_status_success)
    {
        LOGI(this->TAG,"runInternalEnd err: %d\n",   err);
    }

    return err;
}

rpc_status_t ServerWorker::readHeadOfMessage(Codec *codec, message_type_t &msgType, uint32_t &serviceId, uint32_t &methodId,
                                             uint32_t &sequence)
{
    codec->startReadMessage(&msgType, &serviceId, &methodId, &sequence);
    return codec->getStatus();
}

rpc_status_t ServerWorker::processMessage(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
                                          uint32_t sequence)
{
    rpc_status_t err = rpc_status_success;
    Service *service;

    if ((msgType != kInvocationMessage) && (msgType != kOnewayMessage))
    {
        err = rpc_status_invalid_argument;
    }

    if (err == rpc_status_success)
    {
        service = findServiceWithId(serviceId);
        if (service == NULL)
        {
            err = rpc_status_invalid_argument;
        }
    }

    if (err == rpc_status_success)
    {
        err = service->handleInvocation(methodId, sequence, codec, m_messageFactory);
        LOGI(this->TAG,"service `%s` invoked\n",   service->m_name);
    }

    if (err != rpc_status_success)
    {
        LOGI(this->TAG,"processMessage err: %d\n",   err);
    }

    return err;
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
    LOGI(this->TAG,"service No.%" PRIu32 " `%s` found\n",   serviceId, service->m_name);
    return service;
}

void ServerWorker::workerStub(void *arg)
{
    int err = rpc_status_fail;
    ServerWorker *This = reinterpret_cast<ServerWorker *>(arg);
    LOGI(This->TAG, "in stub");
    if (This != NULL)
    {
        int i = 1;
        while (err != rpc_status_success && i <= 5)
        {
            err = This->runInternal();
            i++;
        }
        close(This->m_worker->m_socket);
    }
    LOGI(This->TAG, "work done\n");
    return;
}

void ServerWorker::start(void)
{
    m_workerThread.start(this);
}