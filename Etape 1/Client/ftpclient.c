/*
 * echoclient.c - An echo client
 */
#include "../Lib/csapp.h"
#include "../Lib/ftpprotocol.h"

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
    clientfd = Open_clientfd(host, SERV_PORT);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("Client connecté au serveur FTP\n"); 
    
    Rio_readinitb(&rio, clientfd);

    if (fgets(buf, MAXLINE, stdin) == NULL)
        return 1;

    Rio_writen(clientfd, buf, strlen(buf));

    // Vérification des erreurs
    Response servResponse;
    if ((n = Rio_readnb(&rio, &servResponse, sizeof(Response))) > 0) {
        if (servResponse.statusCode == 404) {
            printf(">> Erreur serveur: %s\n", servResponse.msg);
            exit(1);
        } else if (servResponse.statusCode == 200) {
            char *namefile = malloc(sizeof(char) * (strlen(buf) + sizeof(CLIENT_DIR) - 1));
            strcat(namefile, CLIENT_DIR);
            strncat(namefile, buf, strlen(buf) - 1);
            int f = open(namefile, O_RDWR | O_CREAT, S_IRWXU);
            free(namefile);

            if (f < 0) {
                printf("Erreur création fichier\n");
                exit(1);
            }

            int tailleFichier = servResponse.fileSize;
            printf(">> Début du transfert par le serveur [%d octects]\n", servResponse.fileSize);
            
            /* Début du transfert */
            struct timeval start, end;
            gettimeofday(&start, NULL);
            
            n = Rio_readnb(&rio, &servResponse, sizeof(Response));
            
            if (servResponse.statusCode == 201) {
                printf(">> Reçu %d octets\n", servResponse.fileSize);

                // on réceptionne les données
                char *data = malloc(sizeof(char) * tailleFichier);
                n = Rio_readnb(&rio, data, tailleFichier);
                
                if (n > 0) {
                    Rio_writen(f, data, tailleFichier);
                    n = Rio_readnb(&rio, &servResponse, sizeof(Response));
                }
            }
            
            if (servResponse.statusCode == 202) {
                gettimeofday(&end, NULL);
                float elapsed = (end.tv_sec - start.tv_sec) + 1e-6*(end.tv_usec - start.tv_usec);
                printf("Transfert terminé en %0.6f seconde(s)", elapsed);
                
                double vitesse = tailleFichier/elapsed;
                vitesse /= 1000;
                printf(" (%2.lf Kbytes/s)\n", vitesse);
            } else {
                printf("Erreur lors du transfert, aled\n");
            }
            close(f);   
        }
    }

    Close(clientfd);
    exit(0);
}
