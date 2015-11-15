#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <crypt.h>
#include <limits.h>
#include <gcrypt.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "encryptutils.h"

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

int decrypt_file(RSA* key, char* pfilename, char** data) {

    int padding = 0;                                                                                
    FILE* fp;
    fp = fopen(pfilename, "r");
    fseek(fp, 0, SEEK_END);
    int datalen = ftell(fp);
    char* encrypted_data = (char*)malloc(datalen);
    fread((void*) encrypted_data, 1, datalen, fp);     
    RSA_public_encrypt(datalen, encrypted_data, *data, key, padding);                                
                                                                                                    
    fclose(fp);                                                                                     
    free(encrypted_data);
}

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


