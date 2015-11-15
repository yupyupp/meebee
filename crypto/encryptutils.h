#ifndef encryptutils
#define encryptutils
#include <gcrypt.h>
#include <openssl/rsa.h>

/*for a new user generate the shash*/
char* new_shash(char *password, char *salt);

/*generate a random salt*/
void salt_generator(char** salt);

/*given priv_key and salted hash writes encrypted_priv_key to aes encrypted priv_key*/
int encrypt_private_key(char** priv_key, char** encrypted_priv_key, char* aesSymKey, size_t io_str_len);

/*decrypts encrypted_priv_key with aesSymKey and stors it in priv_key*/
int decrypt_private_key(char** priv_key, char** encrypted_priv_key, char* aesSymKey, size_t io_str_len);

/*encrypts a file and stores it to DATA_ROOT/username/path-to-file*/
int encrypt_file(RSA* key, char* filename, char* data, int datalen);

/*decrypts a file and returns a pointer to the file in memeory*/
int decrypt_file(RSA* key, char* pfilename, char** data);

/*generate a new public private keypair*/
int gen_keypair(char* priv_key, char* pub_key, size_t key_len);

/* read keypair from a file */
int read_key(char* user, char* spass, RSA* keypair);

/* write keypair to a file */
int write_key(char* user, char* spass, RSA* keypair);

#endif
