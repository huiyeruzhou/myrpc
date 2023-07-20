#include "server/simple_server.hpp"
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

SimpleServer::SimpleServer(const char *host, uint16_t port)
    : Server(host, port)
    , m_isServerOn(true)
    , m_serverThread(SimpleServer::networkpollerStub, 1)
    , m_runServer(false)
{

}

SimpleServer::~SimpleServer()
{
    // Close server.
    close();
}

rpc_status SimpleServer::run(void)
{
    rpc_status err = Success;
    while ((err == Success) && m_isServerOn)
    {
        // Sleep 10 ms.
         Thread::sleep(10000);
    }
    return err;
}


void SimpleServer::stop(void)
{
    m_isServerOn = false;
}

void SimpleServer::onNewSocket(int sockfd, int port) {
    TCPTransport *transport_worker = new TCPTransport(sockfd, port);
    ServerWorker *worker = new ServerWorker(methods,  transport_worker);
    worker->start();
}

rpc_status SimpleServer::close()
{
    m_runServer = false;

    if (m_sockfd != -1)
    {
        ::close(m_sockfd);
        m_sockfd = -1;
    }

    return Success;
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
        LOGE(TAG, "failed to create server socket, error: %s", strerror(errno));
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
            LOGE(TAG, "setsockopt failed, error: %s", strerror(errno));
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
            LOGE(TAG, "bind failed, error: %s", strerror(errno));
            status = true;
        }
        else
        {
            //on Success
            char netinfo[24];
            sprint_net_info(netinfo, sizeof(netinfo), (struct sockaddr *) &serverAddress, sizeof(serverAddress));
            LOGI(TAG, "bind to %s", netinfo);
        }

    }

    if (!status)
    {
        // Listen for connections.
        result = listen(m_sockfd, 4);
        //on Failed
        if (result < 0)
        {
            LOGE(TAG, "listen failed, error: %s", strerror(errno));
            status = true;
        }
        //on Success
        LOGI(TAG, "%s", "Listening for connections\n");
    }
    if (!status)
    {

        while (m_runServer)
        {
            incomingSocket = accept(m_sockfd, &incomingAddress, &incomingAddressLength);

            if (incomingSocket >= 0)
            {
                // Successfully accepted a connection.
                char netinfo[24];
                sprint_net_info(netinfo, sizeof(netinfo), &incomingAddress, incomingAddressLength);
                LOGI(TAG, "accepted connection from %s", netinfo);
                

                // should be inherited from accept() socket but it's not always ...
                yes = 1;
                setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, (void *) &yes, sizeof(yes));
                // addfd(epfd, incomingSocket, true);
                onNewSocket(incomingSocket, get_port_from_addr(&incomingAddress, incomingAddressLength));
            }
            else
            {
                LOGE(TAG, "accept failed, error: %s", strerror(errno));
            }
        }
    }
    m_isServerOn = false;
    ::close(m_sockfd);
}


void SimpleServer::networkpollerStub(void *arg)
{
    SimpleServer *This = reinterpret_cast<SimpleServer *>(arg);
    if (This != NULL)
    {
        This->networkpollerThread();
    }
}

rpc_status SimpleServer::open(void)
{
    rpc_status status;
    m_serverThread.setName("Network Poller");
    m_runServer = true;
    m_serverThread.start(this);
    LOGI(TAG, "start running networkpollerThread");
    status = Success;
    return status;
}