#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
	int openFileId;
	int fileSize;
	char c; 
	char fileName[10] = "hello.txt";
	int i; //Index for loop
	
	openFileId = Open(fileName, 1); 
	
	if (openFileId != -1) //if file is opened
	{
		// Seek to the end of src file to get the src file size
		fileSize = Seek(-1, openFileId);
		i = 0;
		
		Seek(0, openFileId);
		
		for (; i < fileSize; i++) //read and print loop
		{
			Read(&c, 1, openFileId); //read every character in the file
			PrintChar(c); //print every character in the console
		}
		Close(openFileId); //close the file=
	}
    
    Halt();
	return 0;
}