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

rpc_status ServerWorker::runInternal(void)
{
    MessageBuffer buff;
    Codec *codec = NULL;

    // Handle the request.
    message_type_t msgType;
    uint32_t serviceId;
    uint32_t methodId;
    uint32_t sequence;

    rpc_status err = runInternalBegin(&codec, buff, msgType, serviceId, methodId, sequence);
    if (err == Success)
    {
        err = runInternalEnd(codec, msgType, serviceId, methodId, sequence);
    }

    return err;
}

rpc_status ServerWorker::runInternalBegin(Codec **codec, MessageBuffer &buff, message_type_t &msgType,
    uint32_t &serviceId, uint32_t &methodId, uint32_t &sequence)
{
    rpc_status err = Success;

    //创建接收缓冲区
    if (m_messageFactory->createServerBuffer() == true)
    {
        buff = m_messageFactory->create();
        if (NULL == buff.get())
        {
            err = MemoryError;
        }
    }

    // Receive the next invocation request.
    if (err == Success)
    {
        err = m_worker->receive(&buff);
    }

    //创建codec
    if (err == Success)
    {
        *codec = m_codecFactory->create();
        if (*codec == NULL)
        {
            err = MemoryError;
        }
    }

    //
    if (err != Success)
    {
        // Dispose of buffers.
        if (buff.get() != NULL)
        {
            m_messageFactory->dispose(&buff);
        }
    }

    if (err == Success)
    {
        (*codec)->setBuffer(buff);
        LOGI(TAG, "%s", "new RPC call");

        err = readHeadOfMessage(*codec, msgType, serviceId, methodId, sequence);

        LOGI(this->TAG, "read head of message:    "
            "msgType: %" "d"
            ", serviceId: %" PRIu32
            ",  methodId: %" PRIu32
            ", sequence: %" PRIu32 "",
            msgType, serviceId, methodId, sequence);
        if (err != Success)
        {
            // Dispose of buffers and codecs.
            disposeBufferAndCodec(*codec);
        }
    }
    if (err != Success)
    {

        LOGW(this->TAG, "runInternalBegin err: %d\n", err);
    }

    return err;
}

rpc_status ServerWorker::runInternalEnd(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
    uint32_t sequence)
{
    rpc_status err = processMessage(codec, msgType, serviceId, methodId, sequence);

    if (err == Success)
    {
        if (msgType != kOnewayMessage)
        {

            err = m_worker->send(codec->getBuffer());
        }
    }
    // Dispose of buffers and codecs.
    disposeBufferAndCodec(codec);
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

rpc_status ServerWorker::readHeadOfMessage(Codec *codec, message_type_t &msgType, uint32_t &serviceId, uint32_t &methodId,
    uint32_t &sequence)
{
    codec->startReadMessage(&msgType, &serviceId, &methodId, &sequence);
    return codec->getStatus();
}

rpc_status ServerWorker::processMessage(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
    uint32_t sequence)
{
    rpc_status err = Success;
    Service *service;

    if ((msgType != kInvocationMessage) && (msgType != kOnewayMessage))
    {
        err = InvalidArgument;
    }

    if (err == Success)
    {
        service = findServiceWithId(serviceId);
        if (service == NULL)
        {
            err = InvalidArgument;
        }
    }

    if (err == Success)
    {
        err = service->handleInvocation(methodId, sequence, codec, m_messageFactory);
        LOGI(this->TAG, "service `%s` invoked", service->m_name);
    }

    if (err != Success)
    {
        LOGI(this->TAG, "processMessage err: %d\n", err);
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