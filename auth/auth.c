#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <openssl/rsa.h>
#include "auth.h"
#include "../config.h"
#include "../crypto/encryptutils.h"

int readcount;
int writecount;
pthread_mutex_t write_mutex;
pthread_mutex_t read_mutex;
sem_t read_block;
sem_t write_block;
sem_t write_pending;

int autheticate(char* username, char*plainpass) {
    int pass = 0;
    char* salt;
    char* spass_check; /* Salted version of the passed password */
    char* spass_stored; /* Salted password stored in the database */
    struct crypt_data cdata;

    /* Allocating space for data */
    salt = malloc(sizeof(char)*SALT_SIZE);
    spass_stored = malloc(sizeof(char)*SALTED_PASS_SIZE);
    

    /* Get stored password */
    if (!get_pass(username, &spass_stored, &salt)) {
        perror("Error getting password from database");
        return -1;
    }


    /* Salt plaintext password */
    cdata.initialized = 0;
    spass_check = strdup(crypt_r(plainpass, salt, &cdata));
    
    /* Check for equality */
    pass = !strcmp(spass_check, spass_stored);

    /* Free malloc'd variables */
    free(salt);
    free(spass_check);
    free(spass_stored);
    
    return pass;
}


int data_init() {
    readcount = 0;
    writecount = 0;
    
    pthread_mutex_init(&write_mutex, NULL);
    pthread_mutex_init(&read_mutex, NULL);

    sem_init(&read_block, 0, 1);
    sem_init(&write_block, 0, 1);
    sem_init(&write_pending, 0, 1);

    return 0;
}

int add_user(char* username, char* pass) {
    FILE* fp;
    char* salt;
    char* spass; /* salted password */
    struct crypt_data cdata;


    /* Allocating space for data */
    salt = malloc(sizeof(char)*SALT_SIZE);
    salt_generator(&salt);
    
   
    /* Salt plaintext password */
    cdata.initialized = 0;
    spass = strdup(crypt_r(pass, salt, &cdata));

    /* Generate new salt for private key */
    salt_generator(&salt);


    pthread_mutex_lock(&write_mutex);
    writecount++;
    if (writecount == 1) {
        sem_wait(&read_block);
    }
    pthread_mutex_unlock(&write_mutex);

    sem_wait(&write_block);

    /* write begin */
    fp = fopen(DATABASE_FILE, "a+");

    fprintf(fp, "%s\t%s\t%s\n", username, spass, salt);

    fclose(fp);
    
    sem_post(&write_block);

    RSA* key = RSA_new();                                                                           
    BIGNUM* bne = BN_new();                                                                         
    unsigned long e = RSA_F4;                                                                     
    BN_set_word(bne,e);                                                                             
    RSA_generate_key_ex(key, USER_KEY_SIZE, bne , NULL);
    write_key(username, strdup(crypt_r(pass, salt, &cdata)), key);
    RSA_free(key);
    /* write end */

    pthread_mutex_lock(&write_mutex);
    writecount--;
    if (writecount == 0) {
        sem_post(&read_block);
    }
    pthread_mutex_unlock(&write_mutex);

    /* Free malloc'd variables */
    free(salt);

    return 0;
}

int get_pass(char* username, char** saltpass, char** salt) {
    FILE* fp;
    char *s_user, *s_spass, *s_pub_key, *s_pri_key, *s_pri_salt;
    int error = 1;

    sem_wait(&write_pending);
    sem_wait(&read_block);


    pthread_mutex_lock(&read_mutex);
    readcount++;
    if (readcount == 1) {
        sem_wait(&write_block);
    }
    pthread_mutex_unlock(&read_mutex);

    /* Read */
    sem_post(&write_pending);

    fp = fopen(DATABASE_FILE, "r");

    while (!feof(fp) && !strcmp(username, s_user)) {

        fscanf(fp, "%s\t%s\t%s\t%s\t%s\n", s_user, s_spass, s_pub_key, s_pri_key, s_pri_salt);
        if (!strcmp(username, s_user)) {
            error = 0;
            break;
        }
    }
    *saltpass = s_spass;
    
    *salt = malloc(sizeof(char)*SALT_SIZE);
    strncpy(*salt, s_spass, SALT_SIZE);

    fclose(fp);
    /* Finished Reading */

    pthread_mutex_lock(&read_mutex);
    readcount--;
    if(readcount == 0) {
        sem_post(&write_block);
    }
    pthread_mutex_unlock(&read_mutex);

    return error;
}
