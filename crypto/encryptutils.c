#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <crypt.h>
#include <limits.h>
#include <gcrypt.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "encryptutils.h"
#include "../config.h"

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


int get_key_path(const char* user, char** pri_dir, char** pub_dir) {

    printf("thingz: %i\n", sizeof(char) * (strlen(DATA_ROOT) + strlen(PRI_KEY_FILE) + strlen(user)));
    *pri_dir = malloc(sizeof(char) * (strlen(DATA_ROOT) + strlen(PRI_KEY_FILE) + strlen(user))+3);
    *pub_dir = malloc(sizeof(char) * (strlen(DATA_ROOT) + strlen(PUB_KEY_FILE) + strlen(user))+3);
    snprintf(*pri_dir, sizeof(char) * (strlen(DATA_ROOT) + strlen(PRI_KEY_FILE) + strlen(user))+3, "%s/%s/%s", DATA_ROOT, user, PRI_KEY_FILE);
    snprintf(*pub_dir, sizeof(char) * (strlen(DATA_ROOT) + strlen(PUB_KEY_FILE) + strlen(user))+3 , "%s/%s/%s", DATA_ROOT, user, PUB_KEY_FILE);
    printf("%s\n", *pri_dir);
    puts(*pub_dir);
    puts(*pri_dir);

}

int write_key(char* user, char* spass, RSA* keypair) {

    FILE* pub_file;
    FILE* pri_file;
    char* pub_filestr;
    char* pri_filestr;

    get_key_path(user, &pri_filestr, &pub_filestr);


    pub_file = fopen(pub_filestr, "w+");
    pri_file = fopen(pri_filestr, "w+");



    PEM_write_RSAPublicKey(pub_file, keypair);
    PEM_write_RSAPrivateKey(pri_file, keypair, PEM_ALG, spass, SALTED_PASS_SIZE, NULL, NULL);
    
    fclose(pub_file);
    fclose(pri_file);
    free(pub_filestr);
    free(pri_filestr);
}

int read_key(char* user, char* spass, RSA* keypair) {

    FILE* pub_file;
    FILE* pri_file;
    char* pub_filestr;
    char* pri_filestr;

    get_key_path(user, &pri_filestr, &pub_filestr);


    pub_file = fopen(pub_filestr, "w+");
    pri_file = fopen(pri_filestr, "w+");
    puts(pub_filestr);
    puts(pri_filestr);

    PEM_read_RSAPublicKey(pub_file, &keypair, NULL, spass);
    PEM_read_RSAPrivateKey(pri_file, &keypair, NULL, spass);
    
    fclose(pub_file);
    fclose(pri_file);
    free(pub_filestr);
    free(pri_filestr);
}
