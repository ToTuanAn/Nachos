
#include "synch.h"
#include "stable.h"

STable::STable() {
    this->bm = new Bitmap(MAX_SEMAPHORE);

    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        this->semTab[i] = NULL;
    }
}

STable::~STable() {
    if (this->bm) {
        delete this->bm;
        this->bm = NULL;
    }
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        if (this->semTab[i]) {
            delete this->semTab[i];
            this->semTab[i] = NULL;
        }
    }
}

int STable::Create(char* name,int init) {  
    // Check semaphore existed
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        if (bm->Test(i)) {
            if (strcmp(name, semTab[i]->GetName()) == 0) {
                return -1;
            }
        }
    }
    // Find slot in semTab
    int id = this->FindFreeSlot();

    // return -1 full slot
    if (id < 0) {
        return -1;
    }

    // create new slot for semTab
    this->semTab[id] = new Sem(name, init);
    return 0;
}

int STable::Wait(char* name) {
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        // Test slot i 
        if (bm->Test(i)) {
            // There is a sem in i 
            if (strcmp(name, semTab[i]->GetName()) == 0) {
                // down sem
                semTab[i]->wait();
                return 0;
            }
        }
    }
    printf("Not created semaphore");
    return -1;
}

int STable::Signal(char* name) {
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        // Test slot i 
        if (bm->Test(i)) {
            // There is a sem in i 
            if (strcmp(name, semTab[i]->GetName()) == 0) {
                // up sem
                semTab[i]->signal();
                return 0;
            }
        }
    }
    printf("Not created semaphore");
    return -1;
}

int STable::FindFreeSlot() { return this->bm->FindAndSet(); }