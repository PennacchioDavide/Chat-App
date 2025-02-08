#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

extern pthread_mutex_t message_mutex;

struct client_info {
    struct socket_t *client;
    char *name;
};

struct socket_t {
    struct sockaddr_in address;
    int sock;
};

void sendSocket(int sock, const char *string);
struct socket_t *createServer();
void runServer(struct socket_t *socket_serv);
void *handle_client(void *arg);

struct socket_t *createClient();
void runClient(struct socket_t *socket_client, char *name);

#endif