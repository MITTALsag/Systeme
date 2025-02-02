#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shell-utils-perso.h"


/*
** fonction interne qui fais un fork et ca gestion d'erreur
** Renvoie le pid si tous c'est bien passé 
** Et exit(EXIT_FAILUR) sinon
*/
pid_t Fork(void)
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        perror("Erreur lors du fork.\n");
        exit(EXIT_FAILURE);
    }
    return pid;
}



int exec_cmd_fils(char* cmd, char** options)
{
    /* Vérification de l'éxistence des arguments */
    if(!cmd || !options)
    {
        fprintf(stderr, "Erreur dans la commande ou les options");
        exit(EXIT_FAILURE);
    }

    pid_t fils = Fork();
    if (fils == 0)
    {
    /* On exécute la commande donné par l'utilisateur.
     * Son nom est dans le premier token (le premier mot tapé)
     * ses arguments (éventuels) seront les tokens suivants */
    execvp(cmd, options);

    /* On ne devrait jamais arriver là */
    perror("execvp");
    exit(EXIT_FAILURE);

    }
    int status;
    int code_retour;

    waitpid(fils, &status, 0); //attente du fils
    if (WIFEXITED(status))
    {
        code_retour = WEXITSTATUS(status);

        if(code_retour !=0)
        {
            fprintf(stderr, "La commande à échoué avec le code de retour %d\n", code_retour);
        }
    }
    else
    {
        fprintf(stderr, "Le processus fils n'a pas terminer normalement.\n");
        exit(EXIT_FAILURE);
    }

    return code_retour;
}



