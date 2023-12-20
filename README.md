# TP_TFTP
 Réalisé par CHEDI et LOSIO
#####################################
 TFTP Formats

   Type   Op #     Format without header

          2 bytes    string   1 byte     string   1 byte
          -----------------------------------------------
   RRQ/  | 01/02 |  Filename  |   0  |    Mode    |   0  |
   WRQ    -----------------------------------------------
          2 bytes    2 bytes       n bytes
          ---------------------------------
   DATA  | 03    |   Block #  |    Data    |
          ---------------------------------
          2 bytes    2 bytes
          -------------------
   ACK   | 04    |   Block #  |
          --------------------
          2 bytes  2 bytes        string    1 byte
          ----------------------------------------
   ERROR | 05    |  ErrorCode |   ErrMsg   |   0  |
          ----------------------------------------
############# gettftp () #############
- Déclarations de variables pour les données de requête de lecture (RRQ) et d'acquittement (ACK).

- Allocation de mémoire pour stocker les données de requête de lecture en fonction du fichier spécifié dans les arguments de ligne de commande. La requête est construite conformément à la spécification du protocole TFTP (Figure 5-1 de datasheet donné avec le TP) en incluant le type de paquet, le nom du fichier et le mode de transfert.

- Initialisation d'une structure hints pour stocker les préférences de l'adresse réseau à utiliser.

- Récupération de l'adresse IP de l'hôte spécifié dans les arguments de ligne de commande en utilisant getaddrinfo().

- Création d'un socket en utilisant les informations obtenues de getaddrinfo().

- Envoi du paquet de requête de lecture (RRQ) au serveur à l'aide de sendto().

- Allocation de mémoire pour stocker les données du fichier reçu et ouverture d'un fichier pour écrire les données reçues.

- Réception et écriture des données du fichier dans le fichier local en utilisant recvfrom() pour recevoir les données, write() pour écrire dans le fichier local et sendto() pour envoyer l'acquittement (ACK) au serveur.

- La boucle continue tant que la taille reçue est de 516 octets (selon la spécification du protocole TFTP). Une fois que la taille reçue est différente de 516, la boucle s'arrête.

- RRQ = calloc(strlen(fileId) + 9, sizeof(char));: Cette ligne alloue de la mémoire pour stocker la requête de lecture (Read Request - RRQ). La longueur de la mémoire allouée est calculée en fonction de la taille du nom de fichier (strlen(fileId)) plus 9 octets supplémentaires pour le format de la requête RRQ conformément à la spécification du protocole TFTP.

- RRQ[1] = 1;: Le deuxième octet de la requête RRQ est défini à 1, ce qui correspond au type de paquet RRQ selon la spécification du protocole TFTP (Figure 5-1).

