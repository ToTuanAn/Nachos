#include "syscall.h"

int main()
{   
    int openFileId = Open("hello.txt", 0);
    return 0;
}