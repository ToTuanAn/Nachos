#include "syscall.h"

int main() {
    int newProc1;
    int newProc2;
    int multex = CreateSemaphore("cshsem", 1);

    newProc1 = Exec("cat"); // Project 01
    newProc2 = Exec("cat"); // Project 01

    Join(newProc1);
    Join(newProc2);
}