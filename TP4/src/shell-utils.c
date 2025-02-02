
#include "shell-utils.h"

#include <string.h>

int split_tokens(char **tokens, char* cmd, int max_token)
{
	int nb_tokens=0;
		
	if (max_token<=0) {
		return -1;
	}

	tokens[nb_tokens++]=strtok(cmd, " \t\n");
	if (tokens[0] == NULL) {
		return 0;
	}

	while (nb_tokens<max_token) {
		char*token=strtok(NULL, " \t\n");
		if (token==NULL) {
			break;
		}
		tokens[nb_tokens++]=token;
	}

	tokens[nb_tokens]=NULL;
	return nb_tokens;

}

char* trouve_redirection(char **tokens, const char* redirection)
{
	char* file;

	while(tokens[0] != NULL) {
		if (strcmp(tokens[0], redirection)==0) {
			file=tokens[1];
			if (file==NULL) {
				/* Un signe de redirection, mais pas
				 * de fichier après : on efface juste
				 * le signe de redirection */
				tokens[0]=NULL;
				return NULL;
			}
			/* On décale les arguments suivant la redirection */
			while (tokens[2]!=NULL) {
				tokens[0]=tokens[2];
				tokens++;
			}
			tokens[0]=NULL;
			return file;
		}
		tokens++;
	}
	/* Rien trouvé */
	return NULL;
}

char** trouve_tube(char **tokens, const char* tube)
{
	while(tokens[0] != NULL) {
		if (strcmp(tokens[0], tube)==0) {
			tokens[0]=NULL;
			return tokens+1; 
			// équivalent à :
			// return &tokens[1];
		}
		tokens++;
	}
	/* Rien trouvé */
	return NULL;
}
