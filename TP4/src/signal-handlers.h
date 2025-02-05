#ifndef SIGNAL_HANDLER
#define SIGNAL_HANDLER

#include <stdbool.h>


/* 
* Variable global qui est mise a true quand on veux utiliser le sigchild_handler
* Par défaut a true car on ne sais pas quand va finir un process en arrière plan
* On mettera a false pour un foreground command et on remettra true juste apres la fin de cette commande
*/
extern bool sigchild_handler_use;


/*
* Fonction qui re map de handler pour le signal SIGCHILD
*/
void sigchild_handler(int sig);


#endif