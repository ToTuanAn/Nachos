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

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	cerr << "Received Exception " << which << " type: " << type << "\n";

    switch (which) {

    	case SyscallException:
		{
			switch(type) 
			{	
				case SC_Halt:
				{
					
					DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
					SysHalt();
					
					break;
				}
				case SC_Create:
				{
					int virtAddr;
					char* filename;
					DEBUG('a', "\n SC_CreateFile call ...");
					DEBUG('a', "\n Reading virtual address of filename");

					virtAddr = kernel->machine->ReadRegister(4); //read file address from reg R4
					DEBUG('a', "\n Reading filename.");
					
					//copy from User to System, maximum (32 + 1) bytes
					filename = User2System(virtAddr, MaxFileLength + 1);
					if (strlen(filename) == 0)
					{
						DEBUG('a', "\n File name is not valid");
						kernel->machine->WriteRegister(2, -1); //return -1 to reg R2
						break;
					}
					
					if (filename == NULL)  //cannot read
					{
						DEBUG('a', "\n Not enough memory in system");
						kernel->machine->WriteRegister(2, -1); //return -1 vao thanh ghi R2
						delete filename;
						break;
					}
					DEBUG('a', "\n Finish reading filename.");
					
					if (!kernel->fileSystem->Create(filename)) //trigger Create of fileSystem, return -1 if fail
					{
						kernel->machine->WriteRegister(2, -1);
						delete filename;

						break;
					}
					
					//create success
					//printf("\n Error create file '%s'", 1);
					kernel->machine->WriteRegister(2, 0);
					delete filename;

					break;
				}
				case SC_Add:
				{
					DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
				
					/* Process SysAdd Systemcall*/
					int result;
					result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
							/* int op2 */(int)kernel->machine->ReadRegister(5));

					DEBUG(dbgSys, "Add returning with " << result << "\n");
					/* Prepare Result */
					kernel->machine->WriteRegister(2, (int)result);
					
					IncreasePC();

					break;
				}
				case SC_Open:
				{
					
					int virtAddr = kernel->machine->ReadRegister(4); //read file address from reg R4
					int type = kernel->machine->ReadRegister(5); //read type from reg R5
					char* filename;
					filename = User2System(virtAddr, MaxFileLength); // 
					
					int freeSlot = kernel->fileSystem->FindFreeSlot();
					

					if (freeSlot != -1) //if free slot
					{
						if (type == 0 || type == 1) //if type == 0 or type == 1
						{
							if ((kernel->fileSystem->openf[freeSlot] = kernel->fileSystem->Open(filename, type)) != NULL) //open success
							{	
								kernel->machine->WriteRegister(2, freeSlot); //return OpenFileID
							}
						}
						delete[] filename;

						break;
					}
					
					kernel->machine->WriteRegister(2, -1); //cannot open file
					
					
					delete[] filename;

					break;
				}
				case SC_Close:
				{
					int fid = kernel->machine->ReadRegister(4); // Lay id cua file tu thanh ghi so 4
					if (fid >= 0 && fid <= 19) //Chi xu li khi fid nam trong [0, 14]
					{
						if (kernel->fileSystem->openf[fid]) //neu mo file thanh cong
						{
							delete kernel->fileSystem->openf[fid]; //Xoa vung nho luu tru file
							kernel->fileSystem->openf[fid] = NULL; //Gan vung nho NULL
							kernel->machine->WriteRegister(2, 0);
							break;
						}
					}
					kernel->machine->WriteRegister(2, -1);
					break;
				}
				
				default:
				{
					cerr << "Unexpected system call " << type << "\n";
					break;
				}
			
			}
			/* Modify return point */
			IncreasePC();
			break;
		}
		default:
		{
			cerr << "Unexpected user mode exception" << (int)which << "\n";
			break;
		}
    }
}
