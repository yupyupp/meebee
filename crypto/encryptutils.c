#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <crypt.h>
#include <limits.h>
#include <gcrypt.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "encriptutils.h"

#define GCRY_CIPHER GCRY_CIPHER_AES128   // Pick the cipher here                                
#define GCRY_C_MODE GCRY_CIPHER_MODE_ECB // Pick the cipher mode here  

int encrypt_file(RSA* key, char* pfilename, char* data, int datalen) {

    int padding = 0;
    FILE* fp;
    char* encrypted_data = (char*)malloc(datalen + 16);
    RSA_public_encrypt(datalen, data, encrypted_data, key, padding);
    fp = fopen(pfilename, "w");
    fwrite((void*) encrypted_data, 1, datalen +16, fp);
    
    fclose(fp);
    free(encrypted_data);
}

/*int decrypt_file(RSA* key, char* filename, char* data) {

    int padding = 0;                                                                                
    FILE* fp;
    fp = fopen(pfilename, "r");
    char* encrypted_data = (char*)malloc(datalen + 16);                                             
    RSA_public_encrypt(datalen, data, encrpyted_data, key, padding);                                
    fwrite((void*) encrypted_data, 1, datalen +16, fp);                                             
                                                                                                    
    fclose(fp);                                                                                     
    free(encrypted_data);
}*/

/*
int gen_keypair(char* priv_key, char* pub_key, size_t key_len) {

    RSA_generate_key_ex(&priv_key, int key_len, 65537, NULL);
}
*/

/*for a new user generate the shash*/
char* new_shash(char *password, char *salt) {

  /*encrypt password*/
  password = crypt(password, salt);

  /* Print the results. */
  return password;
}

/*generate a random salt*/
void salt_generator(char** salt) {
  unsigned long seed[3];                                                                                                                              
  const char *const seedchars =                                                                     
    "./0123456789ABCDEFGHIJKLMNOPQRST"                                                              
    "UVWXYZabcdefghijklmnopqrstuvwxyz";                                                             
  int i;                                                                                            
                                                                                                    
  /* Generate a (not very) random seed.                                                             
     You should do it better than this... */                                                        
  seed[0] = time(NULL);                                                                             
  seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);                                                   
                                                                                                    
  /*better*/                                                                                        
  seed[2] = (unsigned int)seed[1] + UINT_MAX*drand48();                                             
                                                                                                    
                                                                                                    
  /* Turn it into printable characters from ‘seedchars’. */                                         
  for (i = 0; i < 8; i++)                                                                           
    (*salt)[3+i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f]; 

}

/*decrypts priv_key with aesSymKey and stors it in priv_key*/
int decrypt_private_key(char** priv_key, char** encrypted_priv_key, char* aesSymKey, size_t io_str_len) {
    gcry_error_t     gcryError;
    gcry_cipher_hd_t gcryCipherHd;
    size_t           index;

    size_t keyLength = gcry_cipher_get_algo_keylen(GCRY_CIPHER);
    size_t blkLength = gcry_cipher_get_algo_blklen(GCRY_CIPHER);
    size_t txtLength = io_str_len;
    char * encBuffer = *encrypted_priv_key;
    char * outBuffer = *priv_key; // malloc(txtLength);
    char * iniVector = "a test ini value"; // 16 bytes

    /*Make sure important subsystems are initialized*/
    if (!gcry_check_version (GCRYPT_VERSION))
    {
    fputs("libgcrypt version mismatch\n", stderr);
    return 2;
    }

    /* We don't care about secure memory for this example */
    gcry_control(GCRYCTL_DISABLE_SECMEM, 0);

    /* We're done with initialization */
    gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

    gcryError = gcry_cipher_open(
        &gcryCipherHd, // gcry_cipher_hd_t *
        GCRY_CIPHER,   // int
        GCRY_C_MODE,   // int
        0);            // unsigned int
    if (gcryError)
    {
        printf("gcry_cipher_open failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return 2;
    }
 
    gcryError = gcry_cipher_setkey(gcryCipherHd, aesSymKey, keyLength);
    if (gcryError)
    {
        printf("gcry_cipher_setkey failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return 2;
    }
 
    gcryError = gcry_cipher_setiv(gcryCipherHd, iniVector, blkLength);
    if (gcryError)
    {
        printf("gcry_cipher_setiv failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return 2;
    }
 
    gcryError = gcry_cipher_decrypt(
        gcryCipherHd, // gcry_cipher_hd_t
        outBuffer,    // void *
        txtLength,    // size_t
        encBuffer,    // const void *
        txtLength);   // size_t
    if (gcryError)
    {
        printf("gcry_cipher_decrypt failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return 2;
    }
 
    // close cipher and clean up memory
    gcry_cipher_close(gcryCipherHd);

    return 0;
}

/*given priv_key and salted hash rewrites priv_key to aes encrypted priv_key*/
int encrypt_private_key(char** priv_key, char** encrypted_priv_key, char* aesSymKey, size_t io_str_len) {
    gcry_error_t     gcryError;                                                                                                                       
    gcry_cipher_hd_t gcryCipherHd;                                                                  
    size_t           index;                                                                         
                                                                                                    
    size_t keyLength = gcry_cipher_get_algo_keylen(GCRY_CIPHER);                                    
    size_t blkLength = gcry_cipher_get_algo_blklen(GCRY_CIPHER);                                    
    char * txtBuffer = *priv_key;           
    size_t txtLength = io_str_len; // string plus termination                              
    char * encBuffer = *encrypted_priv_key;                                                           
    char * iniVector = "a test ini value"; // 16 bytes

    /*Make sure important subsystems are initialized*/
    if (!gcry_check_version (GCRYPT_VERSION))
    {
    fputs("libgcrypt version mismatch\n", stderr);
    return 2;
    }

    /* We don't care about secure memory for this example */
    gcry_control(GCRYCTL_DISABLE_SECMEM, 0);

    /* We're done with initialization */
    gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

    gcryError = gcry_cipher_open(
        &gcryCipherHd, // gcry_cipher_hd_t *
        GCRY_CIPHER,   // int
        GCRY_C_MODE,   // int
        0);            // unsigned int
    if (gcryError)
    {
        printf("gcry_cipher_open failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return 2;
    }
 
    gcryError = gcry_cipher_setkey(gcryCipherHd, aesSymKey, keyLength);
    if (gcryError)
    {
        printf("gcry_cipher_setkey failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return 2;
    }
 
    gcryError = gcry_cipher_setiv(gcryCipherHd, iniVector, blkLength);
    if (gcryError)
    {
        printf("gcry_cipher_setiv failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return 2;
    }
 
    gcryError = gcry_cipher_encrypt(
        gcryCipherHd, // gcry_cipher_hd_t
        encBuffer,    // void *
        txtLength,    // size_t
        txtBuffer,    // const void *
        txtLength);   // size_t
    if (gcryError)
    {
        printf("gcry_cipher_encrypt failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return 2;
    }

    // close cipher and clean up memory
    gcry_cipher_close(gcryCipherHd);

    return 0;
}

