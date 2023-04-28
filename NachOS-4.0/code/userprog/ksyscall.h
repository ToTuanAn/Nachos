/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "synchconsole.h"


void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}


int SysCreate(char* filename){
  if (strlen(filename) == 0)
	{
		return FALSE;
	}
	
	if (filename == NULL)  //cannot read
	{
		return FALSE;
	}
		
	if (!kernel->fileSystem->Create(filename)) //trigger Create of fileSystem, return -1 if fail
	{
    return FALSE;
	}

  return TRUE;
}

int SysRemove(char* filename){
  if (strlen(filename) == 0)
	{
		return FALSE;
	}
	
	if (filename == NULL)  //cannot read
	{
		return FALSE;
	}

  if (!kernel->fileSystem->Remove(filename)) //trigger Remove of fileSystem, return -1 if fail
	{
    return FALSE;
	}

  return TRUE;

}

int SysOpen(char* filename, int type){
  int freeSlot = kernel->fileSystem->FindFreeSlot();

  if (freeSlot != -1) //if free slot
	{
		if (type == 0 || type == 1) //if type == 0 or type == 1
		{
			if ((kernel->fileSystem->openf[freeSlot] = kernel->fileSystem->Open(filename, type)) != NULL) //open success
			{	
				return freeSlot;
			}
		}
	}

  return -1;
}

int SysRead(char* buffer, int charCount) {
  return kernel->synchConsoleIn->GetString(buffer, charCount);
}


int SysWrite(char* buffer, int charCount){
  return kernel->synchConsoleOut->PutString(buffer, charCount);
}

void SysPrintChar(char character) {
    kernel->synchConsoleOut->PutChar(character);
}

int SysJoin(int id) { return kernel->pTab->JoinUpdate(id); }

int SysExit(int id) { return kernel->pTab->ExitUpdate(id); }

int SysCreateSemaphore(char* name, int initialValue) {
    int res = kernel->semTab->Create(name, initialValue);

    if (res == -1) {
        DEBUG('a', "\nError creating semaphore");
        delete[] name;
        return -1;
    }

    return 0;
}

int SysWait(char* name) {
    int res = kernel->semTab->Wait(name);

    if (res == -1) {
        DEBUG('a', "\nSemaphore not found");
        delete[] name;
        return -1;
    }

    return 0;
}

int SysSignal(char* name) {
    int res = kernel->semTab->Signal(name);

    if (res == -1) {
        DEBUG('a', "\nSemaphore not found");
        delete[] name;
        return -1;
    }

    return 0;
}


#endif /* ! __USERPROG_KSYSCALL_H__ */
