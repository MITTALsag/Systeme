
#ifndef SHELL_UTILS_PERSO
#define SHELL_UTILS_PERSO

void sigchild_handler(int sig);

int exec_cmd(char** tokens, int nb_tokens);


#endif
