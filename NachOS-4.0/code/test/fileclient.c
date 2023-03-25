
#include "syscall.h"
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_MSG_LENGTH 1024

int main() {
    int socketID;
    int byteRec;
    char buffer[MAX_MSG_LENGTH];
    int len = 0;

    int srcFileId;
	int destFileId;
	//fileSize of src
	int fileSize;
	int i; //index for loop
	char c; 
    char source[6] = "a.txt";
	char dest[6] = "b.txt";


    // Open the input file
    srcFileId = Open(source, 1); 
    if (srcFileId != -1) //If file is opened
    {
		fileSize = Seek(-1, srcFileId);
			
        Seek(0, srcFileId); 
        i = 0;
        
        // read and write loop
        for (; i < fileSize; i++) 
        {
            Read(&c, 1, srcFileId); //read char in src file
            buffer[i] = c;
        }
		
        buffer[fileSize] = '\0';

		Close(srcFileId); //close open file
	}

    PrintString(buffer);

        
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
    destFileId = Open(dest, 1); 
    if (destFileId != -1) //If file is opened
    {
        Seek(0, destFileId); 
        i = 0;
        
        // read and write loop
        for (; i < fileSize; i++) 
        {
            Write(&buffer[i], 1, destFileId);
        }
		
		Close(destFileId); //close open file
	}

    
    // Close the output file
    
    return 0;
}
