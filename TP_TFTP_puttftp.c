#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 1024

// Fonction pour copier une chaîne de caractères
char* ncpystr(char* dest, char* src, int nb){
    for (int i = 0; i < nb; i++){
        dest[i] = src[i];
    } 
    return dest;
}

int main(int argc, char *argv[]){
    char *WRQ;  						// Buffer pour la requête WRQ
    char *ACK;  						// Buffer pour l'acquittement
    char *file_data;  						// Buffer pour les données du fichier
    char *data_buffer;  					// Buffer pour les données temporaires
    int sfd, fd, size; 						// Descripteurs de fichiers et taille des données
    unsigned int block_nb;  					// Numéro de bloc pour le transfert
    
    char *host = argv[1];  					// Adresse IP du serveur
    char *fileId;
    fileId = argv[2];  						// Nom du fichier à transférer

    data_buffer = calloc(516, sizeof(char));  			// Allocation mémoire pour le tampon de données

    // Construction de la requête WRQ
    WRQ = calloc(strlen(fileId) + 9, sizeof(char));
    WRQ[1] = 2;  						// Op code pour WRQ (2)
    strcpy(WRQ + 2, fileId);
    strcpy(WRQ + 3 + strlen(fileId), "octet");

    ACK = calloc(4, sizeof(char));  				// Allocation mémoire pour l'acquittement

    struct addrinfo hints = {};
    hints.ai_protocol = IPPROTO_UDP;
    struct addrinfo *res;

    // Obtention des informations sur l'adresse IP et le port
    if (getaddrinfo(host, "1069", &hints, &res) != 0)
        printf("ERROR\n");

    sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);  // Création du socket
    sendto(sfd, WRQ, strlen(fileId) + 9, 0, res->ai_addr, res->ai_addrlen);  // Envoi de la requête WRQ

    file_data = calloc(516, sizeof(char));  			// Allocation mémoire pour les données du fichier
    data_buffer[0] = 0;
    data_buffer[1] = 3;

    fd = open(fileId, O_RDONLY);  				// Ouverture du fichier en lecture seule
    block_nb = 0;

    do {
        memset(data_buffer + 4, 0, 512);  			// Initialisation du tampon de données

        recvfrom(sfd, ACK, 4, 0, res->ai_addr, &res->ai_addrlen);  // Réception de l'acquittement

        // Vérification de l'acquittement pour le bloc actuel
        if ((unsigned char)ACK[2] * 256 + (unsigned char)ACK[3] == block_nb) {
            block_nb++;  					// Incrémentation du numéro de bloc
            data_buffer[0] = 0;
            data_buffer[1] = 3;
            data_buffer[2] = block_nb / 256;  			// Octet haut du numéro de bloc
            data_buffer[3] = block_nb % 256;  			// Octet bas du numéro de bloc

            size = read(fd, data_buffer + 4, 512);  		// Lecture des données du fichier

            if (size < 512) {
                free(file_data);
                file_data = calloc(4 + size, sizeof(char));
                ncpystr(file_data, data_buffer, size + 4);
            } 
	    else {
                ncpystr(file_data, data_buffer, size + 4);
            }

            // Envoi des données au serveur
            sendto(sfd, file_data, size + 4, 0, res->ai_addr, res->ai_addrlen);
        }
    } while (size == 512);  					// Boucle jusqu'à la fin du fichier

    recvfrom(sfd, ACK, 4, 0, res->ai_addr, &res->ai_addrlen);  	// Réception finale pour confirmer la fin du transfert
    close(fd);  						// Fermeture du descripteur de fichier
    free(ACK);  						// Libération de la mémoire
    free(WRQ);
    free(file_data);
    
    return 0;
}

