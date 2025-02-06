#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct server_socket
{
    struct sockaddr_in address;
    char buffer[1024];
    int sock;
};

struct server_socket *createServerSocket();
void bindReadServerSocket(struct server_socket *socket_serv);
void SendServerSocket(struct server_socket *socket_serv,  char *string);

#endif