#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    printf("PPID : %d\nPID  : %d\n", getppid(),getpid());

    exit(0);
}
