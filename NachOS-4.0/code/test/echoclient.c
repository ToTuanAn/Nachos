
#include "syscall.h"
#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_MSG_LENGTH 1024

int main() {
    int byteRec;
    char* buffer;
    int sockets[4];
    char *messages[] = {"aaaaa", "bbbbb", "ccccc", "ddddd"};
    int len = 0;
    int i;
    int sockid;


    for(i = 0 ; i < 4 ; i++){
        sockets[i] = SocketTCP();
        if (sockets[i] < 0) {
            PrintString("Cannot create socket");
            Halt();
        }
    }

    for(i = 0 ; i < 4 ; i++){
        if ((Connect(sockets[i],SERVER_IP,PORT)) < 0) {
        PrintString("Cannot connect to server");
        Halt();
        }
    }
         
    for(i = 0 ;  i < 4 ; i++){
        sockid = sockets[i];
        buffer = messages[i];
        while(buffer[len] != '\0'){ ++len;}

        if (Send(sockid, buffer, len) < 0) {
            PrintString("cannot send");
            Halt();
        }

        byteRec = Receive(sockid,buffer,len);
        PrintString(byteRec);
        if (byteRec == -1) {
        PrintString("cannot recv");
        Halt();
    }
        PrintString("Received message from socket: ");
        PrintString(buffer);
        PrintString("\n");
        CloseSocket1(sockid);
    }
    return 0;
}

