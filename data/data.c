#include <stdlib.h>
#include <stdio.h>
#include <crypt.h>
#include <string.h>
#include <sqlite3.h>

#include "../config.h"

int get_pass(const char* username, char** saltpass, char** salt) {
    sqlite3 *db;
    char* db_command;
    char* error_msg;

    /* TODO - possible multithreading issue location */
    if (sqlite3_open(DATABASE_FILE, &db)) {
        perror("Failed to open database");
        sqlite_close(db);
    }

    if (sqlite3_exec(db, db_command, )) {
        perror("Failed to open database");
        sqlite_close(db);
    }





    sqlite3_close(db);

    return 0;
}

int get_prikey(const char* username, char** pri_key, char** pri_salt) {

    return 0;
}

int get_pubkey(const char* username, char** pub_key) {

    return 0;
}
