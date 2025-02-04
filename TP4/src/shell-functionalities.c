#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>

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

/*
* Fonction interne qui renvoie vrais si le dernier token de options est "&"
* Cette fonction met NULL a la place de "&" si il existe pour eviter de le passer en argument
*/
bool test_arriere_plan(char** options, int taille_options)
{
    /* on regarde si le dernier token de options est & */
    if(options != NULL && strcmp(options[taille_options-1], "&") == 0)
    {
        /* // Retire le & pour éviter de passer comme argument */
        options[taille_options-1] = NULL;
        return true;
    }
    return false;
}

/* 
* Variable global qui est mise a true quand on veux utiliser le sigchild_handler
* Par défaut a true car on ne sais pas quand va finir un process en arrière plan
* On mettera a false pour un foreground command et on remettra true juste apres la fin de cette commande
*/
bool sigchild_handler_using = true;

/*
* Fonction qui re map de handler pour le signal SIGCHILD
*/
void sigchild_handler(int sig) 
{
    if (sigchild_handler_using)
    {
        int statut;
        /* 
        * Le while sert a chercher si il n'y en a pas d'autre 
        * Car un signal d'un meme type n'est sauvegardé qu'une fois
        * donc si il y a plusieur process fils en background qui se termine en meme temps
        * Il faut quand meme tous les wait.
        */
        while(waitpid(-1, &statut, WNOHANG) > 0);
    }
}

/* 
* Fonction interne qui gère les redirection d'entree ( "<" ) 
* Cette fonction est appelée par exec_cmd_dans_fils
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
* Cette fonction est appelée par exec_cmd_dans_fils
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


/*
* Fonction interne qui éxecute la commande cmd du shell avec les options options 
* Cette fonction est appelée par exec_cmd_simple et exec_cmd_pipe
* Elle est appelée dans la partie du fils du fork
* Donc on est bien dans un processus fils du shell
*/
int exec_cmd_dans_fils(char* cmd, char** options)
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


/*
* Fonction interne qui gère le code de retour du fils
* Cette fonction est appelée par exec_cmd_simple et exec_cmd_pipe
* Elle est appelé dans la partie du pere (donc du shell)
*/
int gestion_code_retour_fils(pid_t fils, char* cmd)
{
    int status;
    int code_retour;

    /* attente du fils */
    if (waitpid(fils, &status, 0) == -1) 
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    /* On vérifie que le fils s'est terminé normalement */
    if (WIFEXITED(status))
    {
        /* On récupère le code de retour du fils */
        code_retour = WEXITSTATUS(status);

        /* Mode débug */
        // if (code_retour != 0)
        // {
        //     fprintf(stderr, "Erreur : le processus fils la commande %s s'est terminer avec le code de retour %d\n", cmd, code_retour);
        // }
    }
    else
    {   
        /* Mode debug */
        //fprintf(stderr, "Erreur : le processus fils de la commande %s s'est terminé de manière inconnue.\n", cmd);
        code_retour = 1; // Valeur par défaut pour une erreur
    }

    return code_retour;
}


/* 
* Fonction interne qui exécute une commande simple (sans tube)
* appeler par exec_cmd
* fais la gestion des redirections et la gestion de commande a executer en background ou foreground
*/
int exec_cmd_simple(char* cmd, char** options, int nb_tokens, bool background)
{   
    pid_t fils = Fork();
    if (fils == 0)
    {   
        /* code du fils */
        /* On execute la commande en gérant les éventuelles redirections */
        exec_cmd_dans_fils(cmd, options);

        /* On ne devrait jamais arriver là (car la fonction exec_cmd_dans_fils fais un exit) */
        fprintf(stderr, "On ne dervait jamais arriver ici\n");
        exit(EXIT_FAILURE);
    }

    /* code du pere */

    /* Si on execute en arriere plan : on attend pas le fils et on retourne 0 */
    if (background)
    {   
        printf("[Processus en arrière-plan] PID: %d\n", fils);

        return 0;
    }
    /* sinon on attend le fils et on fais la gestion d'erreur */
    else
    {
        /* mise de la variable global a false : le waitpid de gestion_code_retour_fils car le shell doit attendre la fin du process */
        sigchild_handler_using = false;
        int res = gestion_code_retour_fils(fils, cmd);

        /* Ne pas oublier de remettre a true pour au cas ou l'attend d'un background process */
        sigchild_handler_using = true;

        return res;
    }
    

}

