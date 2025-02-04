La liste des fonctionnalit´es que vous avez mises en oeuvre :


    - Crée un prompt : "shell>" juste avant le fgets

    - Ajout de la fonction (dans shell-functionalities) int exec_cmd(char* cmd, char** options); qui execute
      la commande cmd en créant un autre process grâce a Fork() (fonction du TD).
      fais la gestion d'erreur et renvoie le code de retour de la commande
      executé si le process cree a terminer normalement 
      (le pere (le shell) attend son fils (cmd a executer))

    - modif de l'organisation : 
        * shell.c --> exec_shell.c (le main deviens la
      fonction shell
        * le main est dans shell.c

    - modif exec_shell.c (code fornit) : 
        * ajout de la boucle while(1) pour executer en boucle des
          commandes.
        * ajout du continue pour nb_token ==0
        * ajout strcmp(tokens[0], "exit") pour quit si on fais ^D ou exit 
    
    - Fin 4.2

    - Ajout des recher de ">" et "<" avec trouve_redirection.
      * problème quand on fais ls > a.txt : ca marche mais si on fais ensuite echo "salut" > a : ca ne supprime pas ce qu'il y avais avant.
      Donc on rajoute O_TRUNC pour supprimmer le contenu d'avant dans la redirecetion de sortie ">"

*******************************************************************************************************
    - modif test_basics.sh A faire !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*******************************************************************************************************

    - Ajout de la gestion de | : exec_cmd car le shell doit faire plus de fork.
      * on parse donc les tokens en sous tokens de commande atomic (sans pipe) puis on appelle exec_cmd_pipe qui fais l'algo du cours 
      * bien verifier que les fils ferme les pipe non adjacent
    
    
    - Fin 4.3

    - ajout de sigchild_handler pour la gestion de SIGCHILD et de la variable global sigchild_handler_using.
    - ajout de test_arriere_plan

    - modif dans exec_cmd ---> test_arriere_plan pour bool background
    - modif dnas exec_cmd_simple et exec_cmd_pipe ---> if(background)... dans le code du pere 

    - ajout de signal(SIGCHILD, sigchild_handler) dans le exec_shell.c

    - Fin 4.4

        













— La liste des bugs connus
