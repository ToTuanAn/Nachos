#include "synch.h"
#include "ptable.h"

PTable::PTable(int size) {
    int i;
    psize = size;
    for (i = 0; i < size; i++) {
        pcb[i] = NULL;
    }
    bmsem = new Semaphore("bmsem", 1);
    pcb[0] = new PCB(0);
    pcb[0]->parentID = -1;
}

PTable::~PTable() {
    int i;
    for (i = 0; i < psize; i++) {
        if (!pcb[i]) delete pcb[i];
    }
    delete bmsem;
}

int PTable::ExecUpdate(char* name) {
    // Call mutex->P() to prevent two threads from being loaded at the same time
    bmsem->P();

    // Validate the “name”.
    // Check existance of “name” calling syscall
    // Open class fileSystem.
    if (name == NULL) {
        DEBUG(dbgSys, "\nPTable::Exec : Can't not execute name is NULL.\n");
        bmsem->V();
        return -1;
    }
    
    // Make sure not exec itself
    if (strcmp(name, kernel->currentThread->getName()) == 0) {
        DEBUG(dbgSys, "\nPTable::Exec : Can't not execute itself.\n");
        bmsem->V();
        return -1;
    }

    // FInd slot in Ptable.
    int index = this->GetFreeSlot();
    // Check if have free slot
    if (index < 0) {
        DEBUG(dbgSys, "\nPTable::Exec :There is no free slot.\n");
        bmsem->V();
        return -1;
    }

    // If empty slot then create new index for PCB
    pcb[index] = new PCB(index);
    pcb[index]->SetFileName(name);
    kernel->fileSystem->Renew(index);
    // parrentID is processID of currentThread
    pcb[index]->parentID = kernel->currentThread->processID;

    //Call Exec of PCB.
    int pid = pcb[index]->Exec(name, index);

    // Call bmsem->V()
    bmsem->V();
    // return PCB->Exec.
    return pid;
}

int PTable::ExitUpdate(int exitcode) {
    //If main process then Halt
    int id = kernel->currentThread->processID;
    if (id == 0) {
        kernel->currentThread->FreeSpace();
        kernel->interrupt->Halt();
        return 0;
    }

    if (!IsExist(id)) {
        DEBUG(dbgSys, "Process " << id << " is not exist.");
        return -1;
    }

    //get exit code.
    pcb[id]->SetExitCode(exitcode);
    pcb[pcb[id]->parentID]->DecNumWait();

    //Call JoinRelease to release parent Join
    //CallExitWait() to exit from parent
    pcb[id]->JoinRelease();
    pcb[id]->ExitWait();

    Remove(id);
    return exitcode;
}

int PTable::JoinUpdate(int id) {
    // we check processID id is valid, check Join call
    // is the parent of processID. 
    if (id < 0 || id >= psize || pcb[id] == NULL ||
        pcb[id]->parentID != kernel->currentThread->processID) {
        DEBUG(dbgSys, "\nPTable::Join : Can't not join.\n");
        return -1;
    }

    // Increase numwait and call JoinWait() wait child process to finish
    pcb[pcb[id]->parentID]->IncNumWait();
    pcb[id]->JoinWait();

    //child process finish
    
    //get exit code
    int exit_code = pcb[id]->GetExitCode();
    //Exit child process
    pcb[id]->ExitRelease();

    return exit_code;
}

int PTable::GetFreeSlot() {
    int i;
    for (i = 0; i < psize; i++) {
        if (!pcb[i]) return i;
    }
    return -1;
}

bool PTable::IsExist(int pid) { return pcb[pid] != NULL; }

void PTable::Remove(int pid) {
    if (pcb[pid] != NULL) {
        delete pcb[pid];
        pcb[pid] = NULL;
    }
}

char* PTable::GetFileName(int id) { return pcb[id]->GetFileName(); }