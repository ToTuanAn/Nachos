#include "syscall.h"
#define MAX_LENGTH 32


int main()
{
	//OpenFileId of src1 file, src2 file and dest file 
	int srcFileId1;
	int srcFileId2;
	int destFileId;
	//fileSize of src1, src2
	int fileSize1;
	int fileSize2;
	int i; //Index for loop
	char c; 
	char source1[6] = "a.txt";
	char source2[6] = "b.txt";
	char dest[16] = "concatenate.txt";

    srcFileId1 = Open(source1, 1); 
	srcFileId2 = Open(source2, 1); 
	
	if (srcFileId1 != -1 && srcFileId2 != -1) //If file src1 and file src2 is opened
	{
		//create dest file
		destFileId = Create(dest);
		Close(destFileId);
		
		destFileId = Open(dest, 0); //open dest file
		if (destFileId != -1) //If file is opened
		{
			// Seek to the end of src file to get the src1, src2 file size
			fileSize1 = Seek(-1, srcFileId1);
			fileSize2 = Seek(-1, srcFileId2);
			
			Seek(0, srcFileId1); 
			Seek(0, srcFileId2);

			Seek(0, destFileId); 
			i = 0;
			
			// read and write loop
			for (; i < fileSize1; i++) 
			{
				Read(&c, 1, srcFileId1); //read every char in src1 file
				Write(&c, 1, destFileId); //write every char in dest file
			}

			// read and write loop
			for (; i < fileSize1 + fileSize2; i++) 
			{
				Read(&c, 1, srcFileId2); //read every char in src2 file
				Write(&c, 1, destFileId); //write every char in dest file
			}

			Close(destFileId); //close dest file
		}
		
		Close(srcFileId1); //close the src1 file
		Close(srcFileId2); //close the src2 file
	}

	return 0;
}
