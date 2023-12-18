#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 1024

int main(int argc, char *argv[]){
    // Déclaration des variables pour les données RRQ et ACK
    char *RRQ;
    char* fileId = argv[2];
    RRQ = calloc(strlen(fileId) + 9, sizeof(char));
    RRQ[1] = 1; 						// Type de paquet RRQ
    strcpy(RRQ + 2, fileId);
    strcpy(RRQ + 3 + strlen(fileId), "octet"); 			// Type d'opération
    
    char *ACK;
    ACK = calloc(4, sizeof(char));
    ACK[1] = 4; 						// Type de paquet ACK

    //  Initialisation d'une structure hints pour stocker les préférences de l'adresse réseau à utiliser.
    struct addrinfo hints = {};
    hints.ai_protocol = IPPROTO_UDP;
    struct addrinfo *reslt;

    // Récupération de l'adresse IP de l'hôte spécifié
    char *host = argv[1];
    int i;
    if ((i = getaddrinfo(host, "1069", &hints, &reslt)) != 0) {
        char* c = gai_strerror(i);
        printf("%s", c); 					// Affichage de l'erreur si getaddrinfo échoue
    }
    
    // Création d'un socket en utilisant les informations obtenues de getaddrinfo.
    int sfd;
    sfd = socket(reslt->ai_family, reslt->ai_socktype, reslt->ai_protocol);
    sendto(sfd, RRQ, strlen(fileId) + 9, 0, reslt->ai_addr, reslt->ai_addrlen);  // Envoi du paquet RRQ au serveur.
  
    // Allocation de mémoire pour stocker le fichier reçu
    char *fileReceived;
    fileReceived = calloc(516, sizeof(char));
    
    // Ouverture d'un fichier pour écrire les données reçues
    int fd;
    ssize_t size;
    fd = open(fileId, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);

    // Réception et écriture des données du fichier dans le fichier local
    do {
        size = recvfrom(sfd, fileReceived, SIZE, 0, reslt->ai_addr, &reslt->ai_addrlen);
        ACK[2] = fileReceived[2];
        ACK[3] = fileReceived[3];
        write(fd, fileReceived + 4, size - 4); 			// Écriture dans le fichier local
        sendto(sfd, ACK, 4, 0, reslt->ai_addr, reslt->ai_addrlen); // Envoi de l'ACK
    } while (size == 516); 					// Boucle jusqu'à ce que la taille reçue soit différente de 516
    
    close(fd); 
    
    return 0;
}
