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

/**
 * Gets encrypted private key and the salt used to salt the plain password with
 *
 * const char* username
 * char** pri_key
 * char** pri_salt
 *
 * returns zero on success
 */
int get_prikey(const char* username, char** pri_key, char** pri_salt);

/**
 * const char* username
 * char** pub_key
 *
 * returns zero on success
 */
int get_pubkey(const char* username, char** pub_key);
