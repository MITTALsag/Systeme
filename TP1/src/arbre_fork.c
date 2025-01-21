#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "utils.h"

void arbre1(void);
void arbre2(int n);
void arbre3(int n);

int main(int argc, char* argv[])
{
    if (argc < 2)
        unix_error("il faut mettre 1, 2 ou 3 comme argument");


    if (atoi(argv[1]) == 1)
        arbre1();

    if (atoi(argv[1]) == 2)
        arbre2(4);

    if (atoi(argv[1]) == 3)
        arbre3(4);
}



void arbre1(void)
{
    printf("racine :\nPPID : %d\nPID : %d\n\n", getppid(), getpid());

    pid_t fils1 = Fork();
    if (fils1 == 0)
    {
        printf("fils 1 :\nPPID : %d\nPID : %d\n\n", getppid(), getpid());
        pid_t petit_fils1 = Fork();
        if (petit_fils1 == 0)
        {
            printf("petit fils 1 :\nPPID : %d\nPID : %d\n\n", getppid(), getpid());
            exit(0);
        }
        pid_t petit_fils2 = Fork();
        if (petit_fils2 == 0)
        {
            printf("petit fils 2 :\nPPID : %d\nPID : %d\n\n", getppid(), getpid());
            exit(0);
        }
        exit(0);
    }

    wait(NULL);

    pid_t fils2 = Fork();
    if (fils2 == 0)
    {
        printf("fils 2 :\nPPID : %d\nPID : %d\n\n", getppid(), getpid());
        exit(0);
    }


    pid_t fils3 = Fork();
    if (fils3 == 0)
    {
        printf("fils 3 :\nPPID : %d\nPID : %d\n\n", getppid(), getpid());
        exit(0);
    }

    exit(0);
}


void arbre2(int n)
{
    printf("PPID : %d\nPID : %d\n\n", getppid(), getpid());
    for(int i = 0 ; i < n ; i++)
    {
        if (Fork() == 0)
        {
            printf("PPID : %d\nPID : %d\n\n", getppid(), getpid());
        }
        else
        {
            wait(NULL);
            exit(0);
        }
    }
}


void arbre3(int n)
{
    printf("PPID : %d\nPID : %d\n\n", getppid(), getpid());
    for(int i = 0 ; i < n ; i++)
    {
        if (Fork() == 0)
        {
            printf("PPID : %d\nPID : %d\n\n", getppid(), getpid());
            exit(0);
        }
        wait(NULL);
    }
}
