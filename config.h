/* string sizes */
#define USERNAME_MAX 32
#define PASSWORD_MAX 32
#define SALTED_PASS_SIZE 21
#define SALT_SIZE 12
#define FILENAME_MAX 64

/* Crypto defines */
#define USER_KEY_SIZE 4096
#define PRI_KEY_FILE "priv_key.pem"
#define PUB_KEY_FILE "pub_key.pem"
#define PEM_ALG EVP_aes_256_cbc()

/* data storage defines */
#define DATA_ROOT "/opt/meebee"
#define DATABASE_FILE "/opt/meebee/users.txt"

/* socket defines */
#define PORT 61254
#define MAXMSG 1024
#define SOCKET_BUFFER 32

#include "auth/auth.h"
#include "socket/socket.h"
#include "crypto/encryptutils.h"