- strcpy(RRQ + 2, fileId);: Le nom du fichier (contenu dans fileId) est copié à partir du troisième octet de la requête RRQ (à l'indice 2). Cela respecte la structure spécifiée du paquet RRQ dans le protocole TFTP.

- strcpy(RRQ + 3 + strlen(fileId), "octet");: Le mode de transfert (dans ce cas-ci, "octet") est copié après le nom du fichier, à l'endroit spécifié dans la spécification TFTP.

- ACK = calloc(4, sizeof(char));: Alloue de la mémoire pour stocker le paquet d'acquittement (ACK) avec une taille de 4 octets, conformément à la spécification TFTP.

- ACK[1] = 4;: Définit le deuxième octet du paquet ACK à 4, ce qui correspond au type de paquet ACK selon la spécification du protocole TFTP.

- sendto(sfd, RRQ, strlen(fileId) + 9, 0, reslt->ai_addr, reslt->ai_addrlen);: Envoie le paquet RRQ (la requête de lecture) au serveur. La taille du paquet est spécifiée comme étant la longueur du nom de fichier plus 9 octets (conformément à la taille allouée pour la requête RRQ).
############# puttftp () #############
- Initialisation et allocation de mémoire :
        WRQ, ACK, file_data, data_buffer : Ces variables servent de tampons pour stocker les différentes parties des paquets TFTP.

- Construction de la requête WRQ (Write Request) :
        La requête est construite selon le format défini dans le protocole TFTP (Figure 5-1) avec le nom du fichier à transférer et le mode de transfert (dans ce cas, "octet").

- Création du socket et envoi de la requête WRQ :
        Un socket est créé pour la communication avec le serveur TFTP, et la requête WRQ est envoyée au serveur.

- Lecture et envoi des données du fichier :
        Le fichier spécifié dans les arguments de la ligne de commande est ouvert en mode lecture seule (O_RDONLY).
        Une boucle commence pour lire les données du fichier par blocs de 512 octets, les emballer dans des paquets de données TFTP et les envoyer au serveur.
        Un ACK est attendu pour chaque bloc envoyé avant de passer au bloc suivant.
        La boucle continue jusqu'à ce que la lecture du fichier ne renvoie pas 512 octets, ce qui signifie la fin du fichier.

- Réception finale pour confirmer la fin du transfert :
        Après l'envoi de toutes les données du fichier, une dernière réception est effectuée pour confirmer la fin du transfert.

- Fermeture des descripteurs de fichiers et libération de mémoire :
        Une fois le transfert terminé, les descripteurs de fichiers sont fermés, et la mémoire allouée dynamiquement est libérée.

--> Ce code illustre le processus d'envoi d'un fichier vers un serveur TFTP en respectant le protocole défini par la RFC 1350.

    **)Récupération des arguments de la ligne de commande :
        char *host = argv[1]; : L'adresse IP ou le nom d'hôte du serveur TFTP est passé en tant qu'argument du programme. Cela permet de spécifier le serveur auquel envoyer les requêtes TFTP.
        fileId = argv[2]; : Le nom du fichier à transférer est passé en tant qu'argument du programme. Ce fichier sera ouvert et envoyé au serveur TFTP.

    **)Construction de la requête WRQ (Write Request) :
        WRQ = calloc(strlen(fileId) + 9, sizeof(char)); : Alloue de la mémoire pour stocker la requête WRQ. La taille est calculée en fonction de la longueur du nom de fichier plus 9 octets pour le format de la requête WRQ.
        WRQ[1] = 2; : Définit le deuxième octet de la requête WRQ comme le code opérationnel pour WRQ, qui est 2 selon le protocole TFTP.
        strcpy(WRQ + 2, fileId); : Copie le nom du fichier dans la requête WRQ.
        strcpy(WRQ + 3 + strlen(fileId), "octet"); : Ajoute le mode de transfert ("octet") à la requête WRQ conformément au protocole TFTP.

    **)Allocation de mémoire pour les tampons de données :
        ACK = calloc(4, sizeof(char)); : Alloue de la mémoire pour stocker l'acquittement. Il est de taille 4 octets conformément à la spécification TFTP.
        data_buffer = calloc(516, sizeof(char)); : Alloue de l'espace pour stocker les données à envoyer ou recevoir. Il s'agit d'une taille de paquet maximale de 516 octets (512 octets de données + 4 octets d'en-tête).

    **)Manipulation des paquets et envoi/réception de données :
        sendto(sfd, WRQ, strlen(fileId) + 9, 0, res->ai_addr, res->ai_addrlen); : Envoie la requête WRQ au serveur TFTP via le socket spécifié (sfd) avec l'adresse récupérée par getaddrinfo.
        recvfrom(sfd, ACK, 4, 0, res->ai_addr, &res->ai_addrlen); : Attend et reçoit l'acquittement du serveur.
        Les autres lignes (par exemple, la manipulation de data_buffer pour la construction des paquets de données, la lecture du fichier dans data_buffer, puis l'envoi de ces données au serveur avec sendto) font partie du processus itératif d'envoi des données au serveur TFTP par blocs de 512 octets.