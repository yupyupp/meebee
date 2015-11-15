#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include "../config.h"

int readcount;
int writecount;
pthread_mutex_t write_mutex;
pthread_mutex_t read_mutex;
sem_t read_block;
sem_t write_block;
sem_t write_pending;


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

int add_user(const char* username, const char* spass, const char* pub_key, const char* pri_key, char* pri_salt){
    FILE* fp;

    pthread_mutex_lock(&write_mutex);
    writecount++;
    if (writecount == 1) {
        sem_wait(&read_block);
    }
    pthread_mutex_unlock(&write_mutex);

    sem_wait(&write_block);

    fp = fopen(DATABASE_FILE, "a+");

    fprintf(fp, "%s\t%s\t%s\t%s\t%s\n", username, spass, pub_key, pri_key,pri_salt);

    fclose(fp);
    
    sem_post(&write_block);


    pthread_mutex_lock(&write_mutex);
    writecount--;
    if (writecount == 0) {
        sem_post(&read_block);
    }
    pthread_mutex_unlock(&write_mutex);

    return 0;
}

int get_pass(const char* username, char** saltpass, char** salt) {
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

int get_prikey(const char* username, char** pri_key, char** pri_salt) {
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

    *pri_key = s_pri_key;
    *pri_salt = s_pri_salt;

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

int get_pubkey(const char* username, char** pub_key) {
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

    while (!feof(fp) ) {

        fscanf(fp, "%s\t%s\t%s\t%s\t%s\n", s_user, s_spass, s_pub_key, s_pri_key, s_pri_salt);
        if (!strcmp(username, s_user)) {
            error = 0;
            break;
        }
    }
    *pub_key = s_pub_key;

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
