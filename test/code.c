#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/epoll.h>

#define MAX_THREADS 10
#define MAX_EVENTS 10

// Function to handle incoming connections and requests
void *handle_connection(void *arg) {
    int client_fd = *((int *) arg);

    // Read request from client
    char request[1024];
    int bytes_received = recv(client_fd, request, sizeof(request), 0);
    if (bytes_received < 0)
    {
        perror("recv failed");
        return NULL;
    }

    // Send response to client
    const char *response = "HTTP/1.1 200 OK\n\nHello, World!";
    int bytes_sent = send(client_fd, response, strlen(response), 0);
    if (bytes_sent < 0)
    {
        perror("send failed");
        return NULL;
    }

    // Close the client socket
    close(client_fd);

    return NULL;
}

int main(int argc, char **argv) {
    // Create the server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket failed");
        return EXIT_FAILURE;
    }

    // Bind the server socket to a local address and port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8080);
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        return EXIT_FAILURE;
    }

    // Listen for incoming connections on the server socket
    if (listen(server_fd, SOMAXCONN) < 0)
    {
        perror("listen failed");
        return EXIT_FAILURE;
    }

    // Create an epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        perror("epoll_create1 failed");
        return EXIT_FAILURE;
    }

    // Add the server socket to the epoll instance
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) < 0)
    {
        perror("epoll_ctl failed");
        return EXIT_FAILURE;
    }
    // Create an array of threads to use for handling connections
    pthread_t threads[MAX_THREADS];
    int thread_index = 0;

    // Create an array of events to monitor with epoll
    struct epoll_event events[MAX_EVENTS];

    // Run the server indefinitely
    while (1)
    {
        // Wait for events on the monitored sockets
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events < 0)
        {
            perror("epoll_wait failed");
            return EXIT_FAILURE;
        }

        // Loop through the triggered events
        for (int i = 0; i < num_events; i++)
        {
            // If the event is on the server socket, it means a new connection is
            // incoming and we need to accept it
            if (events[i].data.fd == server_fd)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
                if (client_fd < 0)
                {
                    perror("accept failed");
                    continue;
                }

                // Add the new client socket to the epoll instance
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0)
                {
                    perror("epoll_ctl failed");
                    continue;
                }
            }
            // If the event is on a client socket, it means there is data to be read
            // and we need to handle the request
            else
            {
                // Create a new thread to handle the connection
                int *arg = malloc(sizeof(*arg));
                *arg = events[i].data.fd;
                if (pthread_create(&threads[thread_index], NULL, handle_connection, arg) != 0)
                {
                    perror("pthread_create failed");
                    continue;
                }

                // Increment the thread index and wrap around if necessary
                thread_index++;
                if (thread_index >= MAX_THREADS)
                {
                    thread_index = 0;
                }
            }
        }
    }

    // Close the epoll instance
    close(epoll_fd);

    // Close the server socket
    close(server_fd);

    return EXIT_SUCCESS;
}
