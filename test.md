idl->astnode->info      |
templete->templete_tree >  output

# Erpc服务处理过程梳理

RPC的服务端处理过程从传输层开始。当传输层收到消息，就会根据约定的传输协议接受信息，然后调用序列化层。序列化解析出请求内容，交由代理层处理。代理层通过对Server和Service的抽象，从请求内容中解析要调用的实际方法和参数，并根据有无返回值/是否需要写回数据做进一步处理。
如果有返回消息的必要，则代理层调用序列化层写入返回内容，调用传输层用协议封装信息并进行发送

## 代理层服务器开始服务

一个典型的服务器启动之后便开始不断监听可能到来的请求：

```c++
erpc_status_t SimpleServer::run(void)
{
    erpc_status_t err = kErpcStatus_Success;
    while ((err == kErpcStatus_Success) && m_isServerOn)
    {
        err = runInternal();
    }
    return err;
}
```

处理过程大致可以分为两步,一是信息处理,二是请求处理,三是数据写回.在erpc中对应 `runInternalBegin`和`runInternalEnd`,前者完成第一步,后者完成后两步.

```c++
erpc_status_t SimpleServer::runInternal(void)
{
    MessageBuffer buff;
    Codec *codec = NULL;
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
```

## 信息处理

Server在处理消息之前,必须获取一些关键的信息用于下一步处理事件.概括来说,一般需要以下几类信息:

- RPC协议相关
  - 消息类型: 是单向请求信息,双向请求信息还是响应信息或系统通知等
  - RPC协议版本: 用于正确解析信息
  - 序列化方法: 用于正确配置序列化层
  - 请求序列号: 用于异步响应时区分多个请求
- 代理相关
  - 请求的Service: 请求的包名/类名/服务名等
  - 请求的Method: 请求的具体方法
  - 请求的arguments: 实参
  - 请求的参数类型: 用于重载解析

这些信息有的在解析完之后立刻使用;有的存放起来作为参数在调用栈中传递;有的暂时不解析,而是配置一个codec,到时按需取用.

从代码中可以看到,信息解析的过程主要包括传输层接收,序列化层读取两个过程

```c++
erpc_status_t SimpleServer::runInternalBegin(Codec **codec, MessageBuffer &buff, message_type_t &msgType,
                                             uint32_t &serviceId, uint32_t &methodId, uint32_t &sequence)
{
    erpc_status_t err = kErpcStatus_Success;
    // buffer创建
    if (m_messageFactory->createServerBuffer() == true)
    {
        buff = m_messageFactory->create();
    }
    // 传输层接收
    err = m_transport->receive(&buff);
    // 序列化层初始化及读取
    *codec = m_codecFactory->create();
    (*codec)->setBuffer(buff);
    err = readHeadOfMessage(*codec, msgType, serviceId, methodId, sequence);

    return err;
}
```

### 传输层接收

传输层接受的部分就一句话,简而言之就是将信息读取到buffer中

```c++
   // 传输层接收
    err = m_transport->receive(&buff);
```

erpc在这里为所有按帧传输的传输层准备了接受一帧的函数,先接受请求头,读出请求帧的长度,再接受请求体.

```c++
erpc_status_t FramedTransport::receive(MessageBuffer *message)
{
    Header h;
    erpc_status_t retVal;
    uint16_t computedCrc;
    {
#if !ERPC_THREADS_IS(NONE)
        Mutex::Guard lock(m_receiveLock);
#endif
        // Receive header first.
        retVal = underlyingReceive((uint8_t *)&h, sizeof(h));
         // Receive rest of the message now we know its size.
        retVal = underlyingReceive(message->get(), h.m_messageSize);
    }

    // Verify CRC.
    ...

    message->setUsed(h.m_messageSize);
    return retVal;
}
```

其底层实现为`TCPTransport::underlyingReceive`,本质上就是对read进行了封装.

```c++
erpc_status_t TCPTransport::underlyingReceive(uint8_t *data, uint32_t size){
    ssize_t length;
    erpc_status_t status = kErpcStatus_Success;

    // Block until we have a valid connection.
    while (m_socket <= 0){
        Thread::sleep(10000);
    }
    // Loop until all requested data is received.
    while (size > 0U){
        length = read(m_socket, data, size);

        // Length will be zero if the connection is closed.
        if (length > 0){
            size -= length;
            data += length;
        }else{
            if (length == 0){
                // close socket, not server
                close(false);
                status = kErpcStatus_ConnectionClosed;
            }else{
                status = kErpcStatus_ReceiveFailed;
            }
            break;
        }
    }
    return status;
}
```

通过这个过程,我们将socket中的信息成功的接收到了buf中

### 序列化层解析

在erpc中,codec是对一次序列化的封装,一个codec不仅有读写各种数据类型的方法,还持有目前正在处理的buffer和buffer上的cursor,这纯粹是为了语法上的方便,不必在调用每个读写函数时都额外传递一个buffer参数,只需要在开始时设置即可.

