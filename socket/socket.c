#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "../config.h"

int make_socket (uint16_t port) {
    int sock;
    struct sockaddr_in name;

    /* Create the socket */
    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0 ) {
        perror("socket");
        exit (EXIT_FAILURE);
    }

    /* Name the socket */
    name.sin_family = AF_INET;
    name.sin_port = htons (port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind (sock, (struct sockaddr*) &name, sizeof(name)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    return sock;
}

int read_from_client(int filedes) {
    char buffer[MAXMSG];
    int nbytes;

    nbytes = read (filedes, buffer, MAXMSG);
    

    if (nbytes < 0) {
        /* Read error */
        perror("read");
        exit(EXIT_FAILURE);
    } else if (nbytes == 0) {
        /* EOF */
        return -1;
    } else {
    if (nbytes < MAXMSG) {
        buffer[nbytes-1] = '\0';
    }
        /* Data reading */
        fprintf(stderr, "server: got messeg: %s\n", buffer);
        write(filedes, "this is a test\n", 16);
    }
    return 0;
}

int spawn_socket(void) {
    extern int make_socket (uint16_t port);
    int sock;
    int i;
    fd_set active_fd_set, read_fd_set;
    struct sockaddr_in clientname;
    size_t size;

    /* create the socket */
    sock = make_socket(PORT);
    if (listen(sock, 1) < 0) {
        perror("Error creating socket");
        return -1;
    }
    
    /* Initalize the set of active sockets */
    FD_ZERO(&active_fd_set);
    FD_SET(sock, &active_fd_set);

    while(1) {
        /* Block until input arives on active socket */
        read_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
            perror( "select");
            return -1;
        }

        /* Service sockets with input pending */
        for (i = 0; i < FD_SETSIZE; ++i)
        if (FD_ISSET (i, &read_fd_set)) { 
            if (i == sock) { 
                /* Connection request on original socket. */ 
                int new; 
                size = sizeof (clientname); 
                new = accept (sock, (struct sockaddr *) &clientname, (socklen_t *) &size); 
                if (new < 0) { 
                    perror ("accept"); 
                    return -1;
                } 
                fprintf (stderr, 
                        "Server: connect from host %s, port %hd.\n", 
                        inet_ntoa (clientname.sin_addr), 
                        ntohs (clientname.sin_port));
                FD_SET (new, &active_fd_set); 
            } else { 
                /* Data arriving on an already-connected socket. */ 
                if (read_from_client (i) < 0) { 
                    close (i); 
                    FD_CLR (i, &active_fd_set);
                }
            }
        }
    }
    return 0;
}
                    
void* interpretMessage (void* sock_des) {
    int done = 0;
    ssize_t len;
    char sock_buff[MAXMSG];

    while (!done) {
        len = recv(*(int*)sock_des, sock_buff, MAXMSG, 0);
        if (len <= 0) {
            if (len < 0) perror("recv");
            done = 1;
        }

        if (!done) {
            if (send(*(int*)sock_des, sock_buff, len, 0) < 0) {
                perror("send");
                done = 1;
            }
        }
    }
    close(*(int*)sock_des);
}

int spawn_MT_socket(void) {
    int sock, connected, bytes_received, true = 1;
    struct sockaddr_in server_addr, client_addr;
    int sin_size;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket");
        exit(1);
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &true, sizeof (int)) == -1) {
        perror("Setsockopt");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_addr.sin_zero), 8);

    if (bind(sock, (struct sockaddr *) &server_addr, sizeof (struct sockaddr)) == -1) { 
        perror("Unable to bind");
        exit(1);
    }


    if (listen(sock, 5) == -1) {
        perror("Listen");
        exit(1);
    }

    printf("\nTCPServer Waiting for client on port 5003");
    fflush(stdout); 

    while (1) {
        /* Memory leak here. This child is never freed */
        pthread_t *child = (pthread_t *)malloc( sizeof(pthread_t) );

        sin_size = sizeof (struct sockaddr_in);
        connected = accept(sock, (struct sockaddr *) &client_addr, &sin_size);
        printf("\n I got a connection from (%s , %d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 

        pthread_create(child, NULL, interpretMessage, &connected);
    }

}
