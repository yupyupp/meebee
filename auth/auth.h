/**
 * Checks if the given username and painpassword are valid
 *
 * const char* username
 * const char* plainpass
 *
 * returns 0 if credentials match; non-zero else
 */
int autheticate(char* username, char*plainpass);

/**
 * Initalized mutexes and semaphore for threadsafe operation of reading and writing to the user file returns 0 on success and non-zero on failure
 */
int data_init();

/**
 * Add user and creates new key for the user
 */
int add_user(char* username, char* pass);
 
/**
 * Gets the salted password and the salt used to salt it with
 *
 * const char* username
 * char** saltpass
 * char** salt
 *
 * returns zero on success
 */
int get_pass(char* username, char** spass, char** salt);
