
#include "syscall.h"
#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_MSG_LENGTH 1024

int main() {

  int socketID;
  int byteRec;
  char* buffer;
  int len = 0;

  socketID = SocketTCP();
  if (socketID < 0) {
    PrintString("Cannot create socket");
    Halt();
  }
  
  if ((Connect(socketID,SERVER_IP,PORT)) < 0) {
    PrintString("Cannot connect to server");
    Halt();
  }
  buffer = "thanh cong roiii";
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
  return 0;
}

