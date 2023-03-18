#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include "syscall.h"

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_MSG_LENGTH 1024

int main()
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_MSG_LENGTH] = {0};

    // Create a TCP socket
    server_fd = SocketTCP();
    if (server_fd == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Read the message from the client and convert it to uppercase
    valread = read(new_socket, buffer, MAX_MSG_LENGTH);
    for (int i = 0; i < valread; i++)
    {
        buffer[i] = toupper(buffer[i]);
    }

    // Send the message back to the client
    send(new_socket, buffer, strlen(buffer), 0);
    printf("Message echoed to client: %s\n", buffer);

    // Close the socket
    close(server_fd);
    return 0;
}