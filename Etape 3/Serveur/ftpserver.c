/*
 * echoserveri.c - An iterative echo server
 */

#include "../Lib/csapp.h"
#include "../Lib/ftpprotocol.h"

#define MAX_NAME_LEN 256

void echo(int connfd);

void sig_handler(int sid) {
    exit(0);
}

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv) {
    Signal(SIGINT, sig_handler);
    int listenfd, connfd;
    ssize_t n;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    
    Response servResponse;
    Esclave esclaves[NB_SLAVES];
    int esclave_courant = 0;
    // --
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(MASTER_PORT);
    
    while (1) {
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);
        
        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                INET_ADDRSTRLEN);
        
        printf(">> Nouvelle connexion depuis %s (%s)\n", client_hostname,
            client_ip_string);

        rio_t rio;
        Rio_readinitb(&rio, connfd);
        n = rio_readn(connfd, &servResponse, sizeof(Response));

        if (n > 0) {
            switch(servResponse.statusCode) {
                case 1:
                    /* esclave */
                    esclaves[servResponse.slaveData.id].port = servResponse.slaveData.port;
                    esclaves[servResponse.slaveData.id].id = servResponse.slaveData.id;
                    printf("!! Esclave n°%d connecté (port: %d)\n", servResponse.slaveData.id+1, servResponse.slaveData.port);
                    break;
                case 200:
                    /* client demande esclave */
                    servResponse.statusCode = 201;
                    servResponse.slaveData.port = esclaves[esclave_courant].port;
                    Rio_writen(connfd, &servResponse, sizeof(Response));
                    esclave_courant++;
                    if (esclave_courant >= NB_SLAVES) esclave_courant = 0;
                    break;
            }
        }
        close(connfd);
        
        if(servResponse.statusCode != 1) printf("[%d] Client traité!\n", getpid());
    }
}

