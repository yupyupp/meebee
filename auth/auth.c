#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include "../config.h"
#include "../data/data.h"

int autheticate(const char* username, const char*plainpass) {
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
    *spass_check = strdup(crypt_r(plainpass, salt, cdata));
    
    /* Check for equality */
    pass = !strcmp(spass_check, spass_stored);

    /* Free malloc'd variables */
    free(salt);
    free(spass_check);
    free(spass_stored);
    
    return pass;
}


