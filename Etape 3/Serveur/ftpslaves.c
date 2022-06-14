/*
 * ftpslaves.c
 */

#include "../Lib/csapp.h"
#include "../Lib/ftpprotocol.h"

#define MAX_NAME_LEN 256

void echo(int connfd);

pid_t p_procs[NB_SLAVES];
pid_t pid_pere;

void sig_handler(int sid) {
    if (getpid() == pid_pere) {
        for (int i = 0; i < NB_SLAVES; i++) {
            printf("\nenvoi SIGINT à pool %d [%d]", i+1, p_procs[i]);
            kill(p_procs[i], SIGINT);
        }
        exit(0);
    }
}

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    int clientfd, listenfd, connfd;
    struct sockaddr_in clientaddr;
    socklen_t clientlen;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];

    char *host;
    rio_t rio;
    pid_t p_proc;
    pid_pere = getpid();

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];

    Response servResponse;
    servResponse.statusCode = 1;

    int s_port = SLAVE_PORT;
    int i;
    for (i = 0; i < NB_SLAVES; i++) {
        if (getpid() == pid_pere) {
            p_proc = Fork();
            s_port++;
            p_procs[i] = p_proc;
            if (p_proc != 0) printf("!! Esclave #%d [%d]\n", i+1, p_procs[i]);
        } else {
            break;
        }
    }

    if (p_proc == 0) {
        clientfd = Open_clientfd(host, MASTER_PORT);

        Rio_readinitb(&rio, clientfd);
        Esclave s;
        s.port = s_port;
        s.id = i-1;
        servResponse.slaveData = s;
        // -- signal au maitre de sa présence
        Rio_writen(clientfd, &servResponse, sizeof(servResponse));
        close(clientfd);
        // -- écoute
        clientlen = (socklen_t)sizeof(clientaddr);
        listenfd = Open_listenfd(s_port);

        while (1) {
             connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

            /* determine the name of the client */
            Getnameinfo((SA *) &clientaddr, clientlen,
                        client_hostname, MAX_NAME_LEN, 0, 0, 0);
            
            /* determine the textual representation of the client's IP address */
            Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                    INET_ADDRSTRLEN);
            
            printf("[%d] Nouvelle connexion depuis %s (%s)\n", getpid(), client_hostname,
                client_ip_string);

            echo(connfd);
            printf("[%d] Client traité!\n", getpid());
            close(connfd);
        }
    } else {
        Signal(SIGINT, sig_handler);
        while (1) { }
    }

}