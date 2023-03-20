// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"

#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available

struct OpenFileSocket {
	int socketId;
	OpenFileSocket(int socketId){
		this->socketId = socketId;
	}
	~OpenFileSocket(){
		CloseSocket(this->socketId);
	}
};

class FileSystem {
  public:
  	OpenFile** openf; //check file is opened
	OpenFileSocket** socketDT;
	int index;

	FileSystem(bool format) {
		openf = new OpenFile*[20];
		socketDT = new OpenFileSocket*[20];
		index = 0;
		for (int i = 0; i < 20; ++i)
		{
			openf[i] = NULL;
			socketDT[i] = NULL;
		}   
		this->Create("stdin");
		this->Create("stdout");
		openf[index++] = this->Open("stdin", 1);
		openf[index++] = this->Open("stdout", 1);  
	}

    bool Create(char *name) {
		int fileDescriptor = OpenForWrite(name);

		if (fileDescriptor == -1) return FALSE;
		Close(fileDescriptor); 
		return TRUE; 
	}

    OpenFile* Open(char *name) {
	  int fileDescriptor = OpenForReadWrite(name, FALSE);

	  if (fileDescriptor == -1) return NULL;
	  return new OpenFile(fileDescriptor);
      }

	OpenFile* Open(char *name, int type) {
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1) return NULL;
		//index++;
		return new OpenFile(fileDescriptor, type);
	}

	int FindFreeSlot()
	{
		for(int i = 2; i < 20; i++)
		{
			if(openf[i] == NULL) return i;		
		}
		return -1;
	}

    bool Remove(char *name) { return Unlink(name) == 0; }

	int findFreeSlotSocket(){
		for(int i = 3 ; i < 20 ; i++){
			if(socketDT[i] == NULL){
				return i;
			}
		}

		return -1;
	}

	int initSocketTCP(){
		int index = findFreeSlotSocket();
		if(index == -1){
			return -1;
		}
		int socketId = OpenSocketTCP();
		if(socketId <= -1){
			return -1;
		}
		socketDT[index] = new OpenFileSocket(socketId);
		return index;
	} // return index of the socket

	int connectSocketTCP(int socketId, char* ip, int port){
		if(socketId < 0 || socketId >= 20 || socketDT[socketId] == NULL){
			return -1;
		}
		return ConnectSocket(socketDT[socketId]->socketId,ip,port);
		
	}
	int sendTCP(int socketid, char *buffer, int len){
		if (socketid < 3 || socketid >= 20 || socketDT[socketid] == NULL) {
        	return -1; // invalid socketid
		int sockfd = socketDT[socketid]->socketId;
		return SendToSocketTCP(sockfd,buffer,len);
    }
	}
	int recvTCP(int socketid,char* buffer, int len){
		if (socketid < 3 || socketid >= 20 || socketDT[socketid] == NULL) 
        	return -1; // invalid socketid
		int sockfd = socketDT[socketid]->socketId;
		return RecvFromSocketTCP(sockfd,buffer,len);

	}

	int closeSocketTCP(int socketId){
		if(socketId < 0 || socketId >= 20 || socketDT[socketId] == NULL){
			return -1;
		}
		delete socketDT[socketId];
		socketDT[socketId] = NULL;
		return 0;
	}

};

#else // FILESYS
struct OpenFileSocket {
	int socketId;
	OpenFileSocket(int socketId){
		this->socketId = socketId;
	}
	~OpenFileSocket(){
		CloseSocket(this->socketId);
	}
};
class FileSystem {
  public:
  	OpenFile** openf; //check if file is opened
	OpenFileSocket** socketDT;
	
	int index;

    FileSystem(bool format);		// Initialize the file system.
					// Must be called *after* "synchDisk" 
					// has been initialized.
    					// If "format", there is nothing on
					// the disk, so initialize the directory
    					// and the bitmap of free blocks.

    bool Create(char *name, int initialSize);  	
					// Create a file (UNIX creat)

    OpenFile* Open(char *name); 	// Open a file (UNIX open)

	OpenFile* Open(char *name, int type); 	// Open a file (UNIX open)

    bool Remove(char *name);  		// Delete a file (UNIX unlink)

    void List();			// List all the files in the file system

    void Print();			// List all the files and their contents

	int FindFreeSlot();
	int closeSocketTCP(int socketId);
	int connectSocketTCP(int socketId, char* ip, int port);
	int initSocketTCP();
	int recvTCP(int socketid,char* buffer, int len);
	int findFreeSlotSocket();
	int sendTCP(int socketid, char *buffer, int len);

	//destructor file system
	~FileSystem()
	{
		for (int i = 0; i < 20; ++i)
		{
			if (openf[i] != NULL) delete openf[i];
		}
		delete[] openf;
	}

  private:
   OpenFile* freeMapFile;		// Bit map of free disk blocks,
					// represented as a file
   OpenFile* directoryFile;		// "Root" directory -- list of 
					// file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
