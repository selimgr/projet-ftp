/*
 * ftpclient.c --
 */
#include "../Lib/csapp.h"
#include "../Lib/ftpprotocol.h"

void getFile(int clientfd, rio_t rio, char *args) {
    size_t n;

    Response servResponse;
    servResponse.statusCode = 100;
    strcpy(servResponse.msg, args);
    Rio_writen(clientfd, &servResponse, sizeof(Response));
    // Requête GET au serveur

    if ((n = Rio_readnb(&rio, &servResponse, sizeof(Response))) > 0) {
        if (servResponse.statusCode == 104) {
            printf(">> Erreur serveur: %s\n", servResponse.msg);
            return;
        } else if (servResponse.statusCode == 110) {
            char *fullPath = malloc(sizeof(char) * (strlen(args) + sizeof(CLIENT_DIR)));
            strcpy(fullPath, CLIENT_DIR);
            strcat(fullPath, args);

            int f = open(fullPath, O_WRONLY | O_CREAT, S_IRWXU);
            free(fullPath);

            struct stat f_stat;
            fstat(f, &f_stat);
            lseek(f, f_stat.st_size, SEEK_SET); // au cas où si le fichier
                                                // existe déjà

            if (f < 0) {
                printf("Erreur création fichier\n");
                exit(1);
            }

            int tailleFichier = servResponse.fileSize - f_stat.st_size;

            servResponse.statusCode = 120;
            servResponse.fileSize = f_stat.st_size;
            Rio_writen(clientfd, &servResponse, sizeof(Response));

            if ((n = Rio_readnb(&rio, &servResponse, sizeof(Response))) <= 0) return;
            if (servResponse.statusCode != 112) return;
            
            printf(">> Début du transfert par le serveur [%d octects]\n", tailleFichier);
            
            /* Début du transfert */
            int receivedBytes = 0;
            struct timeval start, end;
            gettimeofday(&start, NULL);

            while (((n = Rio_readnb(&rio, &servResponse, sizeof(Response))) > 0) && receivedBytes < tailleFichier) {
                if (servResponse.statusCode == 113) {
                    receivedBytes += servResponse.fileSize;
//                    float p = (100*receivedBytes)/tailleFichier;
//                    printf("[%1.f%%] >> Reçu %d octets sur %d\n", p, receivedBytes, tailleFichier);
                    Rio_writen(f, servResponse.data, servResponse.fileSize);
                }
            }

            if (servResponse.statusCode == 111) {
                gettimeofday(&end, NULL);
                float elapsed = (end.tv_sec - start.tv_sec) + 1e-6*(end.tv_usec - start.tv_usec);
                printf("Transfert terminé en %0.6f seconde(s)", elapsed);
                
                double vitesse = tailleFichier/elapsed;
                vitesse /= 1000;
                printf(" (%2.lf Kbytes/s)\n", vitesse);
            } else {
                printf("Erreur lors du transfert\n");
            }
            close(f);
        }
    }
}

int main(int argc, char **argv)
{
    int clientfd;
    size_t n;
    char *host, buf[MAXLINE];
    rio_t rio;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd(host, MASTER_PORT);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("Client connecté au serveur maître\n"); 
    
    Rio_readinitb(&rio, clientfd);

    Response servResponse;
    servResponse.statusCode = 200;
    Rio_writen(clientfd, &servResponse, sizeof(Response));

    if ((n = Rio_readn(clientfd, &servResponse, sizeof(Response))) > 0) {
        printf("-- Esclave au port %d\n", servResponse.slaveData.port);
        close(clientfd);
        clientfd = open_clientfd(host, servResponse.slaveData.port);
        if (clientfd <= 0) {
            printf("Erreur de connexion à l'esclave\n");
            exit(1);
        }
        Rio_readinitb(&rio, clientfd);
    }
    printf("--> Connecté au serveur FTP\n"); 

    char delim[] = " ";
    while (1) {
        printf("> ");
        if (fgets(buf, MAXLINE, stdin) == NULL) continue;

        // Analyse commande
        char *cmd = malloc(sizeof(char) * (strlen(buf)));
        strncpy(cmd, buf, strlen(buf) - 1);
        char *args = strtok(cmd, delim);
        
        if (strcmp(args, "bye") == 0) {
            Close(clientfd);
            exit(0);
        }

        if (strcmp(args, "get") == 0) {
            // GET command
            args = strtok(NULL, delim);
            if (args == NULL) continue;
            getFile(clientfd, rio, args);
        }
    
    }
}
