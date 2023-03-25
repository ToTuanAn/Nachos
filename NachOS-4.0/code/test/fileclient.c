
#include "syscall.h"
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_MSG_LENGTH 1024

int main(int argc, char *argv[]) {
    int socketID;
    int byteRec;
    char* buffer;
    int len = 0;
    if (argc != 3) {
        PrintString("Incorrect syntax, try again");
        Halt();
    }

    char *input_file = argv[1];
    char *output_file = argv[2];

    // Open the input file

    // Get the file size

    // Read the file content
        
    socketID = SocketTCP();
    if (socketID < 0) {
        PrintString("Cannot create socket");
        Halt();
    }
  
    if((Connect(socketID,SERVER_IP,PORT)) < 0) {
        PrintString("Cannot connect to server");
        Halt();
    }
    while (buffer[len] != '\0') ++len;
    if (Send(socketID, buffer, len) < 0) {
            PrintString("cannot send");
            Halt();
        }
    byteRec = Receive(socketID,buffer,len);
    if (byteRec == -1) {
            PrintString("cannot recv");
            Halt();
        }
    PrintString("Received message from socket: ");
    PrintString(buffer);
    PrintString("\n");
    CloseSocket1(socketID);


    // Write the response to the output file
    
    // Close the output file
    
    return 0;
}
