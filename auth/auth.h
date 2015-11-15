/**
 * Checks if the given username and painpassword are valid
 *
 * const char* username
 * const char* plainpass
 *
 * returns 0 if credentials match; non-zero else
 */
int autheticate(const char* username, const char*plainpass);

/**
 * Initalized mutexes and semaphore for threadsafe operation of reading and writing to the user file returns 0 on success and non-zero on failure
 */
int data_init();

int add_user(const char* username, const char* spass){
 
/**
 * Gets the salted password and the salt used to salt it with
 *
 * const char* username
 * char** saltpass
 * char** salt
 *
 * returns zero on success
 */
int get_pass(const char* username, char** saltpass, char** salt);

