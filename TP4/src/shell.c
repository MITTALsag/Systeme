
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shell-utils.h"

#define INPUT_BUFFER_SIZE 2048
#define MAX_TOKEN_NB 512

int main() {
	/* Une variable pour sotcker les caractères tapés au clavier */
	char line[INPUT_BUFFER_SIZE+1];

	/* Une variable qui pointera vers les données lues par fgets
	 * Sauf problème (erreur, plus de données, ...), elle pointera en
	 * fait vers la variable précédente (line) */
	char* data;

	/* Un tableau où les mots de l'utilisateur seront stockés
	 * indépendamment les uns des autres
	 * Note: un mot est une suite de lettres séparées par une ou plusieurs
	 * espaces des autres mots.  */
	char* tokens[MAX_TOKEN_NB+1];
	/* variables entières pour compter le nombre de token */
	int nb_tokens;

	/* Récupération des données entrées au clavier */
	data=fgets(line, INPUT_BUFFER_SIZE, stdin);

	if (data==NULL) {
		/* Erreur ou fin de fichier : on quitte tout de suite */
		if (errno) {
			/* Une erreur: on affiche le message correspondant
			 * et on quitte en indiquant une erreur */
			perror("fgets");
			exit(1);
		} else {
			/* Sinon ça doit être une fin de fichier.
			 * On l'indique et on quitte normalement */
			fprintf(stderr, "EOF: exiting\n");
			exit(0);
		}
	}

	/* On vérifie que l'utilisateur n'a pas donné une ligne qui soit trop longue */
	if (strlen(data) == INPUT_BUFFER_SIZE-1) {
		fprintf(stderr, "Input line too long: exiting\n");
		exit(1);
	}

	/* On découpe la ligne en tokens (mots)
	 * Voir documentation dans shell-utils.h (avec des exemples) */
	nb_tokens=split_tokens(tokens, data, MAX_TOKEN_NB);

	/* S'il y a trop de tokens, on abandonne */
	if (nb_tokens==MAX_TOKEN_NB) {
		fprintf(stderr, "Too many tokens: exiting\n");
		exit(1);
	}

	/* S'il n'y a pas de token, c'est que l'utilisateur n'a pas donné de
	 * commande. Il n'y a rien à faire. On arrête tout. */
	if (nb_tokens<=0) {
		fprintf(stderr, "No tokens found: exiting\n");
		exit(1);
	}

	/* On exécute la commande donné par l'utilisateur.
	 * Son nom est dans le premier token (le premier mot tapé)
	 * ses arguments (éventuels) seront les tokens suivants */
	execvp(tokens[0], tokens);

	/* On ne devrait jamais arriver là */

	perror("execvp");
	exit(1);
}
