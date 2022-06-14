#define SERV_PORT 2190
#define NB_PROC 3
#define CHUNK_SIZE 500
#define CLIENT_DIR "./_CLIENTROOT/"
#define FTP_DIR "./_FTPROOT/"

typedef struct {
    int statusCode;
    char msg[MAXLINE];
    // -- for data transfer
    int fileSize;
    char data[CHUNK_SIZE];
} Response;

/**
 * @brief Statuscode
 * 
 * 404 : error
 * 100 : GET command
 * 104 : transfer denied
 * 110 : transfer request
 * 120 : transfer info
 * 112 : transfer accepted
 * 113 : transfer data
 * 111 : transfer complete
 * 
 */
