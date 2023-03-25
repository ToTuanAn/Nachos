#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define PORT 8080
#define MAX_CLIENTS 1

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread, client_sockets[MAX_CLIENTS];
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind socket to a port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections
    int num_clients = 0;
    while (num_clients < MAX_CLIENTS) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Add new client socket to the array
        client_sockets[num_clients] = new_socket;
        num_clients++;

        printf("New client connected. Total clients: %d\n", num_clients);
    }

    // Read and echo messages from clients
    while (1) {
        // Check for incoming messages from each client
        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            int client_socket = client_sockets[i];

            // Read message from client
            valread = read(client_socket, buffer, 1024);

            if (valread > 0) {
                printf("Received message from client %d: %s\n", i+1, buffer);

                // Convert message to uppercase
                int j;
                for (j = 0; j < strlen(buffer); j++) {
                    buffer[j] = toupper(buffer[j]);
                }

                // Send message back to client
                send(client_socket, buffer, strlen(buffer), 0);
                printf("Message sent to client %d: %s\n", i+1, buffer);
            }
        }
    }

    // Close all client connections
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        close(client_sockets[i]);
    }

    return 0;
}
