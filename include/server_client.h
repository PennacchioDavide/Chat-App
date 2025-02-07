#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

extern char server_message[BUFFER_SIZE];
extern char client_message[BUFFER_SIZE];
extern bool new_server_message;
extern bool new_client_message;

extern pthread_mutex_t message_mutex;

struct socket_t {
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    int sock;
};

void sendSocket(int sock, char *string);
void broadcastMessage(const char *message, int sender_sock);
struct socket_t *createServer();
void runServer(struct socket_t *socket_serv);
struct socket_t *createClient();
void runClient(struct socket_t *socket_client);
void *handleClient(void *arg);

#endif