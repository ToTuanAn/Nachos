
// // #include <string.h> // strlen, memset
// // #include <stdio.h>
// // #include <stdlib.h>
// // #include <string.h>
// // #include "syscall.h"

// #ifdef UNIX
// #include <string.h> // strlen, memset
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "syscall.h"
// #define Exit exit
// #else
// #include "syscall.h"
// #endif /* UNIX */

// #define SERVER_IP "127.0.0.1"
// #define PORT 8080
// #define MAX_MSG_LENGTH 1024

// const char message[] = "Hello sockets hehhehe\n";

// int main() {

//   int socketID;
//   char buffer[1024];

//   socketID = socketTCP();
//   if (socketID < 0) {
//     perror("Cannot create socket");
//     exit(1);
//   }
  
//   if ((Connect(socketID,SERVER_IP,PORT)) < 0) {
//     perror("Cannot connect to server");
//     exit(2);
//   }
//   sprintf(buffer, "message from socket");
//   if (Send(socketID, buffer, strlen(buffer)) < 0) {
//             perror("send");
//             exit(1);
//         }
//   memset(buffer, 0, sizeof(buffer));
//   if (Receive(socketID, buffer, strlen(buffer)) == -1) {
//             perror("recv");
//             exit(1);
//         }
//   printf("Received message from socket:", buffer);

//   CloseSocket1(socketID);
//   return 0;
// }

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

#include "syscall.h"

int
main()
{
    int sockid = SocketTCP();

    /* not reached */
}