```c++
class Codec
{
protected:
    MessageBuffer m_buffer;         /*!< Message buffer object */
    MessageBuffer::Cursor m_cursor; /*!< Copy data to message buffers. */
    erpc_status_t m_status;         /*!< Status of serialized data. */
};
```

因此,codec处理的部分先是创建一个codec并绑定buffer,然后再调用内部处理函数读取请求头

```c++
    // 序列化层初始化及读取
    *codec = m_codecFactory->create();
    (*codec)->setBuffer(buff);
    err = readHeadOfMessage(*codec, msgType, serviceId, methodId, sequence);
```

这里我们就不详细探究codec的底层实现了,可以想到对字节流做处理的大概方法就是针对数据类型的长度,读取一定的字节,填入到对象指针指向的内存中,并移动buffer上的cursor.

总体来说,在这里值得注意的地方时codec兼具序列化层和RPC传输协议约定的功能,codec的`readHeadOfMessage`的实现方法其实就对应了协议头如何组织信息.

在默认的codec中,请求头是一个四字节头加上一个int32序列号,四个字节分别表示codecVersion, serviceId, methodId, requestType(不过很明显,先构建codec再读取头信息已经太迟了,所以当他检测到不匹配的codecversion时没法处理,只能直接报错)

```c++
void BasicCodec::startReadMessage(message_type_t *type, uint32_t *service, uint32_t *request, uint32_t *sequence){
    uint32_t header;
    read(&header);
    if (((header >> 24) & 0xffU) != kBasicCodecVersion){
        updateStatus(kErpcStatus_InvalidMessageVersion);
    }
    if (isStatusOk()){
        *service = ((header >> 16) & 0xffU);
        *request = ((header >> 8) & 0xffU);
        *type = static_cast<message_type_t>(header & 0xffU);
        read(sequence);
    }
}
```

## 请求处理

在经过了信息处理之后,我们已经成功的将传输层中的一帧请求接收到buffer中,获得了这个请求的rpc协议参数,接下来要做的就是调用合适的处理程序来完成实际的处理工作.erpc中,这个函数正是`Server::processMessage`

```c++
erpc_status_t SimpleServer::runInternalEnd(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
                                           uint32_t sequence){
    //请求处理
    erpc_status_t err = processMessage(codec, msgType, serviceId, methodId, sequence);

    //消息写回
    if (err == kErpcStatus_Success){
        if (msgType != kOnewayMessage){
                err = m_transport->send(codec->getBuffer());
        }
    }
    // Dispose of buffers and codecs.
    disposeBufferAndCodec(codec);
    return err;
}
```

通过阅读源码,我们看到请求处理分为两个阶段:服务发现和方法调用

```c++
erpc_status_t Server::processMessage(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
                                     uint32_t sequence){
    erpc_status_t err = kErpcStatus_Success;
    Service *service;
    //服务发现
    service = findServiceWithId(serviceId);
    //方法调用
    err = service->handleInvocation(methodId, sequence, codec, m_messageFactory);

    return err;
}
```

### 服务发现

服务发现的过程就是在服务器已注册的服务中寻找能响应请求的那个,一些语言会使用map来完成这个事情,而在erpc中直接使用了serviceId+methodId, 如果idl确定的话,对于S/C端而言,这个id号是可以唯一确定有效服务的.

在服务端,服务用链表存储:

```c++
class Service
{
public:
 
    uint32_t getServiceId(void) const { return m_serviceId; }
    Service *getNext(void) { return m_next; }
    void setNext(Service *next) { m_next = next; }
    virtual erpc_status_t handleInvocation(uint32_t methodId, uint32_t sequence, Codec *codec,
                                           MessageBufferFactory *messageFactory) = 0;
protected:
    uint32_t m_serviceId; /*!< Service unique id. */
    Service *m_next;      /*!< Pointer to next service. */
};
```

服务器在解析信息时通过遍历链表查找id对应的服务,然后调用对应的处理函数:

```c++
erpc_status_t Server::processMessage(Codec *codec, message_type_t msgType, uint32_t serviceId, uint32_t methodId,
                                     uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;
    Service *service;
    service = findServiceWithId(serviceId);
    err = service->handleInvocation(methodId, sequence, codec, m_messageFactory);
    return err;
}
```

### 方法调用

`Server::processMessage`调用的`Service::handleInvocation`是由erpcgen工具根据我们定义的IDL自动生成的.
在.erpc中声明service及其方法, 其中interface关键字后跟服务名, 花括号内可以有若干个方法.

```go
interface MatrixMultiplyService // cover functions for same topic
{
    erpcMatrixMultiply(in Matrix matrix1, in Matrix matrix2, out Matrix result_matrix) -> void
}
```

然后执行下列命令:

```bash
erpcgen idl.erpc -g c
```

即可得到生成的文件

```bash
erpc_matrix_multiply_client.c++
erpc_matrix_multiply_server.c++
erpc_matrix_multiply_server.h
erpc_matrix_multiply.h
```

