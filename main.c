/* External system/library includes */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <gcrypt.h>
#include <sys/types.h>

/* Local project includes */
#include "auth/auth.h"
#include "data/data.h"
#include "socket/socket.h"
#include "config.h"

int main (int argc, char* argv[]) {

    /* Data reading initilization */
    data_init();
    
    /* Encryption backend initilization */
    /*gcry_control(GCRYCTL_SET_THREAD_CBS, 0);*/

    spawn_socket();

    return 0;
}
