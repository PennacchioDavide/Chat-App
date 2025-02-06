#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

struct socket_t
{
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    int sock;
};

struct socket_t *createServer();
void runServer(struct socket_t *socket_serv);

struct socket_t *createClient();
void runClient(struct socket_t *socket_client);

void SendSocket(struct socket_t *socketT,  char *string);

#endif