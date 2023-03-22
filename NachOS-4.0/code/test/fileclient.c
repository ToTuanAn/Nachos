#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <syscall.h>

#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_MSG_LENGTH 1024

int main(int argc, char *argv[]) {
    int socketID;
    char file_content[BUFFER_SIZE];
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *input_file = argv[1];
    char *output_file = argv[2];

    // Open the input file
    FILE *input_fp = fopen(input_file, "r");
    if (input_fp == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // Get the file size
    fseek(input_fp, 0L, SEEK_END);
    long file_size = ftell(input_fp);
    rewind(input_fp);

    // Read the file content
    int bytes_read = fread(file_content, 1, BUFFER_SIZE, input_fp);
    if (bytes_read <= 0) {
        perror("Error reading input file");
        exit(EXIT_FAILURE);
    }

  socketID = socketTCP();
  if (socketID < 0) {
    perror("Cannot create socket");
    exit(1);
  }
  
  if ((Connect(socketID,SERVER_IP,PORT)) < 0) {
    perror("Cannot connect to server");
    exit(2);
  }

   

    // Send the file content to the server
    int bytes_sent = Send(socketID, file_content, bytes_read, 0);
    if (bytes_sent != bytes_read) {
        perror("Error sending file content to server");
        exit(EXIT_FAILURE);
    }

    // Receive the response from the server
    char response[BUFFER_SIZE];
    int bytes_received = Receive(socketID, response, BUFFER_SIZE);
    if (bytes_received <= 0) {
        perror("Error receiving response from server");
        exit(EXIT_FAILURE);
    }

    // Close the socket
    CloseSocket1(socketID);

    // Write the response to the output file
    FILE *output_fp = fopen(output_file, "w");
    if (output_fp == NULL) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    fwrite(response, 1, bytes_received, output_fp);

    // Close the output file
    fclose(output_fp);

    return 0;
}
