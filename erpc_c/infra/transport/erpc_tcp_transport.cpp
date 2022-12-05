/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "erpc_tcp_transport.hpp"

#include <cstdio>
#include <string>

extern "C" {
#if ERPC_HAS_POSIX
#include <err.h>
#endif
#include <errno.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
}

using namespace erpc;

// Set this to 1 to enable debug logging.
// TODO fix issue with the transport not working on Linux if debug logging is disabled.
#define TCP_TRANSPORT_DEBUG_LOG (1)

#if TCP_TRANSPORT_DEBUG_LOG 
#define TCP_DEBUG_PRINT(_fmt_, ...) printf(_fmt_, ##__VA_ARGS__)
#define TCP_DEBUG_ERR(_msg_) err(errno, _msg_)
void print_net_info(const sockaddr *__sockaddr, int __len) {
    char host[32];
    char service[32];
    getnameinfo(__sockaddr, __len, host, 32, service, 32, 0);
    printf("%s:%s\n", host, service);
}
int getPortFormAddr(const sockaddr *__sockaddr, int __len) {
    char service[32];
    int port;
    getnameinfo(__sockaddr, __len, NULL, 0, service, 32, 0);
    if (sscanf(service, "%d", &port) == 1)
    {
        return port;
    }
    return -1;
}
#else
#define TCP_DEBUG_PRINT(_fmt_, ...)
#define TCP_DEBUG_ERR(_msg_)
void print_net_info(const sockaddr *__sockaddr, int __len) {
}
#endif

#define EPOLL_SIZE 1024
int setnonblocking(int sockfd)//非阻塞模式设置
{
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}
void addfd(int epollfd, int fd, bool enable_et)//将fd加入到epoll中，并设置边缘触发模式
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (enable_et)
        ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    setnonblocking(fd);
    printf("server:    fd No.%d added to epoll!\n", fd);
}
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

TCPTransport::TCPTransport(bool isServer)
    : m_isServer(isServer)
    , m_host(NULL)
    , m_port(0)
    , m_socket(-1)
    , m_serverThread(serverThreadStub)
    , m_runServer(true)
{
}

TCPTransport::TCPTransport(const char *host, uint16_t port, bool isServer)
    : m_isServer(isServer)
    , m_host(host)
    , m_port(port)
    , m_socket(-1)
    , m_serverThread(serverThreadStub)
    , m_runServer(true)
{
}

TCPTransport::~TCPTransport(void) {}

void TCPTransport::configure(const char *host, uint16_t port)
{
    m_host = host;
    m_port = port;
}

erpc_status_t TCPTransport::open(void)
{
    erpc_status_t status;

    if (m_isServer)
    {
        m_runServer = true;
        TCP_DEBUG_PRINT("server:    start running serverThread\n");
        m_serverThread.start(this);
        status = kErpcStatus_Success;
    }
    else
    {
        status = connectClient();
    }

    return status;
}

erpc_status_t TCPTransport::connectClient(void)
{
    erpc_status_t status = kErpcStatus_Success;
    struct addrinfo hints = {};
    char portString[8];
    struct addrinfo *res0;
    int result, set;
    int sock = -1;
    struct addrinfo *res;

    if (m_socket != -1)
    {
        TCP_DEBUG_PRINT("%s", "socket already connected\n");
    }
    else
    {
        // Fill in hints structure for getaddrinfo.
        hints.ai_flags = AI_NUMERICSERV;
        hints.ai_family = PF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        // Convert port number to a string.
        result = snprintf(portString, sizeof(portString), "%d", m_port);
        if (result < 0)
        {
            TCP_DEBUG_ERR("snprintf failed");
            status = kErpcStatus_Fail;
        }

        if (status == kErpcStatus_Success)
        {
            // Perform the name lookup.
            result = getaddrinfo(m_host, portString, &hints, &res0);
            if (result != 0)
            {
                // TODO check EAI_NONAME
                TCP_DEBUG_ERR("gettaddrinfo failed");
                status = kErpcStatus_UnknownName;
            }
        }

        if (status == kErpcStatus_Success)
        {
            // Iterate over result addresses and try to connect. Exit the loop on the first successful
            // connection.
            for (res = res0; res; res = res->ai_next)
            {
                // Create the socket.
                sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
                if (sock < 0)
                {
                    continue;
                }
                // Attempt to connect.
                if (connect(sock, res->ai_addr, res->ai_addrlen) < 0)
                {
                    ::close(sock);
                    sock = -1;
                    continue;
                }
                // Exit the loop for the first successful connection.
                TCP_DEBUG_PRINT("client:    successful connection to ");
                print_net_info(res->ai_addr, res->ai_addrlen);
                break;
            }

            // Free the result list.
            freeaddrinfo(res0);

            // Check if we were able to open a connection.
            if (sock < 0)
            {
                // TODO check EADDRNOTAVAIL:
                TCP_DEBUG_ERR("connecting failed");
                status = kErpcStatus_ConnectionFailure;
            }
        }

        if (status == kErpcStatus_Success)
        {
            set = 1;
            if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *) &set, sizeof(int)) < 0)
            {
                ::close(sock);
                TCP_DEBUG_ERR("setsockopt failed");
                status = kErpcStatus_Fail;
            }
        }

        if (status == kErpcStatus_Success)
        {
            // On some systems (BSD) we can disable SIGPIPE on the socket. For others (Linux), we have to
            // ignore SIGPIPE.
#if defined(SO_NOSIGPIPE)

            // Disable SIGPIPE for this socket. This will cause write() to return an EPIPE statusor if the
            // other side has disappeared instead of our process receiving a SIGPIPE.
            set = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *) &set, sizeof(int)) < 0)
            {
                ::close(sock);
                TCP_DEBUG_ERR("setsockopt failed");
                status = kErpcStatus_Fail;
            }
        }

        if (status == kErpcStatus_Success)
        {
#else
            // globally disable the SIGPIPE signal
            signal(SIGPIPE, SIG_IGN);
#endif // defined(SO_NOSIGPIPE)
            m_socket = sock;
        }
    }

    return status;
}

