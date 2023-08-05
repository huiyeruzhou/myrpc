#include "server/simple_server.hpp"
using namespace erpc;
__attribute__((unused)) static const char *TAG = "server";


SimpleServer::SimpleServer(const char *host, uint16_t port)
    : Server(host, port)
    , m_isServerOn(true)
    , m_serverThread(SimpleServer::networkpollerStub, 1)
    , m_runServer(false) {

}

SimpleServer::~SimpleServer() {
    // Close server.
    stop();
}

rpc_status SimpleServer::run(void) {
    rpc_status err = Success;
    while ((err == Success) && m_isServerOn) {
        // Sleep 10 ms.
        Thread::sleep(10000);
    }
    return err;
}


void SimpleServer::stop(void) {
    m_isServerOn = false;
    close();
}

void SimpleServer::onNewSocket(int sockfd, int port) {
    TCPTransport *transport_worker = new TCPTransport(sockfd, port);
    ServerWorker *worker = new ServerWorker(methods, transport_worker);
    worker->start();
}

rpc_status SimpleServer::close() {
    m_runServer = false;
    if (m_sockfd != -1) {
        ::close(m_sockfd);
        m_sockfd = -1;
    }
    return Success;
}



void SimpleServer::networkpollerThread(void) {
    struct sockaddr incomingAddress;
    int incomingSocket;
    socklen_t incomingAddressLength;
    fd_set readFds;
    FD_ZERO(&readFds);
    FD_SET(m_sockfd, &readFds);
    // FD_SET(pipeline[0], &readFds);

    LOGI(TAG, "%s", "networkpollerThread");
    while (m_runServer && m_sockfd > 0) {
        incomingSocket = accept(m_sockfd, &incomingAddress, &incomingAddressLength);
        if (incomingSocket >= 0) {
            // Successfully accepted a connection.
            char netinfo[24];
            sprint_net_info(netinfo, sizeof(netinfo), &incomingAddress, incomingAddressLength);
            LOGI(TAG, "accepted connection from %s", netinfo);
            // should be inherited from accept() socket but it's not always ...
            int yes = 1;
            setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, (void *) &yes, sizeof(yes));
            // cancel recv timeout
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 0;
            setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(struct timeval));
            onNewSocket(incomingSocket, get_port_from_addr(&incomingAddress, incomingAddressLength));
        }
        else {
            LOGE(TAG, "accept failed,errorno: %d, m_sockfd: %d, error: %s",errno, m_sockfd, strerror(errno));
        }
    }
    if (m_sockfd > 0) ::close(m_sockfd);
}


void SimpleServer::networkpollerStub(void *arg) {
    SimpleServer *This = reinterpret_cast<SimpleServer *>(arg);
    if (This != NULL) {
        This->networkpollerThread();
    }
    if (!This->m_isServerOn) {
        LOGE(TAG, "server is stopped, delete it.");
        delete This;
    }
}

rpc_status SimpleServer::open(void) {
    rpc_status status = rpc_status::Success;
    int result;
    struct sockaddr_in serverAddress;
    // Create socket.
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        LOGE(TAG, "failed to create server socket, error: %s", strerror(errno));
        return rpc_status::IOError;
    }

    // Fill in address struct.
    (void) memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
    serverAddress.sin_port = htons(m_port);

    // Turn on reuse address option.
    int yes = 1;
    result = setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (result < 0) {
        LOGE(TAG, "setsockopt failed, error: %s", strerror(errno));
        ::close(m_sockfd);
        return rpc_status::IOError;
    }
    //set accept timeout
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    result = setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(struct timeval));
    if (result < 0) {
        LOGE(TAG, "setsockopt failed, error: %s", strerror(errno));
        ::close(m_sockfd);
        return rpc_status::IOError;
    }

    // Bind socket to address.
    result = bind(m_sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    //on Failed
    if (result < 0) {
        LOGE(TAG, "bind failed, error: %s", strerror(errno));
        ::close(m_sockfd);
        return rpc_status::ConnectionFailure;
    }
    else {
        //on Success
        char netinfo[24];
        sprint_net_info(netinfo, sizeof(netinfo), (struct sockaddr *) &serverAddress, sizeof(serverAddress));
        LOGI(TAG, "bind to %s", netinfo);
    }



    // Listen for connections.
    result = listen(m_sockfd, 4);
    //on Failed
    if (result < 0) {
        LOGE(TAG, "listen failed, error: %s", strerror(errno));
        ::close(m_sockfd);
        return rpc_status::ConnectionFailure;
    }
    //on Success
    LOGI(TAG, "%s", "Listening for connections\n");
    m_serverThread.setName("Network Poller");
    m_runServer = true;
    m_serverThread.start(this);
    LOGI(TAG, "start running networkpollerThread");
    return status;
}