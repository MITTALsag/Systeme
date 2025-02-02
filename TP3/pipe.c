#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define BUFFER_SIZE 1024


int main(void)
{
    int tube[2];
    pid_t pid;

    // Création du tube
    if (pipe(tube) == -1) {
        perror("Erreur lors de la création du tube");
        exit(EXIT_FAILURE);
    }


    // Création du processus fils
    pid = fork();
    if (pid == -1) {
        perror("Erreur lors du fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Code du fils
        close(tube[1]); // Fermer l'entrée du tube (écriture)

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;

        // Lire les données du tube
        while ((bytes_read = read(tube[0], buffer, BUFFER_SIZE)) > 0) {
            write(1, buffer, bytes_read);
        }

        if (bytes_read == 0) {
            printf("Fils : Le père a fermé l'entrée du tube. Fin.\n");
        } else if (bytes_read < 0) {
            perror("Erreur lors de la lecture dans le tube");
        }

        close(tube[0]); // Fermer la sortie du tube
        exit(EXIT_SUCCESS);
    }

    else{
        close(tube[0]);

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;

        printf("Père : Entrez des lignes (Ctrl+D pour terminer) :\n");
        while((bytes_read = read(0, buffer, BUFFER_SIZE)) > 0)
        {
            write(tube[1], buffer, bytes_read);

        }
        close(tube[1]);
        exit(EXIT_SUCCESS);
    }
    return 0;
}