/* 
* Fonction interne qui gère les tubes ( "|" ) 
* Cette fonction est appelée par exec_cmd
*/
int exec_cmd_pipe(char*** atomic_cmd, int nb_atomic_cmd, bool background)
{
    /* 
    * On va faire nb_atomic_cmd fork (un fork par commande)
    */
    pid_t fils[nb_atomic_cmd];

    /* 
    * On va cree nb_atomic_cmd-1 tubes 
    */
    int nb_tube = nb_atomic_cmd-1;
    int tube[nb_tube][2]; 

    //print_char_triple(atomic_cmd);
    // printf("nb_cmd = %d\n", nb_atomic_cmd);
    // printf("nb_tub = %d\n", nb_tube);

    int i;
    /* on cree tous les pipe */
    for(i = 0 ; i < nb_tube ; i++)
    {
        if (pipe(tube[i]) == -1)
        {
            fprintf(stderr, "pipe: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* On va ensuite cree tous les fils (qui auront tous le shell comme père)*/
    for (i = 0 ; i < nb_atomic_cmd ; i++)
    {
        fils[i] = Fork();

        //printf("Fils[%d] = %d\n", i, fils[i]);
        
        /* Code du fils */
        if (fils[i] == 0)
        {

            /* Chaque fils doit fermé le tube qui ne lui sont pas adjacent */
            for(int j = 0 ; j < nb_tube ; j++)
            {
                /* Le tube adjacent sont en position i et i-1 */
                if(j != i-1 && j != i)
                {
                    close(tube[j][0]);
                    close(tube[j][1]);
                }
            }

            /* 
            * Si on est la premiere commande 
            * Alors on ferme le descripteur de lecture du premier tube (tube[0])
            */
            if (i == 0)
            {
                close(tube[i][0]);
                /* On redirige la sortie standard sur le tube */
                if (dup2(tube[i][1], 1) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(tube[i][1]);
            }
            /*
            * Si on est la dernière commande
            * Alors on ferme le descripteur d'écriture du dernier tube (tube[nb_tube -1])
            */
            else if (i == nb_atomic_cmd-1)
            {
                close(tube[nb_tube-1][1]);
                /* On redirige l'entrée standard sur le tube */
                if (dup2(tube[nb_tube-1][0], 0) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(tube[nb_tube - 1][0]);
            }
            /*
            * Sinon on est une commande au milieu
            * On ferme le descripteur d'écriture du tube précédent (tube[i-1])
            * Et on ferme le descripteur de lecture du tube actuel (tube[i])
            */
            else
            {
                close(tube[i-1][1]);
                close(tube[i][0]);
                /* On redirige l'entrée standard sur le tube */
                if (dup2(tube[i-1][0], 0) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                /* On redirige la sortie standard sur le tube */
                if (dup2(tube[i][1], 1) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                    
                }
                close(tube[i - 1][0]);
                close(tube[i][1]);
            }

            /* ici le tube est bien composer (les entree descripteurs inutiles sont bien fermé) */
            /* On execute la commande en gérant les éventuelles redirections */
            exec_cmd_dans_fils(atomic_cmd[i][0], atomic_cmd[i]);

            /* On ne devrait jamais arriver là (car la fonction exec_cmd_dans_fils fais un exit) */
            fprintf(stderr, "On ne dervait jamais arriver ici\n");
            exit(EXIT_FAILURE);

        }

    }

    /* code du père */
    /* On ferme les descripteurs de chaque tube */
    for(i = 0 ; i < nb_tube ; i++)
    {
        close(tube[i][0]);
        close(tube[i][1]);
    }

    /* 
    * Si on execute la commande en background 
    * on affiche les process fils cree en arriere plan (mode debug)
    * et on retourne simplement 0
    */
    if (background)
    {
        /* Mode debug */
        // for (int i = 0; i < nb_atomic_cmd; i++) {
        //     printf("[Processus en arrière-plan] PID: %d\n", fils[i]);
        // }
        return 0;
    }
    /*
    * Sinon on met bien a jour (avant et apres sigchild_handler_using)
    * puis on attend chaque fils avec gestion_code_retour_fils
    */
    else
    {
        /* mise de la variable global a false : le waitpid de gestion_code_retour_fils car le shell doit attendre la fin du process */
        sigchild_handler_using = false;
        for (int i = 0; i < nb_atomic_cmd; i++)
        {
            gestion_code_retour_fils(fils[i], atomic_cmd[i][0]);
        }
        /* Ne pas oublier de remettre a true pour au cas ou l'attend d'un background process */
        sigchild_handler_using = true;

        return 0;
    }

    /* On ne devrait jamais arriver là (car la fonction exec_cmd_dans_fils fais un exit) */
    fprintf(stderr, "On ne dervait jamais arriver ici\n");
    exit(EXIT_FAILURE);
}


/*
* Fonction appeler par le shell 
*/
int exec_cmd(char** tokens, int nb_tokens)
{

    /* Vérification de l'éxistence des arguments */
    if(!tokens)
    {
        fprintf(stderr, "Erreur dans les arguments");
        exit(EXIT_FAILURE);
    }

    /* On regarde si il faut faire la commande en arriere plan */
    /* on le fais directement pour supprimer le & si il y en a un */
    bool background = test_arriere_plan(tokens, nb_tokens);


    /* 
    * Tableau pour stocker les commandes séparées par les tubes
    * (On prend large : taille = nombre de tokens)
    */
    char*** atomic_cmd = calloc(nb_tokens, sizeof(char**)); // Tableau de pointeurs de lignes
    if (!atomic_cmd) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    /* 
    * Fait en sorte que atomic_cmd[0] contienne la permiere commande
    * et atomic_cmd[1] la deuxième (avec potentiellement des tubes)
    */
    atomic_cmd[1] = trouve_tube(tokens, "|");
    atomic_cmd[0] = tokens;

    int i;
    /* On cherche si il y a un tube */
    for(i = 2 ; atomic_cmd[i-1] != NULL ; i++)
    {
        atomic_cmd[i] = trouve_tube(atomic_cmd[i-1], "|");
    }

    /* Ici toutes les commandes séparé par des tubes sont dans le tableau atomic_cmd */
    /* Et il y a i-1 atomic comande */
    int nb_atomic_cmd = i-1;

    /* 
    * si il y a une seule atomic commande alors il n'y a pas de tube
    * On appelle donc exec_cmd_simple
    */
    if (nb_atomic_cmd == 1)
    {   
        free(atomic_cmd);
        return exec_cmd_simple(tokens[0], tokens, nb_tokens, background);
    }

    /* 
    * sinon il y a au moins un tube 
    * Et il faut appeler exec_cmd_pipe
    */
    int res =  exec_cmd_pipe(atomic_cmd, nb_atomic_cmd, background);

    /* Sans oublier de liberer la mémoire */
    free(atomic_cmd);
    
    return res;
    
}



