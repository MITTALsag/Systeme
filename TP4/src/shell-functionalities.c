#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "shell-utils.h"
#include "shell-functionalities.h"


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
        fprintf(stderr, "fork: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return pid;
}

/* Fonction interne qui gère les redirection d'entree ( "<" ) 
* Cette fonction est appelée par exec_cmd_simple
* Elle est appeler dans la partie du fils du fork
* Donc on peut chager ici les descripteurs de fichiers sans affecter le père
*/
void gestion_redirection_entree(char** options)
{
    /* on cherche si il y a un  "<" */
    char* file_in = trouve_redirection(options, "<");
    
    /* si il y a un < */
    if (file_in != NULL)
    {
        /* on ouvre le fichier en lecture (seulement) */
        int fd_in = open(file_in, O_RDONLY);
        /* Gestion d'erreur de open */
        if (fd_in == -1)
        {
            fprintf(stderr, "%s: %s\n", file_in, strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* on redirige l'entrée standard sur le fichier */
        int res_in = dup2(fd_in, 0);
        /* Gestion d'erreur de dup2 */
        if (res_in == -1)
        {
            fprintf(stderr, "%s: %s\n", file_in, strerror(errno));
            exit(EXIT_FAILURE);
        }

    }
    
}

/* Fonction interne qui gère les redirection de sortie ( ">" ) 
* Cette fonction est appelée par exec_cmd_simple
* Elle est appeler dans la partie du fils du fork
* Donc on peut chager ici les descripteurs de fichiers sans affecter le père
*/
void gestion_redirection_sorie(char** options)
{
    /* on cherche si il y a un  ">" */
    char* file_out = trouve_redirection(options, ">");
    
    /* si il y a un > */
    if (file_out != NULL)
    {
        /* 
        * On ouvre le fichier en écriture 
        * On le crée si il n'existe pas 
        * (O_TRUNC sert a effacer le contenu du fichier s'il existe deja)
        * On donne les droit en lecture et écriture au propriétaires
        * Et en lecture seul à tous les autres (644 pour rw- r-- r--)
        */
        int fd_out = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, 644);
        /* Gestion d'erreurs de open */
        if (fd_out == -1)
        {
            fprintf(stderr, "%s: %s\n", file_out, strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* on redirige la sortie standard sur le fichier */
        int res_out = dup2(fd_out, 1);
        /* Gestion d'erreurs de dup2 */
        if (res_out == -1)
        {
            fprintf(stderr, "%s: %s\n", file_out, strerror(errno));
            exit(EXIT_FAILURE);
        }

    }
    
}


/* Fonction interne qui gère les tubes ( "|" ) 
* Cette fonction est appelée par exec_cmd
*/

int exec_cmd(char* cmd, char** options)
{

    /* Vérification de l'éxistence des arguments */
    if(!cmd || !options)
    {
        fprintf(stderr, "Erreur dans la commande ou les options");
        exit(EXIT_FAILURE);
    }

    ./test_basics.sh 


    pid_t fils = Fork();
    if (fils == 0)
    {

        /* Gestion des redirections d'entrée et de sortie */
        gestion_redirection_entree(options);
        gestion_redirection_sorie(options);        

        /* Ici si il y a des redirection d'entrée sortie les descripteurs de fichiers on étais changer */

        /* On exécute la commande donné par l'utilisateur.
        * Son nom est dans le premier token (le premier mot tapé)
        * ses arguments (éventuels) seront les tokens suivants */
        execvp(cmd, options);

        /* On ne devrait jamais arriver là */
        fprintf(stderr, "%s: %s\n", cmd, strerror(errno));
        exit(EXIT_FAILURE);

    }
    /* code du père */

    int status;
    int code_retour;

    /* attente du fils. 
    * On attend la fin de la commande pour demander la commande suivante. */
    waitpid(fils, &status, 0);

    /* On vérifie que le fils s'est terminé normalement */
    if (WIFEXITED(status))
    {
        /* On récupère le code de retour du fils */
        code_retour = WEXITSTATUS(status);

        if(code_retour !=0)
        {
            fprintf(stderr, "La commande à échoué avec le code de retour %d\n", code_retour);
        }
    }
    /* si le processus fils n'a pas terminé normalement (un signal ou autre) */
    else
    {
        fprintf(stderr, "Le processus fils n'a pas terminer normalement.\n");
        exit(EXIT_FAILURE);
    }

    return code_retour;
}



