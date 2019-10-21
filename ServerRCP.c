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

#define DIM_BUFFER 256

int main(int argc, char const *argv[])
{
    //serverRCP
    if(argc != 1){
        perror("Usage: rcpServer");
        exit(1);
    }

    clock_t begin = clock();

    struct sockaddr_in serverSock;
    serverSock.sin_family = AF_INET;
    serverSock.sin_port = htons(12345);
    serverSock.sin_addr.s_addr = INADDR_ANY;

    int fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(fdSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if(fdSocket < 0){
        perror("Impossibile creare la socket.");
        close(fdSocket);
        exit(1);
    }

    //DEVO fare la bind per la socket del server!!!
    if(bind(fdSocket,(struct sockaddr *) &serverSock, sizeof(serverSock)) < 0){
        perror("Errore binding con porta.");
        close(fdSocket);
        exit(1);
    }

    //creo la lista per le richieste di connessione
    listen(fdSocket, 5);

    //struttura dove la accept andrà a scrivere tutti i dati del cliente che si è connesso 
    struct sockaddr_in client;
    int sizeOfClient;

    int fdConnect;
    if((fdConnect = accept(fdSocket,(struct sockaddr *) &client, &sizeOfClient)) < 0){
        perror("Impossibile stabilire la connessione con il cliente.");
        close(fdSocket);
        exit(1);
    }

    //il cliente è connesso e posso usare fdConnect per comunicare
    //leggo il nome del file --> rispondo
    char filename[DIM_BUFFER];
    read(fdConnect, filename, DIM_BUFFER);

    //verifico che il file non sia già presente all'interno della directory /ricevuti
    char pathFile[256] = "/ricevuti/";
    strcat(pathFile, filename);
    
    if(access(pathFile, F_OK) == 0){
        write(fdConnect, "N", 1);
        close(fdConnect);
        close(fdSocket);
        exit(0);
    } 

    write(fdConnect, "S", 1);
    
    //mi aspetto il file da copiare sulla directory /ricevuti
    int fdFile;
    if((fdFile = open(pathFile, O_CREAT | O_WRONLY, 0644)) < 0){
        perror("Impossibile creare il file!");
        exit(1);
    }

    //in fdFile ho il file aperto!
    char tmp[DIM_BUFFER];
    while((read(fdConnect, tmp, DIM_BUFFER)) > 0){
        write(fdFile, tmp, strlen(tmp));
    }

    close(fdConnect);
    close(fdSocket);
    close(fdFile);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("SERVER: tempo di esecuzione %f msec\n", time_spent);

    return 0;
}
