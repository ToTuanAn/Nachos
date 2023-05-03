#include "syscall.h"
#define stdin 0
#define stdout 1

int main() {
    int pid;
    char* argv[4];
    argv[0] = "createfile";
    argv[1] = "-l";
    argv[2] = "-F";
    argv[3] =  "\0";

    pid = ExecV(argv[0], argv);
    
    if (pid < 0) {
        Write("Exec failed: ", 14, stdout);
    } else
        Write("Success", 8, stdout);
        Join(pid);
    
    PrintString("jjj");

    return 0;
}