
#ifndef SHELL_UTILS_PERSO
#define SHELL_UTILS_PERSO



/*
** Exécute la commande "cmd" avec les option "options"
** Renvoie 0 si tous c'est bien passé
** Et exit(EXIT_FAILURE) sinon
*/
int exec_cmd_fils(char* cmd, char** options);


#endif
