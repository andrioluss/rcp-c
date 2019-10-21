#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>

#define DIM_BUFFER 1024

int main(int argc, char const *argv[])
{
    //rcp   nodoserver  nomefile
    if(argc != 3){
        perror("Usage: rcp nodoserver nomefile");
        exit(1);
    }

    clock_t begin = clock();

    struct sockaddr_in serverSock;
    serverSock.sin_family = AF_INET;
    serverSock.sin_port = htons(12345);

    struct hostent *host;
    host = gethostbyname(argv[1]);

    if(host == NULL){
        perror("Impossibile risalire all'indirizzo del server.");
        exit(1);
    }

    serverSock.sin_addr.s_addr= ((struct in_addr *)(host->h_addr_list[0]))->s_addr;

    int fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(fdSocket < 0){
        perror("Impossibile creare la socket.");
        exit(1);
    }

    if((connect(fdSocket,(struct sockaddr *) &serverSock, sizeof(serverSock))) < 0){
        perror("Impossibile instaurare la connessione.");
        exit(1);
    }
    
    //a connessione instaurata mando il nome del file e aspetto S/N
    write(fdSocket, argv[2], strlen(argv[2]));

    char risp;
    read(fdSocket, &risp, 1);

    printf("CLIENT: ho ricevuto dal server %c\n", risp);

    //inizio ad inviare il file fino alla fine!
    int fdFile;
    if((fdFile = open(argv[2], O_RDONLY)) < 0){
        perror("Impossibile creare il file!");
        exit(1);
    }

    //in fdFile ho il file aperto!
    /*-----------------------------------------------------------------------------------------------------------
    TEST!!!
    Scopriamo che con un file da 80000 byte, il client impiega per la sua esecuzione:
     - 0.051145 microsec --> se legge da file e invia su socket un char alla volta! (10^-8)
     - 0.052781 --> se legge 256 byte alla volta e li scrive allo stesso modo
     - 0.000232 --> se legge 1024 byte alla volta e li scrive allo stesso modo

    char tmp;
    while((read(fdFile, &tmp, sizeof(tmp))) > 0){
        write(fdSocket, &tmp, sizeof(char));
    }
    ----------------------------------------------------------------------------------------------------------*/

    
    char tmp[DIM_BUFFER];
    while((read(fdFile, &tmp, sizeof(tmp))) > 0){
        write(fdSocket, &tmp, strlen(tmp));
    }
    close(fdSocket);
    close(fdFile);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("CLIENT: tempo di esecuzione %f\n", time_spent);

    return 0;
}
