/*
 * echo - read and echo text lines until client closes connection
 */
#include "../Lib/csapp.h"
#include "../Lib/ftpprotocol.h"

void sendFile(int connfd, Response servResponse) {
    size_t n;
    char *fullPath = malloc(sizeof(char) * (strlen(servResponse.msg) + strlen(FTP_DIR)));
    strcpy(fullPath, FTP_DIR);
    strcat(fullPath, servResponse.msg);

    int f = open(fullPath, O_RDONLY, 0);
    free(fullPath);
    
    if (f < 0) {
        /* Si le fichier n'existe pas */
        servResponse.statusCode = 104;
        strcpy(servResponse.msg, "Fichier inexistant");
        n = rio_writen(connfd, &servResponse, sizeof(Response));
        if (n == -1) {
            close(connfd);
            return;
        }
    } else {
        /* Si le fichier existe, demande du transfert */
        servResponse.statusCode = 110;

        struct stat f_stat;
        fstat(f, &f_stat);
        
        off_t fileSize = f_stat.st_size;
        servResponse.fileSize = fileSize; 
        n = rio_writen(connfd, &servResponse, sizeof(Response));
        if (n == -1) {
            close(connfd);
            return;
        }
        
        printf("<< Envoi de la demande de transfert\n");

        if ((n = Rio_readn(connfd, &servResponse, sizeof(Response))) <= 0) return;
        if (servResponse.statusCode != 120) return;

        lseek(f, servResponse.fileSize, SEEK_SET);
        fileSize -= servResponse.fileSize;

        if (fileSize > 0 && servResponse.fileSize > 0) {
            printf("-- Reprise de transfert\n");
        }
        servResponse.statusCode = 112;
        rio_writen(connfd, &servResponse, sizeof(Response));
        
        // Transfert
        int sentBytes = 0;

        while ((n = rio_readn(f, &servResponse.data, CHUNK_SIZE)) > 0 && sentBytes < fileSize) {
            servResponse.statusCode = 113;
            servResponse.fileSize = n;
            //float p = (100*sentBytes)/fileSize;
            //printf("[%1.f%%] << Transfert de %d octets sur %lld\n", p, sentBytes, fileSize);
            ssize_t n1 = rio_writen(connfd, &servResponse, sizeof(Response));
            if (n1 == -1) {
                return;
            }
            sentBytes += n;
        }
        
        if (sentBytes == fileSize) {
            servResponse.statusCode = 111;
            Rio_writen(connfd, &servResponse, sizeof(Response));
            
            printf("[%d] Transfert terminé\n", getpid());
        } else {
            servResponse.statusCode = 404;
            Rio_writen(connfd, &servResponse, sizeof(Response));
            printf("Erreur lors du transfert");
        }
    }
    close(f);
}

void echo(int connfd)
{
    size_t n;
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    
    Response servResponse;

    while ((n = rio_readnb(&rio, &servResponse, sizeof(Response))) > 0) {
        if (n == -1) break;

        switch (servResponse.statusCode) {
            case 100: {
                printf("[%d] >> Demande de fichier reçue: %s\n", getpid(), servResponse.msg);
                sendFile(connfd, servResponse);
                break;
            }
            default: {
                printf(">> Requête inconnue.\n");
                servResponse.statusCode = 104;
                strcpy(servResponse.msg, "Requête inconnue.");
                Rio_writen(connfd, &servResponse, sizeof(Response));
                break;
            }
        }

    }
}

