#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define NUM_REQUESTS 10

int main() {
    // Connect to the server on localhost:8080
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        perror("socket failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(8080);
    if (connect(client_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed");
        return EXIT_FAILURE;
    }

    // Send multiple requests simultaneously
    for (int i = 0; i < NUM_REQUESTS; i++)
    {
        const char *request = "GET / HTTP/1.1\n";
        int bytes_sent = send(client_fd, request, strlen(request), 0);
        if (bytes_sent < 0)
        {
            perror("send failed");
            return EXIT_FAILURE;
        }
    }

    // Receive the responses from the server
    char response[1024];
    for (int i = 0; i < NUM_REQUESTS; i++)
    {
        int bytes_received = recv(client_fd, response, sizeof(response), 0);
        if (bytes_received < 0)
        {
            perror("recv failed");
            return EXIT_FAILURE;
        }

        // Print the response
        printf("%s", response);
    }

    // Close the client socket
    close(client_fd);

    return 0;
}
