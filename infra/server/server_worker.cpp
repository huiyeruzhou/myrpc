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
    snprintf(TAG, sizeof(TAG) - 1, "worker %d", m_worker->m_port);
    m_workerThread.setName(TAG);
}

void ServerWorker::disposeBufferAndCodec(Codec *codec)
{
    if (codec != nullptr)
    {
        if (codec->getBuffer() != nullptr)
        {
            m_messageFactory->dispose(codec->getBuffer());
        }
        m_codecFactory->dispose(codec);
    }
}

erpc_status_t ServerWorker::runInternal()
{
    MessageBuffer buff;
    Codec *codec = nullptr;

    // Handle the request.
    message_type_t msgType;
    uint32_t serviceId;
    uint32_t methodId;
    uint32_t sequence;

    erpc_status_t err = runInternalBegin(&codec, buff, msgType, serviceId, methodId, sequence);
    if (err == kErpcStatus_Success)
    {
        err = runInternalEnd(codec, msgType, serviceId, methodId, sequence);
    }

    return err;
}

erpc_status_t ServerWorker::runInternalBegin(Codec **codec, MessageBuffer &buff, message_type_t &msgType,
    uint32_t &serviceId, uint32_t &methodId, uint32_t &sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    //创建接收缓冲区
    if (m_messageFactory->createServerBuffer() == true)
    {
        buff = m_messageFactory->create();
        if (nullptr == buff.get())
        {
            err = kErpcStatus_MemoryError;
        }
    }

    // Receive the next invocation request.
    if (err == kErpcStatus_Success)
    {
        err = m_worker->receive(&buff);
    }

    //创建codec
    if (err == kErpcStatus_Success)
    {
        *codec = m_codecFactory->create();
        if (*codec == nullptr)
        {
            err = kErpcStatus_MemoryError;
        }
    }

    //
    if (err != kErpcStatus_Success)
    {
        // Dispose of buffers.
        if (buff.get() != nullptr)
        {
            m_messageFactory->dispose(&buff);
        }
    }

    if (err == kErpcStatus_Success)
    {
        (*codec)->setBuffer(buff);

        err = readHeadOfMessage(*codec, msgType, serviceId, methodId, sequence);
        LOGI(this->TAG,"read head of message\n"
               "                msgType: %d, serviceId: %lu,  methodId: %lu, sequence: %lu\n", msgType, serviceId, methodId, sequence);
        if (err != kErpcStatus_Success)
        {
            // Dispose of buffers and codecs.
            disposeBufferAndCodec(*codec);
        }
    }
    if (err != kErpcStatus_Success)
    {
        
        LOGI(this->TAG,"runInternalBegin err: %d\n",   err);
    }

    return err;
}

erpc_status_t ServerWorker::runInternalEnd(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
    uint32_t sequence)
{
    erpc_status_t err = processMessage(codec, msgType, serviceId, methodId, sequence);

    if (err == kErpcStatus_Success)
    {
        if (msgType != kOnewayMessage)
        {

            err = m_worker->send(codec->getBuffer());
        }
    }
    // Dispose of buffers and codecs.
    disposeBufferAndCodec(codec);
    if (err != kErpcStatus_Success)
    {
        LOGI(this->TAG,"runInternalEnd err: %d\n",   err);
    }

    return err;
}

erpc_status_t ServerWorker::readHeadOfMessage(Codec *codec, message_type_t &msgType, uint32_t &serviceId, uint32_t &methodId,
    uint32_t &sequence)
{
    codec->startReadMessage(&msgType, &serviceId, &methodId, &sequence);
    return codec->getStatus();
}

erpc_status_t ServerWorker::processMessage(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
    uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;
    Service *service;

    if ((msgType != kInvocationMessage) && (msgType != kOnewayMessage))
    {
        err = kErpcStatus_InvalidArgument;
    }

    if (err == kErpcStatus_Success)
    {
        service = findServiceWithId(serviceId);
        if (service == nullptr)
        {
            err = kErpcStatus_InvalidArgument;
        }
    }

    if (err == kErpcStatus_Success)
    {
        err = service->handleInvocation(methodId, sequence, codec, m_messageFactory);
        LOGI(this->TAG,"service `%s` invoked\n",   service->m_name);
    }

    if (err != kErpcStatus_Success)
    {
        LOGI(this->TAG,"processMessage err: %d\n",   err);
    }

    return err;
}

Service *ServerWorker::findServiceWithId(uint32_t serviceId)
{
    Service *service = m_firstService;
    while (service != nullptr)
    {
        if (service->getServiceId() == serviceId)
        {
            break;
        }

        service = service->getNext();
    }
    LOGI(this->TAG,"service No.%lu `%s` found\n",   serviceId, service->m_name);
    return service;
}

void ServerWorker::workerStub(void *arg)
{
    int err = kErpcStatus_Fail;
    auto *This = reinterpret_cast<ServerWorker *>(arg);
    LOGI(This->TAG, "in stub");
    if (This != nullptr)
    {
        int i = 1;
        while (err != kErpcStatus_Success && i <= 5)
        {
            err = This->runInternal();
            i++;
        }
        close(This->m_worker->m_socket);
    }
    LOGI(This->TAG, "work done\n");
}

void ServerWorker::start()
{
    m_workerThread.start(this);
}