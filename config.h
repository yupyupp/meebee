#define USERNAME_MAX 32

#define USER_KEY_SIZE 4096

#define SALTED_PASS_SIZE 21
#define SALT_SIZE 12

#define PRI_KEY_FILE "priv_key.pem"
#define PUB_KEY_FILE "pub_key.pem"
#define PEM_ALG EVP_aes_256_cbc()


#define DATA_ROOT "/opt/meebee"
#define DATABASE_FILE "/opt/meebee/users.txt"

#define PORT 61254
#define MAXMSG 512

#include "auth/auth.h"
#include "socket/socket.h"
#include "crypto/encryptutils.h"

