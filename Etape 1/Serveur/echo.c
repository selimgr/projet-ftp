/*
 * echo - read and echo text lines until client closes connection
 */
#include "../Lib/csapp.h"
#include "../Lib/ftpprotocol.h"

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);

    if ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {

        /* reading file name (removing newline) */
        char *name = malloc(sizeof(char) * (n + sizeof(CLIENT_DIR) - 1));
        strcat(name, FTP_DIR);
        strncat(name, buf, n - 1);
        printf(">> Demande de fichier reçue: %s\n", name);

        Response servResponse; // Structure réponse
        
        int f = open(name, O_RDONLY, 0);
        free(name);
        
        if (f < 0) {
            /* Si le fichier n'existe pas */
            servResponse.statusCode = 404;
            strcpy(servResponse.msg, "Fichier inéxistant");
            Rio_writen(connfd, &servResponse, sizeof(Response));
        } else {
            /* Si le fichier existe, début du transfert */
            servResponse.statusCode = 200;

            struct stat f_stat;
            fstat(f, &f_stat);
            
            off_t fileSize = f_stat.st_size;
            servResponse.fileSize = fileSize; 
            Rio_writen(connfd, &servResponse, sizeof(Response));
            printf("<< Envoi de début de transfert au client\n");
            
            // Transfert
            servResponse.data = malloc(sizeof(char) * fileSize);

            servResponse.statusCode = 201;
            printf("<< Transfert de %ld octets\n", fileSize);
            Rio_writen(connfd, &servResponse, sizeof(Response));

            // Envoi data
            n = Rio_readn(f, servResponse.data, fileSize);
            Rio_writen(connfd, servResponse.data, fileSize);
            free(servResponse.data);

            servResponse.statusCode = 202;
            Rio_writen(connfd, &servResponse, sizeof(Response));
            
            printf("Transfert terminé\n");
        }
        close(f);
    }
}

