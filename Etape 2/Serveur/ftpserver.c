/*
 * echoserveri.c - An iterative echo server
 */

#include "../Lib/csapp.h"
#include "../Lib/ftpprotocol.h"

#define MAX_NAME_LEN 256

pid_t p_procs[NB_PROC];
pid_t pid_serveur;

void echo(int connfd);

void sigc_handler(int sid) {
    exit(0);
}

void sig_handler(int sid) {
    if (getpid() == pid_serveur) {
        for (int i = 0; i < NB_PROC; i++) {
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
int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    // --
    pid_t p_proc;
    pid_serveur = getpid();
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(SERV_PORT);
    
    for (int i = 0; i < NB_PROC; i++) {
        if (getpid() == pid_serveur) {
            p_proc = Fork();
            p_procs[i] = p_proc;
            if (p_proc != 0) printf("!! Pool #%d [%d]\n", i+1, p_procs[i]);
        } else {
            break;
        }
    }

    Signal(SIGPIPE, SIG_IGN); // permet d'éviter que le client crash..
    
    if (p_proc != 0) {
        Signal(SIGINT, sig_handler);
    }
    
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
        close(connfd);
        
        printf("[%d] Client traité!\n", getpid());
    }
}

