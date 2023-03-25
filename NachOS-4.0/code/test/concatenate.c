#include "syscall.h"
#define MAX_LENGTH 32


int main()
{
	//OpenFileId cua file nguon va file dich
	int srcFileId1;
	int srcFileId2;
	int destFileId;
	//Do dai file
	int fileSize1;
	int fileSize2;
	int i; //Index for loop
	char c; //Ky tu de in ra
	char source1[6] = "a.txt";
	char source2[6] = "b.txt";
	char dest[16] = "concatenate.txt";

    srcFileId1 = Open(source1, 1); // Goi ham Open de mo file nguon
	srcFileId2 = Open(source2, 1); // Goi ham Open de mo file nguon
	
	if (srcFileId1 != -1 && srcFileId2 != -1) //Kiem tra mo file thanh cong
	{
		//Tao file moi voi ten la chuoi luu trong "dest"
		destFileId = Create(dest);
		Close(destFileId);
		
		destFileId = Open(dest, 0); // Goi ham Open de mo file dich
		if (destFileId != -1) //Kiem tra mo file thanh cong
		{
			// Seek den cuoi file nguon de lay duoc do dai noi dung file nguon (fileSize)
			fileSize1 = Seek(-1, srcFileId1);
			fileSize2 = Seek(-1, srcFileId2);
			
			//Chuan bi sao chep
			Seek(0, srcFileId1); // Seek den dau file nguon
			Seek(0, srcFileId2);

			Seek(0, destFileId); // Seek den dau file dich
			i = 0;
			
			// Vong lap chay tu dau file nguon den het file nguon
			for (; i < fileSize1; i++) 
			{
				Read(&c, 1, srcFileId1); //Doc tung ki tu cua file nguon
				Write(&c, 1, destFileId); //Ghi vao file dich
			}

			// Vong lap chay tu dau file nguon den het file nguon
			for (; i < fileSize1 + fileSize2; i++) 
			{
				Read(&c, 1, srcFileId2); //Doc tung ki tu cua file nguon
				Write(&c, 1, destFileId); //Ghi vao file dich
			}

			Close(destFileId); // Goi ham Close de dong file dich
		}
		
		Close(srcFileId1); // Goi ham Close de dong file nguon
		Close(srcFileId2); // Goi ham Close de dong file nguon
	}

    Halt();
	return 0;
}
