#include <stdio.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "signal-handlers.h"


/* initialisation de la variable a true car on ne sais pas quand va finir un process en arrière plan */
bool sigchild_handler_use = true;


/*
* Fonction qui re map de handler pour le signal SIGCHILD
*/
void sigchild_handler(int sig) 
{
    if (sigchild_handler_use)
    {
        int statut;
        pid_t pid;
        /* 
        * Le while sert a chercher si il n'y en a pas d'autre 
        * Car un signal d'un meme type n'est sauvegardé qu'une fois
        * donc si il y a plusieur process fils en background qui se termine en meme temps
        * Il faut quand meme tous les wait.
        */
        while((pid = waitpid(-1, &statut, WNOHANG)) > 0)
        {
            /* mode debug */
            //printf("\nProcessus enfant %d terminé avec le statut %d\n", pid, statut);
        }
    }
}