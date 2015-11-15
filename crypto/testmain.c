#include "encryptutils.h"
#include "auth.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <crypt.h>
#include <limits.h>
#include <gcrypt.h>
#include <gpgme.h>
#include <openssl/rsa.h>                                                                            
#include <openssl/pem.h>

/*for testing only*/
int main(void) {

  char* password;
    int i = 0;

    char* salt = (char*)malloc(sizeof(char) * 12);
    strncpy(salt, "$1$........", 12);
    salt_generator(&salt);
    /* Read in the user’s password*/
    password = getpass("Password:");
    password = new_shash(password, salt);

    //printf("hashed pass: %s\n", password);
    //printf("salt is: %s\n", salt);
    char* test_priv_key = (char *)malloc(sizeof(char) *128);
    strncpy(test_priv_key,"123456789 abcdefghijklmnopqrstuvwzyz ABCDEFGHIJKLMNOPQRSTUVWZYZ 123456789 abcdefghijklmnopqrstuvwzyz ABCDEFGHIJKLMNOPQRSTUVWZYZ", 128);
    //char* encrypt_priv_key = (char*)malloc(strlen(test_priv_key) +1);
    //puts(test_priv_key);
    //encrypt_private_key(&test_priv_key, &encrypt_priv_key, password, 128);
    //for (i = 0; i<128; i++) {
    //    test_priv_key[i] = '0';                                                       
    //    printf("%02X", (unsigned char)encrypt_priv_key[i]);
    //}
    //printf("\n");
    //decrypt_private_key(&test_priv_key, &encrypt_priv_key, password, 128);
    //puts("---------------\n");
    //puts(test_priv_key);

    RSA* key = RSA_new();
    BIGNUM* bne = BN_new();
    unsigned long   e = RSA_F4;
    BN_set_word(bne,e);    
    RSA_generate_key_ex(key, 1024, bne , NULL);

    puts(test_priv_key);
    encrypt_file(key, "/home/yupyupp/test1232.txt", test_priv_key, strlen(test_priv_key));

    write_key("test", password, key);
    read_key("test", password, key);

    decrypt_file(key, "/home/yupyupp/test1232.txt", &test_priv_key);


    puts(test_priv_key);
    RSA_free(key);

    
}    
