#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "utils.h"

int main(void) 
{
    pid_t pid;

    pid = Fork();
    if (pid == 0) {  /* child */
        printf("child!\n");
        exit(0);
    }
    
    /* parent */
    printf("parent!\n");
    sleep(100);
    wait(NULL);
    
    exit(0);
}
