#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

struct client_socket
{
    struct sockaddr_in address;
    char buffer[1024];
    int sock;
};

struct server_socket *createServerSocket();
void bindReadClientSocket(struct client_socket *socket_client);
void SendClientSocket(struct client_socket *socket_client,  char *string);

#endif