erpc_status_t TCPTransport::close(bool stopServer)
{
    if (m_isServer && stopServer)
    {
        m_runServer = false;
    }

    if (m_socket != -1)
    {
        ::close(m_socket);
        m_socket = -1;
    }

    return kErpcStatus_Success;
}



void TCPTransport::serverThread(void)
{
    int yes = 1;
    int serverSocket;
    int result;
    struct sockaddr incomingAddress;
    socklen_t incomingAddressLength;
    int incomingSocket;
    bool status = false;
    struct sockaddr_in serverAddress;
    int epfd;
    static struct epoll_event events[EPOLL_SIZE];

    TCP_DEBUG_PRINT("server:    %s", "in server thread\n");

    // Create socket.
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        TCP_DEBUG_ERR("failed to create server socket");
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
        result = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        if (result < 0)
        {
            TCP_DEBUG_ERR("setsockopt failed");
            status = true;
        }
    }

    if (!status)
    {
        // Bind socket to address.
        result = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
        //on Failed
        if (result < 0)
        {
            TCP_DEBUG_ERR("server:    bind failed");
            status = true;
        }
        //on Success
        TCP_DEBUG_PRINT("server:    bind to ");
        print_net_info((struct sockaddr *) &serverAddress, sizeof(serverAddress));
    }

    if (!status)
    {
        // Listen for connections.
        result = listen(serverSocket, 1);
        //on Failed
        if (result < 0)
        {
            TCP_DEBUG_ERR("server:    listen failed");
            status = true;
        }
        //on Success
        TCP_DEBUG_PRINT("server:    %s", "Listening for connections\n");
    }
    if (!status)
    {
        //在内核中创建事件表
        epfd = epoll_create(EPOLL_SIZE);
        if (epfd < 0)
        {
            TCP_DEBUG_ERR("server:    epfd error");
            status = true;
        }
        TCP_DEBUG_PRINT("server:    epoll created, epollfd = %d\n", epfd);
        //往内核事件表里添加事件
        addfd(epfd, serverSocket, true);

    }
    if (!status)
    {

        while (m_runServer)
        {

            int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
            if (epoll_events_count < 0)
            {
                perror("epoll failure");
                break;
            }
            //处理所有已经就绪的事件
            for (int i = 0; i < epoll_events_count; ++i)
            {
                int sockfd = events[i].data.fd;
                //新用户连接
                if (sockfd == serverSocket)
                {
                    incomingAddressLength = sizeof(struct sockaddr);
                    // we should use select() otherwise we can't end the server properly
                    incomingSocket = accept(serverSocket, &incomingAddress, &incomingAddressLength);

                    if (incomingSocket > 0)
                    {
                        // Successfully accepted a connection.
                        TCP_DEBUG_PRINT("server:    accepted connection from ");
                        print_net_info(&incomingAddress, incomingAddressLength);
                        
                        // should be inherited from accept() socket but it's not always ...
                        yes = 1;
                        setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (void *) &yes, sizeof(yes));
                        addfd(epfd, incomingSocket, true);
                        m_socket = incomingSocket;
                        m_server->onNewSocket(incomingSocket, getPortFormAddr(&incomingAddress, incomingAddressLength));
                    }
                    else
                    {
                        TCP_DEBUG_ERR("accept failed");
                    }
                }
                //TODO:处理客户端的进一步输入
                else
                {
                }
            }
        }
    }
    ::close(serverSocket);
}


void TCPTransport::serverThreadStub(void *arg)
{
    TCPTransport *This = reinterpret_cast<TCPTransport *>(arg);

    TCP_DEBUG_PRINT("server:    in serverThreadStub (arg=%p)\n", arg);
    if (This != NULL)
    {
        This->serverThread();
    }
}

erpc_status_t TCPTransport::underlyingReceive(uint8_t *data, uint32_t size)
{
    ssize_t length;
    erpc_status_t status = kErpcStatus_Success;

    // Block until we have a valid connection.
    while (m_socket <= 0)
    {
        // Sleep 10 ms.
        Thread::sleep(10000);
    }

    // Loop until all requested data is received.
    while (size > 0U)
    {
        length = read(m_socket, data, size);

        // Length will be zero if the connection is closed.
        if (length > 0)
        {
            size -= length;
            data += length;
        }
        else
        {
            if (length == 0)
            {
                // close socket, not server
                close(false);
                status = kErpcStatus_ConnectionClosed;
            }
            else
            {
                status = kErpcStatus_ReceiveFailed;
            }
            break;
        }
    }

    return status;
}

erpc_status_t TCPTransport::underlyingSend(const uint8_t *data, uint32_t size)
{
    erpc_status_t status = kErpcStatus_Success;
    ssize_t result;

    if (m_socket <= 0)
    {
        // we should not pretend to have a succesful Send or we create a deadlock
        status = kErpcStatus_ConnectionFailure;
    }
    else
    {
        // Loop until all data is sent.
        while (size > 0U)
        {
            result = write(m_socket, data, size);
            if (result >= 0)
            {
                size -= result;
                data += result;
            }
            else
            {
                if (result == EPIPE)
                {
                    // close socket, not server
                    close(false);
                    status = kErpcStatus_ConnectionClosed;
                }
                else
                {
                    status = kErpcStatus_SendFailed;
                }
                break;
            }
        }
    }

    return status;
}


