/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * Copyright 2019-2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "simple_server.hpp"
using namespace erpc;
__attribute__((unused)) static const char *TAG = "server";
// #define EPOLL_SIZE 1024
// int setnonblocking(int sockfd)//非阻塞模式设置
// {
//     fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
//     return 0;
// }
// void addfd(int epollfd, int fd, bool enable_et)//将fd加入到epoll中，并设置边缘触发模式
// {
//     struct epoll_event ev;
//     ev.data.fd = fd;
//     ev.events = EPOLLIN;
//     if (enable_et)
//         ev.events = EPOLLIN | EPOLLET;
//     epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
//     setnonblocking(fd);
//     printf(TAG, "fd No.%d added to epoll!\n", fd);
// }

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

SimpleServer::SimpleServer(const char *host, uint16_t port, MessageBufferFactory *message_buffer_factory)
    : Server(host, port)
    , m_isServerOn(true)
    , m_serverThread(SimpleServer::networkpollerStub)
    , m_runServer(false)
{
    BasicCodecFactory *codecFactory = new BasicCodecFactory();

    // Init server with the provided transport.
    this->setCodecFactory(codecFactory);
    this->setMessageBufferFactory(message_buffer_factory);

}

SimpleServer::~SimpleServer()
{
    delete m_codecFactory;
}

rpc_status_t SimpleServer::run(void)
{
    rpc_status_t err = rpc_status_success;
    while ((err == rpc_status_success) && m_isServerOn)
    {
        // Sleep 10 ms.
         Thread::sleep(10000);
    }
    return err;
}

// rpc_status_t SimpleServer::poll(void)
// {
//     rpc_status_t err;

//     if (m_isServerOn)
//     {
//         if (m_transport->hasMessage() == true)
//         {
//             err = runInternal();
//         }
//         else
//         {
//             err = rpc_status_success;
//         }
//     }
//     else
//     {
//         err = kErpcStatus_ServerIsDown;
//     }

//     return err;
// }

void SimpleServer::stop(void)
{
    m_isServerOn = false;
}

void SimpleServer::onNewSocket(int sockfd, int port) {
    TCPWorker *transport_worker = new TCPWorker(sockfd, port);
    ServerWorker *worker = new ServerWorker(m_firstService, m_messageFactory, m_codecFactory, transport_worker);
    worker->start();
}

rpc_status_t SimpleServer::close(bool stopServer)
{
    if (stopServer)
    {
        m_runServer = false;
    }

    if (m_sockfd != -1)
    {
        ::close(m_sockfd);
        m_sockfd = -1;
    }

    return rpc_status_success;
}



void SimpleServer::networkpollerThread(void)
{
    int yes = 1;
    int result;
    struct sockaddr incomingAddress;
    socklen_t incomingAddressLength;
    int incomingSocket;
    bool status = false;
    struct sockaddr_in serverAddress;
    // int epfd;
    // static struct epoll_event events[EPOLL_SIZE];

    LOGI(TAG, "%s", "networkpollerThread");

    // Create socket.
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0)
    {
        LOGE(TAG, "failed to create server socket");
        status = true;
    }
    if (!status)
    {
        // Fill in address struct.
        (void) memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY; // htonl(local ? INADDR_LOOPBACK : INADDR_ANY);
        serverAddress.sin_port = htons(m_port);

        // Turn on reuse address option.
        result = setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        if (result < 0)
        {
            LOGE(TAG, "setsockopt failed");
            status = true;
        }
    }

    if (!status)
    {
        // Bind socket to address.
        result = bind(m_sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
        //on Failed
        if (result < 0)
        {
            LOGE(TAG, "bind failed");
            status = true;
        }
        //on Success
        char netinfo[24];
        sprint_net_info(netinfo, sizeof(netinfo), (struct sockaddr *) &serverAddress, sizeof(serverAddress));
        LOGI(TAG, "bind to %s", netinfo);
        
    }

    if (!status)
    {
        // Listen for connections.
        result = listen(m_sockfd, 1);
        //on Failed
        if (result < 0)
        {
            LOGE(TAG, "listen failed");
            status = true;
        }
        //on Success
        LOGI(TAG, "%s", "Listening for connections\n");
    }
    if (!status)
    {
        // //在内核中创建事件表
        // epfd = epoll_create(EPOLL_SIZE);
        // if (epfd < 0)
        // {
        //     LOGE(TAG, "epfd error");
        //     status = true;
        // }
        // LOGI(TAG, "epoll created, epollfd = %d\n", epfd);
        // //往内核事件表里添加事件
        // addfd(epfd, m_sockfd, true);

    }
    if (!status)
    {

        while (m_runServer)
        {
            incomingSocket = accept(m_sockfd, &incomingAddress, &incomingAddressLength);

            if (incomingSocket > 0)
            {
                // Successfully accepted a connection.
                char netinfo[24];
                sprint_net_info(netinfo, sizeof(netinfo), &incomingAddress, incomingAddressLength);
                LOGI(TAG, "accepted connection from %s", netinfo);
                

                // should be inherited from accept() socket but it's not always ...
                yes = 1;
                setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, (void *) &yes, sizeof(yes));
                // addfd(epfd, incomingSocket, true);
                onNewSocket(incomingSocket, getPortFormAddr(&incomingAddress, incomingAddressLength));
            }
            else
            {
                LOGE(TAG, "accept failed");
            }
            // int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
            // if (epoll_events_count < 0)
            // {
            //     perror("epoll failure");
            //     break;
            // }
            // //处理所有已经就绪的事件
            // for (int i = 0; i < epoll_events_count; ++i)
            // {
            //     int sockfd = events[i].data.fd;
            //     //新用户连接
            //     if (sockfd == m_sockfd)
            //     {
            //         incomingAddressLength = sizeof(struct sockaddr);
            //         // we should use select() otherwise we can't end the server properly
            //         incomingSocket = accept(m_sockfd, &incomingAddress, &incomingAddressLength);

            //         if (incomingSocket > 0)
            //         {
            //             // Successfully accepted a connection.
            //             LOGI(TAG, "accepted connection from ");
            //             sprint_net_info(&incomingAddress, incomingAddressLength);

            //             // should be inherited from accept() socket but it's not always ...
            //             yes = 1;
            //             setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, (void *) &yes, sizeof(yes));
            //             addfd(epfd, incomingSocket, true);
            //             onNewSocket(incomingSocket, getPortFormAddr(&incomingAddress, incomingAddressLength));
            //         }
            //         else
            //         {
            //             LOGE("accept failed");
            //         }
            //     }
            //     //TODO:处理客户端的进一步输入
            //     else
            //     {
            //     }
            // }
        }
    }
    close(m_sockfd);
}


void SimpleServer::networkpollerStub(void *arg)
{
    SimpleServer *This = reinterpret_cast<SimpleServer *>(arg);

    LOGI(TAG, "in networkpollerStub (arg=%p)", arg);
    if (This != NULL)
    {
        This->networkpollerThread();
    }
}

rpc_status_t SimpleServer::open(void)
{
    rpc_status_t status;
    m_serverThread.setName("Network Poller");
    m_runServer = true;
    m_serverThread.start(this);
    LOGI(TAG, "start running networkpollerThread");
    status = rpc_status_success;
    return status;
}