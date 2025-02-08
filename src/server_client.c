#include "../include/server_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

pthread_mutex_t message_mutex = PTHREAD_MUTEX_INITIALIZER;

void sendSocket(int sock, const char *string) {
    if (send(sock, string, strlen(string), 0) < 0) {
        perror("Send failed");
    }
}

struct socket_t *createServer() {
    struct socket_t *server = malloc(sizeof(struct socket_t));
    if (!server) {
        perror("Failed to allocate memory for server");
        exit(EXIT_FAILURE);
    }

    server->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server->sock < 0) {
        perror("Server socket creation failed");
        exit(EXIT_FAILURE);
    }

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(PORT);

    if (bind(server->sock, (struct sockaddr *)&server->address, sizeof(server->address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server->sock, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started, waiting for connections...\n");
    return server;
}

void runServer(struct socket_t *socket_serv) {
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int client_sock = accept(socket_serv->sock, (struct sockaddr *)&client_addr, &addrlen);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Client connected...\n");
        pthread_t client_handler_tid;
        int *client_sock_ptr = malloc(sizeof(int));
        *client_sock_ptr = client_sock;
        pthread_create(&client_handler_tid, NULL, handle_client, client_sock_ptr);
        pthread_detach(client_handler_tid);
    }
}

void *handle_client(void *arg) 
{
    int client_sock = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    char *name = "Client";

    while (1)
    {
        int bytes_read = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read <= 0)
        {
            printf("%s disconnected.\n", name);
            break;
        }
        buffer[bytes_read] = '\0';
        printf("%s: %s\n", name, buffer);
    }

    close(client_sock);
    return NULL;
}


struct socket_t *createClient()
{
    struct socket_t *client = malloc(sizeof(struct socket_t));
    if (!client)
    {
        perror("Failed to allocate memory for client");
        exit(EXIT_FAILURE);
    }

    client->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sock < 0)
    {
        perror("Client socket creation failed");
        exit(EXIT_FAILURE);
    }

    client->address.sin_family = AF_INET;
    client->address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &client->address.sin_addr) <= 0)
    {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    return client;
}

void runClient(struct socket_t *socket_client, char *name)
{
    if (connect(socket_client->sock, (struct sockaddr *)&socket_client->address, sizeof(socket_client->address)) < 0)
    {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");

    fd_set read_fds;
    char recv_buffer[BUFFER_SIZE];
    char send_buffer[BUFFER_SIZE];

    while (1)
    {
        printf("%s: ", name);
        fgets(send_buffer, BUFFER_SIZE, stdin);
        send_buffer[strcspn(send_buffer, "\n")] = '\0';

        if (strcmp(send_buffer, "quit") == 0)
        {
            printf("Exiting client...\n");
            break;
        }
        sendSocket(socket_client->sock, send_buffer);
    }

    close(socket_client->sock);
}