// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "filesys.h"
#include"synchconsole.h"


#define MaxFileLength 32 

void IncreasePC()
{
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
}

char* User2System(int virtAddr,int limit)
{
	int i;// index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit +1];//need for terminal string

	if (kernelBuf == NULL)
		return kernelBuf;

	memset(kernelBuf,0,limit+1);

	for (i = 0 ; i < limit ;i++)
	{
		kernel->machine->ReadMem(virtAddr+i,1,&oneChar);
		kernelBuf[i] = (char)oneChar;

		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

int System2User(int virtAddr,int len,char* buffer)
{
	if (len < 0) return -1;
	if (len == 0) return len;

	int i = 0;
	int oneChar = 0 ;

	do{
		oneChar= (int) buffer[i];
		kernel->machine->WriteMem(virtAddr+i,1,oneChar);
		i ++;
	}while(i < len && oneChar != 0);

	return i;
}
   
void SC_CreateFile_func() {
    int virtAddr;
	char* filename;

	virtAddr = kernel->machine->ReadRegister(4); //read file address from reg R4
	
	//copy from User to System, maximum (32 + 1) bytes
	filename = User2System(virtAddr, MaxFileLength);

	if(SysCreate(filename))
	{
		kernel->machine->WriteRegister(2, 0);
	}else
	{
		kernel->machine->WriteRegister(2, -1);
	}
	
	delete filename;
    IncreasePC();

	return;
}

void SC_Add_func() {
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	
	IncreasePC();
	return;
}

void SC_Open_func() {
	int virtAddr = kernel->machine->ReadRegister(4); //read file address from reg R4
	int type = kernel->machine->ReadRegister(5); //read type from reg R5
	char* filename;
	filename = User2System(virtAddr, MaxFileLength); // 
	
	int freeSlot = SysOpen(filename, type);

	if(freeSlot != -1){
		kernel->machine->WriteRegister(2, freeSlot);
		cerr << freeSlot << "\n";
	}else{
		kernel->machine->WriteRegister(2, -1); //cannot open file
	}

	delete[] filename;
	IncreasePC();
	return;
}

void SC_Close_func() {
	int fid = kernel->machine->ReadRegister(4); // get file id from reg 4

	if (fid >= 0 && fid <= 19) //in range [0, 19]
	{
		if (kernel->fileSystem->openf[fid]) //open file success
		{
			delete kernel->fileSystem->openf[fid]; //delete memory open file
			kernel->fileSystem->openf[fid] = NULL; //attach NULL
			kernel->machine->WriteRegister(2, 0);
		}
	}
	kernel->machine->WriteRegister(2, -1);
	IncreasePC();
	return;
}

void SC_Read_func() {
	int virtAddr = kernel->machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
	int charcount = kernel->machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
	int id = kernel->machine->ReadRegister(6); // Lay id cua file tu thanh ghi so 6 
	
	int OldPos;
	int NewPos;
	char *buf;

	if (id < 0 || id > 19) //out of range 0-20 file descriptors
	{
		kernel->machine->WriteRegister(2, -1);
		IncreasePC();
		return;
	}

	if (kernel->fileSystem->openf[id] == NULL) //file not exist
	{
		kernel->machine->WriteRegister(2, -1);
		IncreasePC();
		return;
	}

	if (id == 1) //file stdout cannot read
	{
		kernel->machine->WriteRegister(2, -1);
		IncreasePC();
		return;
	}

	OldPos = kernel->fileSystem->openf[id]->GetCurrentPos(); //get OldPos 

	buf = User2System(virtAddr, charcount); 

	if (id == 0)
	{
		int size = SysRead(buf, charcount);
		System2User(virtAddr, size, buf);
		kernel->machine->WriteRegister(2, size); // return number of byte read
		delete buf;
		IncreasePC();
		return;
	}

	if ((kernel->fileSystem->openf[id]->Read(buf, charcount)) > 0)
	{
		NewPos = kernel->fileSystem->openf[id]->GetCurrentPos();
 
		System2User(virtAddr, NewPos - OldPos, buf); 
		kernel->machine->WriteRegister(2, NewPos - OldPos);
	}
	else
	{
		kernel->machine->WriteRegister(2, -2);
	}
	delete buf;
	IncreasePC();

	return;
}

void SC_Write_func(){
	int virtAddr = kernel->machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
	int charcount = kernel->machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
	int id = kernel->machine->ReadRegister(6); // Lay id cua file tu thanh ghi so 6

	int OldPos;
	int NewPos;
	char *buf;

	// out of range
	if (id < 0 || id > 19)
	{
		kernel->machine->WriteRegister(2, -1);
		IncreasePC();
		return;
	}

	// file not exist
	if (kernel->fileSystem->openf[id] == NULL)
	{
		kernel->machine->WriteRegister(2, -1);
		IncreasePC();
		return;
	}

	// cannot write stdin file
	if (id == 0)
	{
		kernel->machine->WriteRegister(2, -1);
		IncreasePC();
		return;
	}

	// cannot write read only file
	if (kernel->fileSystem->openf[id]->type == 1)
	{
		kernel->machine->WriteRegister(2, -1);
		IncreasePC();
		return;
	}

	OldPos = kernel->fileSystem->openf[id]->GetCurrentPos(); 
	buf = User2System(virtAddr, charcount);  
	
	if (kernel->fileSystem->openf[id]->type == 0)
	{
		if ((kernel->fileSystem->openf[id]->Write(buf, charcount)) > 0)
		{
			NewPos = kernel->fileSystem->openf[id]->GetCurrentPos();
			kernel->machine->WriteRegister(2, NewPos - OldPos);
			delete buf;
			IncreasePC();
			return;
		}
	}

	if (id == 1) // Xet truong hop con lai ghi file stdout (type quy uoc la 3)
	{
		int size = SysWrite(buf, charcount);
		kernel->machine->WriteRegister(2, size); // Tra ve so byte thuc su write duoc
		delete buf;
		IncreasePC();
		return;
	}

}

void SC_Seek_func(){
	int pos = kernel->machine->ReadRegister(4); // cursor pos from reg 4
	int id = kernel->machine->ReadRegister(5); // id from reg 5

	if (id < 0 || id > 19)
	{
		kernel->machine->WriteRegister(2, -1);
		IncreasePC();
		return;
	}

	// file not exist
	if (kernel->fileSystem->openf[id] == NULL)
	{
		kernel->machine->WriteRegister(2, -1);
		IncreasePC();
		return;
	}

	if (id == 0 || id == 1)
	{
		kernel->machine->WriteRegister(2, -1);
		IncreasePC();
		return;
	}

	pos = (pos == -1) ? kernel->fileSystem->openf[id]->Length() : pos;

	if (pos > kernel->fileSystem->openf[id]->Length() || pos < 0)
	{
		kernel->machine->WriteRegister(2, -1);
	}
	else
	{
		kernel->fileSystem->openf[id]->Seek(pos);
		kernel->machine->WriteRegister(2, pos);
	}
	IncreasePC();

	return;
}

void SC_Remove_func(){
	int virtAddr;
	char* filename;

	virtAddr = kernel->machine->ReadRegister(4); //read file address from reg R4
	//copy from User to System, maximum (32 + 1) bytes
	filename = User2System(virtAddr, MaxFileLength);

	if(SysRemove(filename))
	{
		kernel->machine->WriteRegister(2, 0);
	}else
	{
		kernel->machine->WriteRegister(2, -1);
	}
	
	delete filename;
    IncreasePC();

	return;
}

void SC_SocketTCP_func(){

	int sockfd = kernel->fileSystem->initSocketTCP(); // index
	if(sockfd != -1){
		kernel->machine->WriteRegister(2, sockfd);
		cerr << "create socket: "<<sockfd << "\n";
	}else{
		cerr<< "Cannot open socket" << "\n";
		kernel->machine->WriteRegister(2, -1); //cannot open file
	}

	IncreasePC();
	return;
}

void SC_Connect_func(){
	int sockID = kernel->machine->ReadRegister(4); //read socket id from reg R4
	int virIP = kernel->machine->ReadRegister(5); //read type from reg R5
	int port = kernel->machine->ReadRegister(6);
	char* ip;
	int status;
	ip = User2System(virIP, MaxFileLength); //
	status = kernel->fileSystem->connectSocketTCP(sockID,ip,port);
	if(status == -1){
		kernel->machine->WriteRegister(2,-1);
	} else {
		kernel->machine->WriteRegister(2,status);
	}
	
	IncreasePC();
	return;
}

void SC_Send_func(){
	//load parameter
	int bytes_sent;
	int sockID = kernel->machine->ReadRegister(4); 
	int virAdd = kernel->machine->ReadRegister(5); 
	int len = kernel->machine->ReadRegister(6);
	char* buffer;
	buffer = User2System(virAdd, MaxFileLength); 
	bytes_sent = kernel->fileSystem->sendTCP(sockID,buffer,len);
	if (bytes_sent < 0) {
       kernel->machine->WriteRegister(2,-1); // error in sending data
    } else if (bytes_sent == 0) {
        kernel->machine->WriteRegister(2,0);; // connection closed
    }
    kernel->machine->WriteRegister(2,bytes_sent);// success

	IncreasePC();
	return;
}

void SC_Receive_func(){
	int bytes_recv;
	int sockID = kernel->machine->ReadRegister(4); 
	int virAdd = kernel->machine->ReadRegister(5); 
	int len = kernel->machine->ReadRegister(6);
	char* buffer;
	buffer = User2System(virAdd, MaxFileLength); 
	//
	bytes_recv = kernel->fileSystem->recvTCP(sockID,buffer,len);
	if (bytes_recv < 0) {
         kernel->machine->WriteRegister(2,-1); // error in receiving data
    } else if (bytes_recv == 0) {
         kernel->machine->WriteRegister(2,0); // connection closed
    }
	System2User(virAdd,len, buffer);
    kernel->machine->WriteRegister(2,bytes_recv); // success
	IncreasePC();
	return;
}

void SC_CloseSocket1_func(){
	int socketID = kernel->machine->ReadRegister(4);
	int status = kernel->fileSystem->closeSocketTCP(socketID);
	if(status < -1){
		kernel->machine->WriteRegister(2,-1);\
	} else {
		kernel->machine->WriteRegister(2,0);
	}
	IncreasePC();
	return;
}

void SC_PrintString_func(){
	 int memPtr = kernel->machine->ReadRegister(4);  // read address of C-string
    char* buffer = User2System(memPtr,MaxFileLength);

    SysWrite(buffer, strlen(buffer));
    delete[] buffer;
    IncreasePC();
	return;
}

void SC_Exit_func(){
	int id = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysExit(id));
    IncreasePC();
	return;
}


//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    //DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	//cerr << "Received Exception " << which << " type: " << type << "\n";

    switch (which) {
		  case NoException:  // return control to kernel
            kernel->interrupt->setStatus(SystemMode);
            DEBUG(dbgSys, "Switch to system mode\n");
            break;
          case PageFaultException:
		  	break;
          case ReadOnlyException:
		  	break;
          case BusErrorException:
		  	break;
          case AddressErrorException:
		  	break;
          case OverflowException:
		  	break;
          case IllegalInstrException:
		  	break;
          case NumExceptionTypes:
            cerr << "Error " << which << " occurs\n";
            SysHalt();
            ASSERTNOTREACHED();
			break;

    	case SyscallException:
		{
			switch(type) 
			{	
				case SC_Halt:
				{
					SysHalt();
					break;
				}
				case SC_Exit:{
					SC_Exit_func();
					break;
				}
				case SC_Create:
				{
					SC_CreateFile_func();
					break;
				}
				case SC_Add:
				{
					
					SC_Add_func();
					break;
				}
				case SC_Open:
				{
					SC_Open_func();
					break;
				}
				case SC_Close:
				{
					SC_Close_func();
					break;
				}
				case SC_Read:
				{
					SC_Read_func();
					break;
				}
				case SC_Write:
				{
					SC_Write_func();
					break;
				}
				case SC_Seek:
				{
					SC_Seek_func();
					break;
				}
				case SC_Remove:
				{
					SC_Remove_func();
					break;
				}
				case SC_SocketTCP : {
					SC_SocketTCP_func();
					break;
				}
				case SC_Connect :{
					SC_Connect_func();
					break;
				}
				case SC_Send :{
					SC_Send_func();					
					break;
				}
				case SC_Receive:{
					SC_Receive_func();
					break;
				}
				case SC_CloseSocket1:{
					SC_CloseSocket1_func();
					break;
				}
				case SC_PrintString:
				{	
					SC_PrintString_func();
					break;
				
				}
				default:
				{
					cerr << "Unexpected system call " << type << "\n";
					break;
				}
			}
			break;
		}
		default:
		{
			cerr << "Unexpected user mode exception " << (int)which << "\n";
			break;
		}
	}
}
