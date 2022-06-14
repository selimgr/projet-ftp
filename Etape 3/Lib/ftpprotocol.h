#define MASTER_PORT 2190
#define SLAVE_PORT 2100

#define NB_SLAVES 5
#define CHUNK_SIZE 500
#define CLIENT_DIR "./_CLIENTROOT/"
#define FTP_DIR "./_FTPROOT/"

typedef struct {
    int port;
    int id;
} Esclave;

typedef struct {
    int statusCode;
    char msg[MAXLINE];
    // -- for data transfer
    int fileSize;
    char data[CHUNK_SIZE];
    // -- slave
    Esclave slaveData;
} Response;


/**
 * @brief Statuscode
 * 
 * 1 : slave connect
 * 404 : error
 * 100 : GET command
 * 101 : transfer denied
 * 110 : transfer request
 * 120 : transfer info
 * 112 : transfer accepted
 * 113 : transfer data
 * 111 : transfer complete
 * 
 */
