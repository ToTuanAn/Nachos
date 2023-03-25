
#include "syscall.h"


#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_MSG_LENGTH 1024

//const char message[] = "Hello sockets hehhehe\n";

int main() {

  int socketID;
  int byteRec;
  char* buffer;
  char* buffer2;
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
  //memset(buffer, 0, sizeof(buffer));
  buffer2[len];
  byteRec = Receive(socketID,buffer,len);
  if (byteRec == -1) {
            PrintString("cannot recv");
            Halt();
        }
  PrintString("Received message from socket:");
  PrintString(buffer);

  CloseSocket1(socketID);
  return 0;
}

/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

// #include "syscall.h"

// int
// main()
// {
//     int sockid = SocketTCP();

//     /* not reached */
// }
