La liste des fonctionnalit´es que vous avez mises en oeuvre :


    - Crée un prompt : "shell>" juste avant le fgets

    - Ajout de la fonction (dans shell-utils-perso) int exec_cmd_fils(char* cmd, char** options); qui execute
      la commande cmd en créant un autre process grâce a Fork().
      fais la gestion d'erreur et renvoie le code de retour de la commande
      executé si le process cree a terminer normalement 

    - modif de l'organisation : 
        * shell.c --> exec_shell.c (le main deviens la
      fonction shell
        * le main est dans shell.c

        - modif exec_shell.c (code fornit) : 
            * ajout de la boucle while(1) pour executer en boucle des
              commandes.
            * ajout du continue pour nb_token ==0












— La liste des bugs connus
