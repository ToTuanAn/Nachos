#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
	int openFileId;
	int fileSize;
	char c; //Ky tu de in ra
	char fileName[10] = "hello.txt";
	int i; //Index for loop
	
	openFileId = Open(fileName, 1); // Goi ham Open de mo file 
	
	if (openFileId != -1) //Kiem tra Open co loi khong
	{
		//Seek den cuoi file de lay duoc do dai noi dung (fileSize)
		fileSize = Seek(-1, openFileId);
		i = 0;
		// Seek den dau tap tin de tien hanh Read
		Seek(0, openFileId);
		
		for (; i < fileSize; i++) // Cho vong lap chay tu 0 - fileSize
		{
			Read(&c, 1, openFileId); // Goi ham Read de doc tung ki tu noi dung file
			PrintChar(c); // Goi ham PrintChar de in tung ki tu ra man hinh
		}
		Close(openFileId); // Goi ham Close de dong file
	}
    
    Halt();
	return 0;
}