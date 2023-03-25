#include "syscall.h"
#define MAX_LENGTH 32


int main()
{
	//OpenFileId of src file and dest file 
	int srcFileId;
	int destFileId;
	//fileSize of src
	int fileSize;
	int i; //index for loop
	char c; 
	char source[6] = "a.txt";
	char dest[6] = "b.txt";

    srcFileId = Open(source, 1); 
	
	if (srcFileId != -1) //If file is opened
	{
		//create dest file
		destFileId = Create(dest);
		Close(destFileId);
		
		destFileId = Open(dest, 0); //open dest file
		if (destFileId != -1) //If file is opened
		{
			// Seek to the end of src file to get the src file size
			fileSize = Seek(-1, srcFileId);
			
			Seek(0, srcFileId); 
			Seek(0, destFileId); 
			i = 0;
			
			// read and write loop
			for (; i < fileSize; i++) 
			{
				Read(&c, 1, srcFileId); //read char in src file
				Write(&c, 1, destFileId); //write char to dest file
			}
			
			Close(destFileId); //close dest file
		}
		
		Close(srcFileId); //close open file
	}

    Halt();
	return 0;
}
