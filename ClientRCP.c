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

int main(int argc, char const *argv[])
{
    //rcp   nodoserver  nomefile
    if(argc != 3){
        perror("Usage: rcp nodoserver nomefile");
        exit(1);
    }

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
    char tmp;
    while((read(fdFile, &tmp, sizeof(tmp))) > 0){
        write(fdSocket, &tmp, sizeof(char));
    }

    close(fdSocket);
    return 0;
}
