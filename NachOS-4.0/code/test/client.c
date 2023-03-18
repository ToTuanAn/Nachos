
#include <string.h> // strlen, memset
#include "syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_MSG_LENGTH 1024

const char message[] = "Hello sockets world\n";

int main() {

  int socketID;
  char buffer[1024];

  socketID = socketTCP();
  if (socketID < 0) {
    perror("Cannot create socket");
    exit(1);
  }
  
  if ((Connect(socketID,SERVER_IP,PORT)) < 0) {
    perror("Cannot connect to server");
    exit(2);
  }
  sprintf(buffer, "message from socket");
  if (Send(socketID, buffer, strlen(buffer)) < 0) {
            perror("send");
            exit(1);
        }
  memset(buffer, 0, sizeof(buffer));
  if (Receive(socketID, buffer, strlen(buffer)) == -1) {
            perror("recv");
            exit(1);
        }
  printf("Received message from socket:", buffer);

  CloseSocket1(socketID);
  return 0;
}