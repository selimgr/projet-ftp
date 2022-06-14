#define SERV_PORT 2190
#define NB_PROC 5
#define CLIENT_DIR "./_CLIENTROOT/"
#define FTP_DIR "./_FTPROOT/"

typedef struct {
    int statusCode;
    char msg[MAXLINE];
    // -- for data transfer
    int fileSize;
    char *data;
} Response;

/**
 * @brief Statuscode
 * 
 * 404 : file error, does not exist
 * 100 : GET request
 * 200 : transfer begin
 * 201 : transfer data
 * 202 : transfer complete
 * 
 */