其中,`erpc_matrix_multiply_server.c++`就有服务处理函数的定义.

```c++
erpc_status_t MatrixMultiplyService_service::handleInvocation(uint32_t methodId, uint32_t sequence, Codec * codec, MessageBufferFactory *messageFactory)
{
    erpc_status_t erpcStatus;
    switch (methodId){
        case kMatrixMultiplyService_erpcMatrixMultiply_id:
            erpcStatus = erpcMatrixMultiply_shim(codec, messageFactory, sequence);
            break;
        default:
            erpcStatus = kErpcStatus_InvalidArgument;
            break;
    }
    return erpcStatus;
}
```

可以看到, 处理函数进一步通过methodId判断该调用Service中的哪个方法的shim, 这个shim就是方法对应的代理.
代理完成的内容就是组织codec进行反序列化, 获取入参,然后对out类型的数据进行序列化写回, 返回值也一并写回

```c++
erpc_status_t MatrixMultiplyService_service::erpcMatrixMultiply_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    Matrix matrix1;
    Matrix matrix2;
    Matrix result_matrix;

    for (uint32_t arrayCount0 = 0U; arrayCount0 < 2U; ++arrayCount0){
        for (uint32_t arrayCount1 = 0U; arrayCount1 < 2U; ++arrayCount1){
            codec->read(&matrix1[arrayCount0][arrayCount1]);
        }
    }

    for (uint32_t arrayCount0 = 0U; arrayCount0 < 2U; ++arrayCount0){
        for (uint32_t arrayCount1 = 0U; arrayCount1 < 2U; ++arrayCount1){
            codec->read(&matrix2[arrayCount0][arrayCount1]);
        }
    }
    // preparing codec for serializing data
    codec->reset();
    
    //调用实际方法
    erpcMatrixMultiply(matrix1, matrix2, result_matrix);

    // Build response message.
    codec->startWriteMessage(kReplyMessage, kMatrixMultiplyService_service_id, kMatrixMultiplyService_erpcMatrixMultiply_id, sequence);

    for (uint32_t arrayCount0 = 0U; arrayCount0 < 2U; ++arrayCount0){
        for (uint32_t arrayCount1 = 0U; arrayCount1 < 2U; ++arrayCount1){
                codec->write(result_matrix[arrayCount0][arrayCount1]);
        }
    }

    return err;
}

```

### 数据写回

首先需要注意的是,在调用实际方法之前,对codec进行了重置:

```c++
    // preparing codec for serializing data
    codec->reset();
```

这个重置函数将内部的buffer和cursor恢复到使用之前的状态,相当于将这个读缓冲区又作为写缓冲区复用.在写回过程中，序列化首先调用codec的startWriteMessage方法写回响应头.和我们之前谈到的一样,这一步也是和RPC协议相关的,即codec写入头消息的方式就是rpc协议的方式.

```c++
void BasicCodec::startWriteMessage(message_type_t type, uint32_t service, uint32_t request, uint32_t sequence)
{
    uint32_t header =
        (kBasicCodecVersion << 24u) | ((service & 0xffu) << 16u) | ((request & 0xffu) << 8u) | ((uint32_t)type & 0xffu);

    write(header);

    write(sequence);
}
```

然后, 调用Codec的write方法.在这里,对于数组,erpcgen将会自动生成对应的循环结构来写入.

```c++
    ...    
        for (uint32_t arrayCount0 = 0U; arrayCount0 < 2U; ++arrayCount0)
        {
            for (uint32_t arrayCount1 = 0U; arrayCount1 < 2U; ++arrayCount1)
            {
                codec->write(result_matrix[arrayCount0][arrayCount1]);
            }
        }
    ...
```

Codec对c++的所有数据类型重载一个write方法, 这个方法是对writeData的封装, 后者将按照数据的值和长度传递写入数据

```c++
void BasicCodec::writeData(const void *value, uint32_t length){
    if (isStatusOk()){
        m_status = m_cursor.write(value, length);
    }
}
```

cursor将作为buffer的内部类完成信息的写入，就是将信息复制到缓冲区里

```c++
erpc_status_t MessageBuffer::Cursor::write(const void *data, uint32_t length){
    erpc_status_t err = kErpcStatus_Success;
    if (length > 0U){
        if (data == NULL){
            err = kErpcStatus_MemoryError;
        }else if (length > getRemaining()){
            err = kErpcStatus_BufferOverrun;
        }else{
            (void)memcpy(m_pos, data, length);
            m_pos += length;
            m_buffer->setUsed(m_buffer->getUsed() + length);
        }
    }
    return err;
}
```

完成write之后shim就会直接返回,之后可以直接利用send发送,因为我们在生成的shim函数中已经完成了需要写回的数据的序列化

```c++
    //数据发送
    if (err == kErpcStatus_Success){
        if (msgType != kOnewayMessage){
                err = m_transport->send(codec->getBuffer());
        }
    }
```
## 总结
