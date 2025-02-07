#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <raylib.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

extern char server_message[BUFFER_SIZE];
extern char client_message[BUFFER_SIZE];
extern bool new_server_message;
extern bool new_client_message;

extern pthread_mutex_t message_mutex;

struct socket_t 
{
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    int sock;
};

void sendSocket(int sock,  char *string);

struct socket_t *createServer();
void runServer(struct socket_t *socket_serv);

struct socket_t *createClient();
void runClient(struct socket_t *socket_client);

void *handleClient(void *arg);

#endif