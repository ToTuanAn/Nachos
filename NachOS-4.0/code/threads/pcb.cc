#include "synch.h"
#include "pcb.h"

PCB::PCB(int id) {
    this->processID = kernel->currentThread->processID;
    joinsem = new Semaphore("joinsem", 0);
    exitsem = new Semaphore("exitsem", 0);
    multex = new Semaphore("multex", 1);
}

PCB::~PCB() {
    delete joinsem;
    delete exitsem;
    delete multex;

    if (thread) {
        thread->FreeSpace();
        thread->Finish();
        // delete thread;
    }

    delete[] filename;
}

void StartProcess_2(void* pid) {
    int id;
    id = *((int*)pid);
    // Lay fileName cua process id nay
    char* fileName = kernel->pTab->GetFileName(id);

    AddrSpace* space;
    space = new AddrSpace(fileName);

    if (space == NULL) {
        printf("\nPCB::Exec: Can't create AddSpace.");
        return;
    }

    space->Execute();
    // kernel->currentThread->space = space;

    // space->InitRegisters();	// set the initial register values
    // space->RestoreState();	// load page table register

    // kernel->machine->Run();	// jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

int PCB::Exec(char* filename, int id) {
    // cerr << filename << ' ' << pid << endl;
    multex->P();

    this->thread = new Thread(filename, true);
    if (this->thread == NULL) {
        printf("\nPCB::Exec: Not enough memory!\n");
        multex->V();  // Nha CPU de nhuong CPU cho tien trinh khac
        return -1;    // Tra ve -1 neu that bai
    }

    //  Set processID of this thread is id.
    this->thread->processID = id;
    // Set parrentID of this thread is processID of thread calling Exec
    this->parentID = kernel->currentThread->processID;
    // Call Fork(StartProcess_2,id) to execute the new thread; 
    // Cast the thread to int, then cast it back to its original type in the StartProcess function

    this->thread->Fork(StartProcess_2, &this->thread->processID);

    multex->V();
    // Trả về id.
    return id;
}

int PCB::GetID() { return thread->processID; }

int PCB::GetNumWait() { return numwait; }

void PCB::JoinWait() {
    // Call joinsem->P() process change to block state and stop,
    // Wait JoinRelease to unlock.
    joinsem->P();
}

void PCB::ExitWait() {
    // Call exitsem->V() process change to block state and stop,
    // Wait ExitRelease to unlock.
    exitsem->P();
}

void PCB::JoinRelease() {
    // Call joinsem->V() to release JoinWait().
    joinsem->V();
}

void PCB::ExitRelease() {
    // Call exitsem->V() to release ExitWait().
    exitsem->V();
}

void PCB::IncNumWait() {
    multex->P();
    ++numwait;
    multex->V();
}

void PCB::DecNumWait() {
    multex->P();
    if (numwait > 0) --numwait;
    multex->V();
}

void PCB::SetExitCode(int ec) { exitcode = ec; }

int PCB::GetExitCode() { return exitcode; }

void PCB::SetFileName(char* fn) { strcpy(filename, fn); }

char* PCB::GetFileName() {
    // cerr << "get file name" << ' ' << filename << endl;
    return filename;
}